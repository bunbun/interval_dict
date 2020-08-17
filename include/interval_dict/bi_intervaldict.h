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
/// \file bi_intervaldict.h
/// \brief Declaration of the BiIntervalDictExp class
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_H
#define INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_H

#include "adaptor_traits.h"
#include "bi_intervaldict_forward.h"
#include "intervaldict.h"
#include "intervaldict_func.h"

namespace interval_dict
{
/// \brief bidirectional one-to-many dictionary where key-values vary over
/// intervals.
///
/// Typically used for time-varying dictionaries.
/// \tparam Key Type of keys
/// \tparam Val Type of Values
/// \tparam IntervalType Interval Type. E.g. boost::icl::right_open_interval<Date>
/// \tparam Impl Implementation of the interval dictionary in the forwards direction per key
/// \tparam InverseImpl Implementation of the interval dictionary in the inverse direction per value
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
class BiIntervalDictExp
{
public:
    /// equality operator
    bool operator==(const BiIntervalDictExp& rhs) const;

    /// inequality operator
    bool operator!=(const BiIntervalDictExp& rhs) const;

public:
    // friend of all other BiIntervalDictExp
    template <typename K, typename V, typename I, typename Im, typename InvIm>
    friend class BiIntervalDictExp;

    /// @cond Suppress_Doxygen_Warning
    using IntervalType = Interval;
    using BaseType = typename IntervalTraits<Interval>::BaseType;
    using KeyType = Key;
    using ValType = Val;
    using BaseDifferenceType =
        typename IntervalTraits<Interval>::BaseDifferenceType;
    // boost icl interval_set of Intervals
    using Intervals = interval_dict::Intervals<Interval>;
    using ForwardDict = IntervalDictExp<Key, Val, Interval, Impl>;
    using InverseDict = IntervalDictExp<Val, Key, Interval, InverseImpl>;

    using ImplType = Impl;
    using InverseImplType = InverseImpl;

    template <typename OtherVal>
    using OtherImplType = typename Rebased<Val, OtherVal, Interval, Impl>::type;
    using DataType = std::map<Key, Impl>;
    using InverseDataType = std::map<Key, InverseImpl>;
/// @endcond

    /// Default Constructor
    BiIntervalDictExp() = default;
    /// Default copy constructor
    BiIntervalDictExp(const BiIntervalDictExp&) = default;
    /// Default move constructor
    BiIntervalDictExp(BiIntervalDictExp&& other) noexcept = default;
    /// Default copy assignment operator
    BiIntervalDictExp& operator=(const BiIntervalDictExp& other) = default;
    /// Default move assignment operator
    BiIntervalDictExp& operator=(BiIntervalDictExp&& other) noexcept = default;

    /// Explicit initialise from underlying data
    explicit BiIntervalDictExp(
        IntervalDictExp<Key, Val, Interval, Impl> forward,
        IntervalDictExp<Val, Key, Interval, InverseImpl> inverse);

    /// Construct from key-value-intervals
    explicit BiIntervalDictExp(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Insert key-value pairs valid over interval
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           Interval interval = interval_extent<Interval>);

    /// Insert key-value-intervals
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& insert(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Insert key-value pairs valid from @p first to @p last
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    insert(const std::vector<std::pair<Key, Val>>& key_value_pairs,
           BaseType first,
           BaseType last = IntervalTraits<Interval>::max());

    /// Insert key-value-intervals
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& inverse_insert(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Insert value-key pairs valid over interval
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   Interval interval = interval_extent<Interval>);

    /// Insert value-key pairs valid from @p first to @p last
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    inverse_insert(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                   BaseType first,
                   BaseType last = IntervalTraits<Interval>::max());

    /// Erase key-value-intervals
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& erase(
        const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals);

    /// Erase key-value pairs valid over interval
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          Interval interval = interval_extent<Interval>);

