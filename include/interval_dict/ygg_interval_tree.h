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
/// \file ygg_interval_tree_adaptor.h
/// \brief Definitions of functions to implement IntervalDict with
/// tinloaf/ygg
///
/// This stores values as non-disjoint intervals to be used to
/// implement interval associative dictionaries
///
//// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_YGG_INTERVAL_TREE_H
#define INCLUDE_INTERVAL_DICT_YGG_INTERVAL_TREE_H

#include <src/rbtree.hpp>
#include <src/ygg.hpp>

#include <boost/icl/interval_traits.hpp>

#include <algorithm>
#include <boost/icl/concept/interval.hpp>
#include <boost/icl/type_traits/is_asymmetric_interval.hpp>
#include <boost/icl/type_traits/is_continuous_interval.hpp>
#include <boost/icl/type_traits/is_discrete_interval.hpp>
#include <iostream>
#include <string>

namespace ygg
{

namespace details
{

template <class BaseType, class IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_asymmetric_interval<IntervalType>::value, bool>
    exclusive_less(const BaseType& left_upper, const IntervalType& right)
{
    return boost::icl::domain_less_equal<IntervalType>(left_upper,
                                                       lower(right));
}

template <class BaseType, class IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_discrete_interval<IntervalType>::value, bool>
    exclusive_less(const BaseType& left_upper, const IntervalType& right)
{
    return boost::icl::domain_less<IntervalType>(left_upper, first(right));
}

template <class BaseType, class IntervalType>
inline typename std::
    enable_if_t<boost::icl::has_symmetric_bounds<IntervalType>::value, bool>
    exclusive_less(const BaseType& left_upper, const IntervalType& right)
{
    return boost::icl::domain_less<IntervalType>(left_upper, first(right));
}

template <class BaseType, class IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_continuous_interval<IntervalType>::value, bool>
    exclusive_less(const BaseType& left_upper, const IntervalType& right)
{
    return boost::icl::domain_less<IntervalType>(left_upper, lower(right));
}

template <class IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_asymmetric_interval<IntervalType>::value, bool>
    upper_bound(const IntervalType& interval)
{
    return upper(interval);
}

template <class IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_discrete_interval<IntervalType>::value, bool>
    upper_bound(const IntervalType& interval)
{
    return last(interval);
}

template <class IntervalType>
inline typename std::
    enable_if_t<boost::icl::has_symmetric_bounds<IntervalType>::value, bool>
    upper_bound(const IntervalType& interval)
{
    return last(interval);
}

template <class IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_continuous_interval<IntervalType>::value, bool>
    upper_bound(const IntervalType& interval)
{
    return upper(interval);
}

} // namespace details

template <typename Interval, typename Val>
class IntervalNode
    : public RBTreeNodeBase< // typename IntervalNode<Interval, Val>,
          Interval,
          TreeOptions<TreeFlags::MULTIPLE>>
{
public:
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;
    using IntervalType = Interval;
    using ValueType = Val;
    typename BaseType _it_max_upper;
    Interval interval;
    Val value;

    bool operator<(const IntervalNode& rhs) const
    {
        return std::tie(interval, value) < std::tie(rhs.interval, rhs.value);
    }

    bool operator==(const IntervalNode& rhs) const
    {
        return std::tie(_it_max_upper, interval, value) ==
               std::tie(rhs._it_max_upper, rhs.interval, rhs.value);
    }
};

namespace intervaltree_internal
{
template <typename Node,
          typename INB,
          typename NodeTraits,
          bool skipfirst,
          typename Comparable>
Node* find_next_overlapping(Node* cur, const Comparable& q);

template <class KeyType> class DummyRange : public std::pair<KeyType, KeyType>
{
public:
    DummyRange(KeyType lower, KeyType upper);
};

template <typename Interval, typename Val> class IntervalCompare
{
public:
    bool operator()(const IntervalNode<Interval, Val>& lhs,
                    const IntervalNode<Interval, Val>& rhs) const
    {
        return lhs.interval < rhs.interval;
    }
};

// TODO add a possibility for bulk updates
template <typename Interval, typename Val>
class ExtendedNodeTraits : public NodeTraits
{
public:
    using Node = IntervalNode<Interval, Val>;
    // TODO these can probably be made more efficient
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

