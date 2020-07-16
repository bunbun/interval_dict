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
/// \file intervaldict.h
/// \brief Declaration of the IntervalDict / IntervalDictExp classes
/// Interval associative dictionaries that can be templated on different
/// key, value, interval and implementations
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_INTERVALDICT_H
#define INCLUDE_INTERVAL_DICT_INTERVALDICT_H

#include "interval_traits.h"
#include "intervaldict_forward.h"

#include <cppcoro/generator.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/map.hpp>

#include <iostream>
#include <tuple>
#include <unordered_set>

namespace interval_dict
{

/// \brief one-to-many dictionary where key-values vary over intervals.
///
/// Typically used for time-varying dictionaries.
/// Implemented as a std::map of Boost  interval_map
///
/// \tparam typename IntervalTraits<Interval>::BaseType Type underlying
/// intervals. E.g. Time, Date, Int
/// \tparam Key Type of keys
/// \tparam Val Type of Values
/// \tparam Interval Interval Type. E.g. boost::icl::right_open_interval<Date>
template <typename Key, typename Val, typename IntervalType, typename Impl>
class IntervalDictExp
{
public:
    bool operator==(const IntervalDictExp& rhs) const;

    bool operator!=(const IntervalDictExp& rhs) const;

public:
    using Interval = IntervalType;
    using BaseType = typename IntervalTraits<Interval>::BaseType;
    using KeyType = Key;
    using ValType = Val;
    using BaseDifferenceType =
        typename IntervalTraits<Interval>::BaseDifferenceType;
    // boost icl interval_set of Intervals
    using Intervals = interval_dict::Intervals<Interval>;
    using ImplType = Impl;

    /// Default Constructors / assignment operators
    IntervalDictExp() = default;
    IntervalDictExp(const IntervalDictExp&) = default;
    IntervalDictExp(IntervalDictExp&& other) noexcept = default;
    IntervalDictExp& operator=(const IntervalDictExp& other) = default;
    IntervalDictExp& operator=(IntervalDictExp&& other) noexcept = default;

    /// Construct from key-value-intervals
    explicit IntervalDictExp(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Insert key-value pairs valid over interval
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           Interval interval = interval_extent<Interval>);

    /// Insert key-value-intervals
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& insert(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Insert key-value pairs valid from @p first to @p last
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           BaseType first,
           BaseType last = IntervalTraits<Interval>::max());

    /// Insert key-value-intervals
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& inverse_insert(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Insert value-key pairs valid over interval
    IntervalDictExp<Key, Val, Interval, Impl>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   Interval interval = interval_extent<Interval>);

    /// Insert value-key pairs valid from @p first to @p last
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   BaseType first,
                   BaseType last = IntervalTraits<Interval>::max());

    /// Erase key-value-intervals
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& erase(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Erase key-value pairs valid over interval
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          Interval interval = interval_extent<Interval>);

    /// Erase key-value pairs valid from @p first to @p last
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values with the specified @p key over the given @p interval.
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& erase(const Key& key,
                                                     Interval interval = interval_extent<Interval>);

    /// Erase all values with the specified @p key over the
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(const Key& key,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values over the given @p interval.
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& erase(Interval interval);

    /// Erase all values over the given @p interval.
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(BaseType first, BaseType last = IntervalTraits<Interval>::max());

    /// Erase value-key-intervals
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& inverse_erase(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Erase value-key pairs valid over interval
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    inverse_erase(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                  Interval interval = interval_extent<Interval>);

    /// Return all keys in sorted order
    [[nodiscard]] std::vector<Key> keys() const;

    /// Return whether there are no keys
    [[nodiscard]] bool empty() const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] std::size_t count(const Key& key) const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] bool contains(const Key& key) const;

