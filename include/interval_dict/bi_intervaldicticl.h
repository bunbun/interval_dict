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
/// \file biintervaldicticl.h
/// \brief Declaration of the BiIntervalDictICLExp class
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H
#define INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H

#include "intervaldicticl.h"

namespace interval_dict
{

/*
 * Forward declaration of friendly free functions
 */
template <typename Key, typename Val, typename IntervalType>
class BiIntervalDictICLExp;
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>
subtract(BiIntervalDictICLExp<Key, Val, Interval> dict1,
         const BiIntervalDictICLExp<Key, Val, Interval>& dict2);
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>
merge(BiIntervalDictICLExp<Key, Val, Interval> dict1,
      const BiIntervalDictICLExp<Key, Val, Interval>& dict2);
template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
          std::vector<Key> keys,
          Interval query_interval);
template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const std::vector<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
    std::vector<Key> keys,
    Interval query_interval);

/// \brief bidirectional one-to-many dictionary where key-values vary over
/// intervals.
///
/// Typically used for time-varying dictionaries.
///
template <typename Key, typename Val, typename IntervalType>
class BiIntervalDictICLExp
{
public:
    using Interval = IntervalType;
    using BaseType = typename IntervalTraits<Interval>::BaseType;
    using KeyType = Key;
    using ValType = Val;
    using BaseDifferenceType =
        typename IntervalTraits<Interval>::BaseDifferenceType;
    // boost icl interval_set of Intervals
    using Intervals = interval_dict::Intervals<Interval>;
    using ForwardDict = IntervalDictICLExp<Key, Val, IntervalType>;
    using InverseDict = IntervalDictICLExp<Val, Key, IntervalType>;

    /// Default Constructors / assignment operators
    BiIntervalDictICLExp() = default;
    BiIntervalDictICLExp(const BiIntervalDictICLExp&) = default;
    BiIntervalDictICLExp(BiIntervalDictICLExp&& other) noexcept = default;
    BiIntervalDictICLExp&
    operator=(const BiIntervalDictICLExp& other) = default;
    BiIntervalDictICLExp&
    operator=(BiIntervalDictICLExp&& other) noexcept = default;

    /// Explicit initialise from underlying data
    explicit BiIntervalDictICLExp(
        IntervalDictICLExp<Key, Val, IntervalType> forward,
        IntervalDictICLExp<Val, Key, IntervalType> inverse);