    // Make our DummyRange comparable
    static typename NodeTraits::key_type get_lower(
        const intervaltree_internal::DummyRange<typename NodeTraits::key_type>&
            range);
    static typename NodeTraits::key_type get_upper(
        const intervaltree_internal::DummyRange<typename NodeTraits::key_type>&
            range);
};
} // namespace intervaltree_internal

/**
 * @brief Abstract base class for the Node Traits that need to be implemented
 *
 * Every Interval Tree needs to be supplied with a node traits class that must
 * be derived from this class. In your derived class, you must define the
 * key_type as the type of your interval's bounds, and you must implement
 * get_lower() and get_upper() to return the interval bounds of your nodes.
 */
template <typename Interval, typename Val> class ITreeNodeTraits
{
public:
    /**
     * @brief The type of your interval bounds. This is the type that
     * get_lower() and get_upper() must return. This type must be comparable,
     * i.e., operator< etc. must be implemented.
     */
    using key_type = IntervalNode<Interval, Val>::BaseType;

    /**
     * Must be implemented to return the lower bound of the interval represented
     * by n.
     *
     * @param n The node whose lower interval bound should be returned.
     * @return Must return the lower interval bound of n
     */
    static key_type get_lower(const typename IntervalNode<Interval, Val>& n)
    {
        return boost::icl::lower(n.interval);
    }

    /**
     * Must be implemented to return the upper bound of the interval represented
     * by n.
     *
     * @param n The node whose upper interval bound should be returned.
     * @return Must return the upper interval bound of n
     */
    static key_type get_upper(const typename IntervalNode<Interval, Val>& n)
    {
        return boost::icl::upper(n.interval);
    }
};

/**
 * @brief Stores an Interval Tree
 *
 * This class stores an interval tree on the nodes it contains. It is
 * implemented via the 'augmented red-black tree' described by Cormen et al.
 *
 * @tparam Node 				The node class for this Interval Tree. Must
 * be derived from ITreeNodeBase.
 * @tparam NodeTraits 	The node traits for this Interval Tree. Must be derived
 * from
 * @tparam Options			Passed through to RBTree. See there for
 * documentation.
 * @tparam Tag					Used to add nodes to multiple interval
 * trees. See RBTree documentation for details.
 */
template <typename Interval, typename Val>
class IntervalTree
    : private RBTree<
          IntervalNode<Interval, Val>,
          intervaltree_internal::ExtendedNodeTraits<Interval, Val>,
          TreeOptions<TreeFlags::MULTIPLE, TreeFlags::CONSTANT_TIME_SIZE>,
          int,
          intervaltree_internal::IntervalCompare<Interval, Val>>
{
public:
    using Node = IntervalNode<Interval, Val>;
    using Key = typename NodeTraits::key_type;
    using MyClass = IntervalTree<Interval, Val, Options, Tag>;
    using NodeTraits = ITreeNodeTraits<Interval, Val>;

    using ENodeTraits =
        intervaltree_internal::ExtendedNodeTraits<Interval, Val>;
    using BaseTree =
        RBTree<Node,
               ENodeTraits,
               TreeOptions<TreeFlags::MULTIPLE, TreeFlags::CONSTANT_TIME_SIZE>,
               int,
               intervaltree_internal::IntervalCompare<Interval, Val>>;

    IntervalTree();

    bool verify_integrity() const;
    void dump_to_dot(const std::string& filename) const;

    /* Import some of RBTree's methods into the public namespace */
    using BaseTree::empty;
    using BaseTree::insert;
    using BaseTree::remove;

    // Iteration of sets of intervals
    template <class Comparable> class QueryResult
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

            const_iterator(Node* n, const Comparable& q);
            const_iterator(const const_iterator& other);
            ~const_iterator();

            const_iterator& operator=(const const_iterator& other);

            bool operator==(const const_iterator& other) const;
            bool operator!=(const const_iterator& other) const;

            const_iterator& operator++();
            const_iterator operator++(int);

            const_reference operator*() const;
            const_pointer operator->() const;

        private:
            Node* n;
            Comparable q;
        };

        QueryResult(Node* n, const Comparable& q);

        const_iterator begin() const;
        const_iterator end() const;

