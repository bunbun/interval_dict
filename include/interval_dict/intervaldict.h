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

#include "adaptor_traits.h"
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

/// \brief One-to-many dictionary where key-values vary over intervals.
///
/// Typically used for time-varying dictionaries.
///
/// Most of the time, IntervalDict should be sufficient.
/// IntervalDictExp is useful when you need to specify the exact
/// boost::icl interval type, such as
///
/// - `left_open_interval<BaseType>`
/// - `right_open_interval<BaseType>`
/// - `open_interval<BaseType>`
/// - `closed_interval<BaseType>`
///
/// \tparam Key Type of keys
/// \tparam Val Type of Values
/// \tparam Interval Interval Type. E.g. boost::icl::right_open_interval<Date>
/// \tparam Impl Implementation type. E.g. boost::icl::interval_map...
template <typename Key, typename Val, typename Interval, typename Impl>
class IntervalDictExp
{
public:
    // friend of all other IntervalDictExp
    template <typename K, typename V, typename I, typename Im>
    friend class IntervalDictExp;

    /// @cond Suppress_Doxygen_Warning
    using IntervalType = Interval;
    using BaseType = typename IntervalTraits<Interval>::BaseType;
    using KeyType = Key;
    using ValType = Val;
    using BaseDifferenceType =
        typename IntervalTraits<Interval>::BaseDifferenceType;
    // boost icl interval_set of Intervals
    using Intervals = interval_dict::Intervals<Interval>;
    using ImplType = Impl;
    using InverseImplType = typename Rebased<Val, Key, Interval, Impl>::type;
    template <typename OtherVal>
    using OtherImplType = typename Rebased<Val, OtherVal, Interval, Impl>::type;
    using DataType = std::map<Key, Impl>;
    /// @endcond

    /// @name Constructors
    /// @{
    /// Constructors and assignment operators

    /// Default Constructor
    IntervalDictExp() = default;
    /// Default copy constructor
    IntervalDictExp(const IntervalDictExp&) = default;
    /// Default move constructor
    IntervalDictExp(IntervalDictExp&& other) noexcept = default;
    /// Default copy assignment operator
    IntervalDictExp& operator=(const IntervalDictExp& other) = default;
    /// Default move assignment operator
    IntervalDictExp& operator=(IntervalDictExp&& other) noexcept = default;

    /// Construct from a vector of [key-value-interval]s
    explicit IntervalDictExp(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Construct from underlying data type
    explicit IntervalDictExp(IntervalDictExp::DataType internal);

    /// @}
    /// @name Insert and Erase
    /// Inserting into and removing from the dictionary
    /// @{

    /// Insert key-value pairs valid over the specified interval
    ///
    /// For batch inserting multiple key-value for a single interval.
    /// \param key_value_pairs is a vector of key-value
    /// \param interval defaults to `interval_extent`, i.e. The key values associations are
    /// always valid
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           Interval interval = interval_extent<Interval>);

    /// Insert key-value-intervals
    ///
    /// For batch inserting key-values each for a different interval.
    /// \param key_value_intervals is a vector of Key-Val-Interval
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& insert(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Insert key-value pairs valid from @p first to @p last
    ///
    /// For batch inserting multiple key-value over a time or date span.
    /// \param key_value_pairs is a vector of key-value
    /// \param first is the first point from which the data will be valid
    /// \param last is the last point for which the data will be valid. Defaults to `max()` i.e.
    /// the mapping will always be valid from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the underlying interval
    /// type is open/close etc.
    ///
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           BaseType first,
           BaseType last = IntervalTraits<Interval>::max());

    /// Insert value-key-intervals
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting values-keys each for a different interval.
    /// \param value_key_intervals is a vector of Val-Key-Interval
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& inverse_insert(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Insert value-key pairs valid over the specified interval
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting multiple value-key for a single interval.
    /// \param value_key_pairs is a vector of value-key
    /// \param interval defaults to `interval_extent`, i.e. The key values associations are
    /// always valid
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   Interval interval = interval_extent<Interval>);

