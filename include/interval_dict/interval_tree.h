//  IntervalDict library
//
//  Copyright Leo Goodstadt 2020-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0.
//  (See http://www.boost.org/LICENSE_1_0.txt)
//
//  Project home: https://github.com/goodstadt/intervaldict
//
/// \file interval_tree.h
/// \brief Definitions of functions to implement IntervalDict using an
/// Interval Tree (currently using tinloaf/ygg and boost::intrusive)
///
/// Interval tree logic mostly from tinloaf/ygg but with my own bugs! and
/// the collapsing of the abstract methods with a more concrete
/// boost::icl::interval
///
/// This stores values as non-disjoint intervals
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_INTERVAL_TREE_H
#define INCLUDE_INTERVAL_DICT_INTERVAL_TREE_H

#include "gregorian.h"

// ygg
#include <src/options.hpp>
#include <src/rbtree.hpp>

#include <boost/icl/concept/interval.hpp>
#include <boost/icl/interval_traits.hpp>
#include <boost/icl/type_traits/is_asymmetric_interval.hpp>
#include <boost/icl/type_traits/is_continuous_interval.hpp>
#include <boost/icl/type_traits/is_discrete_interval.hpp>
#include <boost/intrusive/set.hpp>
#include <boost/intrusive/set_hook.hpp>

#include <cppcoro/generator.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>

namespace interval_dict
{

//
// exclusive_less()
// Comparison operators between a single point and an interval on the right
// Specialised for the different sorts of boost::icl::Interval
//
namespace details
{
template <class BaseType, class IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    bool>
exclusive_less(const BaseType& left_upper, const IntervalType& right)
{
    return boost::icl::domain_less_equal<IntervalType>(left_upper,
                                                       lower(right));
}

template <class BaseType, class IntervalType>
inline typename std::enable_if_t<
    boost::icl::has_symmetric_bounds<IntervalType>::value ||
        boost::icl::is_discrete_interval<IntervalType>::value,
    bool>
exclusive_less(const BaseType& left_upper, const IntervalType& right)
{
    return boost::icl::domain_less<IntervalType>(left_upper, first(right));
}

//
// upper_edge()
// Get the edges of an interval, consistent across dynamic and static
// interval types
// Specialised for the different sorts of boost::icl::Interval
//
template <class IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
upper_edge(const IntervalType& interval)
{
    return boost::icl::upper(interval);
}

template <class IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_discrete_interval<IntervalType>::value ||
        boost::icl::has_symmetric_bounds<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
upper_edge(const IntervalType& interval)
{
    return boost::icl::last(interval);
}

//
// lower_edge()
// Get the edges of an interval, consistent across dynamic and static
// interval types
// Specialised for the different sorts of boost::icl::Interval
//
template <class IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
lower_edge(const IntervalType& interval)
{
    return boost::icl::lower(interval);
}

template <class IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_discrete_interval<IntervalType>::value ||
        boost::icl::has_symmetric_bounds<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
lower_edge(const IntervalType& interval)
{
    return boost::icl::first(interval);
}

} // namespace details


//
// IntervalNode
//
// Holds a single Value valid over a boost:icl::Interval
// Inherits from ygg::RBTreeNodeBase and boost::intrusive::set_base_hook
// So can be inserted at the same time in both a ygg::RBTree and
// boost::intrusive::set
//
template <typename Val, typename Interval>
struct IntervalNode : public ygg::RBTreeNodeBase<IntervalNode<Val, Interval>,
                                                 ygg::DefaultOptions,
                                                 int>,
                      public boost::intrusive::set_base_hook<>
{
public:
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;
    using IntervalType = Interval;
    using ValueType = Val;
    Interval interval;
    Val value;
    BaseType max_right_edge;

    template <typename Other> bool operator<(const Other& rhs) const
    {
        const auto my_interval = std::tuple(details::lower_edge(interval),
                                            details::upper_edge(interval));
        const auto other_interval =
            std::tuple(details::lower_edge(rhs.interval),
                       details::upper_edge(rhs.interval));
        return std::tie(my_interval, value) <
               std::tie(other_interval, rhs.value);
    }

    template <typename Other> bool operator==(const Other& rhs) const
    {
        return std::tie(interval, value) == std::tie(rhs.interval, rhs.value);
    }

    IntervalNode(Val value, Interval interval)
        : interval(interval), value(value)
    {
    }
};

template <typename Val, typename Interval>
std::ostream& operator<<(std::ostream& os,
                         const IntervalNode<Val, Interval>& node)
{
    os << node.value << ", " << node.interval << " -> " << node.max_right_edge;
    return os;
}

//
// ValIntervalRef
//
// Lightweight reference object for comparisons with IntervalNode without the
// construction costs.
// Reference semantics: beware of lifetime issues. Do not store!
//
template <typename Val, typename Interval> struct ValIntervalRef
{
    ValIntervalRef(Val& value, Interval& interval)
        : value(value), interval(interval)
    {
    }
    Val& value;
    Interval& interval;
};

} // namespace interval_dict