    /// erase all keys
    void clear();

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query start. Only really makes sense for closed intervals
    [[nodiscard]] std::vector<Val> find(const Key& key, BaseType query) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval from first to last
    [[nodiscard]] std::vector<Val>
    find(const Key& key, BaseType first, BaseType last) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval
    [[nodiscard]] std::vector<Val> find(const Key& key,
                                        Interval interva = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p keys on
    /// the given query interval
    [[nodiscard]] std::vector<Val> find(const std::vector<Key>& keys,
                                        Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query intervals
    [[nodiscard]] std::vector<Val> find(const Key& key,
                                        const Intervals& query_intervals) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    template <typename KeyRange>
    [[nodiscard]] IntervalDictExp<Key, Val, Interval, Impl>
    subset(const KeyRange& keys, Interval interval = interval_extent<Interval>) const;

    /// Returns a new IntervalDict that contains only the specified \p keys and
    /// \p values for the specified \p interval
    template <typename KeyRange, typename ValRange>
    [[nodiscard]] IntervalDictExp<Key, Val, Interval, Impl> subset(
        const KeyRange& keys, const ValRange& values, Interval interval = interval_extent<Interval>) const;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values -> Keys
    [[nodiscard]] IntervalDictExp<Val, Key, Interval, Impl> invert() const;

    /// Returns the number of unique keys
    [[nodiscard]] std::size_t size() const;

    /// Returns a dictionary A -> C that spans A -> B -> C
    /// whenever there are A -> B and B -> C mapping over common intervals
    template <typename OtherVal>
    [[nodiscard]] IntervalDictExp<Key, OtherVal, Interval, Impl> joined_to(
        const IntervalDictExp<Val, OtherVal, Interval, Impl>& b_to_c) const;

    /// Supplement with entries from other only for missing keys or gaps where
    /// a key does not map to any values.
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    fill_gaps_with(const IntervalDictExp<Key, Val, Interval, Impl>& other);

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
    IntervalDictExp<Key, Val, Interval, Impl>& fill_to_start(
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
    IntervalDictExp<Key, Val, Interval, Impl>& fill_to_end(
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
    IntervalDictExp<Key, Val, Interval, Impl>& extend_into_gaps(
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
    IntervalDictExp<Key, Val, Interval, Impl>& fill_gaps(
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
            IntervalTraits<Interval>::max_size());

    /// Returns the asymmetrical difference with another interval dictionary
    IntervalDictExp<Key, Val, Interval, Impl>&
    operator-=(const IntervalDictExp<Key, Val, Interval, Impl>& other);

    /// Returns the union with another interval dictionary
    IntervalDictExp<Key, Val, Interval, Impl>&
    operator+=(const IntervalDictExp<Key, Val, Interval, Impl>& other);

    // friends
    friend IntervalDictExp operator-
        <>(IntervalDictExp dict1, const IntervalDictExp& dict2);

    friend IntervalDictExp operator+
        <>(IntervalDictExp dict1, const IntervalDictExp& dict2);

    friend IntervalDictExp subtract<>(IntervalDictExp dict1,
                                      const IntervalDictExp& dict2);

    friend IntervalDictExp merge<>(IntervalDictExp dict1,
                                   const IntervalDictExp& dict2);

    friend cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
    intervals<>(const IntervalDictExp& interval_dict,
                std::vector<Key> keys,
                Interval query_interval);

    friend cppcoro::generator<
        std::tuple<const Key&, const std::set<Val>&, Interval>>
    disjoint_intervals<>(const IntervalDictExp& interval_dict,
                         std::vector<Key> keys,
                         Interval query_interval);

    template <typename Key_,
              typename Val_,
              typename Interval_,
              typename Impl_,
              typename KeyRange>
    friend std::vector<std::tuple<Key_, Val_, Interval_>>
    detail::subset_inserts(
        const IntervalDictExp<Key_, Val_, Interval_, Impl_>& interval_dict,
        const KeyRange& keys_subset,
        Interval_ query_interval);

    template <typename Key_,
              typename Val_,
              typename Interval_,
              typename Impl_,
              typename KeyRange,
              typename ValRange>
    friend std::vector<std::tuple<Key_, Val_, Interval_>>
    detail::subset_inserts(
        const IntervalDictExp<Key_, Val_, Interval_, Impl_>& interval_dict,
        const KeyRange& keys_subset,
        const ValRange& values_subset,
        Interval_ query_interval);

    friend std::vector<std::tuple<Key, Val, Interval>>
    detail::fill_gaps_with_inserts<>(
        const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
        const IntervalDictExp<Key, Val, Interval, Impl>& other);

    friend std::vector<std::tuple<Key, Val, Interval>>
    detail::fill_to_start_inserts<>(
        const IntervalDictExp& interval_dict,
        BaseType starting_from,
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::vector<std::tuple<Key, Val, Interval>>
    detail::fill_to_end_inserts<>(
        const IntervalDictExp& interval_dict,
        BaseType starting_from,
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::vector<std::tuple<Key, Val, Interval>>
    detail::fill_gaps_inserts<>(
        const IntervalDictExp& interval_dict,
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::vector<std::tuple<Key, Val, Interval>>
    detail::extend_into_gaps_inserts<>(
        const IntervalDictExp& interval_dict,
        GapExtensionDirection gap_extension_direction,
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::tuple<Insertions<Key, Val, Interval>,
                      Erasures<Key, Val, Interval>>
    detail::flatten_actions<>(const IntervalDictExp& interval_dict,
                              FlattenPolicy<Key, Val, Interval> keep_one_value);

private:
    using DataType = std::map<Key, Impl>;
    DataType data;
};

template <typename Key, typename Val, typename BaseType, typename Impl>
using IntervalDict =
    IntervalDictExp<Key,
                    Val,
                    typename boost::icl::interval<BaseType>::type,
                    Impl>;

/* _____________________________________________________________________________
 *
 * Implementations of member functions
 *
 */

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>::IntervalDictExp(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    insert(key_value_intervals);
}

template <typename Key, typename Val, typename Interval, typename Impl>
bool IntervalDictExp<Key, Val, Interval, Impl>::empty() const
{
    return data.size() == 0;
}

/// Return whether the specified key is in the dictionary
template <typename Key, typename Val, typename Interval, typename Impl>
std::size_t
IntervalDictExp<Key, Val, Interval, Impl>::count(const Key& key) const
{
    return data.count(key);
}

/// Return whether the specified key is in the dictionary
template <typename Key, typename Val, typename Interval, typename Impl>
bool IntervalDictExp<Key, Val, Interval, Impl>::contains(const Key& key) const
{
    return data.contains(key);
}

/// erase all keys
template <typename Key, typename Val, typename Interval, typename Impl>
void IntervalDictExp<Key, Val, Interval, Impl>::clear()
{
    return data.clear();
}

/*
 * Insert
 */
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::insert(
    const std::vector<std::pair<Key, Val>>& key_value_pairs, Interval interval)
{
    for (const auto& [key, value] : key_value_pairs)
    {
        if (!boost::icl::is_empty(interval))
        {
            implementation::insert(data[key], interval, value);
        }
    }
    return *this;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::insert(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    for (const auto& [key, value, interval] : key_value_intervals)
    {
        if (!boost::icl::is_empty(interval))
        {
            implementation::insert(data[key], interval, value);
        }
    }
    return *this;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::insert(
    const std::vector<std::pair<Key, Val>>& key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return insert(key_value_pairs, Interval{first, last});
}

/*
 * Inverse insert
 */
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::inverse_insert(
    const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals)
{
    for (const auto& [value, key, interval] : value_key_intervals)
    {
        if (!boost::icl::is_empty(interval))
        {
            implementation::insert(data[key], interval, value);
        }
    }
    return *this;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::inverse_insert(
    const std::vector<std::pair<Val, Key>>& value_key_pairs, Interval interval)
{
    if (!boost::icl::is_empty(interval))
    {
        for (const auto& [value, key] : value_key_pairs)
        {
            implementation::insert(data[key], interval, value);
        }
    }
    return *this;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::inverse_insert(
    const std::vector<std::pair<Val, Key>>& value_key_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return inverse_insert(value_key_pairs, Interval{first, last});
}

/*
 * Erase
 */
namespace detail
{
template <typename Key, typename Impl>
void cleanup_empty_keys(std::map<Key, Impl>& data,
                        const std::set<Key>& keys_with_erases)
{
    for (const auto& key : keys_with_erases)
    {
        if (implementation::empty(data[key]))
        {
            data.erase(key);
        }
    }
}
} // namespace detail

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto& [key, value, interval] : key_value_intervals)
    {
        if (!boost::icl::is_empty(interval))
        {
            keys_with_erases.insert(key);
            implementation::erase(data[key], interval, value);
        }
    }
    detail::cleanup_empty_keys(data, keys_with_erases);
    return *this;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    const std::vector<std::pair<Key, Val>>& key_value_pairs, Interval interval)
{
    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    if (!boost::icl::is_empty(interval))
    {
        for (const auto& [key, value] : key_value_pairs)
        {
            keys_with_erases.insert(key);
            implementation::erase(data[key], interval, value);
        }
    }
    detail::cleanup_empty_keys(data, keys_with_erases);
    return *this;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    const std::vector<std::pair<Key, Val>>& key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key_value_pairs, Interval{first, last});
}

/// Erase all values with the specified @p key over the given @p interval.
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(const Key& key,
                                                 Interval query_interval)
{
    if (boost::icl::is_empty(query_interval))
    {
        return *this;
    }

    // Do not assume key is valid
    const auto ff = data.find(key);
    if (ff == data.end())
    {
        return *this;
    }

    implementation::erase(data[key], query_interval);
    if (implementation::empty(data[key]))
    {
        data.erase(key);
    }

    return *this;
}

/// Erase all values with the specified @p key over the
/// given query interval from @p first to @p last
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key, Interval{first, last});
}

/// Erase all values over the given @p interval.
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(Interval query_interval)
{
    if (boost::icl::is_empty(query_interval))
    {
        return *this;
    }

    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto& [key, interval_values] : data)
    {
        implementation::erase(data[key], query_interval);
        keys_with_erases.insert(key);
    }
    detail::cleanup_empty_keys(data, keys_with_erases);
    return *this;
}

/// Erase all values over the given @p interval.
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(Interval{first, last});
}

/*
 * inverse_erase
 */
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::inverse_erase(
    const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals)
{
    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto& [value, key, interval] : value_key_intervals)
    {
        if (!boost::icl::is_empty(interval))
        {
            implementation::erase(data[key], interval, value);
            keys_with_erases.insert(key);
        }
    }
    detail::cleanup_empty_keys(data, keys_with_erases);
    return *this;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::inverse_erase(
    const std::vector<std::pair<Val, Key>>& value_key_pairs, Interval interval)
{
    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    if (!boost::icl::is_empty(interval))
    {
        for (const auto& [value, key] : value_key_pairs)
        {
            implementation::erase(data[key], interval, value);
            keys_with_erases.insert(key);
        }
    }
    detail::cleanup_empty_keys(data, keys_with_erases);
    return *this;
}

/*
 * find
 */
// Returns all the values as a sorted vector for a specified key over the
// specified intervals. Used for implementing the various forms of find
template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Val> IntervalDictExp<Key, Val, Interval, Impl>::find(
    const Key& key, const Intervals& query_intervals) const
{
    // Do not assume key is valid
    const auto ff = data.find(key);
    if (ff == data.end())
    {
        return {};
    }

    std::set<Val> unique_results;
    for (const auto& query_interval : query_intervals)
    {
        if (!boost::icl::is_empty(query_interval))
        {
            for (const auto& [_, value] :
                 implementation::intervals(ff->second, query_interval))
            {
                unique_results.insert(value);
            }
        }
    }
    return {unique_results.begin(), unique_results.end()};
}

template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Val>
IntervalDictExp<Key, Val, Interval, Impl>::find(const std::vector<Key>& keys,
                                                Interval query_interval) const
{
    if (boost::icl::is_empty(query_interval))
    {
        return {};
    }
    std::set<Val> unique_results;

    // Do not assume key is valid
    for (const auto& key : keys)
    {
        const auto ff = data.find(key);
        if (ff == data.end())
        {
            return {};
        }

        for (const auto& [_, value] :
             implementation::intervals(ff->second, query_interval))
        {
            unique_results.insert(value);
        }
    }
    return {unique_results.begin(), unique_results.end()};
}

template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Val> IntervalDictExp<Key, Val, Interval, Impl>::find(
    const Key& key, typename IntervalTraits<Interval>::BaseType query) const
{
    return find(key, Interval{query, query});
}

template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Val> IntervalDictExp<Key, Val, Interval, Impl>::find(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
{
    return find(std::vector{key}, Interval{first, last});
}

template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Val>
IntervalDictExp<Key, Val, Interval, Impl>::find(const Key& key,
                                                Interval interval) const
{
    return find(std::vector{key}, interval);
}

template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Key> IntervalDictExp<Key, Val, Interval, Impl>::keys() const
{
    return data | ranges::views::keys | ranges::to_vector;
}

template <typename Key, typename Val, typename Interval, typename Impl>
std::size_t IntervalDictExp<Key, Val, Interval, Impl>::size() const
{
    return data.size();
}

/*
 * subset
 */
template <typename Key, typename Val, typename Interval, typename Impl>
template <typename KeyRange, typename ValRange>
IntervalDictExp<Key, Val, Interval, Impl>
IntervalDictExp<Key, Val, Interval, Impl>::subset(const KeyRange& keys_subset,
                                                  const ValRange& values_subset,
                                                  Interval query_interval) const
{
    if (boost::icl::is_empty(query_interval))
    {
        return {};
    }

    return IntervalDictExp<Key, Val, Interval, Impl>().insert(
        detail::subset_inserts(
            *this, keys_subset, values_subset, query_interval));
}

template <typename Key, typename Val, typename Interval, typename Impl>
template <typename KeyRange>
IntervalDictExp<Key, Val, Interval, Impl>
IntervalDictExp<Key, Val, Interval, Impl>::subset(const KeyRange& keys_subset,
                                                  Interval query_interval) const
{
    if (boost::icl::is_empty(query_interval))
    {
        return {};
    }
    return IntervalDictExp<Key, Val, Interval, Impl>().insert(
        detail::subset_inserts(*this, keys_subset, query_interval));
}

/*
 * invert
 */
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Val, Key, Interval, Impl>
IntervalDictExp<Key, Val, Interval, Impl>::invert() const
{
    IntervalDictExp<Val, Key, Interval, Impl> results;
    for (const auto& [key, interval_values] : data)
    {
        for (const auto& [interval, value] :
             implementation::intervals(interval_values))
        {
            implementation::insert(results[value], interval, key);
        }
    }
    return results;
}

/*
 *  operator members
 */
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::operator-=(
    const IntervalDictExp<Key, Val, Interval, Impl>& other)
{
    // Iterate using the vector of keys() is a much more conservative choice at
    // the cost of making a copy of the keys:
    // makes sure we never change the std::map in the
    // middle of key iteration for example in the course of `myself -= myself;`
    for (auto& key : keys())
    {
        if (const auto f = other.data.find(key); f != other.data.end())
        {
            auto& interval_values = data.find(key)->second;
            implementation::subtract_by(interval_values, f->second);
            // remove empty keys
            if (implementation::empty(interval_values))
            {
                data.erase(key);
            }
        }
    }
    return *this;
}

/// Returns the union with another interval dictionary
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::operator+=(
    const IntervalDictExp<Key, Val, Interval, Impl>& other)
{
    for (const auto& [key_other, interval_values_other] : other.data)
    {
        auto f = data.find(key_other);
        if (f == data.end())
        {
            data[key_other] = interval_values_other;
        }
        else
        {
            implementation::merged_with(f->second, interval_values_other);
        }
    }
    return *this;
}

template <typename A, typename B, typename Interval, typename Impl>
template <typename C>
IntervalDictExp<A, C, Interval, Impl>
IntervalDictExp<A, B, Interval, Impl>::joined_to(
    const IntervalDictExp<B, C, Interval, Impl>& b_to_c) const
{
    IntervalDictExp<A, C, Interval, Impl> results;
    for (const auto& [key_a, interval_values_ab] : data)
    {
        for (const auto& [interval_ab, value_b] :
             implementation::intervals(interval_values_ab))
        {
            // Ignore values that are missing from the other dictionary
            const auto ii = b_to_c.find(value_b);
            if (ii == b_to_c.end())
            {
                continue;
            }

            // look up b->c values that overlap the a->b interval
            const auto& interval_values_bc = ii->second;
            for (const auto& [interval_bc, value_c] :
                 implementation::intervals(interval_values_bc, interval_ab))
            {
                implementation::insert(
                    results[key_a], value_c, interval_ab & interval_bc);
            }
        }
    }
    return results;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::fill_gaps_with(
    const IntervalDictExp<Key, Val, Interval, Impl>& other)
{
    return insert(detail::fill_gaps_with_inserts(*this, other));
}

/*
 * Gap filling
 */
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::fill_to_start(
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    return insert(
        detail::fill_to_start_inserts(*this, starting_point, max_extension));
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::fill_to_end(
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    return insert(
        detail::fill_to_end_inserts(*this, starting_point, max_extension));
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::extend_into_gaps(
    GapExtensionDirection gap_extension_direction,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    return insert(detail::extend_into_gaps_inserts(
        *this, gap_extension_direction, max_extension));
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::fill_gaps(
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    return insert(detail::fill_gaps_inserts(*this, max_extension));
}

template <typename Key, typename Val, typename IntervalType, typename Impl>
bool IntervalDictExp<Key, Val, IntervalType, Impl>::operator==(
    const IntervalDictExp& rhs) const
{
    return data == rhs.data;
}

template <typename Key, typename Val, typename IntervalType, typename Impl>
bool IntervalDictExp<Key, Val, IntervalType, Impl>::operator!=(
    const IntervalDictExp& rhs) const
{
    return !(rhs == *this);
}

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVALDICT_H
