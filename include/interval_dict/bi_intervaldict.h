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
/// \brief Declaration of the BiIntervalDict/BiIntervalDictExp classes
/// Bidirectional interval associative dictionaries that can be templated on
/// different key, value, interval and implementations
///
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_H
#define INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_H

#include "intervaldict.h"

namespace interval_dict
{
  // forward declaration of BiIntervalDictExp
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  class BiIntervalDictExp;

  /* _____________________________________________________________________________
   *
   * Forward declarations of associated functions
   */

  /// subtract()
  /// \return The asymmetrical difference between two interval dictionaries
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> subtract (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2);

  /// merge()
  /// \return The union of two interval dictionaries
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> merge (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2);

  /// intervals()
  ///
  /// returns constituent intervals for specified \p keys
  /// Silently ignores missing keys
  /// \param interval_dict
  /// \param keys Only show intervals for the specified keys
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, value, interval>
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
               &interval_dict,
             std::vector<Key> keys,
             Interval query_interval = interval_extent<Interval>);

  /// intervals()
  ///
  /// returns constituent intervals and their values for the
  /// specified \p key. <br>Silently ignores missing keys
  ///
  /// \param interval_dict The input interval dictionary
  /// \param key Only show intervals for the specified key
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return A sorted std::vector of std::tuple<key, value, interval>
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
               &interval_dict,
             const Key &key,
             Interval query_interval = interval_extent<Interval>);

  /// intervals()
  ///
  /// returns all constituent intervals
  /// \param interval_dict
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, value, interval>
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
               &interval_dict,
             Interval query_interval = interval_extent<Interval>);

  /// disjoint_intervals()
  ///
  /// returns all mapped values for each disjoint, non-overlapping interval
  /// for the specified keys and overlapping the query interval.
  /// Silently ignores missing keys.
  /// \param interval_dict
  /// \param keys Only show intervals for the specified keys
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, std::vector<value>,
  /// interval>
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
      &interval_dict,
    std::vector<Key> keys,
    Interval query_interval = interval_extent<Interval>);

  /// disjoint_intervals()
  ///
  /// Returns all mapped values for each disjoint, non-overlapping interval
  /// for the specified key and overlapping the query interval.
  /// Silently ignores missing keys.
  /// \param interval_dict
  /// \param key Only show intervals for the specified key
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, std::vector<value>,
  /// interval>
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
      &interval_dict,
    const Key &key,
    Interval query_interval = interval_extent<Interval>);

  /// disjoint_intervals()
  /// Returns all mapped values for each disjoint, non-overlapping interval
  /// for all keys and overlapping the query interval.
  /// Silently ignores missing keys.
  /// \param interval_dict
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, std::vector<value>,
  /// interval>
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
      &interval_dict,
    Interval query_interval = interval_extent<Interval>);

  /// binary operator - asymmeterical difference from dict_1 to dict_2
  /// \return a new BiIntervalDict
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> operator- (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2);

  /// binary operator + merges key values in dict_1 with dict_2
  /// \return a new BiIntervalDict
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> operator+ (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2);

  /// subtract()
  /// asymmeterical difference from dict_1 to dict_2
  /// \return a new BiIntervalDict
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> subtract (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2);

  /// merge() key values in dict_1 with dict_2
  /// \return a new BiIntervalDict
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> merge (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2);

  /// flatten()
  /// \brief Flattens dictionary to one value per key per interval
  ///
  /// If the underlying data has a 1:1 key-value correspondence, glitches will
  /// appear as short intervals with extra values. The default policy is thus to
  /// prefer the status quo value, and discard temporary intrusions.
  ///
  /// Another alternative is to just discard all data in intervals with multiple
  /// values:
  /// ~~~
  /// flattened(interval_dict, keep_one_value=flatten_policy_discard())
  /// ~~~
  ///
  /// Otherwise, a custom \p FlattenPolicy callback function or callable object
  /// can be used to select the correct value.
  /// The callback function is passed the status quo value in the preceding
  /// interval, the current interval, the current key, and a vector of multiple
  /// values to choose from.
  ///
  /// The returned optional single value will be used to populate the interval
  /// in question. Note that the return value is not restricted to the supplied
  /// vector of values. If no value is returned, then the interval is discarded.
  ///
  /// \param interval_dict
  /// \param keep_one_value Callback policy to select the correct value
  /// \return a flattened dictionary where keys and values correspond 1:1
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> flattened (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> interval_dict,
    FlattenPolicy<typename details::identity<Key>::type,
                  typename details::identity<Value>::type,
                  typename details::identity<Interval>::type> keep_one_value
    = flatten_policy_prefer_status_quo ());

  /// output streaming operator: prints disjoint intervals
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::ostream &
  operator<< (std::ostream &ostream,
              const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
                &interval_dict);

  /**
   * @brief Bidirectional one-to-many dictionary where key-values vary over
   * intervals.
   *
   *  Typically used for time-varying dictionaries.
   *
   *  `BiIntervalDictExp` is useful for specifying the exact inclusive or
   * exclusive interval type.
   *
   * Choices are [boost::icl intervals
   * ](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html#boost_icl.introduction.icl_s_class_templates)
   * :
   *
   *  - `left_open_interval<BaseType>`
   *  - `right_open_interval<BaseType>`
   *  - `open_interval<BaseType>`
   *  - `closed_interval<BaseType>`
   *
   *  `BaseType` can be a Date or Time type, for example.
   *
   * @tparam Key Type of keys
   * @tparam Value Type of Values
   * @tparam Interval Interval Type. E.g. boost::icl::right_open_interval<Date>
   * @tparam Impl Implementation of the interval dictionary in the forwards
   * direction per key
   * @tparam InverseImpl Implementation of the interval dictionary in the
   * inverse direction per value
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  class BiIntervalDictExp
  {
    public:
    // friend of all other BiIntervalDictExp
    template<typename K, typename V, typename I, typename Im, typename InvIm>
    friend class BiIntervalDictExp;

    /// @cond Suppress_Doxygen_Warning
    using IntervalType = Interval;
    using BaseType = typename IntervalTraits<Interval>::BaseType;
    using KeyType = Key;
    using ValType = Value;
    using BaseDifferenceType =
      typename IntervalTraits<Interval>::BaseDifferenceType;
    // boost icl interval_set of Intervals
    using Intervals = interval_dict::Intervals<Interval>;
    using ForwardDict = IntervalDictExp<Key, Value, Interval, Impl>;
    using InverseDict = IntervalDictExp<Value, Key, Interval, InverseImpl>;

    using ImplType = Impl;
    using InverseImplType = InverseImpl;

    template<typename OtherVal>
    using OtherImplType =
      typename Implementation<Value, Interval, Impl>::template rebind<
        OtherVal>::type;
    using DataType = std::map<Key, Impl>;
    using InverseDataType = std::map<Key, InverseImpl>;
    using KeyValueIntervals = std::vector<std::tuple<Key, Value, Interval>>;
    /// @endcond

    /// @name Constructors
    /// @{
    /// Constructors and assignment operators

    /// Default Constructor
    BiIntervalDictExp () = default;
    /// Default copy constructor
    BiIntervalDictExp (const BiIntervalDictExp &) = default;
    /// Default move constructor
    BiIntervalDictExp (BiIntervalDictExp &&other) noexcept = default;
    /// Default copy assignment operator
    BiIntervalDictExp &operator= (const BiIntervalDictExp &other) = default;
    /// Default move assignment operator
    BiIntervalDictExp &operator= (BiIntervalDictExp &&other) noexcept = default;

    /// Construct from a vector of [key-value-interval]s
    explicit BiIntervalDictExp (
      const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals);

    /// Construct from underlying data types
    explicit BiIntervalDictExp (
      IntervalDictExp<Key, Value, Interval, Impl> forward,
      IntervalDictExp<Value, Key, Interval, InverseImpl> inverse);

    /// @}
    /// @name Insert and Erase Member Functions
    /// Inserting into and removing from the dictionary
    /// @{

    /// Insert key-value pairs valid over the specified interval
    ///
    /// For batch inserting multiple key-value for a single interval.
    /// \param key_value_pairs is a vector of key-value
    /// \param interval defaults to `interval_extent`, i.e. The key values
    /// associations are always valid \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    insert (const std::vector<std::pair<Key, Value>> &key_value_pairs,
            Interval interval = interval_extent<Interval>);

    /// Insert key-value-intervals
    ///
    /// For batch inserting key-values each for a different interval.
    /// \param key_value_intervals is a vector of Key-Value-Interval
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &insert (
      const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals);

    /// Insert key-value pairs valid from @p first to @p last
    ///
    /// For batch inserting multiple key-value over a time or date span.
    /// \param key_value_pairs is a vector of key-value
    /// \param first is the first point from which the data will be valid
    /// \param last is the last point for which the data will be valid. Defaults
    /// to `max()` i.e. the mapping will always be valid from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the
    /// underlying interval type is open/close etc.
    ///
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    insert (const std::vector<std::pair<Key, Value>> &key_value_pairs,
            BaseType first,
            BaseType last = IntervalTraits<Interval>::maximum ());

    /// Insert value-key-intervals
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting values-keys each for a different interval.
    /// \param value_key_intervals is a vector of Value-Key-Interval
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &inverse_insert (
      const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals);

    /// Insert value-key pairs valid over the specified interval
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting multiple value-key for a single interval.
    /// \param value_key_pairs is a vector of value-key
    /// \param interval defaults to `interval_extent`, i.e. The key values
    /// associations are always valid \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    inverse_insert (const std::vector<std::pair<Value, Key>> &value_key_pairs,
                    Interval interval = interval_extent<Interval>);

    /// Insert value-key pairs valid from @p first to @p last
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting multiple value-key over a time or date span.
    /// \param value_key_pairs is a vector of value-key
    /// \param first is the first point from which the data will be valid
    /// \param last is the last point for which the data will be valid. Defaults
    /// to `max()` i.e. the mapping will always be valid from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the
    /// underlying interval type is open/close etc.
    ///
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    inverse_insert (const std::vector<std::pair<Value, Key>> &value_key_pairs,
                    BaseType first,
                    BaseType last = IntervalTraits<Interval>::maximum ());

    /// Erase key-value-intervals
    ///
    /// For batch erasing key-values each for a different interval.
    /// \param key_value_intervals is a vector of Key-Value-Interval
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &erase (
      const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals);

    /// Erase key-value pairs over the specified interval
    ///
    /// For batch erasing multiple key-value for a single interval.
    /// \param key_value_pairs is a vector of key-value
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    erase (const std::vector<std::pair<Key, Value>> &key_value_pairs,
           Interval interval = interval_extent<Interval>);

    /// Erase key-value pairs from @p first to @p last
    ///
    /// For batch erasing multiple key-value over a time or date span.
    /// \param key_value_pairs is a vector of key-value
    /// \param first is the first point from which the data will be erased
    /// \param last is the last point for which the data will be erased.
    /// Defaults to `max()` i.e. all data matching key-value will be erased from
    /// first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the
    /// underlying interval type is open/close etc. \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    erase (const std::vector<std::pair<Key, Value>> &key_value_pairs,
           BaseType first,
           BaseType last = IntervalTraits<Interval>::maximum ());

    /// Erase all values for @p key over the specified @p interval
    ///
    /// \param key
    /// \param interval defaults to `interval_extent`, i.e. All data for @p key
    /// are removed over all intervals
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    erase (const Key &key, Interval interval = interval_extent<Interval>);

    /// Erase all values for @p key from @p first to @p last
    ///
    /// \param key
    /// \param first is the first point from which the data will be erased
    /// \param last is the last point for which the data will be erased.
    /// Defaults to `max()` i.e. all data matching key-value will be erased from
    /// first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the
    /// underlying interval type is open/close etc. \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    erase (const Key &key,
           BaseType first,
           BaseType last = IntervalTraits<Interval>::maximum ());

    /// Erase all values over the given @p interval.
    ///
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    erase (Interval interval);

    /// Erase all values from @p first to @p last
    ///
    /// \param first is the first point from which the data will be erased
    /// \param last is the last point for which the data will be erased.
    /// Defaults to `max()` i.e. all data matching key-value will be erased from
    /// first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the
    /// underlying interval type is open/close etc. \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    erase (BaseType first,
           BaseType last = IntervalTraits<Interval>::maximum ());

    /// Erase value-key-intervals
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch erasing key-values each for a different interval.
    /// \param value_key_intervals is a vector of Value-Key-Interval
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &inverse_erase (
      const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals);

    /// Erase all values-keys over the specified @p interval
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// \param value_key_pairs is a vector of value-key
    /// \param interval defaults to `interval_extent`, i.e. All data for @p key
    /// are removed over all intervals
    /// \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    inverse_erase (const std::vector<std::pair<Value, Key>> &value_key_pairs,
                   Interval interval = interval_extent<Interval>);

    /// erase all keys
    void clear ();

    /// @}

    /// @name Find Member Functions
    /// @{
    /// find data for specified key(s)

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query start. Only really makes sense for closed intervals
    /// \param key
    /// \param query The point at which values match @p key
    /// \return std::vector of matching values
    [[nodiscard]] std::vector<Value> find (const Key &key,
                                           BaseType query) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval from @p first to @p last
    /// \param key
    /// \param first The first point at which values may match @p key
    /// \param last The last point at which values may match @p key
    /// \return std::vector of matching values
    [[nodiscard]] std::vector<Value>
    find (const Key &key, BaseType first, BaseType last) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval
    /// \param key
    /// \param interval defaults to `interval_extent`, i.e. All
    /// associations for this key are removed over all intervals
    [[nodiscard]] std::vector<Value>
    find (const Key &key, Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p keys on
    /// the given query interval
    /// \param keys A std::vector of keys
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals
    [[nodiscard]] std::vector<Value> find (const std::vector<Key> &keys,
                                           Interval interval
                                           = interval_extent<Interval>) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query intervals
    /// \param key
    /// \param query_intervals boost::icl interval_set<Interval>
    [[nodiscard]] std::vector<Value>
    find (const Key &key, const Intervals &query_intervals) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query start. Only really makes sense for closed intervals
    /// \param value
    /// \param query The point at which values match @p value
    /// \return std::vector of matching keys
    [[nodiscard]] std::vector<Key> inverse_find (const Value &value,
                                                 BaseType query) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query interval from @p first to @p last
    /// \param value
    /// \param first The first point at which keys may match @p value
    /// \param last The last point at which keys may match @p value
    /// \return std::vector of matching keys
    [[nodiscard]] std::vector<Key>
    inverse_find (const Value &value, BaseType first, BaseType last) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query interval
    /// \param value
    /// \param interval defaults to `interval_extent`, i.e. All
    /// associations for this value are removed over all intervals
    [[nodiscard]] std::vector<Key>
    inverse_find (const Value &value,
                  Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped keys in a sorted list for the specified @p values
    /// on the given query interval
    /// \param values A std::vector of values
    /// \param interval defaults to `interval_extent`, i.e. All
    /// associations for these values are removed over all intervals
    [[nodiscard]] std::vector<Key>
    inverse_find (const std::vector<Value> &values,
                  Interval interval = interval_extent<Interval>) const;

    /// Returns all mapped keys in a sorted list for the specified @p value on
    /// the given query intervals
    /// \param value
    /// \param query_intervals boost::icl interval_set<Interval>
    [[nodiscard]] std::vector<Key>
    inverse_find (const Value &value, const Intervals &query_intervals) const;

    /// @}

    /// @name Gap-filling Member Functions
    /// @{
    /// Filling gaps in the mapping from key to value
    /// This is usually necessary to remedy data errors or dropouts

    /// Supplement with entries from @p other only for missing keys or gaps
    /// where a key does not map to any values. \return *this
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &fill_gaps_with (
      const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &other);

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
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &fill_to_start (
      BaseType starting_point = IntervalTraits<Interval>::maximum (),
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

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
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &fill_to_end (
      BaseType starting_point = IntervalTraits<Interval>::minimum (),
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

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
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
    extend_into_gaps (
      GapExtensionDirection gap_extension_direction
      = GapExtensionDirection::Both,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

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
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &fill_gaps (
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());
    /// @}

    /// Return all keys in sorted order
    [[nodiscard]] std::vector<Key> keys () const;

    /// Return all values in sorted order
    [[nodiscard]] std::vector<Value> values () const;

    /// Returns the number of unique keys
    [[nodiscard]] std::size_t size () const;

    /// Returns the number of unique values
    [[nodiscard]] std::size_t inverse_size () const;

    /// Return whether there are no keys
    [[nodiscard]] bool empty () const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] std::size_t count (const Key &key) const;

    /// Return whether the specified value is in the dictionary
    [[nodiscard]] std::size_t count_value (const Value &value) const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] bool contains (const Key &key) const;

    /// Return whether the specified value is in the dictionary
    [[nodiscard]] bool contains_value (const Value &value) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    /// \param keys Any sequence of Key (suitable for range-based for loop)
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals
    template<typename KeyRange>
    [[nodiscard]] BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
    subset (const KeyRange &keys,
            Interval interval = interval_extent<Interval>) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    /// \param values Any sequence of Value (suitable for range-based for loop)
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals
    template<typename ValRange>
    [[nodiscard]] BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
    subset_values (const ValRange &values,
                   Interval interval = interval_extent<Interval>) const;

    /// Returns a new IntervalDict that contains only the specified \p keys and
    /// \p values for the specified \p interval
    /// \param keys Any sequence of Key (suitable for range-based for loop)
    /// \param values Any sequence of Value (suitable for range-based for loop)
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals
    template<typename KeyRange, typename ValRange>
    [[nodiscard]] BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
    subset (const KeyRange &keys,
            const ValRange &values,
            Interval interval = interval_extent<Interval>) const;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values and Keys swapped. Note that unless Value and Key are identical
    /// types, the resulting IntervalDict will also have a new type.
    [[nodiscard]] BiIntervalDictExp<Value, Key, Interval, InverseImpl, Impl>
    invert () const &;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values and Keys swapped. Note that unless Value and Key are identical
    /// types, the resulting IntervalDict will also have a new type.
    // Moved from objects can just have their m_forward/m_inverse fields swapped
    // for maximal efficiency
    [[nodiscard]] BiIntervalDictExp<Value, Key, Interval, InverseImpl, Impl>
    invert () &&;

    /// Joins to a second dictionary with matching values so that if
    /// *this and the parameter have key-value types of
    /// A -> B and B -> C respectively, returns a dictionary A -> C that
    /// spans A -> B -> C over common intervals
    template<typename OtherVal, typename OtherImpl, typename OtherInverseImpl>
    [[nodiscard]] BiIntervalDictExp<Key,
                                    OtherVal,
                                    Interval,
                                    OtherImplType<OtherVal>,
                                    InverseImpl>
    joined_to (const BiIntervalDictExp<Value,
                                       OtherVal,
                                       Interval,
                                       OtherImpl,
                                       OtherInverseImpl> &b_to_c) const;

    /// Returns the asymmetrical differences with another interval dictionary
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &operator-= (
      const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &other);

    /// Returns the union with another interval dictionary
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &operator+= (
      const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &other);

    /// Equality operator
    bool operator== (const BiIntervalDictExp &rhs) const;

    /// Inequality operator
    bool operator!= (const BiIntervalDictExp &rhs) const;

    // friends
    /// @cond Suppress_Doxygen_Warning
    friend BiIntervalDictExp operator-<> (BiIntervalDictExp dict_1,
                                          const BiIntervalDictExp &dict_2);

    friend BiIntervalDictExp operator+<> (BiIntervalDictExp dict_1,
                                          const BiIntervalDictExp &dict_2);

    friend BiIntervalDictExp subtract<> (BiIntervalDictExp dict_1,
                                         const BiIntervalDictExp &dict_2);

    friend BiIntervalDictExp merge<> (BiIntervalDictExp dict_1,
                                      const BiIntervalDictExp &dict_2);

    friend cppcoro::generator<std::tuple<const Key &, const Value &, Interval>>
    intervals<> (const BiIntervalDictExp &interval_dict,
                 std::vector<Key> keys,
                 Interval query_interval);

    friend cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
    disjoint_intervals<> (const BiIntervalDictExp &interval_dict,
                          std::vector<Key> keys,
                          Interval query_interval);

    friend std::ostream &operator<< <> (std::ostream &,
                                        const BiIntervalDictExp &interval_dict);

    friend BiIntervalDictExp flattened<> (
      BiIntervalDictExp interval_dict,
      FlattenPolicy<typename details::identity<Key>::type,
                    typename details::identity<Value>::type,
                    typename details::identity<Interval>::type> keep_one_value);
    /// @endcond

    private:
    ForwardDict m_forward;
    InverseDict m_inverse;
  };

  /// \brief Bidirectional one-to-many dictionary where key-values vary over
  /// intervals. Typically used for time-varying dictionaries.
  ///
  /// \tparam Key Type of keys
  /// \tparam Value Type of Values
  /// \tparam BaseType E.g. Time/Date: The type for intervals begin/ends
  /// \tparam Impl Implementation type. E.g. boost::icl::interval_map...
  /// \tparam InverseImpl Implementation of the interval dictionary in the
  /// inverse direction per value
  template<typename Key,
           typename Value,
           typename BaseType,
           typename Impl,
           typename InverseImpl>
  using BiIntervalDict
    = BiIntervalDictExp<Key,
                        Value,
                        typename boost::icl::interval<BaseType>::type,
                        Impl,
                        InverseImpl>;

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::
    BiIntervalDictExp (
      IntervalDictExp<Key, Value, Interval, Impl> forward,
      IntervalDictExp<Value, Key, Interval, InverseImpl> inverse)
    : m_forward (std::move (forward))
    , m_inverse (std::move (inverse))
  {
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::
    BiIntervalDictExp (
      const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals)
  {
    insert (key_value_intervals);
  }

  /* _____________________________________________________________________________
   *
   * Implementations of member functions
   *
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  bool
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::empty () const
  {
    return m_forward.empty ();
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::size_t
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::count (
    const Key &key) const
  {
    return m_forward.count (key);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  bool BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::contains (
    const Key &key) const
  {
    return m_forward.contains (key);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::size_t
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::count_value (
    const Value &value) const
  {
    return m_inverse.count (value);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  bool
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::contains_value (
    const Value &value) const
  {
    return m_inverse.contains (value);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  void BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::clear ()
  {
    m_forward.clear ();
    m_inverse.clear ();
  }

  /*
   * Insert
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::insert (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    Interval interval)
  {
    if (!boost::icl::is_empty (interval))
    {
      m_forward.insert (key_value_pairs, interval);
      m_inverse.inverse_insert (key_value_pairs, interval);
    }
    return *this;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::insert (
    const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals)
  {
    m_forward.insert (key_value_intervals);
    m_inverse.inverse_insert (key_value_intervals);
    return *this;
  }

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::insert (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
  {
    return insert (key_value_pairs, Interval {first, last});
  }
  /// @endcond

  /*
   * Inverse insert
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_insert (
    const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals)
  {
    m_forward.inverse_insert (value_key_intervals);
    m_inverse.insert (value_key_intervals);
    return *this;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_insert (
    const std::vector<std::pair<Value, Key>> &value_key_pairs,
    Interval interval)
  {
    if (!boost::icl::is_empty (interval))
    {
      m_forward.inverse_insert (value_key_pairs, interval);
      m_inverse.insert (value_key_pairs, interval);
    }
    return *this;
  }

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_insert (
    const std::vector<std::pair<Value, Key>> &value_key_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
  {
    return inverse_insert (value_key_pairs, Interval {first, last});
  }
  /// @endcond

  /*
   * Erase
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::erase (
    const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals)
  {
    m_forward.erase (key_value_intervals);
    m_inverse.inverse_erase (key_value_intervals);
    return *this;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::erase (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    Interval interval)
  {
    if (boost::icl::is_empty (interval))
    {
      return *this;
    }

    m_forward.erase (key_value_pairs, interval);
    m_inverse.inverse_erase (key_value_pairs, interval);
    return *this;
  }

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::erase (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
  {
    return erase (key_value_pairs, Interval {first, last});
  }
  /// @endcond

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::erase (
    const Key &key, Interval interval)
  {
    if (boost::icl::is_empty (interval))
    {
      return *this;
    }

    const auto values = m_forward.find (key, interval);
    m_forward.erase (key, interval);
    for (const auto &value : values)
    {
      m_inverse.erase (value, interval);
    }
    return *this;
  }

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::erase (
    const Key &key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
  {
    return erase (key, Interval {first, last});
  }
  /// @endcond

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::erase (
    Interval interval)
  {
    if (!boost::icl::is_empty (interval))
    {
      m_forward.erase (interval);
      m_inverse.erase (interval);
    }

    return *this;
  }

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::erase (
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last)
  {
    return erase (Interval {first, last});
  }
  /// @endcond

  /*
   * inverse_erase
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_erase (
    const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals)
  {
    m_forward.inverse_erase (value_key_intervals);
    m_inverse.erase (value_key_intervals);
    return *this;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_erase (
    const std::vector<std::pair<Value, Key>> &value_key_pairs,
    Interval interval)
  {
    if (!boost::icl::is_empty (interval))
    {
      m_forward.inverse_erase (value_key_pairs, interval);
      m_inverse.erase (value_key_pairs, interval);
    }

    return *this;
  }

  /*
   * find
   */
  // Returns all the values as a sorted vector for a specified key over the
  // specified intervals. Used for implementing the various forms of find
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Value>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::find (
    const Key &key, const Intervals &query_intervals) const
  {
    return m_forward.find (key, query_intervals);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Value>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::find (
    const std::vector<Key> &keys, Interval query_interval) const
  {
    return m_forward.find (keys, query_interval);
  }

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Value>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::find (
    const Key &key, typename IntervalTraits<Interval>::BaseType query) const
  {
    return find (key, Interval {query, query});
  }
  /// @endcond

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Value>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::find (
    const Key &key,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
  {
    return find (std::vector {key}, Interval {first, last});
  }
  /// @endcond

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Value>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::find (
    const Key &key, Interval interval) const
  {
    return find (std::vector {key}, interval);
  }

  /*
   * inverse_find
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Key>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_find (
    const Value &value, const Intervals &query_intervals) const
  {
    return m_inverse.find (value, query_intervals);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Key>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_find (
    const std::vector<Value> &values, Interval query_interval) const
  {
    return m_inverse.find (values, query_interval);
  }

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Key>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_find (
    const Value &value, typename IntervalTraits<Interval>::BaseType query) const
  {
    return inverse_find (std::vector {value}, Interval {query, query});
  }
  /// @endcond

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Key>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_find (
    const Value &value,
    typename IntervalTraits<Interval>::BaseType first,
    typename IntervalTraits<Interval>::BaseType last) const
  {
    return inverse_find (std::vector {value}, Interval {first, last});
  }
  /// @endcond

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Key>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_find (
    const Value &value, Interval interval) const
  {
    return inverse_find (std::vector {value}, interval);
  }

  /*
   * Keys and Values
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Key>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::keys () const
  {
    return m_forward.keys ();
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::vector<Value>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::values () const
  {
    return m_inverse.keys ();
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::size_t
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::size () const
  {
    return m_forward.size ();
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::size_t
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::inverse_size ()
    const
  {
    return m_inverse.size ();
  }

  /*
   * subset
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  template<typename KeyRange, typename ValRange>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::subset (
    const KeyRange &keys_subset,
    const ValRange &values_subset,
    Interval query_interval) const
  {
    if (boost::icl::is_empty (query_interval))
    {
      return {};
    }

    return BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> ()
      .insert (details::subset_inserts (
        m_forward, keys_subset, values_subset, query_interval));
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  template<typename KeyRange>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::subset (
    const KeyRange &keys_subset, Interval query_interval) const
  {
    if (boost::icl::is_empty (query_interval))
    {
      return {};
    }
    return BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> ()
      .insert (
        details::subset_inserts (m_forward, keys_subset, query_interval));
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  template<typename ValRange>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::subset_values (
    const ValRange &values_subset, Interval query_interval) const
  {
    if (boost::icl::is_empty (query_interval))
    {
      return {};
    }
    return BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> ()
      .inverse_insert (
        details::subset_inserts (m_inverse, values_subset, query_interval));
  }

  /*
   * invert
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Value, Key, Interval, InverseImpl, Impl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::invert () const &
  {
    return BiIntervalDictExp<Value, Key, Interval, InverseImpl, Impl> (
      m_inverse, m_forward);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Value, Key, Interval, InverseImpl, Impl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::invert () &&
  {
    return BiIntervalDictExp<Value, Key, Interval, InverseImpl, Impl> (
      std::move (m_inverse), std::move (m_forward));
  }

  /*
   *  operator members
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::operator-= (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &other)
  {
    m_forward -= other.m_forward;
    m_inverse -= other.m_inverse;
    return *this;
  }

  /// Returns the union with another interval dictionary
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::operator+= (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &other)
  {
    m_forward += other.m_forward;
    m_inverse += other.m_inverse;
    return *this;
  }

  // Doxygen has problems matching this with the declaration
  /// @cond Suppress_Doxygen_Warning
  template<typename A,
           typename B,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  template<typename C, typename OtherImpl, typename OtherInverseImpl>
  BiIntervalDictExp<
    A,
    C,
    Interval,
    typename Implementation<B, Interval, Impl>::template rebind<C>::type,
    InverseImpl>
  BiIntervalDictExp<A, B, Interval, Impl, InverseImpl>::joined_to (
    const BiIntervalDictExp<B, C, Interval, OtherImpl, OtherInverseImpl>
      &b_to_c) const
  {
    auto forward = m_forward.joined_to (b_to_c.m_forward);
    // Since we are creating forward from scratch, there is little advantage in
    // doing the same operation with m_inverse rather than just inverting the
    // results of the forward operation
    auto inverse = forward.invert ();
    return BiIntervalDictExp<A, C, Interval, OtherImplType<C>, InverseImpl> {
      std::move (forward), std::move (inverse)};
  }
  /// @endcond

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::fill_gaps_with (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &other)
  {
    const auto insertions
      = details::fill_gaps_with_inserts (m_forward, other.m_forward);
    m_forward.insert (insertions);
    m_inverse.inverse_insert (insertions);
    return *this;
  }

  /*
   * Gap filling
   */
  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::fill_to_start (
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
  {
    const auto insertions = details::fill_to_start_inserts (
      m_forward, starting_point, max_extension);
    m_forward.insert (insertions);
    m_inverse.inverse_insert (insertions);
    return *this;
  }
  /// @endcond

  /// @cond Suppress_Doxygen_Warning
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::fill_to_end (
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
  {
    const auto insertions
      = details::fill_to_end_inserts (m_forward, starting_point, max_extension);
    m_forward.insert (insertions);
    m_inverse.inverse_insert (insertions);
    return *this;
  }
  /// @endcond

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::extend_into_gaps (
    GapExtensionDirection gap_extension_direction,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
  {
    const auto insertions = details::extend_into_gaps_inserts (
      m_forward, gap_extension_direction, max_extension);
    m_forward.insert (insertions);
    m_inverse.inverse_insert (insertions);
    return *this;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::fill_gaps (
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
  {
    const auto insertions
      = details::fill_gaps_inserts (m_forward, max_extension);
    m_forward.insert (insertions);
    m_inverse.inverse_insert (insertions);
    return *this;
  }

  /*
   * Comparisons
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  bool BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::operator== (
    const BiIntervalDictExp &rhs) const
  {
    return m_forward == rhs.m_forward;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  bool BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>::operator!= (
    const BiIntervalDictExp &rhs) const
  {
    return !(rhs == *this);
  }

  /*
   * Associated functions:
   *
   * intervals()
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
               &interval_dict,
             std::vector<Key> keys,
             const Interval query_interval)
  {
    return intervals (
      interval_dict.m_forward, std::move (keys), query_interval);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
               &interval_dict,
             const Key &key,
             const Interval query_interval)
  {
    return intervals (interval_dict, std::vector {key}, query_interval);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
               &interval_dict,
             const Interval query_interval)
  {
    return intervals (interval_dict, interval_dict.keys (), query_interval);
  }

  /*
   * disjoint_intervals
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
      &interval_dict,
    std::vector<Key> keys,
    const Interval query_interval)
  {
    return disjoint_intervals (
      interval_dict.m_forward, std::move (keys), query_interval);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
      &interval_dict,
    const Key &key,
    const Interval query_interval)
  {
    return disjoint_intervals (
      interval_dict, std::vector {key}, query_interval);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
      &interval_dict,
    const Interval query_interval)
  {
    return disjoint_intervals (
      interval_dict, interval_dict.keys (), query_interval);
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> subtract (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2)
  {
    dict_1 -= dict_2;
    return dict_1;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> operator- (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2)
  {
    dict_1 -= dict_2;
    return dict_1;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> operator+ (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2)
  {
    dict_1 += dict_2;
    return dict_1;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> merge (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> &dict_2)
  {
    dict_1 += dict_2;
    return dict_1;
  }

  /*
   * stream output operator
   */
  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  std::ostream &
  operator<< (std::ostream &ostream,
              const BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl>
                &interval_dict)
  {
    ostream << interval_dict.m_forward;
    return ostream;
  }

  template<typename Key,
           typename Value,
           typename Interval,
           typename Impl,
           typename InverseImpl>
  BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> flattened (
    BiIntervalDictExp<Key, Value, Interval, Impl, InverseImpl> interval_dict,
    FlattenPolicy<typename details::identity<Key>::type,
                  typename details::identity<Value>::type,
                  typename details::identity<Interval>::type> keep_one_value)
  {
    const auto [insertions, erasures]
      = details::flatten_actions (interval_dict.m_forward, keep_one_value);
    interval_dict.insert (insertions);
    interval_dict.erase (erasures);
    return interval_dict;
  }

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_H