    /// Insert key-value pairs valid over interval
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           Interval interval);

    /// Insert key-value-intervals
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& insert(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Insert key-value pairs valid from @p first to @p last
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           BaseType first,
           BaseType last = IntervalTraits<Interval>::max());

    /// Insert key-value-intervals
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& inverse_insert(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Insert value-key pairs valid over interval
    BiIntervalDictICLExp<Key, Val, Interval>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   Interval interval);

    /// Insert value-key pairs valid from @p first to @p last
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   BaseType first,
                   BaseType last = IntervalTraits<Interval>::max());

    /// Erase key-value-intervals
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& erase(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Erase key-value pairs valid over interval
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          Interval interval);

    /// Erase key-value pairs valid from @p first to @p last
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values with the specified @p key over the given @p interval.
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& erase(const Key& key,
                                                    Interval interval);

    /// Erase all values with the specified @p key over the
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    erase(const Key& key,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values over the given @p interval.
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& erase(Interval interval);

    /// Erase all values over the given @p interval.
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    erase(BaseType first, BaseType last = IntervalTraits<Interval>::max());

    /// Erase value-key-intervals
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& inverse_erase(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Erase value-key pairs valid over interval
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    inverse_erase(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                  Interval interval);

    /// Return all keys in sorted order
    [[nodiscard]] std::vector<Key> keys() const;

    /// Return all values in sorted order
    [[nodiscard]] std::vector<Val> values() const;

    /// Return whether there are no keys / values
    [[nodiscard]] bool empty() const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] std::size_t count(const Key& key) const;

    /// Return whether the specified value is in the dictionary
    [[nodiscard]] std::size_t count_value(const Val& value) const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] bool contains(const Key& key) const;

    /// Return whether the specified value is in the dictionary
    [[nodiscard]] bool contains_value(const Val& value) const;

    /// erase all keys / values
    void clear();

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query start. Only really makes sense for closed intervals
    [[nodiscard]] std::vector<Val> find(const Key& key, BaseType query) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval from first to last
    [[nodiscard]] std::vector<Val>
    find(const Key& key,
         BaseType first,
         BaseType last = IntervalTraits<Interval>::max()) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval
    [[nodiscard]] std::vector<Val> find(const Key& key,
                                        Interval interval) const;

    /// Returns all mapped values in a sorted list for the specified @p keys on
    /// the given query interval
    [[nodiscard]] std::vector<Val> find(const std::vector<Key>& keys,
                                        Interval interval) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query intervals
    [[nodiscard]] std::vector<Val> find(const Key& key,
                                        const Intervals& query_intervals) const;

    /// Returns all mapped values in a sorted list for the specified @p value on
    /// the given query start. Only really makes sense for closed intervals
    [[nodiscard]] std::vector<Val> inverse_find(const Val& value,
                                                BaseType query) const;

    /// Returns all mapped values in a sorted list for the specified @p value on
    /// the given query interval from first to last
    [[nodiscard]] std::vector<Val>
    inverse_find(const Val& value,
                 BaseType first,
                 BaseType last = IntervalTraits<Interval>::max()) const;

    /// Returns all mapped values in a sorted list for the specified @p value on
    /// the given query interval
    [[nodiscard]] std::vector<Val> inverse_find(const Val& value,
                                                Interval interval) const;

    /// Returns all mapped values in a sorted list for the specified @p values
    /// on the given query interval
    [[nodiscard]] std::vector<Val> inverse_find(const std::vector<Val>& values,
                                                Interval interval) const;

    /// Returns all mapped values in a sorted list for the specified @p value on
    /// the given query intervals
    [[nodiscard]] std::vector<Val>
    inverse_find(const Val& value, const Intervals& query_intervals) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    template <typename KeyRange>
    [[nodiscard]] BiIntervalDictICLExp<Key, Val, Interval>
    subset(const KeyRange& keys, Interval interval) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    template <typename ValRange>
    [[nodiscard]] BiIntervalDictICLExp<Key, Val, Interval>
    subset_values(const ValRange& values_subset, Interval interval) const;

    /// Returns a new IntervalDict that contains only the specified \p keys and
    /// \p values for the specified \p interval
    template <typename KeyRange, typename ValRange>
    [[nodiscard]] BiIntervalDictICLExp<Key, Val, Interval> subset(
        const KeyRange& keys, const ValRange& values, Interval interval) const;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values -> Keys
    [[nodiscard]] BiIntervalDictICLExp<Key, Val, Interval> invert() const&;
    [[nodiscard]] BiIntervalDictICLExp<Key, Val, Interval> invert() &&;

    /// Returns the number of unique keys
    [[nodiscard]] std::size_t size() const;

    /// Returns the number of unique keys
    [[nodiscard]] std::size_t inverse_size() const;

    /// Returns a dictionary A -> C that spans A -> B -> C
    /// whenever there are A -> B and B -> C mapping over common intervals
    template <typename OtherVal>
    [[nodiscard]] BiIntervalDictICLExp<Key, OtherVal, Interval> joined_to(
        const BiIntervalDictICLExp<Val, OtherVal, Interval>& b_to_c) const;

    /// Supplement with entries from other only for missing keys or gaps where
    /// a key does not map to any values.
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>&
    fill_gaps_with(const BiIntervalDictICLExp<Key, Val, Interval>& other);

    /// fill_to_start()
    /// Back fill initial gaps from a specified starting_point.
    ///
    /// Useful if the data in the dictionary only starts at a particular
    /// point in time, but we wish to extend the same relationships to earlier
    /// in the past.
    ///
    /// For each key, it checks if the interval with initial data derives from
    /// the \p starting_point or before, and extends this by \p maximum_extent.
    ///
    /// By default, \p starting_point=max() (i.e. all first intervals are
    /// extended) and \p maximum_extent=max() (i.e. each initial gap is filled
    /// all the way whatever its size)
    ///
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& fill_to_start(
        BaseType starting_point = IntervalTraits<Interval>::max(),
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
            IntervalTraits<Interval>::max_size());

    /// fill_to_end()
    /// Forward fill the final gaps from a specified starting_point.
    ///
    /// Useful if the data in the dictionary ends prematurely, but we wish
    /// to continue to extend the data after a specified point in time.
    ///
    /// For each key, it checks if the interval with initial data derives from
    /// the \p starting_point or before, and extends this by \p maximum_extent.
    ///
    /// By default, \p starting_point=max() (i.e. all first intervals are
    /// extended) and \p maximum_extent=max() (i.e. each initial gap is filled
    /// all the way whatever its size)
    ///
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& fill_to_end(
        BaseType starting_point = IntervalTraits<Interval>::lowest(),
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
            IntervalTraits<Interval>::max_size());

    /// extend_into_gaps()
    /// Fill gaps (the key maps to no values) by extending values
    /// from either side of each gap.
    /// Specify the direction to fill from (Forward, backward fill, or both)
    ///
    /// Dangerous function for the brave! Useful if there are other ways tou
    /// cleanup inevitable errors afterwards or if dictionary errors are
    /// inconsequential.
    ///
    /// For each key, checks gap intervals where there are no values. Values
    /// from flanking intervals are extended into the gap by \p max_extension at
    /// most from either side.
    ///
    /// By default, \p maximum_extent=max (i.e. gaps are filled all the way
    /// whatever their size)
    ///
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& extend_into_gaps(
        GapExtensionDirection gap_extension_direction =
            GapExtensionDirection::Both,
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
            IntervalTraits<Interval>::max_size());

    /// fill_gaps()
    /// Fill gaps (where the key maps to no values) by looking for common
    /// values on either side of the gap.
    ///
    /// The assumption is that momentary interruptions or gaps in a contiguous
    /// key-value relationship can be corrected safely.
    /// For example, key-values data might be missing for every weekend when
    /// businesses are closed.
    ///
    /// For each key, checks gap intervals where there are no values. The two
    /// flanking intervals are examined for shared values. Any common values are
    /// extended across the gap by \p max_extension at most from each side.
    ///
    /// By default, \p maximum_extent=max (i.e. gaps are filled all the way
    /// whatever their size)
    ///
    /// \return *this
    BiIntervalDictICLExp<Key, Val, Interval>& fill_gaps(
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
            IntervalTraits<Interval>::max_size());

    /// Returns the asymmetrical difference with another interval dictionary
    BiIntervalDictICLExp<Key, Val, Interval>&
    operator-=(const BiIntervalDictICLExp<Key, Val, Interval>& other);

    /// Returns the union with another interval dictionary
    BiIntervalDictICLExp<Key, Val, Interval>&
    operator+=(const BiIntervalDictICLExp<Key, Val, Interval>& other);

    // friends
    friend BiIntervalDictICLExp operator-
        <>(BiIntervalDictICLExp dict1, const BiIntervalDictICLExp& dict2);
    friend BiIntervalDictICLExp operator+
        <>(BiIntervalDictICLExp dict1, const BiIntervalDictICLExp& dict2);
    friend BiIntervalDictICLExp subtract<>(BiIntervalDictICLExp dict1,
                                           const BiIntervalDictICLExp& dict2);
    friend BiIntervalDictICLExp merge<>(BiIntervalDictICLExp dict1,
                                        const BiIntervalDictICLExp& dict2);
    friend cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
    intervals<>(const BiIntervalDictICLExp& interval_dict,
                std::vector<Key> keys,
                Interval query_interval);
    friend cppcoro::generator<
        std::tuple<const Key&, const std::vector<Val>&, Interval>>
    disjoint_intervals<>(const BiIntervalDictICLExp& interval_dict,
                         std::vector<Key> keys,
                         Interval query_interval);