    private:
        Node* n;
        Comparable q;
    };

    /**
     * @brief Queries intervals contained in the interval tree
     *
     * This method queries for intervals overlapping a query interval.
     * The query parameter q can be anything that is comparable to an interval.
     * A class <Comparable> is comparable to an interval if the NodeTraits
     * implement a get_lower(const Comparable &) and get_upper(const Comparable
     * &) method.
     *
     * The return value is a QueryResult that contains all intervals that
     * overlap the given query.
     *
     * @param q Anything that is comparable (i.e., has get_lower() and
     * get_upper() methods in NodeTraits) to an interval
     * @result A QueryResult holding all intervals in the tree that overlap q
     */
    template <class Comparable>
    QueryResult<Comparable> query(const Comparable& q) const;

    template <class Comparable>
    typename BaseTree::template const_iterator<false>
    interval_upper_bound(const Comparable& query_range) const;

    // TODO FIXME this is actually very specific?
    void fixup_maxima(Node& lowest);

    // Iterating the events should still be possible
    using BaseTree::begin;
    using BaseTree::end;

private:
    bool verify_maxima(Node* n) const;
};

namespace intervaltree_internal
{

template <class Node, class INB, class NodeTraits>
template <class BaseTree>
void ExtendedNodeTraits<Node, INB, NodeTraits>::leaf_inserted(Node& node,
                                                              BaseTree& t)
{
    (void)t;

    node.INB::_it_max_upper = NodeTraits::get_upper(node);

    // Propagate up
    Node* cur = node.get_parent();
    while ((cur != nullptr) &&
           (cur->INB::_it_max_upper < node.INB::_it_max_upper))
    {
        cur->INB::_it_max_upper = node.INB::_it_max_upper;
        cur = cur->get_parent();
    }
}

template <class Node, class INB, class NodeTraits>
void ExtendedNodeTraits<Node, INB, NodeTraits>::fix_node(Node& node)
{
    auto old_val = node.INB::_it_max_upper;
    node.INB::_it_max_upper = NodeTraits::get_upper(node);

    if (node.get_left() != nullptr)
    {
        node.INB::_it_max_upper = std::max(node.INB::_it_max_upper,
                                           node.get_left()->INB::_it_max_upper);
    }

    if (node.get_right() != nullptr)
    {
        node.INB::_it_max_upper = std::max(
            node.INB::_it_max_upper, node.get_right()->INB::_it_max_upper);
    }

    if (old_val != node.INB::_it_max_upper)
    {
        // propagate up
        Node* cur = node.get_parent();
        if (cur != nullptr)
        {
            if ((cur->INB::_it_max_upper < node.INB::_it_max_upper) ||
                (cur->INB::_it_max_upper == old_val))
            {
                fix_node(*cur);
            }
        }
    }
}

template <class Node, class INB, class NodeTraits>
template <class BaseTree>
void ExtendedNodeTraits<Node, INB, NodeTraits>::rotated_left(Node& node,
                                                             BaseTree& t)
{
    (void)t;

    // 'node' is the node that was the old parent.
    fix_node(node);
    fix_node(*(node.get_parent()));
}

template <class Node, class INB, class NodeTraits>
template <class BaseTree>
void ExtendedNodeTraits<Node, INB, NodeTraits>::rotated_right(Node& node,
                                                              BaseTree& t)
{
    (void)t;

    // 'node' is the node that was the old parent.
    fix_node(node);
    fix_node(*(node.get_parent()));
}

template <class Node, class INB, class NodeTraits>
template <class BaseTree>
void ExtendedNodeTraits<Node, INB, NodeTraits>::deleted_below(Node& node,
                                                              BaseTree& t)
{
    (void)t;

    fix_node(node);
}

template <class Node, class INB, class NodeTraits>
template <class BaseTree>
void ExtendedNodeTraits<Node, INB, NodeTraits>::swapped(Node& n1,
                                                        Node& n2,
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

template <class Node, class INB, class NodeTraits>
typename NodeTraits::key_type
ExtendedNodeTraits<Node, INB, NodeTraits>::get_lower(
    const intervaltree_internal::DummyRange<typename NodeTraits::key_type>&
        range)
{
    return std::get<0>(range);
}

template <class Node, class INB, class NodeTraits>
typename NodeTraits::key_type
ExtendedNodeTraits<Node, INB, NodeTraits>::get_upper(
    const intervaltree_internal::DummyRange<typename NodeTraits::key_type>&
        range)
{
    return std::get<1>(range);
}
} // namespace intervaltree_internal

template <class Node, class NodeTraits, class Options, class Tag>
IntervalTree<Node, NodeTraits, Options, Tag>::IntervalTree()
{
}

template <class Node, class NodeTraits, class Options, class Tag>
bool IntervalTree<Node, NodeTraits, Options, Tag>::verify_integrity() const
{
    bool base_verification = this->BaseTree::verify_integrity();
    assert(base_verification);
    bool maxima_valid =
        this->root == nullptr ? true : this->verify_maxima(this->root);
    assert(maxima_valid);

    return base_verification && maxima_valid;
}

template <class Node, class NodeTraits, class Options, class Tag>
bool IntervalTree<Node, NodeTraits, Options, Tag>::verify_maxima(Node* n) const
{
    bool valid = true;
    auto maximum = NodeTraits::get_upper(*n);

    if (n->get_right() != nullptr)
    {
        maximum = std::max(maximum, n->get_right()->INB::_it_max_upper);
        valid &= this->verify_maxima(n->get_right());
    }
    if (n->get_left() != nullptr)
    {
        maximum = std::max(maximum, n->get_left()->INB::_it_max_upper);
        valid &= this->verify_maxima(n->get_left());
    }

    valid &= (maximum == n->INB::_it_max_upper);

    return valid;
}

template <class Node, class NodeTraits, class Options, class Tag>
void IntervalTree<Node, NodeTraits, Options, Tag>::fixup_maxima(Node& node)
{
    ENodeTraits::fix_node(node);
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
typename IntervalTree<Node, NodeTraits, Options, Tag>::template QueryResult<
    Comparable>
IntervalTree<Node, NodeTraits, Options, Tag>::query(const Comparable& q) const
{
    Node* cur = this->root;
    if (this->root == nullptr)
    {
        return QueryResult<Comparable>(nullptr, q);
    }

    while ((cur->get_left() != nullptr) &&
           (cur->get_left()->INB::_it_max_upper >= NodeTraits::get_lower(q)))
    {
        cur = cur->get_left();
    }

    Node* hit;
    // If this overlaps, this is our first hit. otherwise, find the next one
    if ((NodeTraits::get_lower(q) <= NodeTraits::get_upper(*cur)) &&
        (NodeTraits::get_upper(q) >= NodeTraits::get_lower(*cur)))
    {
        hit = cur;
    }
    else
    {
        hit = intervaltree_internal::
            find_next_overlapping<Node, INB, NodeTraits, false, Comparable>(cur,
                                                                            q);
    }

    return QueryResult<Comparable>(hit, q);
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
typename IntervalTree<Node, NodeTraits, Options, Tag>::BaseTree::
    template const_iterator<false>
    IntervalTree<Node, NodeTraits, Options, Tag>::interval_upper_bound(
        const Comparable& query_range) const
{
    // An interval lying strictly after <query> is an upper-bound (in the RBTree
    // sense) of the interval that just spans the last point of <query>
    intervaltree_internal::DummyRange<typename NodeTraits::key_type>
        dummy_range(NodeTraits::get_upper(query_range),
                    NodeTraits::get_upper(query_range));

    return this->upper_bound(dummy_range);
}

// TODO move stuff here
namespace intervaltree_internal
{

template <class KeyType>
DummyRange<KeyType>::DummyRange(KeyType lower, KeyType upper)
    : std::pair<KeyType, KeyType>(lower, upper)
{
}

template <class Node,
          class INB,
          class NodeTraits,
          bool skipfirst,
          class Comparable>
Node* find_next_overlapping(Node* cur, const Comparable& q)
{
    // We search for the next bigger node, pruning the search as necessary. When
    // Pruning occurrs, we need to restart the search for the next larger node.

    do
    {
        // We make sure that at the start of the loop, the lower of cur is
        // smaller than the upper of q. Thus, we need to only check the upper to
        // check for overlap.
        if (cur->get_right() != nullptr)
        {
            // go to smallest larger-or-equal child
            cur = cur->get_right();
            if (cur->INB::_it_max_upper < NodeTraits::get_lower(q))
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
                    if (cur->INB::_it_max_upper < NodeTraits::get_lower(q))
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

        if (NodeTraits::get_lower(*cur) > NodeTraits::get_upper(q))
        {
            // No larger node can be an overlap!
            return nullptr;
        }

        if (NodeTraits::get_upper(*cur) >= NodeTraits::get_lower(q))
        {
            // Found!
            return cur;
        }

    } while (true);
}

} // namespace intervaltree_internal

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<
    Comparable>::QueryResult(Node* n_in, const Comparable& q_in)
    : n(n_in), q(q_in)
{
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
typename IntervalTree<Node, NodeTraits, Options, Tag>::template QueryResult<
    Comparable>::const_iterator
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<Comparable>::begin()
    const
{
    return const_iterator(this->n, this->q);
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
typename IntervalTree<Node, NodeTraits, Options, Tag>::template QueryResult<
    Comparable>::const_iterator
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<Comparable>::end()
    const
{
    return const_iterator(nullptr, this->q);
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<
    Comparable>::const_iterator::const_iterator(Node* n_in,
                                                const Comparable& q_in)
    : n(n_in), q(q_in)
{
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<Comparable>::
    const_iterator::const_iterator(
        const typename IntervalTree<Node, NodeTraits, Options, Tag>::
            template QueryResult<Comparable>::const_iterator& other)
    : n(other.n), q(other.q)
{
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<
    Comparable>::const_iterator::~const_iterator()
{
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
typename IntervalTree<Node, NodeTraits, Options, Tag>::template QueryResult<
    Comparable>::const_iterator&
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<Comparable>::
    const_iterator::operator=(
        const typename IntervalTree<Node, NodeTraits, Options, Tag>::
            template QueryResult<Comparable>::const_iterator& other)
{
    this->n = other.n;
    this->q = other.q;
    return *this;
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
bool IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<Comparable>::
    const_iterator::operator==(
        const typename IntervalTree<Node, NodeTraits, Options, Tag>::
            template QueryResult<Comparable>::const_iterator& other) const
{
    return (
        (this->n == other.n) &&
        (NodeTraits::get_lower(this->q) == NodeTraits::get_lower(other.q)) &&
        (NodeTraits::get_upper(this->q) == NodeTraits::get_upper(other.q)));
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
bool IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<Comparable>::
    const_iterator::operator!=(
        const typename IntervalTree<Node, NodeTraits, Options, Tag>::
            template QueryResult<Comparable>::const_iterator& other) const
{
    return !(*this == other);
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
typename IntervalTree<Node, NodeTraits, Options, Tag>::template QueryResult<
    Comparable>::const_iterator&
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<
    Comparable>::const_iterator::operator++()
{
    this->n = intervaltree_internal::
        find_next_overlapping<Node, INB, NodeTraits, false, Comparable>(
            this->n, this->q);

    return *this;
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
typename IntervalTree<Node, NodeTraits, Options, Tag>::template QueryResult<
    Comparable>::const_iterator
IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<
    Comparable>::const_iterator::operator++(int)
{
    typename IntervalTree<Node, NodeTraits, Options, Tag>::template QueryResult<
        Comparable>::const_iterator cpy(*this);

    this->operator++();

    return cpy;
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
const Node& IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<
    Comparable>::const_iterator::operator*() const
{
    return *(this->n);
}

template <class Node, class NodeTraits, class Options, class Tag>
template <class Comparable>
const Node* IntervalTree<Node, NodeTraits, Options, Tag>::QueryResult<
    Comparable>::const_iterator::operator->() const
{
    return this->n;
}

template <class Node, class NodeTraits, class Options, class Tag>
void IntervalTree<Node, NodeTraits, Options, Tag>::dump_to_dot(
    const std::string& filename) const
{
    this->dump_to_dot_base(filename, [&](const Node* node) {
        return NodeTraits::get_id(node) + std::string("\n[") +
               std::to_string(NodeTraits::get_lower(*node)) +
               std::string(", ") +
               std::to_string(NodeTraits::get_upper(*node)) +
               std::string("]\n") + std::string("-> ") +
               std::to_string(node->INB::_it_max_upper);
    });
}

} // namespace ygg

#endif // INCLUDE_INTERVAL_DICT_YGG_INTERVAL_TREE_H