// Partial specialisation of std::hash for IntervalNode and ValIntervalRef
namespace std
{
namespace
{
// Code from boost
// Reciprocal of the golden ratio helps spread entropy
//     and handles duplicates.
// See Mike Seymour in magic-numbers-in-boosthash-combine:
//     http://stackoverflow.com/questions/4948780
template <class T> inline void hash_combine(std::size_t& seed, T const& v)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
} // namespace

/// partial specialise std::hash for IntervalNode
/// assumes Interval::BaseType and Val are hashable types
template <typename Val, typename Interval>
struct hash<interval_dict::IntervalNode<Val, Interval>>
{
    size_t
    operator()(interval_dict::IntervalNode<Val, Interval> const& in) const
    {
        size_t hash_value = 0;
        hash_combine(hash_value, boost::icl::lower(in.interval));
        hash_combine(hash_value, boost::icl::upper(in.interval));
        hash_combine(hash_value, in.value);
        return hash_value;
    }
};

template <typename Val, typename Interval>
struct hash<interval_dict::ValIntervalRef<Val, Interval>>
{
    size_t
    operator()(interval_dict::IntervalNode<Val, Interval> const& in) const
    {
        size_t hash_value = 0;
        hash_combine(hash_value, boost::icl::lower(in.interval));
        hash_combine(hash_value, boost::icl::upper(in.interval));
        hash_combine(hash_value, in.value);
        return hash_value;
    }
};

} // namespace std



namespace interval_dict
{
namespace details
{

// Class to sort nodes on their lower or upper edges
template <typename Val, typename Interval> struct EdgeNode
{
    using Basetype = typename IntervalNode<Val, Interval>::BaseType;
    Basetype edge;
    const IntervalNode<Val, Interval>* pnode;
    EdgeNode(const IntervalNode<Val, Interval>* pnode, Basetype edge)
        : edge(edge), pnode(pnode)
    {
    }
};

// Default sort by edge
// Used for lower edges to sort the leftmost nodes first
template <typename Val, typename Interval>
bool operator<(const EdgeNode<Val, Interval>& a,
               const EdgeNode<Val, Interval>& b)
{
    return std::tie(a.edge, *a.pnode) < std::tie(b.edge, *b.pnode);
}

// Sort in reverse order
// Used for upper edges to sort the rightmost nodes first
struct Greater
{
    template <typename Val, typename Interval>
    bool operator()(const EdgeNode<Val, Interval>& a,
                    const EdgeNode<Val, Interval>& b) const
    {
        return std::tie(a.edge, *a.pnode) > std::tie(b.edge, *b.pnode);
    }
};

// Compare Intervals and IntervalNodes by their lower/upper edges
template <typename Val, typename Interval>
const Interval& get_interval(const IntervalNode<Val, Interval>& node)
{
    return node.interval;
}

template <typename Interval>
const Interval& get_interval(const Interval& interval)
{
    return interval;
}

template <typename Val, typename Interval>
IntervalNode<Val, Interval>*
find_next_overlapping(IntervalNode<Val, Interval>* cur, const Interval& q);

// Compare Interval then value
class CompareInterval
{
public:
    template <typename Val, typename Interval>
    bool operator()(const IntervalNode<Val, Interval>& lhs,
                    const IntervalNode<Val, Interval>& rhs) const
    {
        const auto lhs_interval =
            std::tuple(details::lower_edge(get_interval(lhs)),
                       details::upper_edge(get_interval(lhs)));
        const auto rhs_interval =
            std::tuple(details::lower_edge(get_interval(rhs)),
                       details::upper_edge(get_interval(rhs)));
        return std::tie(lhs_interval, lhs.value) <
               std::tie(rhs_interval, rhs.value);
    }