    /// Erase key-value pairs valid from @p first to @p last
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    erase(const std::vector<std::pair<Key, Val>>& key_value_pairs,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values with the specified @p key over the given @p interval.
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    erase(const Key& key, Interval interval = interval_extent<Interval>);

    /// Erase all values with the specified @p key over the
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    erase(const Key& key,
          BaseType first,
          BaseType last = IntervalTraits<Interval>::max());

    /// Erase all values over the given @p interval.
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    erase(Interval interval);

    /// Erase all values over the given @p interval.
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    erase(BaseType first, BaseType last = IntervalTraits<Interval>::max());

    /// Erase value-key-intervals
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& inverse_erase(
        const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals);

    /// Erase value-key pairs valid over interval
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
    inverse_erase(const std::vector<std::pair<Val, Key>>& value_key_pairs,
                  Interval interval = interval_extent<Interval>);

    /// Return all keys in sorted order
    [[nodiscard]] std::vector<Key> keys() const;

    /// Return all values in sorted order
    [[nodiscard]] std::vector<Val> values() const;

    /// Return whether there are no keys
    [[nodiscard]] bool is_empty() const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] std::size_t count(const Key& key) const;

    /// Return whether the specified value is in the dictionary
    [[nodiscard]] std::size_t count_value(const Val& value) const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] bool contains(const Key& key) const;