    /// Insert value-key pairs valid from @p first to @p last
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting multiple value-key over a time or date span.
    /// \param value_key_pairs is a vector of value-key
    /// \param first is the first point from which the data will be valid
    /// \param last is the last point for which the data will be valid. Defaults to `max()` i.e.
    /// the mapping will always be valid from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the underlying interval
    /// type is open/close etc.
    ///
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   BaseType first,
                   BaseType last = IntervalTraits<Interval>::max());

    /// Erase key-value-intervals
    ///
    /// For batch erasing key-values each for a different interval.
    /// \param key_value_intervals is a vector of Key-Val-Interval
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& erase(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Erase key-value pairs over the specified interval
    ///
    /// For batch erasing multiple key-value for a single interval.
    /// \param key_value_pairs is a vector of key-value
    /// \param interval defaults to `interval_extent`, i.e. All the specified key values
    /// associations are removed over all intervals
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          Interval interval = interval_extent<Interval>);

    /// Erase key-value pairs from @p first to @p last
    ///
    /// For batch erasing multiple key-value over a time or date span.
    /// \param key_value_pairs is a vector of key-value
    /// \param first is the first point from which the data will be erased
    /// \param last is the last point for which the data will be erased. Defaults to `max()` i.e.
    /// all data matching key-value will be erased from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the underlying interval
    /// type is open/close etc.
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values for @p key over the specified @p interval
    ///
    /// \param key
    /// \param interval defaults to `interval_extent`, i.e. All data for @p key
    /// are removed over all intervals
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(const Key& key, Interval interval = interval_extent<Interval>);

    /// Erase all values for @p key from @p first to @p last
    ///
    /// \param key
    /// \param first is the first point from which the data will be erased
    /// \param last is the last point for which the data will be erased. Defaults to `max()` i.e.
    /// all data matching key-value will be erased from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the underlying interval
    /// type is open/close etc.
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(const Key& key,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values over the given @p interval.
    ///
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& erase(Interval interval);

    /// Erase all values from @p first to @p last
    ///
    /// \param first is the first point from which the data will be erased
    /// \param last is the last point for which the data will be erased. Defaults to `max()` i.e.
    /// all data matching key-value will be erased from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the underlying interval
    /// type is open/close etc.
    IntervalDictExp<Key, Val, Interval, Impl>&
    erase(BaseType first, BaseType last = IntervalTraits<Interval>::max());

    /// Erase value-key-intervals
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch erasing key-values each for a different interval.
    /// \param value_key_intervals is a vector of Val-Key-Interval
    /// \return *this
    IntervalDictExp<Key, Val, Interval, Impl>& inverse_erase(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Erase all values-keys over the specified @p interval
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// \param value_key_pairs is a vector of value-key
    /// \param interval defaults to `interval_extent`, i.e. All data for @p key
    /// are removed over all intervals
    IntervalDictExp<Key, Val, Interval, Impl>&
    inverse_erase(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                  Interval interval = interval_extent<Interval>);

    /// Erase all data for all keys
    void clear();

    /// @}

    /// @name find
    /// @{
    /// find data for specified key(s)

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query start. Only really makes sense for closed intervals
    /// \param key
    /// \param query The point at which values match @p key
    /// \return std::vector of matching values
    [[nodiscard]] std::vector<Val> find(const Key& key, BaseType query) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval from @p first to @p last
    /// \param key
    /// \param first The first point at which values may match @p key
    /// \param last The last point at which values may match @p key
    /// \return std::vector of matching values
    [[nodiscard]] std::vector<Val>
    find(const Key& key, BaseType first, BaseType last) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval
    /// \param key
    /// \param interval defaults to `interval_extent`, i.e. All the specified key values
    /// associations are removed over all intervals
    [[nodiscard]] std::vector<Val>
    find(const Key& key, Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p keys on
    /// the given query interval
    /// \param keys A std::vector of keys
    /// \param interval defaults to `interval_extent`, i.e. All the specified key values
    /// associations are removed over all intervals
    [[nodiscard]] std::vector<Val>
    find(const std::vector<Key>& keys,
         Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query intervals
    /// \param key
    /// \param query_intervals boost::icl interval_set<Interval>
    [[nodiscard]] std::vector<Val> find(const Key& key,
                                        const Intervals& query_intervals) const;

    /// @}

    /// @name GapFilling
    /// @{
    /// Filling gaps in the mapping from key to value
    /// This is usually necessary to remedy data errors or dropouts

    /// Supplement with entries from @p other only for missing keys or gaps where
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

    /// @}

    /// Return all keys in sorted order
    [[nodiscard]] std::vector<Key> keys() const;

    /// Returns the number of unique keys
    [[nodiscard]] std::size_t size() const;

    /// Return whether there are no keys
    [[nodiscard]] bool is_empty() const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] std::size_t count(const Key& key) const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] bool contains(const Key& key) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    /// \param keys Any sequence of Key (suitable for range-based for loop)
    /// \param interval defaults to `interval_extent`, i.e. All the specified key values
    /// associations are removed over all intervals
    template <typename KeyRange>
    [[nodiscard]] IntervalDictExp<Key, Val, Interval, Impl>
    subset(const KeyRange& keys,
           Interval interval = interval_extent<Interval>) const;

    /// Returns a new IntervalDict that contains only the specified \p keys and
    /// \p values for the specified \p interval
    /// \param keys Any sequence of Key (suitable for range-based for loop)
    /// \param values Any sequence of Value (suitable for range-based for loop)
    /// \param interval defaults to `interval_extent`, i.e. All the specified key values
    /// associations are removed over all intervals
    template <typename KeyRange, typename ValRange>
    [[nodiscard]] IntervalDictExp<Key, Val, Interval, Impl>
    subset(const KeyRange& keys,
           const ValRange& values,
           Interval interval = interval_extent<Interval>) const;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values and Keys swapped. Note that unless Value and Key are identical
    /// types, the resulting IntervalDict will also have a new type.
    [[nodiscard]] IntervalDictExp<Val, Key, Interval, InverseImplType>
    invert() const;

    /// Joins to a second dictionary with matching values so that if
    /// *this and the parameter have key-value types of
    /// A -> B and B -> C respectively, returns a dictionary A -> C that
    /// spans A -> B -> C over common intervals
    template <typename OtherVal, typename OtherImpl>
    [[nodiscard]] IntervalDictExp<Key,
                                  OtherVal,
                                  Interval,
                                  OtherImplType<OtherVal>>
    joined_to(const IntervalDictExp<Val, OtherVal, Interval, OtherImpl>& b_to_c)
        const;

    /// Returns the asymmetrical differences with another interval dictionary
    IntervalDictExp<Key, Val, Interval, Impl>&
    operator-=(const IntervalDictExp<Key, Val, Interval, Impl>& other);

    /// Returns the union with another interval dictionary
    IntervalDictExp<Key, Val, Interval, Impl>&
    operator+=(const IntervalDictExp<Key, Val, Interval, Impl>& other);

    /// Equality operator
    bool operator==(const IntervalDictExp& rhs) const;

    /// Inequality operator
    bool operator!=(const IntervalDictExp& rhs) const;

    // friends
    /// @cond Suppress_Doxygen_Warning
    friend IntervalDictExp operator-
        <>(IntervalDictExp dict_1, const IntervalDictExp& dict_2);

    friend IntervalDictExp operator+
        <>(IntervalDictExp dict_1, const IntervalDictExp& dict_2);

    friend IntervalDictExp subtract<>(IntervalDictExp dict_1,
                                      const IntervalDictExp& dict_2);

    friend IntervalDictExp merge<>(IntervalDictExp dict_1,
                                   const IntervalDictExp& dict_2);

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

    friend std::tuple<detail::Insertions<Key, Val, Interval>,
                      detail::Erasures<Key, Val, Interval>>
    detail::flatten_actions<>(const IntervalDictExp& interval_dict,
                              FlattenPolicy<Key, Val, Interval> keep_one_value);

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

    friend cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
    intervals<>(const IntervalDictExp& interval_dict,
                std::vector<Key> keys,
                Interval query_interval);

    friend cppcoro::generator<
        std::tuple<const Key&, const std::set<Val>&, Interval>>
    disjoint_intervals<>(const IntervalDictExp& interval_dict,
                         std::vector<Key> keys,
                         Interval query_interval);
    /// @endcond

private:
    DataType data;
};

/// \brief One-to-many dictionary where key-values vary over intervals.
/// Typically used for time-varying dictionaries.
///
/// \tparam Key Type of keys
/// \tparam Val Type of Values
/// \tparam BaseType E.g. Time/Date: The type for intervals begin/ends
/// \tparam Impl Implementation type. E.g. boost::icl::interval_map...
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
IntervalDictExp<Key, Val, Interval, Impl>::IntervalDictExp(
    IntervalDictExp::DataType internal)
    : data(std::move(internal))
{
}

template <typename Key, typename Val, typename Interval, typename Impl>
bool IntervalDictExp<Key, Val, Interval, Impl>::is_empty() const
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
#if __cplusplus > 201703L
    return data.contains(key);
#else
    return data.count(key) != 0;
#endif
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
    if (!boost::icl::is_empty(interval))
    {
        for (const auto& [key, value] : key_value_pairs)
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

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::insert(
    const std::vector<std::pair<Key, Val>>& key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return insert(key_value_pairs, Interval{first, last});
}
/// @endcond

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

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::inverse_insert(
    const std::vector<std::pair<Val, Key>>& value_key_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return inverse_insert(value_key_pairs, Interval{first, last});
}
/// @endcond

/*
 * Erase
 */
/// @cond Suppress_Doxygen_Warning
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
/// @endcond

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
    if (boost::icl::is_empty(interval))
    {
        return *this;
    }

    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto& [key, value] : key_value_pairs)
    {
        keys_with_erases.insert(key);
        implementation::erase(data[key], interval, value);
    }
    detail::cleanup_empty_keys(data, keys_with_erases);
    return *this;
}

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    const std::vector<std::pair<Key, Val>>& key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key_value_pairs, Interval{first, last});
}
/// @endcond

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

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key, Interval{first, last});
}
/// @endcond

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

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::erase(
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(Interval{first, last});
}
/// @endcond

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
    if (boost::icl::is_empty(interval))
    {
        return *this;
    }

    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto& [value, key] : value_key_pairs)
    {
        implementation::erase(data[key], interval, value);
        keys_with_erases.insert(key);
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
            continue;
        }

        for (const auto& [_, value] :
             implementation::intervals(ff->second, query_interval))
        {
            unique_results.insert(value);
        }
    }
    return {unique_results.begin(), unique_results.end()};
}

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Val> IntervalDictExp<Key, Val, Interval, Impl>::find(
    const Key& key, typename IntervalTraits<Interval>::BaseType query) const
{
    return find(key, Interval{query, query});
}
/// @endcond

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<Val> IntervalDictExp<Key, Val, Interval, Impl>::find(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
{
    return find(std::vector{key}, Interval{first, last});
}
/// @endcond

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
IntervalDictExp<Val,
                Key,
                Interval,
                typename Rebased<Val, Key, Interval, Impl>::type>
IntervalDictExp<Key, Val, Interval, Impl>::invert() const
{
    using InverseDataType =
        typename IntervalDictExp<Val, Key, Interval, InverseImplType>::DataType;
    InverseDataType inverted_data;
    for (const auto& [key, interval_values] : data)
    {
        for (const auto& [interval, value] : implementation::intervals(
                 interval_values, interval_extent<Interval>))
        {
            implementation::insert<Key, Interval>(
                inverted_data[value], interval, key);
        }
    }
    return IntervalDictExp<Val, Key, Interval, InverseImplType>(inverted_data);
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

// Doxygen has problems matching this with the declaration
/// @cond Suppress_Doxygen_Warning
template <typename A, typename B, typename Interval, typename Impl>
template <typename C, typename OtherImpl>
IntervalDictExp<A, C, Interval, typename Rebased<B, C, Interval, Impl>::type>
IntervalDictExp<A, B, Interval, Impl>::joined_to(
    const IntervalDictExp<B, C, Interval, OtherImpl>& b_to_c) const
{
    using ReturnType = IntervalDictExp<A, C, Interval, OtherImplType<C>>;
    using OtherDataType = typename ReturnType::DataType;
    OtherDataType other_data;
    for (const auto& [key_a, interval_values_ab] : data)
    {
        for (const auto& [interval_ab, value_b] : implementation::intervals(
                 interval_values_ab, interval_extent<Interval>))
        {
            // Ignore values that are missing from the other dictionary
            const auto ii = b_to_c.data.find(value_b);
            if (ii == b_to_c.data.end())
            {
                continue;
            }

            // look up b->c values that overlap the a->b interval
            const auto& interval_values_bc = ii->second;
            for (const auto& [interval_bc, value_c] :
                 implementation::intervals(interval_values_bc, interval_ab))
            {
                implementation::insert(
                    other_data[key_a], interval_ab & interval_bc, value_c);
            }
        }
    }
    return ReturnType(other_data);
}
/// @endcond

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

/// @cond Suppress_Doxygen_Warning
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>&
IntervalDictExp<Key, Val, Interval, Impl>::fill_to_end(
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    return insert(
        detail::fill_to_end_inserts(*this, starting_point, max_extension));
}
/// @endcond

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

template <typename Key, typename Val, typename Interval, typename Impl>
bool IntervalDictExp<Key, Val, Interval, Impl>::operator==(
    const IntervalDictExp& rhs) const
{
    return data == rhs.data;
}

template <typename Key, typename Val, typename Interval, typename Impl>
bool IntervalDictExp<Key, Val, Interval, Impl>::operator!=(
    const IntervalDictExp& rhs) const
{
    return !(rhs == *this);
}

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVALDICT_H