    template <typename T1, typename T2>
    bool operator()(const T1& lhs, const T2& rhs) const
    {
        const auto lhs_interval =
            std::tuple(details::lower_edge(get_interval(lhs)),
                       details::upper_edge(get_interval(lhs)));
        const auto rhs_interval =
            std::tuple(details::lower_edge(get_interval(rhs)),
                       details::upper_edge(get_interval(rhs)));

        return lhs_interval < rhs_interval;
    }
};

// Callback functions to propagate the max_right_node up the tree
template <typename Val, typename Interval> class ExtendedNodeTraits
{
public:
    using Node = IntervalNode<Val, Interval>;
    using BaseType = typename Node::BaseType;

    // TODO: do these do redundant fix_node() operations?
    template <class BaseTree>
    static void leaf_inserted(Node& node, BaseTree& t);

    static void fix_node(Node& node);
    template <class BaseTree> static void rotated_left(Node& node, BaseTree& t);
    template <class BaseTree>
    static void rotated_right(Node& node, BaseTree& t);
    template <class BaseTree>
    static void deleted_below(Node& node, BaseTree& t);
    template <class BaseTree> static void delete_leaf(Node& node, BaseTree& t)
    {
        (void)node;
        (void)t;
    }
    template <class BaseTree>
    static void swapped(Node& n1, Node& n2, BaseTree& t);
};

// \brief Sorts by Value then intervals.
// Intervals compare normally unless overlap when it always returns false
// Heterogeneous comparator
struct CompareValIntervalOverlap
{
    using is_transparent = void;
    template <typename S, typename T>
    bool operator()(const S& s, const T& t) const
    {
        if (s.value != t.value)
        {
            return s.value < t.value;
        }
        if (boost::icl::intersects(s.interval, t.interval))
        {
            return false;
        }
        return s.interval < t.interval;
    }
};

// \brief Sorts by Value then intervals.
// Used in insert() to gather (& combine) touching intervals with the same value
// Intervals compare normally unless overlap or *touch* when it returns false
// Heterogeneous comparator
struct CompareValIntervalTouches
{
    using is_transparent = void;
    template <typename S, typename T>
    bool operator()(const S& s, const T& t) const
    {
        if (s.value != t.value)
        {
            return s.value < t.value;
        }
        if (s.interval == t.interval)
        {
            return false;
        }
        if (s.interval < t.interval)
        {

            if (boost::icl::intersects(s.interval, t.interval) ||
                boost::icl::touches(s.interval, t.interval))
            {
                return false;
            }
            return true;
        }
        return false;
    }
};

// \brief Compare by member value then by interval
struct CompareValInterval
{
    using is_transparent = void;
    template <typename S, typename T>
    bool operator()(const S& s, const T& t) const
    {
        return std::tie(s.value, s.interval) < std::tie(t.value, t.interval);
    }
};

} // namespace details

/// @endcond

/**
 * @brief Stores an Interval Tree
 *
 * This class stores an interval tree on the nodes it contains. It is
 * implemented via the 'augmented red-black tree' described by Cormen et al.
 */
template <typename Val, typename Interval> class IntervalTree
{
public:
    using Node = IntervalNode<Val, Interval>;
    using Key = typename Node::BaseType;

    using ENodeTraits = details::ExtendedNodeTraits<Val, Interval>;
    using YggRBTree =
        ygg::RBTree<Node,
                    ENodeTraits,
                    ygg::TreeOptions<ygg::TreeFlags::MULTIPLE,
                                     ygg::TreeFlags::CONSTANT_TIME_SIZE>,
                    int,
                    details::CompareInterval>;
    using HashSetIntervalNode = std::unordered_set<Node>;
    using IntrusiveSetNodeValInterval = boost::intrusive::set<
        Node,
        boost::intrusive::compare<details::CompareValInterval>,
        boost::intrusive::constant_time_size<true>>;

    /// Constructor
    IntervalTree();
    /// Copy constructor
    IntervalTree(const IntervalTree&);
    /// Copy assignment operator
    IntervalTree& operator=(const IntervalTree& other);
    /// Default move constructor
    IntervalTree(IntervalTree&& other) noexcept = default;
    /// Default move assignment operator
    IntervalTree& operator=(IntervalTree&& other) noexcept = default;

    bool verify_integrity() const;

    /// Add value and interval to search in multiple ways
    /// - ygg::RBTree (this): iterate on interval
    /// - boost::intrusive::set<Node,CompareValInterval>: interate on value /
    /// interval
    typename YggRBTree::template iterator<false> insert(Interval interval,
                                                        const Val& value);

    /// @return true if tree is empty
    bool is_empty() const;

    /// @erase specified value over interval
    void erase(const Interval& query_interval, const Val& value);

    /// @erase all values over interval
    void erase(const Interval& query_interval);

    /// Merges intervals from another tree into this
    IntervalTree& merged_with(const IntervalTree& other);

    /// Returns the asymmetrical difference with another set of
    /// implementation-dependent interval-values
    IntervalTree& subtract_by(const IntervalTree& other);

    /// @return coroutine enumerating all interval/values over @p query_interval
    cppcoro::generator<std::tuple<const Interval&, const Val&>>
    intervals(const Interval& query_interval) const;

    /// @return the gaps between intervals
    cppcoro::generator<Interval> gaps() const;

    /// @return the gaps between intervals and the values on either side
    cppcoro::generator<std::tuple<const std::vector<Val>&,
                                  const Interval&,
                                  const std::vector<Val>&>>
    sandwiched_gaps() const;

    /// @return coroutine enumerating all interval/values over @p query_interval
    cppcoro::generator<std::tuple<const Interval&, const std::set<Val>&>>
    disjoint_intervals(const Interval& query_interval) const;

    /// Returns the first disjoint interval (possibly containing multiple
    /// values)
    std::tuple<Interval, std::set<Val>> initial_values() const;

    /// @return last disjoint interval (possibly containing multiple values)
    std::tuple<Interval, std::set<Val>> final_values() const;

    /// @return all sorted unique values over the specified interval
    std::vector<Val> values(const Interval& query_interval) const;

    /// For range iteration
    auto begin()
    {
        return p_nodes_by_interval->begin();
    }
    /// For range iteration
    auto end()
    {
        return p_nodes_by_interval->end();
    }

    /// For range iteration of const
    auto begin() const
    {
        return p_nodes_by_interval->begin();
    }
    /// For range iteration of const
    auto end() const
    {
        return p_nodes_by_interval->end();
    }

    /// equality operator
    bool operator==(const IntervalTree& other) const;

    /// @cond Suppress_Doxygen_Warning
private:
    /// Error check
    bool verify_maxima(Node* n) const;

    /// Find matching nodes within an interval
    class QueryResult
    {
    public:
        class const_iterator
        {
        public:
            typedef ptrdiff_t difference_type;
            typedef Node value_type;
            typedef const Node& const_reference;
            typedef const Node* const_pointer;
            typedef std::input_iterator_tag iterator_category;

            const_iterator(Node* n, const Interval& q);
            const_iterator(const const_iterator& other);
            ~const_iterator() = default;

            const_iterator& operator=(const const_iterator& other);

            bool operator==(const const_iterator& other) const;
            bool operator!=(const const_iterator& other) const;

            const_iterator& operator++();
            const_iterator operator++(int);

            const_reference operator*() const;
            const_pointer operator->() const;

        private:
            Node* n;
            Interval q;
        };

        QueryResult(Node* n, const Interval& q);

        const_iterator begin() const;
        const_iterator end() const;

    private:
        Node* n;
        Interval q;
    };

    /**
     * @brief Queries intervals contained in the interval tree
     *
     * This method queries for intervals overlapping a query interval.
     *
     * The return value is a QueryResult that contains all intervals that
     * overlap the given query.
     *
     * @param q Anything that is comparable (i.e., has get_lower() and
     * get_upper() methods in NodeTraits) to an interval
     * @result A QueryResult holding all intervals in the tree that overlap q
     */
    QueryResult query(const Interval& q) const;

    /// Returns the first disjoint interval (possibly containing multiple
    /// values)
    void get_max_right_edge_nodes(Node& node,
                                  Interval& interval,
                                  std::set<Val>& values,
                                  Key max_right_edge) const;

    Node* insert_helper(Interval interval, const Val& value);

    /// @endcond
public:
    /// unordered_set: holds node and allows exact lookups
    HashSetIntervalNode nodes_exact_match;

    /// augmented Ygg::RBTree : interval
    std::unique_ptr<YggRBTree> p_nodes_by_interval;

    /// boost::intrusive::set: lookup by value then interval
    std::unique_ptr<IntrusiveSetNodeValInterval> p_nodes_by_val_interval;
};

/// @cond Suppress_Doxygen_Warning

namespace details
{

template <typename Val, typename Interval>
template <class BaseTree>
void ExtendedNodeTraits<Val, Interval>::leaf_inserted(
    ExtendedNodeTraits::Node& node, BaseTree& t)
{
    (void)t;

    node.max_right_edge = upper_edge(node.interval);

    // Propagate up
    Node* cur = node.get_parent();
    while ((cur != nullptr) && (cur->max_right_edge < node.max_right_edge))
    {
        cur->max_right_edge = node.max_right_edge;
        cur = cur->get_parent();
    }
}

template <typename Val, typename Interval>
void ExtendedNodeTraits<Val, Interval>::fix_node(ExtendedNodeTraits::Node& node)
{
    auto old_val = node.max_right_edge;
    node.max_right_edge = details::upper_edge(node.interval);

    if (node.get_left() != nullptr)
    {
        node.max_right_edge =
            std::max(node.max_right_edge, node.get_left()->max_right_edge);
    }

    if (node.get_right() != nullptr)
    {
        node.max_right_edge =
            std::max(node.max_right_edge, node.get_right()->max_right_edge);
    }

    if (old_val != node.max_right_edge)
    {
        // propagate up
        Node* cur = node.get_parent();
        if (cur != nullptr)
        {
            if ((cur->max_right_edge < node.max_right_edge) ||
                (cur->max_right_edge == old_val))
            {
                fix_node(*cur);
            }
        }
    }
}

template <typename Val, typename Interval>
template <class BaseTree>
void ExtendedNodeTraits<Val, Interval>::rotated_left(
    ExtendedNodeTraits::Node& node, BaseTree& t)
{
    (void)t;

    // 'node' is the node that was the old parent.
    fix_node(node);
    fix_node(*(node.get_parent()));
}

template <typename Val, typename Interval>
template <class BaseTree>
void ExtendedNodeTraits<Val, Interval>::rotated_right(
    ExtendedNodeTraits::Node& node, BaseTree& t)
{
    (void)t;
    // 'node' is the node that was the old parent.
    fix_node(node);
    fix_node(*(node.get_parent()));
}

template <typename Val, typename Interval>
template <class BaseTree>
void ExtendedNodeTraits<Val, Interval>::deleted_below(
    ExtendedNodeTraits::Node& node, BaseTree& t)
{
    (void)t;
    fix_node(node);
}

template <typename Val, typename Interval>
template <class BaseTree>
void ExtendedNodeTraits<Val, Interval>::swapped(ExtendedNodeTraits::Node& n1,
                                                ExtendedNodeTraits::Node& n2,
                                                BaseTree& t)
{
    (void)t;

    fix_node(n1);
    if (n1.get_parent() != nullptr)
    {
        fix_node(*(n1.get_parent()));
    }

    fix_node(n2);
    if (n2.get_parent() != nullptr)
    {
        fix_node(*(n2.get_parent()));
    }
}

} // namespace details

template <typename Val, typename Interval>
bool IntervalTree<Val, Interval>::operator==(const IntervalTree& other) const
{
    return *p_nodes_by_val_interval == *other.p_nodes_by_val_interval;
}

template <typename Val, typename Interval>
IntervalTree<Val, Interval>::IntervalTree()
    : p_nodes_by_interval(std::make_unique<YggRBTree>()),
      p_nodes_by_val_interval(std::make_unique<IntrusiveSetNodeValInterval>())
{
}

template <typename Val, typename Interval>
IntervalTree<Val, Interval>::IntervalTree(const IntervalTree& other)
    : p_nodes_by_interval(std::make_unique<YggRBTree>()),
      p_nodes_by_val_interval(std::make_unique<IntrusiveSetNodeValInterval>())

{
    for (const auto& node : other)
    {
        insert_helper(node.interval, node.value);
    }
}

template <typename Val, typename Interval>
IntervalTree<Val, Interval>&
IntervalTree<Val, Interval>::operator=(const IntervalTree& other)
{
    p_nodes_by_interval->clear();
    p_nodes_by_val_interval->clear();
    nodes_exact_match.clear();
    for (const auto& node : other)
    {
        insert_helper(node.interval, node.value);
    }
    return *this;
}

template <typename Val, typename Interval>
IntervalTree<Val, Interval>& IntervalTree<Val, Interval>::merged_with(
    const IntervalTree<Val, Interval>& other)
{
    for (const auto& node : other)
    {
        insert(node.interval, node.value);
    }
    return *this;
}

template <typename Val, typename Interval>
IntervalTree<Val, Interval>& IntervalTree<Val, Interval>::subtract_by(
    const IntervalTree<Val, Interval>& other)
{
    for (const auto& node : other)
    {
        erase(node.interval, node.value);
    }
    return *this;
}

template <typename Val, typename Interval>
cppcoro::generator<std::tuple<const Interval&, const Val&>>
IntervalTree<Val, Interval>::intervals(const Interval& interval) const
{
    for (const auto& node : query(interval))
    {
        co_yield std::tuple{node.interval & interval, node.value};
    }
}

template <typename Val, typename Interval>
std::vector<Val>
IntervalTree<Val, Interval>::values(const Interval& interval) const
{
    std::set<Val> results;
    for (const auto& node : query(interval))
    {
        results.insert(node.value);
    }
    return {results.begin(), results.end()};
}

template <typename Val, typename Interval>
cppcoro::generator<Interval> IntervalTree<Val, Interval>::gaps() const
{
    Interval current_interval;
    for (const auto& node : *this)
    {
        if (boost::icl::is_empty(current_interval))
        {
            current_interval = node.interval;
            continue;
        }

        // extend gapless interval
        if (boost::icl::intersects(current_interval, node.interval))
        {
            current_interval =
                boost::icl::hull(current_interval, node.interval);
            continue;
        }

        // Make sure gap is not empty: intervals on either side may touch
        auto gap_interval =
            boost::icl::inner_complement(current_interval, node.interval);
        if (!boost::icl::is_empty(gap_interval))
        {
            co_yield gap_interval;
        }

        current_interval = node.interval;
    }
}

namespace details
{
template <typename Interval>
bool gap_between(const Interval& a, const Interval& b)
{
    return !boost::icl::is_empty(boost::icl::inner_complement(a, b));
}

template <typename Val, typename Interval>
std::tuple<const std::vector<Val>&, const Interval&, const std::vector<Val>&>
sandwiched_gap(const std::set<details::EdgeNode<Val, Interval>>& left_edges,
               const std::set<details::EdgeNode<Val, Interval>,
                              details::Greater>& right_edges,
               std::set<Val>& values_before_set,
               std::set<Val>& values_after_set,
               std::vector<Val>& values_before,
               Interval& gap_interval,
               std::vector<Val>& values_after)
{
    values_before_set.clear();
    values_after_set.clear();
    Interval interval_before = right_edges.begin()->pnode->interval;
    Interval interval_after = left_edges.begin()->pnode->interval;
    for (const auto& edge : left_edges)
    {
        values_after_set.insert(edge.pnode->value);
        interval_after = interval_after & edge.pnode->interval;
    }

    for (const auto& edge : right_edges)
    {
        values_before_set.insert(edge.pnode->value);
        interval_before = interval_before & edge.pnode->interval;
    }
    gap_interval =
        boost::icl::inner_complement(interval_before, interval_after);
    assert(!boost::icl::is_empty(gap_interval));
    values_before.assign(values_before_set.begin(), values_before_set.end());
    values_after.assign(values_after_set.begin(), values_after_set.end());

    return {values_before, gap_interval, values_after};
}

} // namespace details
template <typename Val, typename Interval>
cppcoro::generator<std::tuple<const std::vector<Val>&,
                              const Interval&,
                              const std::vector<Val>&>>
IntervalTree<Val, Interval>::sandwiched_gaps() const
{
    // right edges of intervals that are in play
    std::set<details::EdgeNode<Val, Interval>> left_edges;
    std::set<details::EdgeNode<Val, Interval>, details::Greater>
        right_edges;
    std::vector<Val> values_before;
    std::vector<Val> values_after;
    std::set<Val> values_before_set;
    std::set<Val> values_after_set;
    Interval gap_interval;

    for (const auto& node : *this)
    {
        assert(!boost::icl::is_empty(node.interval));
        const auto node_left_edge = details::lower_edge(node.interval);
        const auto node_right_edge = details::upper_edge(node.interval);

        if (!left_edges.empty())
        {
            assert(!right_edges.empty());
            // same left edge: store
            if (left_edges.begin()->edge == node_left_edge)
            {
                left_edges.insert({&node, node_left_edge});
                continue;
            }

            // different left edge: process
            co_yield details::sandwiched_gap(left_edges,
                                             right_edges,
                                             values_before_set,
                                             values_after_set,
                                             values_before,
                                             gap_interval,
                                             values_after);

            for (const auto& edge_node : left_edges)
            {
                const auto right_edge =
                    details::lower_edge(edge_node.pnode->interval);
                if (right_edge > right_edges.begin()->edge)
                {
                    right_edges.clear();
                }
                right_edges.insert({edge_node.pnode, right_edge});
            }
            left_edges.clear();
        }

        // process current edge: it is either a left edge or a right edge
        if (right_edges.size() &&
            details::gap_between(right_edges.begin()->pnode->interval,
                                 node.interval))
        {
            left_edges.insert({&node, node_left_edge});
            continue;
        }

        if (node_right_edge > right_edges.begin()->edge)
        {
            right_edges.clear();
        }
        right_edges.insert({&node, node_right_edge});
    }

    if (!left_edges.empty())
    {
        assert(!right_edges.empty());
        co_yield details::sandwiched_gap(left_edges,
                                         right_edges,
                                         values_before_set,
                                         values_after_set,
                                         values_before,
                                         gap_interval,
                                         values_after);
    }
}

template <typename Val, typename Interval>
std::tuple<Interval, std::set<Val>>
IntervalTree<Val, Interval>::final_values() const
{
    if (!p_nodes_by_interval->get_root() || is_empty())
    {
        return {};
    }

    Interval interval;
    std::set<Val> values;
    get_max_right_edge_nodes(*p_nodes_by_interval->get_root(),
                             interval,
                             values,
                             p_nodes_by_interval->get_root()->max_right_edge);
    return {interval, values};
}

template <typename Val, typename Interval>
void IntervalTree<Val, Interval>::get_max_right_edge_nodes(
    IntervalTree::Node& node,
    Interval& interval,
    std::set<Val>& values,
    IntervalTree::Key max_right_edge) const
{
    assert(node.max_right_edge == max_right_edge);

    if (details::upper_edge(node.interval) == max_right_edge)
    {
        if (values.empty())
        {
            interval = node.interval;
        }
        else
        {
            interval = interval & node.interval;
        }
        values.insert(node.value);
    }

    if (node.get_left() != nullptr &&
        node.get_left()->max_right_edge == max_right_edge)
    {
        get_max_right_edge_nodes(
            *node.get_left(), interval, values, max_right_edge);
    }
    if (node.get_right() != nullptr &&
        node.get_right()->max_right_edge == max_right_edge)
    {
        get_max_right_edge_nodes(
            *node.get_right(), interval, values, max_right_edge);
    }
}

template <typename Val, typename Interval>
std::tuple<Interval, std::set<Val>>
IntervalTree<Val, Interval>::initial_values() const
{
    if (!p_nodes_by_interval->get_root() || is_empty())
    {
        return {};
    }

    Interval interval = begin()->interval;
    auto lower_edge = details::lower_edge(interval);
    std::set<Val> values;
    for (const auto& node : *this)
    {
        // get all values / intersecting intervals for all nodes
        // that touch the lower edge
        if (details::lower_edge(node.interval) == lower_edge)
        {
            interval = interval & node.interval;
            values.insert(node.value);
        }
        else
        {
            break;
        }
    }
    assert(!boost::icl::is_empty(interval));
    assert(values.size());
    return {interval, values};
}

template <typename Val, typename Interval>
cppcoro::generator<std::tuple<const Interval&, const std::set<Val>&>>
IntervalTree<Val, Interval>::disjoint_intervals(
    const Interval& query_interval) const
{
    // right edges of intervals that are in play
    std::set<details::EdgeNode<Val, Interval>> right_edges;
    std::set<Val> values;
    Interval total_interval;
    const IntervalNode<Val, Interval>* prev_node = nullptr;
    for (const auto& node : query(query_interval))
    {
        assert(!boost::icl::is_empty(node.interval));
        const auto node_left_edge = details::lower_edge(node.interval);
        const auto prev_node_edge =
            prev_node == nullptr ? node_left_edge
                                 : details::lower_edge(prev_node->interval);
        prev_node = &node;

        // new or node overlapping on the left edge:
        // push and handle together
        if (node_left_edge == prev_node_edge)
        {
            values.insert(node.value);
            right_edges.insert({&node, details::upper_edge(node.interval)});
            total_interval = boost::icl::hull(total_interval, node.interval);
            continue;
        }

        // Handle all intervals whose right edge before the node's left
        while (right_edges.size() &&
               right_edges.begin()->edge <= node_left_edge)
        {
            auto right_edge_node = right_edges.begin()->pnode;

            // Disjoint interval is the intersection between all the intervals
            // in flight and the node that will form the right edge
            // For dynamic intervals, the right edge will have correct type
            // Ignore empty intervals, including where multiple intervals
            // have the same right edge
            auto interval = total_interval & right_edge_node->interval;
            if (!boost::icl::is_empty(interval))
            {
                co_yield std::tuple{interval & query_interval, values};
            }
            values.erase(right_edge_node->value);
            right_edges.erase(right_edges.begin());
            total_interval = boost::icl::left_subtract(
                total_interval, right_edge_node->interval);
        }

        // Interval from previous right edge to the left edge of the new node
        if (!values.empty())
        {
            auto interval =
                boost::icl::right_subtract(total_interval, node.interval);
            if (!boost::icl::is_empty(interval))
            {
                co_yield std::tuple{interval & query_interval, values};
                // total_interval now starts off with new open
                total_interval = boost::icl::hull(
                    node.interval,
                    boost::icl::left_subtract(total_interval, node.interval));
            }
        }
        else
        {
            total_interval = node.interval;
        }

        values.insert(node.value);
        right_edges.insert({&node, details::upper_edge(node.interval)});
        total_interval = boost::icl::hull(total_interval, node.interval);
    }
    while (right_edges.size())
    {
        auto right_edge_node = right_edges.begin()->pnode;
        auto interval = total_interval & right_edge_node->interval;
        if (!boost::icl::is_empty(interval))
        {
            co_yield std::tuple{interval & query_interval, values};
        }
        assert(values.count(right_edge_node->value));
        values.erase(right_edge_node->value);
        total_interval = boost::icl::left_subtract(total_interval,
                                                   right_edge_node->interval);
        right_edges.erase(right_edges.begin());
    }
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::Node*
IntervalTree<Val, Interval>::insert_helper(Interval interval, const Val& value)
{
    if (boost::icl::is_empty(interval))
    {
        return nullptr;
    }
    // Add new interval including overlapping
    Node& node =
        const_cast<Node&>(*nodes_exact_match.insert({value, interval}).first);
    p_nodes_by_val_interval->insert(node);
    p_nodes_by_interval->insert(node);
    return &node;
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::YggRBTree::template iterator<false>
IntervalTree<Val, Interval>::insert(Interval interval, const Val& value)
{
    // ignore empty intervals
    if (boost::icl::is_empty(interval))
    {
        return end();
    }
    ValIntervalRef query{value, interval};

    // Efficiently check unordered_set for exact match in c++20
    // auto pexactnode = nodes_exact_match.find(query);
    // if (pexactnode != nodes_exact_match.end())
    // {
    //     return &*pexactnode;
    // }

    // find all nodes that either touch or overlap interval to remove before
    // inserting the total spanning hull
    auto pnode = p_nodes_by_val_interval->lower_bound(
        query, details::CompareValIntervalTouches());
    auto end = p_nodes_by_val_interval->upper_bound(
        query, details::CompareValIntervalTouches());

    // If first contains interval, skip: nothing will change on insert
    if (pnode != end && boost::icl::contains(pnode->interval, interval))
    {
        return &*pnode;
    }

    // Remove overlapping or even touching
    auto total_interval = interval;
    while (pnode != end)
    {
        total_interval = boost::icl::hull(pnode->interval, total_interval);
        Node& node = const_cast<Node&>(*pnode);
        ++pnode;
        p_nodes_by_interval->remove(node);
        p_nodes_by_val_interval->erase(node);
        nodes_exact_match.erase(node);
    }

    // Add new interval including overlapping/touching
    assert(!boost::icl::is_empty(total_interval));
    return insert_helper(total_interval, value);
}

template <typename Val, typename Interval>
void IntervalTree<Val, Interval>::erase(const Interval& interval,
                                        const Val& value)
{
    ValIntervalRef query{value, interval};

    // Efficiently check unordered_set for exact match in c++20
    // auto pexactnode = nodes_exact_match.find(query);
    // if (pexactnode != nodes_exact_match.end())
    // {
    //     Node& node = const_cast<Node&>(*pexactnode);
    //     p_nodes_by_interval->remove(node);
    //     p_nodes_by_val_interval->erase(node);
    //     nodes_exact_match.erase(pexactnode);
    //     return;
    // }

    // find all nodes that overlap interval
    // Note that p_nodes_by_val_interval contains unique non-overlapping
    // node/intervals
    auto total_interval = interval;
    auto pnode = p_nodes_by_val_interval->lower_bound(
        query, details::CompareValIntervalOverlap());
    auto end = p_nodes_by_val_interval->upper_bound(
        query, details::CompareValIntervalOverlap());
    while (pnode != end)
    {
        total_interval = boost::icl::hull(pnode->interval, total_interval);
        Node& node = const_cast<Node&>(*pnode);
        ++pnode;
        p_nodes_by_interval->remove(node);
        p_nodes_by_val_interval->erase(node);
        nodes_exact_match.erase(node);
    }

    if (boost::icl::lower(total_interval) < boost::icl::lower(interval))
    {
        insert_helper(boost::icl::right_subtract(total_interval, interval),
                      value);
    }
    if (boost::icl::upper(total_interval) > boost::icl::upper(interval))
    {
        insert_helper(boost::icl::left_subtract(total_interval, interval),
                      value);
    }
}

template <typename Val, typename Interval>
bool IntervalTree<Val, Interval>::is_empty() const
{
    return p_nodes_by_val_interval->size() == 0;
}

template <typename Val, typename Interval>
void IntervalTree<Val, Interval>::erase(const Interval& interval)
{
    auto overlapping_nodes = query(interval);
    auto pnode = overlapping_nodes.begin();
    auto end = overlapping_nodes.end();
    while (pnode != end)
    {
        auto deleted_interval = pnode->interval;
        auto value = std::move(pnode->value);
        Node& node = const_cast<Node&>(*pnode);
        ++pnode;
        p_nodes_by_interval->remove(node);
        p_nodes_by_val_interval->erase(node);
        nodes_exact_match.erase(node);

        if (boost::icl::lower(deleted_interval) < boost::icl::lower(interval))
        {
            insert_helper(
                boost::icl::right_subtract(deleted_interval, interval), value);
        }
        if (boost::icl::upper(deleted_interval) > boost::icl::upper(interval))
        {
            insert_helper(boost::icl::left_subtract(deleted_interval, interval),
                          value);
        }
    }
}

template <typename Val, typename Interval>
bool IntervalTree<Val, Interval>::verify_integrity() const
{
    bool base_verification = p_nodes_by_interval->verify_integrity();
    assert(base_verification);
    bool maxima_valid = p_nodes_by_interval->get_root() == nullptr
                            ? true
                            : p_nodes_by_interval->verify_maxima(
                                  p_nodes_by_interval->get_root());
    assert(maxima_valid);

    return base_verification && maxima_valid;
}

template <typename Val, typename Interval>
bool IntervalTree<Val, Interval>::verify_maxima(IntervalTree::Node* n) const
{
    bool valid = true;
    auto maximum = details::upper_edge(n->interval);

    if (n->get_right() != nullptr)
    {
        maximum = std::max(maximum, n->get_right()->max_right_edge);
        valid &= p_nodes_by_interval->verify_maxima(n->get_right());
    }
    if (n->get_left() != nullptr)
    {
        maximum = std::max(maximum, n->get_left()->max_right_edge);
        valid &= p_nodes_by_interval->verify_maxima(n->get_left());
    }

    valid &= (maximum == n->max_right_edge);

    return valid;
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::QueryResult
IntervalTree<Val, Interval>::query(const Interval& q) const
{
    IntervalNode<Val, Interval>* cur = p_nodes_by_interval->get_root();
    if (p_nodes_by_interval->get_root() == nullptr)
    {
        return QueryResult(nullptr, q);
    }

    while ((cur->get_left() != nullptr) &&
           !details::exclusive_less(cur->get_left()->max_right_edge, q))
    {
        cur = cur->get_left();
    }

    IntervalNode<Val, Interval>* hit;
    // If this overlaps, this is our first hit. otherwise, find the next one
    if (boost::icl::intersects(q, cur->interval))
    {
        hit = cur;
    }
    else
    {
        hit = details::find_next_overlapping<Val, Interval>(cur, q);
    }

    return QueryResult(hit, q);
}

namespace details
{

template <typename Val, typename Interval>
IntervalNode<Val, Interval>*
find_next_overlapping(IntervalNode<Val, Interval>* cur, const Interval& q)
{
    // We search for the next bigger node, pruning the search as necessary. When
    // Pruning occurs, we need to restart the search for the next larger node.
    do
    {
        // We make sure that at the start of the loop, the lower of cur is
        // smaller than the upper of q. Thus, we need to only check the upper to
        // check for overlap.
        if (cur->get_right() != nullptr)
        {
            // go to smallest larger-or-equal child
            cur = cur->get_right();
            if (details::exclusive_less(cur->max_right_edge, get_interval(q)))
            {
                // Prune!
                // Nothing starting from this node can overlap b/c of upper
                // limit. Backtrack.
                while ((cur->get_parent() != nullptr) &&
                       (cur->get_parent()->get_right() == cur))
                { // these are the nodes which are smaller and were
                    // already visited
                    cur = cur->get_parent();
                }

                // go one further up
                if (cur->get_parent() == nullptr)
                {
                    // Backtracked out of the root
                    return nullptr;
                }
                else
                {
                    // go up
                    cur = cur->get_parent();
                }
            }
            else
            {
                while (cur->get_left() != nullptr)
                {
                    cur = cur->get_left();
                    if (details::exclusive_less(cur->max_right_edge,
                                                get_interval(q)))
                    {
                        // Prune!
                        // Nothing starting from this node can overlap.
                        // Backtrack.
                        cur = cur->get_parent();
                        break;
                    }
                }
            }
        }
        else
        {
            // go up
            // skip over the nodes already visited
            while ((cur->get_parent() != nullptr) &&
                   (cur->get_parent()->get_right() == cur))
            { // these are the nodes which are smaller and were already
                // visited
                cur = cur->get_parent();
            }

            // go one further up
            if (cur->get_parent() == nullptr)
            {
                // Backtracked into the root
                return nullptr;
            }
            else
            {
                // go up
                cur = cur->get_parent();
            }
        }

        if (boost::icl::exclusive_less(get_interval(q), cur->interval))
        {
            // No larger node can be an overlap!
            return nullptr;
        }

        if (!boost::icl::exclusive_less(cur->interval, get_interval(q)))
        {
            // Found!
            return cur;
        }

    } while (true);
}

} // namespace details

template <typename Val, typename Interval>
IntervalTree<Val, Interval>::QueryResult::QueryResult(IntervalTree::Node* n_in,
                                                      const Interval& q_in)
    : n(n_in), q(q_in)
{
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::QueryResult::const_iterator
IntervalTree<Val, Interval>::QueryResult::begin() const
{
    return const_iterator(this->n, this->q);
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::QueryResult::const_iterator
IntervalTree<Val, Interval>::QueryResult::end() const
{
    return const_iterator(nullptr, this->q);
}

template <typename Val, typename Interval>
IntervalTree<Val, Interval>::QueryResult::const_iterator::const_iterator(
    IntervalTree::Node* n_in, const Interval& q_in)
    : n(n_in), q(q_in)
{
}

template <typename Val, typename Interval>
IntervalTree<Val, Interval>::QueryResult::const_iterator::const_iterator(
    const typename IntervalTree<Val, Interval>::QueryResult::const_iterator&
        other)
    : n(other.n), q(other.q)
{
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::QueryResult::const_iterator&
IntervalTree<Val, Interval>::QueryResult::const_iterator::operator=(
    const typename IntervalTree<Val, Interval>::QueryResult::const_iterator&
        other)
{
    this->n = other.n;
    this->q = other.q;
    return *this;
}

template <typename Val, typename Interval>
bool IntervalTree<Val, Interval>::QueryResult::const_iterator::operator==(
    const typename IntervalTree<Val, Interval>::QueryResult::const_iterator&
        other) const
{
    // Same node same query
    return this->n == other.n && this->q == other.q;
}

template <typename Val, typename Interval>
bool IntervalTree<Val, Interval>::QueryResult::const_iterator::operator!=(
    const typename IntervalTree<Val, Interval>::QueryResult::const_iterator&
        other) const
{
    return !(*this == other);
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::QueryResult::const_iterator&
IntervalTree<Val, Interval>::QueryResult::const_iterator::operator++()
{
    this->n = details::find_next_overlapping<Val, Interval>(this->n, this->q);

    return *this;
}

template <typename Val, typename Interval>
typename IntervalTree<Val, Interval>::QueryResult ::const_iterator
IntervalTree<Val, Interval>::QueryResult ::const_iterator::operator++(int)
{
    typename IntervalTree<Val, Interval>::QueryResult ::const_iterator cpy(
        *this);

    this->operator++();

    return cpy;
}

template <typename Val, typename Interval>
const typename IntervalTree<Val, Interval>::Node&
IntervalTree<Val, Interval>::QueryResult ::const_iterator::operator*() const
{
    return *(this->n);
}

template <typename Val, typename Interval>
const typename IntervalTree<Val, Interval>::Node*
IntervalTree<Val, Interval>::QueryResult ::const_iterator::operator->() const
{
    return this->n;
}

} // namespace interval_dict

/// @endcond

#endif