    /// Return whether the specified value is in the dictionary
    [[nodiscard]] bool contains_value(const Val& value) const;

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
    [[nodiscard]] std::vector<Val>
    find(const Key& key, Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p keys on
    /// the given query interval
    [[nodiscard]] std::vector<Val>
    find(const std::vector<Key>& keys,
         Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query intervals
    [[nodiscard]] std::vector<Val> find(const Key& key,
                                        const Intervals& query_intervals) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query start. Only really makes sense for closed intervals
    [[nodiscard]] std::vector<Key> inverse_find(const Val& value,
                                                BaseType query) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query interval from first to last
    [[nodiscard]] std::vector<Key>
    inverse_find(const Val& value, BaseType first, BaseType last) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query interval
    [[nodiscard]] std::vector<Key> inverse_find(const Val& value,
                                                Interval interval) const;

    /// Returns all mapped keys in a sorted list for the specified @p values
    /// on the given query interval
    [[nodiscard]] std::vector<Key> inverse_find(const std::vector<Val>& values,
                                                Interval interval) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query intervals
    [[nodiscard]] std::vector<Key>
    inverse_find(const Val& value, const Intervals& query_intervals) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    template <typename KeyRange>
    [[nodiscard]] BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
    subset(const KeyRange& keys,
           Interval interval = interval_extent<Interval>) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    template <typename ValRange>
    [[nodiscard]] BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
    subset_values(const ValRange& values,
                  Interval interval = interval_extent<Interval>) const;

    /// Returns a new IntervalDict that contains only the specified \p keys and
    /// \p values for the specified \p interval
    template <typename KeyRange, typename ValRange>
    [[nodiscard]] BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
    subset(const KeyRange& keys,
           const ValRange& values,
           Interval interval = interval_extent<Interval>) const;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values -> Keys
    [[nodiscard]] BiIntervalDictExp<Val, Key, Interval, InverseImpl, Impl>
    invert() const&;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values -> Keys
    [[nodiscard]] BiIntervalDictExp<Val, Key, Interval, InverseImpl, Impl>
    invert() &&;

    /// Returns the number of unique keys
    [[nodiscard]] std::size_t size() const;

    /// Returns the number of unique values
    [[nodiscard]] std::size_t inverse_size() const;

    /// Returns a dictionary A -> C that spans A -> B -> C
    /// whenever there are A -> B and B -> C mapping over common intervals
    template <typename OtherVal, typename OtherImpl, typename OtherInverseImpl>
    [[nodiscard]] BiIntervalDictExp<Key,
                                    OtherVal,
                                    Interval,
                                    OtherImplType<OtherVal>,
                                    InverseImpl>
    joined_to(const BiIntervalDictExp<Val,
                                      OtherVal,
                                      Interval,
                                      OtherImpl,
                                      OtherInverseImpl>& b_to_c) const;

    /// Supplement with entries from other only for missing keys or gaps where
    /// a key does not map to any values.
    /// \return *this
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& fill_gaps_with(
        const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& other);

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
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& fill_to_start(
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
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& fill_to_end(
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
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& extend_into_gaps(
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
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& fill_gaps(
        typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
            IntervalTraits<Interval>::max_size());

    /// Returns the asymmetrical difference with another interval dictionary
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& operator-=(
        const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& other);

    /// Returns the union with another interval dictionary
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& operator+=(
        const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& other);

    // friends
    /// @cond Suppress_Doxygen_Warning
    friend BiIntervalDictExp operator-
        <>(BiIntervalDictExp dict_1, const BiIntervalDictExp& dict_2);

    friend BiIntervalDictExp operator+
        <>(BiIntervalDictExp dict_1, const BiIntervalDictExp& dict_2);

    friend BiIntervalDictExp subtract<>(BiIntervalDictExp dict_1,
                                        const BiIntervalDictExp& dict_2);

    friend BiIntervalDictExp merge<>(BiIntervalDictExp dict_1,
                                     const BiIntervalDictExp& dict_2);

    friend cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
    intervals<>(const BiIntervalDictExp& interval_dict,
                std::vector<Key> keys,
                Interval query_interval);

    friend cppcoro::generator<
        std::tuple<const Key&, const std::set<Val>&, Interval>>
    disjoint_intervals<>(const BiIntervalDictExp& interval_dict,
                         std::vector<Key> keys,
                         Interval query_interval);

    friend std::ostream& operator<<<>(std::ostream&,
                                      const BiIntervalDictExp& interval_dict);

    friend BiIntervalDictExp
    flattened<>(BiIntervalDictExp interval_dict,
                FlattenPolicy<typename detail::identity<Key>::type,
                              typename detail::identity<Val>::type,
                              typename detail::identity<Interval>::type>
                    keep_one_value);
    /// @endcond

private:
    ForwardDict m_forward;
    InverseDict m_inverse;
};

/// Template on underlying start/end type for intervals e.g. dates, times
/// We use the appropriate, default Interval Type
template <typename Key,
          typename Val,
          typename BaseType,
          typename Impl,
          typename InverseImpl>
using BiIntervalDict =
    BiIntervalDictExp<Key,
                      Val,
                      typename boost::icl::interval<BaseType>::type,
                      Impl,
                      InverseImpl>;

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::BiIntervalDictExp(
    IntervalDictExp<Key, Val, Interval, Impl> forward,
    IntervalDictExp<Val, Key, Interval, InverseImpl> inverse)
    : m_forward(std::move(forward)), m_inverse(std::move(inverse))
{
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::BiIntervalDictExp(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    insert(key_value_intervals);
}

/* _____________________________________________________________________________
 *
 * Implementations of member functions
 *
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
bool BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::is_empty() const
{
    return m_forward.is_empty();
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::size_t BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::count(
    const Key& key) const
{
    return m_forward.count(key);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
bool BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::contains(
    const Key& key) const
{
    return m_forward.contains(key);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::size_t
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::count_value(
    const Val& value) const
{
    return m_inverse.count(value);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
bool BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::contains_value(
    const Val& value) const
{
    return m_inverse.contains(value);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
void BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::clear()
{
    m_forward.clear();
    m_inverse.clear();
}

/*
 * Insert
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::insert(
    const std::vector<std::pair<Key, Val>>& key_value_pairs, Interval interval)
{
    if (!boost::icl::is_empty(interval))
    {
        m_forward.insert(key_value_pairs, interval);
        m_inverse.inverse_insert(key_value_pairs, interval);
    }
    return *this;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::insert(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    m_forward.insert(key_value_intervals);
    m_inverse.inverse_insert(key_value_intervals);
    return *this;
}

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::insert(
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
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_insert(
    const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals)
{
    m_forward.inverse_insert(value_key_intervals);
    m_inverse.insert(value_key_intervals);
    return *this;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_insert(
    const std::vector<std::pair<Val, Key>>& value_key_pairs, Interval interval)
{
    if (!boost::icl::is_empty(interval))
    {
        m_forward.inverse_insert(value_key_pairs, interval);
        m_inverse.insert(value_key_pairs, interval);
    }
    return *this;
}

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_insert(
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
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::erase(
    const std::vector<std::tuple<Key, Val, Interval>>& key_value_intervals)
{
    m_forward.erase(key_value_intervals);
    m_inverse.inverse_erase(key_value_intervals);
    return *this;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::erase(
    const std::vector<std::pair<Key, Val>>& key_value_pairs, Interval interval)
{
    if (boost::icl::is_empty(interval))
    {
        return *this;
    }

    m_forward.erase(key_value_pairs, interval);
    m_inverse.inverse_erase(key_value_pairs, interval);
    return *this;
}

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::erase(
    const std::vector<std::pair<Key, Val>>& key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key_value_pairs, Interval{first, last});
}
/// @endcond

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::erase(
    const Key& key, Interval interval)
{
    if (boost::icl::is_empty(interval))
    {
        return *this;
    }

    const auto values = m_forward.find(key, interval);
    m_forward.erase(key, interval);
    for (const auto& value : values)
    {
        m_inverse.erase(value, interval);
    }
    return *this;
}

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::erase(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(key, Interval{first, last});
}
/// @endcond

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::erase(
    Interval interval)
{
    if (!boost::icl::is_empty(interval))
    {
        m_forward.erase(interval);
        m_inverse.erase(interval);
    }

    return *this;
}

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::erase(
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
{
    return erase(Interval{first, last});
}
/// @endcond

/*
 * inverse_erase
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_erase(
    const std::vector<std::tuple<Val, Key, Interval>>& value_key_intervals)
{
    m_forward.inverse_erase(value_key_intervals);
    m_inverse.erase(value_key_intervals);
    return *this;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_erase(
    const std::vector<std::pair<Val, Key>>& value_key_pairs, Interval interval)
{
    if (!boost::icl::is_empty(interval))
    {
        m_forward.inverse_erase(value_key_pairs, interval);
        m_inverse.erase(value_key_pairs, interval);
    }

    return *this;
}

/*
 * find
 */
// Returns all the values as a sorted vector for a specified key over the
// specified intervals. Used for implementing the various forms of find
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Val> BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::find(
    const Key& key, const Intervals& query_intervals) const
{
    return m_forward.find(key, query_intervals);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Val> BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::find(
    const std::vector<Key>& keys, Interval query_interval) const
{
    return m_forward.find(keys, query_interval);
}

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Val> BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::find(
    const Key& key, typename IntervalTraits<Interval>::BaseType query) const
{
    return find(key, Interval{query, query});
}
/// @endcond

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Val> BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::find(
    const Key& key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
{
    return find(std::vector{key}, Interval{first, last});
}
/// @endcond

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Val> BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::find(
    const Key& key, Interval interval) const
{
    return find(std::vector{key}, interval);
}

/*
 * inverse_find
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Key>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_find(
    const Val& value, const Intervals& query_intervals) const
{
    return m_inverse.find(value, query_intervals);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Key>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_find(
    const std::vector<Val>& values, Interval query_interval) const
{
    return m_inverse.find(values, query_interval);
}

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Key>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_find(
    const Val& value, typename IntervalTraits<Interval>::BaseType query) const
{
    return inverse_find(std::vector{value}, Interval{query, query});
}
/// @endcond

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Key>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_find(
    const Val& value,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
{
    return inverse_find(std::vector{value}, Interval{first, last});
}
/// @endcond

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Key>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_find(
    const Val& value, Interval interval) const
{
    return inverse_find(std::vector{value}, interval);
}

/*
 * Keys and Values
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Key>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::keys() const
{
    return m_forward.keys();
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<Val>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::values() const
{
    return m_inverse.keys();
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::size_t
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::size() const
{
    return m_forward.size();
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::size_t
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::inverse_size() const
{
    return m_inverse.size();
}

/*
 * subset
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
template <typename KeyRange, typename ValRange>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::subset(
    const KeyRange& keys_subset,
    const ValRange& values_subset,
    Interval query_interval) const
{
    if (boost::icl::is_empty(query_interval))
    {
        return {};
    }

    return BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>().insert(
        detail::subset_inserts(
            m_forward, keys_subset, values_subset, query_interval));
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
template <typename KeyRange>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::subset(
    const KeyRange& keys_subset, Interval query_interval) const
{
    if (boost::icl::is_empty(query_interval))
    {
        return {};
    }
    return BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>().insert(
        detail::subset_inserts(m_forward, keys_subset, query_interval));
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
template <typename ValRange>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::subset_values(
    const ValRange& values_subset, Interval query_interval) const
{
    if (boost::icl::is_empty(query_interval))
    {
        return {};
    }
    return BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>()
        .inverse_insert(
            detail::subset_inserts(m_inverse, values_subset, query_interval));
}

/*
 * invert
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Val, Key, Interval, InverseImpl, Impl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::invert() const&
{
    return BiIntervalDictExp<Val, Key, Interval, InverseImpl, Impl>(m_inverse,
                                                                    m_forward);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Val, Key, Interval, InverseImpl, Impl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::invert() &&
{
    return BiIntervalDictExp<Val, Key, Interval, InverseImpl, Impl>(
        std::move(m_inverse), std::move(m_forward));
}

/*
 *  operator members
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::operator-=(
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& other)
{
    m_forward -= other.m_forward;
    m_inverse -= other.m_inverse;
    return *this;
}

/// Returns the union with another interval dictionary
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::operator+=(
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& other)
{
    m_forward += other.m_forward;
    m_inverse += other.m_inverse;
    return *this;
}

// Doxygen has problems matching this with the declaration
/// @cond Suppress_Doxygen_Warning
template <typename A,
          typename B,
          typename Interval,
          typename Impl,
          typename InverseImpl>
template <typename C, typename OtherImpl, typename OtherInverseImpl>
BiIntervalDictExp<A,
                  C,
                  Interval,
                  typename Rebased<B, C, Interval, Impl>::type,
                  InverseImpl>
BiIntervalDictExp<A, B, Interval, Impl, InverseImpl>::joined_to(
    const BiIntervalDictExp<B, C, Interval, OtherImpl, OtherInverseImpl>&
        b_to_c) const
{
    auto forward = m_forward.joined_to(b_to_c.m_forward);
    // Since we are creating forward from scratch, there is little advantage in
    // doing the same operation with m_inverse rather than just inverting the
    // results of the forward operation
    auto inverse = forward.invert();
    return BiIntervalDictExp<A, C, Interval, OtherImplType<C>, InverseImpl>{
        std::move(forward), std::move(inverse)};
}
/// @endcond

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::fill_gaps_with(
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& other)
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
/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::fill_to_start(
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions =
        detail::fill_to_start_inserts(m_forward, starting_point, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}
/// @endcond

/// @cond Suppress_Doxygen_Warning
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::fill_to_end(
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions =
        detail::fill_to_end_inserts(m_forward, starting_point, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}
/// @endcond

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::extend_into_gaps(
    GapExtensionDirection gap_extension_direction,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions = detail::extend_into_gaps_inserts(
        m_forward, gap_extension_direction, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::fill_gaps(
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    const auto insertions = detail::fill_gaps_inserts(m_forward, max_extension);
    m_forward.insert(insertions);
    m_inverse.inverse_insert(insertions);
    return *this;
}

/*
 * Comparisons
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
bool BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::operator==(
    const BiIntervalDictExp& rhs) const
{
    return m_forward == rhs.m_forward;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
bool BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>::operator!=(
    const BiIntervalDictExp& rhs) const
{
    return !(rhs == *this);
}

/*
 * Associated functions:
 *
 * intervals()
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
              interval_dict,
          std::vector<Key> keys,
          const Interval query_interval)
{
    return intervals(interval_dict.m_forward, std::move(keys), query_interval);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
              interval_dict,
          const Key& key,
          const Interval query_interval)
{
    return intervals(interval_dict, std::vector{key}, query_interval);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
              interval_dict,
          const Interval query_interval)
{
    return intervals(interval_dict, interval_dict.keys(), query_interval);
}

/*
 * disjoint_intervals
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
        interval_dict,
    std::vector<Key> keys,
    const Interval query_interval)
{
    return disjoint_intervals(
        interval_dict.m_forward, std::move(keys), query_interval);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
        interval_dict,
    const Key& key,
    const Interval query_interval)
{
    return disjoint_intervals(interval_dict, std::vector{key}, query_interval);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
        interval_dict,
    const Interval query_interval)
{
    return disjoint_intervals(
        interval_dict, interval_dict.keys(), query_interval);
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
subtract(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
         const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2)
{
    dict_1 -= dict_2;
    return dict_1;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
operator-(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
          const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2)
{
    dict_1 -= dict_2;
    return dict_1;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
operator+(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
          const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2)
{
    dict_1 += dict_2;
    return dict_1;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
merge(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
      const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2)
{
    dict_1 += dict_2;
    return dict_1;
}

/*
 * stream output operator
 */
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::ostream&
operator<<(std::ostream& ostream,
           const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
               interval_dict)
{
    ostream << interval_dict.m_forward;
    return ostream;
}

template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> flattened(
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> interval_dict,
    FlattenPolicy<typename detail::identity<Key>::type,
                  typename detail::identity<Val>::type,
                  typename detail::identity<Interval>::type> keep_one_value)
{
    const auto [insertions, erasures] =
        detail::flatten_actions(interval_dict.m_forward, keep_one_value);
    interval_dict.insert(insertions);
    interval_dict.erase(erasures);
    return interval_dict;
}

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_H