private:
    ForwardDict m_forward;
    InverseDict m_inverse;
};

template <typename Key, typename Val, typename BaseType>
using BiIntervalDictICLExpCL =
    BiIntervalDictICLExp<Key,
                         Val,
                         typename boost::icl::interval<BaseType>::type>;

/* _____________________________________________________________________________
 *
 * Implementations of member functions
 *
 */
template <typename Key, typename Val, typename Interval>
bool IntervalDictICLExp<Key, Val, Interval>::empty() const
{
    return m_forward.empty();
}

template <typename Key, typename Val, typename Interval>
std::size_t IntervalDictICLExp<Key, Val, Interval>::count(const Key& key) const
{
    return m_forward.count(key);
}

template <typename Key, typename Val, typename Interval>
bool IntervalDictICLExp<Key, Val, Interval>::contains(const Key& key) const
{
    return m_forward.contains(key);
}

template <typename Key, typename Val, typename Interval>
std::size_t
IntervalDictICLExp<Key, Val, Interval>::count_value(const Val& value) const
{
    return m_inverse.count(value);
}

template <typename Key, typename Val, typename Interval>
bool IntervalDictICLExp<Key, Val, Interval>::contains_value(
    const Val& value) const
{
    return m_inverse.contains(value);
}

template <typename Key, typename Val, typename Interval>
void IntervalDictICLExp<Key, Val, Interval>::clear()
{
    m_forward.clear();
    m_inverse.clear();
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>::BiIntervalDictICLExp(
    IntervalDictICLExp<Key, Val, Interval> forward,
    IntervalDictICLExp<Val, Key, Interval> inverse)
    : m_forward(std::move(forward)), m_inverse(std::move(inverse))
{
}

/*
 * Insert
 */
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::insert(
    const std::vector<std::pair<Key, Val>>& key_value_pairs, Interval interval)
{
    m_forward.insert(key_value_pairs, interval);
    m_inverse.inverse_insert(key_value_pairs, interval);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::insert(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    m_forward.insert(key_value_intervals);
    m_inverse.inverse_insert(key_value_intervals);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::insert(
    const std::vector<std::pair<Key, Val>>& key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return insert(key_value_pairs, Interval{first, last});
}

/*
 * Inverse insert
 */
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::inverse_insert(
    const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals)
{
    m_forward.inverse_insert(value_key_intervals);
    m_inverse.insert(value_key_intervals);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::inverse_insert(
    const std::vector<std::pair<Val, Key>>& value_key_pairs, Interval interval)
{
    m_forward.inverse_insert(value_key_pairs, interval);
    m_inverse.insert(value_key_pairs, interval);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::inverse_insert(
    const std::vector<std::pair<Val, Key>>& value_key_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    inverse_insert(value_key_pairs, Interval{first, last});
}

/*
 * Erase
 */
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::erase(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    m_forward.erase(key_value_intervals);
    m_inverse.inverse_erase(key_value_intervals);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::erase(
    const std::vector<std::pair<Key, Val>>& key_value_pairs, Interval interval)
{
    m_forward.erase(key_value_pairs, interval);
    m_inverse.inverse_erase(key_value_pairs, interval);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::erase(
    const std::vector<std::pair<Key, Val>>& key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key_value_pairs, Interval{first, last});
}

/// Erase all values with the specified @p key over the given @p interval.
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::erase(const Key& key,
                                                Interval interval)
{
    const auto values = m_forward.find(key, interval);
    m_forward.erase(key, interval);
    m_inverse.erase(values, interval);
    return *this;
}

/// Erase all values with the specified @p key over the
/// given query interval from @p first to @p last
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::erase(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key, {first, last});
}

/// Erase all values over the given @p interval.
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::erase(Interval interval)
{
    m_forward.erase(interval);
    m_inverse.erase(interval);
    return *this;
}

/// Erase all values over the given @p interval.
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::erase(
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase({first, last});
}

/*
 * inverse_erase
 */
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::inverse_erase(
    const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals)
{
    m_forward.inverse_erase(value_key_intervals);
    m_inverse.erase(value_key_intervals);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::inverse_erase(
    const std::vector<std::pair<Val, Key>>& value_key_pairs, Interval interval)
{
    m_forward.inverse_erase(value_key_pairs, interval);
    m_inverse.erase(value_key_pairs, interval);
    return *this;
}

/*
 * find
 */
// Returns all the values as a sorted vector for a specified key over the
// specified intervals. Used for implementing the various forms of find
template <typename Key, typename Val, typename Interval>
std::vector<Val> BiIntervalDictICLExp<Key, Val, Interval>::find(
    const Key& key, const Intervals& query_intervals) const
{
    return m_forward.find(key, query_intervals);
}

template <typename Key, typename Val, typename Interval>
std::vector<Val>
BiIntervalDictICLExp<Key, Val, Interval>::find(const std::vector<Key>& keys,
                                               Interval query_interval) const
{
    return m_forward.find(keys, query_intervals);
}

template <typename Key, typename Val, typename Interval>
std::vector<Val> BiIntervalDictICLExp<Key, Val, Interval>::find(
    const Key& key, typename IntervalTraits<Interval>::BaseType query) const
{
    return find(key, {Interval{query, query}});
}

template <typename Key, typename Val, typename Interval>
std::vector<Val> BiIntervalDictICLExp<Key, Val, Interval>::find(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
{
    return find(key, {Interval{first, last}});
}

template <typename Key, typename Val, typename Interval>
std::vector<Val>
BiIntervalDictICLExp<Key, Val, Interval>::find(const Key& key,
                                               Interval interval) const
{
    return find(key, {interval});
}

/*
 * inverse_find
 */
// Returns all the values as a sorted vector for a specified value over the
// specified intervals.
template <typename Key, typename Val, typename Interval>
std::vector<Key> BiIntervalDictICLExp<Key, Val, Interval>::inverse_find(
    const Val& value, const Intervals& query_intervals) const
{
    return m_forward.inverse_find(value, query_intervals);
}

template <typename Key, typename Val, typename Interval>
std::vector<Key> BiIntervalDictICLExp<Key, Val, Interval>::inverse_find(
    const std::vector<Val>& values, Interval query_interval) const
{
    return m_forward.inverse_find(values, query_intervals);
}

template <typename Key, typename Val, typename Interval>
std::vector<Key> BiIntervalDictICLExp<Key, Val, Interval>::inverse_find(
    const Val& value, typename IntervalTraits<Interval>::BaseType query) const
{
    return inverse_find(value, {Interval{query, query}});
}

template <typename Key, typename Val, typename Interval>
std::vector<Key> BiIntervalDictICLExp<Key, Val, Interval>::inverse_find(
    const Val& value,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
{
    return inverse_find(value, {Interval{first, last}});
}

template <typename Key, typename Val, typename Interval>
std::vector<Key>
BiIntervalDictICLExp<Key, Val, Interval>::inverse_find(const Val& value,
                                                       Interval interval) const
{
    return inverse_find(value, {interval});
}

/*
 * Keys and Values
 */
template <typename Key, typename Val, typename Interval>
std::vector<Key> BiIntervalDictICLExp<Key, Val, Interval>::keys() const
{
    return m_forward.keys();
}

template <typename Key, typename Val, typename Interval>
std::vector<Val> BiIntervalDictICLExp<Key, Val, Interval>::values() const
{
    return m_inverse.keys();
}

template <typename Key, typename Val, typename Interval>
std::size_t BiIntervalDictICLExp<Key, Val, Interval>::size() const
{
    return m_forward.size();
}

template <typename Key, typename Val, typename Interval>
std::size_t BiIntervalDictICLExp<Key, Val, Interval>::inverse_size() const
{
    return m_inverse.size();
}

/*
 * subset
 */
template <typename Key, typename Val, typename Interval>
template <typename KeyRange, typename ValRange>
BiIntervalDictICLExp<Key, Val, Interval>
BiIntervalDictICLExp<Key, Val, Interval>::subset(const KeyRange& keys_subset,
                                                 const ValRange& values_subset,
                                                 Interval query_interval) const
{
    auto forward = m_forward.subset(keys_subset, values_subset, query_interval);
    auto inverse = forward.invert();
    return {std::move(m_forward), std::move(m_inverse)};
}

template <typename Key, typename Val, typename Interval>
template <typename KeyRange>
BiIntervalDictICLExp<Key, Val, Interval>
BiIntervalDictICLExp<Key, Val, Interval>::subset(const KeyRange& keys_subset,
                                                 Interval query_interval) const
{
    auto forward = m_forward.subset(keys_subset, query_interval);
    auto inverse = forward.invert();
    return {std::move(m_forward), std::move(m_inverse)};
}

template <typename Key, typename Val, typename Interval>
template <typename ValRange>
BiIntervalDictICLExp<Key, Val, Interval>
BiIntervalDictICLExp<Key, Val, Interval>::subset_values(
    const ValRange& values_subset, Interval query_interval) const
{
    auto inverse = m_inverse.subset(values_subset, query_interval);
    auto forward = inverse.invert();
    return {std::move(m_forward), std::move(m_inverse)};
}

/*
 * invert
 */
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>
BiIntervalDictICLExp<Key, Val, Interval>::invert() const&
{
    return BiIntervalDictICLExp<Key, Val, Interval>(m_inverse, m_forward);
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>
BiIntervalDictICLExp<Key, Val, Interval>::invert() &&
{
    return BiIntervalDictICLExp<Key, Val, Interval>(std::move(m_inverse),
                                                    std::move(m_forward));
}

/*
 *  operator members
 */
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::operator-=(
    const BiIntervalDictICLExp<Key, Val, Interval>& other)
{
    m_forward -= other.m_forward;
    m_inverse -= other.m_inverse;
    return *this;
}

/// Returns the union with another interval dictionary
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::operator+=(
    const BiIntervalDictICLExp<Key, Val, Interval>& other)
{
    m_forward += other.m_forward;
    m_inverse += other.m_inverse;
    return *this;
}

template <typename A, typename B, typename Interval>
template <typename C>
BiIntervalDictICLExp<A, C, Interval>
BiIntervalDictICLExp<A, B, Interval>::joined_to(
    const BiIntervalDictICLExp<B, C, Interval>& b_to_c) const
{
    auto forward = m_forward.joined_to(b_to_c);
    auto inverse = forward.invert();
    return {std::move(m_forward), std::move(m_inverse)};
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::fill_gaps_with(
    const BiIntervalDictICLExp<Key, Val, Interval>& other)
{
    const auto insertions =
        detail::fill_gaps_with_inserts(m_forward, other.m_forward);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}

/*
 * Gap filling
 */
template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::fill_to_start(
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions =
        detail::fill_to_start_inserts(m_forward, starting_point, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::fill_to_end(
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions =
        detail::fill_to_end_inserts(m_forward, starting_point, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::extend_into_gaps(
    GapExtensionDirection gap_extension_direction,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions = detail::extend_into_gaps_inserts(
        m_forward, gap_extension_direction, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}

template <typename Key, typename Val, typename Interval>
BiIntervalDictICLExp<Key, Val, Interval>&
BiIntervalDictICLExp<Key, Val, Interval>::fill_gaps(
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions = detail::fill_gaps_inserts(m_forward, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}

/*
 * Free functions:
 *
 * intervals()
 */
template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
          std::vector<Key> keys,
          const Interval query_interval)
{
    return intervals(interval_dict.m_forward, keys, query_interval);
}

template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
          const Interval query_interval)
{
    const auto keys = interval_dict.keys();
    return intervals(interval_dict, keys, query_interval);
}

template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
          const Key& key,
          const Interval query_interval)
{
    return intervals(interval_dict, std::vector{key}, query_interval);
}

/*
 * disjoint_intervals
 */
template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const std::vector<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
    std::vector<Key> keys,
    const Interval query_interval)
{
    return disjoint_intervals(interval_dict.m_forward, keys, query_interval);
}

template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const std::vector<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
    const Key& key,
    const Interval query_interval)
{
    const auto keys = interval_dict.keys();
    return disjoint_intervals(interval_dict, keys, query_interval);
}

template <typename Key, typename Val, typename Interval>
cppcoro::generator<std::tuple<const Key&, const std::vector<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictICLExp<Key, Val, Interval>& interval_dict,
    const Interval query_interval)
{
    const auto keys = interval_dict.keys();
    return disjoint_intervals(interval_dict, keys, query_interval);
}

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H
