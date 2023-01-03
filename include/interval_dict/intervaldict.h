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

#include "interval_compare.h"
#include "interval_operators.h"
#include "interval_traits.h"
#include "std_ranges_23_patch.h"

#include <cppcoro/generator.hpp>
// #include <range/v3/algorithm/sort.hpp>
// #include <range/v3/range/conversion.hpp>
// #include <range/v3/view/map.hpp>

#include <boost/icl/interval_set.hpp>

#include <iostream>
#include <map>
#include <ranges>
#include <tuple>
#include <unordered_set>

// TODO
// FillPolicy::UNSAFE_FORWARDS
// FillPolicy::UNSAFE_BACKWARDS
// FillPolicy::UNSAFE_BOTHWAYS
// FillPolicy::GAPS
// FillPolicy::INTERRUPTIONS
// LargeGapExtendPolicy::PARTIAL
// LargeGapExtendPolicy::NONE

// TODO
// transform_values
// transform_keys
// self-join

namespace interval_dict
{
  /// Code for different internal implementations of interval associative
  /// dictionaries live in this namespace
  namespace implementation
  {
  }

  /// \brief Set of disjoint intervals
  template<typename Interval>
  using Intervals = boost::icl::interval_set<
    typename boost::icl::interval_traits<Interval>::domain_type,
    std::less,
    Interval>;

  /// \brief Whether to use values from one side or another to fill in gaps
  /// (Forward fill, or backward fill or both).
  /// See extend_into_gaps()
  enum class GapExtensionDirection
  {
    Backwards = 1,
    Forwards = 2,
    Both = 3
  };

  // forward declaration of IntervalDictExp
  template<typename Key, typename Value, typename Interval, typename Impl>
  class IntervalDictExp;

  /* _____________________________________________________________________________
   *
   * Forward declarations of associated functions whose definitions are in
   */

  /// Asymmetrical difference between two interval dictionaries
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  subtract (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
            const IntervalDictExp<Key, Value, Interval, Impl> &dict_2);

  /// The union of two interval dictionaries in a new IntervalDict
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  merge (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
         const IntervalDictExp<Key, Value, Interval, Impl> &dict_2);

  /// Constituent intervals and their values for the
  /// specified \p key. <br>Silently ignores missing keys
  ///
  /// \param interval_dict The input interval dictionary
  /// \param key Only show intervals for the specified key
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return A sorted std::vector of std::tuple<key, value, interval>
  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
             const Key &key,
             Interval query_interval = interval_extent<Interval>);

  /// Constituent intervals for specified \p keys
  ///
  /// Silently ignores missing keys
  /// \param interval_dict
  /// \param keys Only show intervals for the specified keys
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, value, interval>
  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
             std::vector<Key> keys,
             Interval query_interval = interval_extent<Interval>);

  /// All constituent intervals
  ///
  /// \param interval_dict
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, value, interval>
  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
             Interval query_interval = interval_extent<Interval>);

  /// All mapped values for each disjoint, non-overlapping interval
  /// for the specified keys and overlapping the query interval.
  ///
  /// Silently ignores missing keys.
  /// \param interval_dict
  /// \param keys Only show intervals for the specified keys
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, std::vector<value>,
  /// interval>
  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
    std::vector<Key> keys,
    Interval query_interval = interval_extent<Interval>);

  /// All mapped values for each disjoint, non-overlapping interval
  /// for the specified key and overlapping the query interval.
  ///
  /// Silently ignores missing keys.
  /// \param interval_dict
  /// \param key Only show intervals for the specified key
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, std::vector<value>,
  /// interval>
  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
    const Key &key,
    Interval query_interval = interval_extent<Interval>);

  /// All mapped values for each disjoint, non-overlapping interval
  /// for all keys and overlapping the query interval.
  ///
  /// Silently ignores missing keys.
  /// \param interval_dict
  /// \param query_interval Restrict to intervals intersecting with the query
  /// \return sorted std::vector of std::tuple<key, std::vector<value>,
  /// interval>
  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
    Interval query_interval = interval_extent<Interval>);

  /// Asymmeterical difference from dict_1 to dict_2
  /// \return a new IntervalDict
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  operator- (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
             const IntervalDictExp<Key, Value, Interval, Impl> &dict_2);

  /// Merges key values in dict_1 with dict_2
  /// \return a new IntervalDict
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  operator+ (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
             const IntervalDictExp<Key, Value, Interval, Impl> &dict_2);

  /// \brief Policy for flattening dictionaries used by flatten()
  template<typename Key, typename Value, typename Interval>
  using FlattenPolicy
    = std::function<std::optional<Value> (const std::optional<Value> &,
                                          Interval,
                                          const Key &,
                                          const std::vector<Value> &)>;

  /// \brief Policy that simply discards data for intervals where a key maps to
  /// multiple values
  struct FlattenPolicyDiscard
  {
    /// \brief Policy that simply discards data for intervals where a key maps
    /// to multiple values. Implemented as non-template function objects with
    /// templated operator() function call operators.
    /// This can be passed without (re-)specifying template arguments
    template<typename Key, typename Value, typename Interval>
    std::optional<Value> operator() (const std::optional<Value> &,
                                     Interval,
                                     const Key &,
                                     const std::vector<Value> &);
  };

  /// \brief Policy that simply discards data for intervals where a key maps to
  /// multiple values
  FlattenPolicyDiscard flatten_policy_discard ();

  /// \brief Policy that prefers to extend the preceding interval if possible.
  template<typename FlattenPolicy>
  struct FlattenPolicyPreferStatusQuo
  {
    /// \brief Policy that prefers to extend the preceding interval if possible.
    /// Otherwise, uses the supplied (nested) fallback policy. Implemented as
    /// non-template function objects with templated operator() function call
    /// operators so can be passed without (re-)specifying template arguments.
    /// The templated type is only for the nested fallback policy.
    template<typename Key, typename Value, typename Interval>
    std::optional<Value> operator() (const std::optional<Value> &,
                                     Interval,
                                     const Key &,
                                     const std::vector<Value> &);

    /// Constructor for a flattening policy that prefers to extend the
    /// preceding interval if possible. Otherwise, uses a nested fallback
    /// parameter
    FlattenPolicyPreferStatusQuo (FlattenPolicy fallback_policy)
      : fallback_policy (fallback_policy)
    {
    }

    /// @cond Suppress_Doxygen_Warning
    FlattenPolicy fallback_policy;
    /// @endcond
  };

  /// \brief Policy that prefers to extend the preceding interval if possible.
  /// Otherwise, uses the supplied (nested) fallback policy
  template<typename FlattenPolicy = FlattenPolicyDiscard>
  FlattenPolicyPreferStatusQuo<FlattenPolicy>
  flatten_policy_prefer_status_quo (FlattenPolicy fallback_policy
                                    = flatten_policy_discard ());

  /// @cond Suppress_Doxygen_Warning
  namespace details
  {
    // Exclude specific arguments from deduction for flattened()
    // (available as std::type_identity as of C++20)
    // Otherwise, we would, for example, have to use a cast from lambda to
    // std::function
    template<typename T>
    struct identity
    {
      typedef T type;
    };
  } // namespace details
  /// @endcond

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
  template<typename Key, typename Value, typename Interval, typename Impl>
  [[nodiscard]] IntervalDictExp<Key, Value, Interval, Impl> flattened (
    IntervalDictExp<Key, Value, Interval, Impl> interval_dict,
    FlattenPolicy<typename details::identity<Key>::type,
                  typename details::identity<Value>::type,
                  typename details::identity<Interval>::type> keep_one_value
    =
      // flatten_policy_prefer_status_quo<Key, Value, Interval>());
    flatten_policy_prefer_status_quo ());

  /// output streaming operator: prints disjoint intervals
  template<typename Key, typename Value, typename Interval, typename Impl>
  std::ostream &
  operator<< (std::ostream &ostream,
              const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict);

  /// @cond Suppress_Doxygen_Warning
  namespace details
  {
    /*
     * Insertion and Erasure types
     */
    template<typename Key, typename Value, typename Interval>
    using Insertions = std::vector<KeyValueInterval<Key, Value, Interval>>;
    template<typename Key, typename Value, typename Interval>
    using Erasures = std::vector<KeyValueInterval<Key, Value, Interval>>;

    // Helper function for subset()
    // returns a vector for passing to insert()
    template<typename Key,
             typename Value,
             typename Interval,
             typename Impl,
             typename KeyRange,
             typename ValRange>
    Insertions<Key, Value, Interval> subset_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      const KeyRange &keys_subset,
      const ValRange &values_subset,
      Interval query_interval);

    // Helper function for subset()
    // returns a vector for passing to insert()
    template<typename Key,
             typename Value,
             typename Interval,
             typename Impl,
             typename KeyRange>
    Insertions<Key, Value, Interval> subset_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      const KeyRange &keys_subset,
      Interval query_interval);

    // Helper function for fill_gaps_with()
    // returns a vector for passing to insert()
    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_gaps_with_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      const IntervalDictExp<Key, Value, Interval, Impl> &other);

    // fill_to_start_inserts()
    // Helper function for fill_to_start()
    // returns a vector for passing to insert()
    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_to_start_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      typename IntervalTraits<Interval>::BaseType starting_point
      = IntervalTraits<Interval>::maximum (),
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

    // fill_to_end_inserts()
    // Helper function for fill_to_end()
    // returns a vector for passing to insert()
    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_to_end_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      typename IntervalTraits<Interval>::BaseType starting_point
      = IntervalTraits<Interval>::minimum (),
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

    // extend_into_gaps_inserts()
    // Helper function for extend_into_gaps()
    // returns a vector for passing to insert()
    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> extend_into_gaps_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      GapExtensionDirection gap_extension_direction
      = GapExtensionDirection::Both,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

    // fill_gaps_inserts()
    // Helper function for fill_gaps()
    // returns a vector for passing to insert()
    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_gaps_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

    // flatten_actions()
    // Helper function for flatten()
    // returns a tuple of vectors for passing to insert() and erase()
    template<typename Key, typename Value, typename Interval, typename Impl>
    std::tuple<Insertions<Key, Value, Interval>, Erasures<Key, Value, Interval>>
    flatten_actions (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      FlattenPolicy<Key, Value, Interval> keep_one_value);

  } // namespace details
  /// @endcond

  /**
   * @brief One-to-many dictionary where key-values vary over intervals.
   *
   *  Typically used for time-varying dictionaries.
   *
   *  `IntervalDictExp` is useful for specifying the exact inclusive or
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
   */
  //
  // Some IntervalDict functionality is implemented in two steps:
  // 1) functions in the details namespace that return a list of insertion or
  //    erase operations
  // 2) Calling insert/erase with these lists
  // This is less efficient because it requires storage for the intermediate
  // results but make the operations much more straightforward. It also allows
  // bidirectional maps to be implemented using one set of operations.
  //
  template<typename Key, typename Value, typename Interval, typename Impl>
  class IntervalDictExp
  {
    public:
    // friend of all other IntervalDictExp
    template<typename K, typename V, typename I, typename Im>
    friend class IntervalDictExp;

    /// @cond Suppress_Doxygen_Warning
    using IntervalType = Interval;
    using BaseType = typename IntervalTraits<Interval>::BaseType;
    using KeyType = Key;
    using ValType = Value;
    using BaseDifferenceType =
      typename IntervalTraits<Interval>::BaseDifferenceType;
    // boost icl interval_set of Intervals
    using Intervals = interval_dict::Intervals<Interval>;
    using ImplType = Impl;
    using InverseImplType =
      typename Implementation<Value, Interval, Impl>::template rebind<
        Key>::type;
    template<typename OtherVal>
    using OtherImplType =
      typename Implementation<Value, Interval, Impl>::template rebind<
        OtherVal>::type;
    using DataType = std::map<Key, Impl>;
    using KeyValueIntervals = std::vector<std::tuple<Key, Value, Interval>>;
    /// @endcond

    /// @name Constructors
    /// @{
    /// Constructors and assignment operators

    /// Default Constructor
    IntervalDictExp () = default;
    /// Default copy constructor
    IntervalDictExp (const IntervalDictExp &) = default;
    /// Default move constructor
    IntervalDictExp (IntervalDictExp &&other) noexcept = default;
    /// Default copy assignment operator
    IntervalDictExp &operator= (const IntervalDictExp &other) = default;
    /// Default move assignment operator
    IntervalDictExp &operator= (IntervalDictExp &&other) noexcept = default;

    /// Construct from a vector of [key-value-interval]s
    explicit IntervalDictExp (
      const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals);

    /// Construct from underlying data type
    explicit IntervalDictExp (IntervalDictExp::DataType internal);

    /// @}
    /// @name Insert and Erase Member Functions
    /// Inserting into and removing from the dictionary
    /// @{

    /// Insert key-value pairs valid over the specified interval
    ///
    /// For batch inserting multiple key-value for a single interval.
    /// \param key_value_pairs is a vector of key-value
    /// \param interval defaults to `interval_extent`, i.e. The key values
    /// associations are always valid
    ///
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
    insert (const std::vector<std::pair<Key, Value>> &key_value_pairs,
            Interval interval = interval_extent<Interval>);

    /// Insert key-value-intervals
    ///
    /// For batch inserting key-values each for a different interval.
    /// \param key_value_intervals is a vector of Key-Value-Interval
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &insert (
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
    IntervalDictExp<Key, Value, Interval, Impl> &
    insert (const std::vector<std::pair<Key, Value>> &key_value_pairs,
            IntervalDictExp::BaseType first,
            IntervalDictExp::BaseType last
            = IntervalTraits<Interval>::maximum ());

    /// Insert key-value pair valid from @p first to @p last
    ///
    /// For inserting single key-value over a time or date span.
    /// \param key_value_pair is a pair of key-value
    /// \param first is the first point from which the data will be valid
    /// \param last is the last point for which the data will be valid. Defaults
    /// to `max()` i.e. the mapping will always be valid from first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the
    /// underlying interval type is open/close etc.
    ///
    /// TODO: Write
    ///
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
    insert (std::pair<Key, Value> &key_value_pair,
            IntervalDictExp::BaseType first,
            IntervalDictExp::BaseType last
            = IntervalTraits<Interval>::maximum ());

    /// Insert value-key-intervals
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting values-keys each for a different interval.
    /// \param value_key_intervals is a vector of Value-Key-Interval
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &inverse_insert (
      const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals);

    /// Insert value-key pairs valid over the specified interval
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch inserting multiple value-key for a single interval.
    /// \param value_key_pairs is a vector of value-key
    /// \param interval defaults to `interval_extent`, i.e. The key values
    /// associations are always valid \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
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
    IntervalDictExp<Key, Value, Interval, Impl> &
    inverse_insert (const std::vector<std::pair<Value, Key>> &value_key_pairs,
                    IntervalDictExp::BaseType first,
                    IntervalDictExp::BaseType last
                    = IntervalTraits<Interval>::maximum ());

    /// Erase key-value-intervals
    ///
    /// For batch erasing key-values each for a different interval.
    /// \param key_value_intervals is a vector of Key-Value-Interval
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &erase (
      const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals);

    /// Erase key-value pairs over the specified interval
    ///
    /// For batch erasing multiple key-value for a single interval.
    /// \param key_value_pairs is a vector of key-value
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
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
    IntervalDictExp<Key, Value, Interval, Impl> &
    erase (const std::vector<std::pair<Key, Value>> &key_value_pairs,
           IntervalDictExp::BaseType first,
           IntervalDictExp::BaseType last
           = IntervalTraits<Interval>::maximum ());

    /// Erase all values for @p key over the specified @p interval
    ///
    /// \param key
    /// \param interval defaults to `interval_extent`, i.e. All data for @p key
    /// are removed over all intervals
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
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
    IntervalDictExp<Key, Value, Interval, Impl> &
    erase (const Key &key,
           IntervalDictExp::BaseType first,
           IntervalDictExp::BaseType last
           = IntervalTraits<Interval>::maximum ());

    /// Erase all values over the given @p interval.
    ///
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &erase (Interval interval);

    /// Erase all values from @p first to @p last
    ///
    /// \param first is the first point from which the data will be erased
    /// \param last is the last point for which the data will be erased.
    /// Defaults to `max()` i.e. all data matching key-value will be erased from
    /// first onwards.
    ///
    /// The exact interpretation of first and last depends on whether the
    /// underlying interval type is open/close etc. \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
    erase (IntervalDictExp::BaseType first,
           IntervalDictExp::BaseType last
           = IntervalTraits<Interval>::maximum ());

    /// Erase value-key-intervals
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// For batch erasing key-values each for a different interval.
    /// \param value_key_intervals is a vector of Value-Key-Interval
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &inverse_erase (
      const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals);

    /// Erase all values-keys over the specified @p interval
    ///
    /// N.B. key/values are specified in swapped order
    ///
    /// \param value_key_pairs is a vector of value-key
    /// \param interval defaults to `interval_extent`, i.e. All data for @p key
    /// are removed over all intervals
    /// \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
    inverse_erase (const std::vector<std::pair<Value, Key>> &value_key_pairs,
                   Interval interval = interval_extent<Interval>);

    /// Erase all data for all keys
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
    [[nodiscard]] std::vector<Value>
    find (const Key &key, IntervalDictExp::BaseType query) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval from @p first to @p last
    /// \param key
    /// \param first The first point at which values may match @p key
    /// \param last The last point at which values may match @p key
    /// \return std::vector of matching values
    [[nodiscard]] std::vector<Value>
    find (const Key &key,
          IntervalDictExp::BaseType first,
          IntervalDictExp::BaseType last) const;

    /// Returns all mapped values in a sorted list for the specified @p key on
    /// the given query interval
    /// \param key
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals
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

    /// @}

    /// @name Gap-filling Member Functions
    /// @{
    /// Filling gaps in the mapping from key to value
    /// This is usually necessary to remedy data errors or dropouts

    /// Supplement with entries from @p other only for missing keys or gaps
    /// where a key does not map to any values. \return *this
    IntervalDictExp<Key, Value, Interval, Impl> &
    fill_gaps_with (const IntervalDictExp<Key, Value, Interval, Impl> &other);

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
    IntervalDictExp<Key, Value, Interval, Impl> &
    fill_to_start (IntervalDictExp::BaseType starting_point
                   = IntervalTraits<Interval>::maximum (),
                   IntervalDictExp::BaseDifferenceType max_extension
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
    IntervalDictExp<Key, Value, Interval, Impl> &
    fill_to_end (IntervalDictExp::BaseType starting_point
                 = IntervalTraits<Interval>::minimum (),
                 IntervalDictExp::BaseDifferenceType max_extension
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
    IntervalDictExp<Key, Value, Interval, Impl> &extend_into_gaps (
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
    IntervalDictExp<Key, Value, Interval, Impl> &fill_gaps (
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension
      = IntervalTraits<Interval>::max_size ());

    /// @}

    /// Return all keys in sorted order
    [[nodiscard]] std::vector<Key> keys () const;

    /// Returns the number of unique keys
    [[nodiscard]] std::size_t size () const;

    /// Return whether there are no keys
    [[nodiscard]] bool empty () const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] std::size_t count (const Key &key) const;

    /// Return whether the specified key is in the dictionary
    [[nodiscard]] bool contains (const Key &key) const;

    /// Returns a new IntervalDict that contains only the specified \p keys for
    /// the specified \p interval
    /// \param keys Any sequence of Key (suitable for range-based for loop)
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals
    template<typename KeyRange>
    [[nodiscard]] IntervalDictExp<Key, Value, Interval, Impl>
    subset (const KeyRange &keys,
            Interval interval = interval_extent<Interval>) const;

    /// Returns a new IntervalDict that contains only the specified \p keys and
    /// \p values for the specified \p interval
    /// \param keys Any sequence of Key (suitable for range-based for loop)
    /// \param values Any sequence of Value (suitable for range-based for loop)
    /// \param interval defaults to `interval_extent`, i.e. All the specified
    /// key values associations are removed over all intervals
    template<typename KeyRange, typename ValRange>
    [[nodiscard]] IntervalDictExp<Key, Value, Interval, Impl>
    subset (const KeyRange &keys,
            const ValRange &values,
            Interval interval = interval_extent<Interval>) const;

    /// Returns a new Dictionary that contains the same intervals but with
    /// Values and Keys swapped. Note that unless Value and Key are identical
    /// types, the resulting IntervalDict will also have a new type.
    [[nodiscard]] IntervalDictExp<Value, Key, Interval, InverseImplType>
    invert () const;

    /// Joins to a second dictionary with matching values so that if
    /// *this and the parameter have key-value types of
    /// A -> B and B -> C respectively, returns a dictionary A -> C that
    /// spans A -> B -> C over common intervals
    template<typename OtherVal, typename OtherImpl>
    [[nodiscard]] IntervalDictExp<Key,
                                  OtherVal,
                                  Interval,
                                  OtherImplType<OtherVal>>
    joined_to (const IntervalDictExp<Value, OtherVal, Interval, OtherImpl>
                 &b_to_c) const;

    /// Returns the asymmetrical differences with another interval dictionary
    IntervalDictExp<Key, Value, Interval, Impl> &
    operator-= (const IntervalDictExp<Key, Value, Interval, Impl> &other);

    /// Returns the union with another interval dictionary
    IntervalDictExp<Key, Value, Interval, Impl> &
    operator+= (const IntervalDictExp<Key, Value, Interval, Impl> &other);

    /// Equality operator
    bool operator== (const IntervalDictExp &rhs) const;

    /// Inequality operator
    bool operator!= (const IntervalDictExp &rhs) const;

    // friends
    /// @cond Suppress_Doxygen_Warning
    friend IntervalDictExp operator-<> (IntervalDictExp dict_1,
                                        const IntervalDictExp &dict_2);

    friend IntervalDictExp operator+<> (IntervalDictExp dict_1,
                                        const IntervalDictExp &dict_2);

    friend IntervalDictExp subtract<> (IntervalDictExp dict_1,
                                       const IntervalDictExp &dict_2);

    friend IntervalDictExp merge<> (IntervalDictExp dict_1,
                                    const IntervalDictExp &dict_2);

    friend std::vector<std::tuple<Key, Value, Interval>>
    details::fill_gaps_with_inserts<> (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      const IntervalDictExp<Key, Value, Interval, Impl> &other);

    friend std::vector<std::tuple<Key, Value, Interval>>
    details::fill_to_start_inserts<> (
      const IntervalDictExp &interval_dict,
      BaseType starting_from,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::vector<std::tuple<Key, Value, Interval>>
    details::fill_to_end_inserts<> (
      const IntervalDictExp &interval_dict,
      BaseType starting_from,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::vector<std::tuple<Key, Value, Interval>>
    details::fill_gaps_inserts<> (
      const IntervalDictExp &interval_dict,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::vector<std::tuple<Key, Value, Interval>>
    details::extend_into_gaps_inserts<> (
      const IntervalDictExp &interval_dict,
      GapExtensionDirection gap_extension_direction,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension);

    friend std::tuple<details::Insertions<Key, Value, Interval>,
                      details::Erasures<Key, Value, Interval>>
    details::flatten_actions<> (
      const IntervalDictExp &interval_dict,
      FlattenPolicy<Key, Value, Interval> keep_one_value);

    template<typename Key_,
             typename Val_,
             typename Interval_,
             typename Impl_,
             typename KeyRange>
    friend std::vector<std::tuple<Key_, Val_, Interval_>>
    details::subset_inserts (
      const IntervalDictExp<Key_, Val_, Interval_, Impl_> &interval_dict,
      const KeyRange &keys_subset,
      Interval_ query_interval);

    template<typename Key_,
             typename Val_,
             typename Interval_,
             typename Impl_,
             typename KeyRange,
             typename ValRange>
    friend std::vector<std::tuple<Key_, Val_, Interval_>>
    details::subset_inserts (
      const IntervalDictExp<Key_, Val_, Interval_, Impl_> &interval_dict,
      const KeyRange &keys_subset,
      const ValRange &values_subset,
      Interval_ query_interval);

    friend cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
    intervals<> (const IntervalDictExp &interval_dict,
                 std::vector<Key> keys,
                 Interval query_interval);

    friend cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
    disjoint_intervals<> (const IntervalDictExp &interval_dict,
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
  /// \tparam Value Type of Values
  /// \tparam BaseType E.g. Time/Date: The type for intervals begin/ends
  /// \tparam Impl Implementation type. E.g. boost::icl::interval_map...
  template<typename Key, typename Value, typename BaseType, typename Impl>
  using IntervalDict
    = IntervalDictExp<Key,
                      Value,
                      typename boost::icl::interval<BaseType>::type,
                      Impl>;

  /* _____________________________________________________________________________
   *
   * Implementations of member functions
   *
   */

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>::IntervalDictExp (
    const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals)
  {
    insert (key_value_intervals);
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>::IntervalDictExp (
    IntervalDictExp::DataType internal)
    : data (std::move (internal))
  {
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  bool IntervalDictExp<Key, Value, Interval, Impl>::empty () const
  {
    return data.size () == 0;
  }

  /// Return whether the specified key is in the dictionary
  template<typename Key, typename Value, typename Interval, typename Impl>
  std::size_t
  IntervalDictExp<Key, Value, Interval, Impl>::count (const Key &key) const
  {
    return data.count (key);
  }

  /// Return whether the specified key is in the dictionary
  template<typename Key, typename Value, typename Interval, typename Impl>
  bool
  IntervalDictExp<Key, Value, Interval, Impl>::contains (const Key &key) const
  {
#if __cplusplus > 201703L
    return data.contains (key);
#else
    return data.count (key) != 0;
#endif
  }

  /// erase all keys
  template<typename Key, typename Value, typename Interval, typename Impl>
  void IntervalDictExp<Key, Value, Interval, Impl>::clear ()
  {
    return data.clear ();
  }

  /*
   * Insert
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::insert (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    Interval interval)
  {
    if (!boost::icl::is_empty (interval))
    {
      for (const auto &[key, value] : key_value_pairs)
      {
        Implementation<Value, Interval, Impl>::insert (
          data[key], interval, value);
      }
    }
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::insert (
    const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals)
  {
    for (const auto &[key, value, interval] : key_value_intervals)
    {
      if (!boost::icl::is_empty (interval))
      {
        Implementation<Value, Interval, Impl>::insert (
          data[key], interval, value);
      }
    }
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::insert (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    typename IntervalDictExp::BaseType first,
    typename IntervalDictExp::BaseType last)
  {
    return insert (key_value_pairs, Interval {first, last});
  }

  /*
   * Inverse insert
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::inverse_insert (
    const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals)
  {
    for (const auto &[value, key, interval] : value_key_intervals)
    {
      if (!boost::icl::is_empty (interval))
      {
        Implementation<Value, Interval, Impl>::insert (
          data[key], interval, value);
      }
    }
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::inverse_insert (
    const std::vector<std::pair<Value, Key>> &value_key_pairs,
    Interval interval)
  {
    if (!boost::icl::is_empty (interval))
    {
      for (const auto &[value, key] : value_key_pairs)
      {
        Implementation<Value, Interval, Impl>::insert (
          data[key], interval, value);
      }
    }
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::inverse_insert (
    const std::vector<std::pair<Value, Key>> &value_key_pairs,
    typename IntervalDictExp::BaseType first,
    typename IntervalDictExp::BaseType last)
  {
    return inverse_insert (value_key_pairs, Interval {first, last});
  }

  /*
   * Erase
   */
  /// @cond Suppress_Doxygen_Warning
  namespace details
  {
    template<typename Value, typename Interval, typename Key, typename Impl>
    void cleanup_empty_keys (std::map<Key, Impl> &data,
                             const std::set<Key> &keys_with_erases)
    {
      for (const auto &key : keys_with_erases)
      {
        if (Implementation<Value, Interval, Impl>::empty (data[key]))
        {
          data.erase (key);
        }
      }
    }
  } // namespace details
  /// @endcond

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::erase (
    const std::vector<std::tuple<Key, Value, Interval>> &key_value_intervals)
  {
    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto &[key, value, interval] : key_value_intervals)
    {
      if (!boost::icl::is_empty (interval))
      {
        keys_with_erases.insert (key);
        Implementation<Value, Interval, Impl>::erase (
          data[key], interval, value);
      }
    }
    details::cleanup_empty_keys<Value, Interval> (data, keys_with_erases);
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::erase (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    Interval interval)
  {
    if (boost::icl::is_empty (interval))
    {
      return *this;
    }

    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto &[key, value] : key_value_pairs)
    {
      keys_with_erases.insert (key);
      Implementation<Value, Interval, Impl>::erase (data[key], interval, value);
    }
    details::cleanup_empty_keys<Value, Interval> (data, keys_with_erases);
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::erase (
    const std::vector<std::pair<Key, Value>> &key_value_pairs,
    typename IntervalDictExp::BaseType first,
    typename IntervalDictExp::BaseType last)
  {
    return erase (key_value_pairs, Interval {first, last});
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::erase (const Key &key,
                                                      Interval query_interval)
  {
    if (boost::icl::is_empty (query_interval))
    {
      return *this;
    }

    // Do not assume key is valid
    const auto ff = data.find (key);
    if (ff == data.end ())
    {
      return *this;
    }

    Implementation<Value, Interval, Impl>::erase (data[key], query_interval);
    if (Implementation<Value, Interval, Impl>::empty (data[key]))
    {
      data.erase (key);
    }

    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::erase (
    const Key &key,
    typename IntervalDictExp::BaseType first,
    typename IntervalDictExp::BaseType last)
  {
    return erase (key, Interval {first, last});
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::erase (Interval query_interval)
  {
    if (boost::icl::is_empty (query_interval))
    {
      return *this;
    }

    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto &[key, interval_values] : data)
    {
      Implementation<Value, Interval, Impl>::erase (data[key], query_interval);
      keys_with_erases.insert (key);
    }
    details::cleanup_empty_keys<Value, Interval> (data, keys_with_erases);
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::erase (
    typename IntervalDictExp::BaseType first,
    typename IntervalDictExp::BaseType last)
  {
    return erase (Interval {first, last});
  }

  /*
   * inverse_erase
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::inverse_erase (
    const std::vector<std::tuple<Value, Key, Interval>> &value_key_intervals)
  {
    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto &[value, key, interval] : value_key_intervals)
    {
      if (!boost::icl::is_empty (interval))
      {
        Implementation<Value, Interval, Impl>::erase (
          data[key], interval, value);
        keys_with_erases.insert (key);
      }
    }
    details::cleanup_empty_keys<Value, Interval> (data, keys_with_erases);
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::inverse_erase (
    const std::vector<std::pair<Value, Key>> &value_key_pairs,
    Interval interval)
  {
    if (boost::icl::is_empty (interval))
    {
      return *this;
    }

    // cleanup keys without intervals afterwards
    std::set<Key> keys_with_erases;
    for (const auto &[value, key] : value_key_pairs)
    {
      Implementation<Value, Interval, Impl>::erase (data[key], interval, value);
      keys_with_erases.insert (key);
    }
    details::cleanup_empty_keys<Value, Interval> (data, keys_with_erases);
    return *this;
  }

  /*
   * find
   */
  // Returns all the values as a sorted vector for a specified key over the
  // specified intervals. Used for implementing the various forms of find
  template<typename Key, typename Value, typename Interval, typename Impl>
  std::vector<Value> IntervalDictExp<Key, Value, Interval, Impl>::find (
    const Key &key, const Intervals &query_intervals) const
  {
    // Do not assume key is valid
    const auto ff = data.find (key);
    if (ff == data.end ())
    {
      return {};
    }

    std::set<Value> unique_results;
    for (const auto &query_interval : query_intervals)
    {
      if (!boost::icl::is_empty (query_interval))
      {
        for (const auto &[value, _] :
             Implementation<Value, Interval, Impl>::intervals (ff->second,
                                                               query_interval))
        {
          unique_results.insert (value);
        }
      }
    }
    return {unique_results.begin (), unique_results.end ()};
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  std::vector<Value> IntervalDictExp<Key, Value, Interval, Impl>::find (
    const std::vector<Key> &keys, Interval query_interval) const
  {
    if (boost::icl::is_empty (query_interval))
    {
      return {};
    }
    std::set<Value> unique_results;

    // Do not assume key is valid
    for (const auto &key : keys)
    {
      const auto ff = data.find (key);
      if (ff == data.end ())
      {
        continue;
      }

      for (const auto &[value, _] :
           Implementation<Value, Interval, Impl>::intervals (ff->second,
                                                             query_interval))
      {
        unique_results.insert (value);
      }
    }
    return {unique_results.begin (), unique_results.end ()};
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  std::vector<Value> IntervalDictExp<Key, Value, Interval, Impl>::find (
    const Key &key, typename IntervalDictExp::BaseType query) const
  {
    return find (key, Interval {query, query});
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  std::vector<Value> IntervalDictExp<Key, Value, Interval, Impl>::find (
    const Key &key,
    typename IntervalDictExp::BaseType first,
    typename IntervalDictExp::BaseType last) const
  {
    return find (std::vector {key}, Interval {first, last});
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  std::vector<Value>
  IntervalDictExp<Key, Value, Interval, Impl>::find (const Key &key,
                                                     Interval interval) const
  {
    return find (std::vector {key}, interval);
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  std::vector<Key> IntervalDictExp<Key, Value, Interval, Impl>::keys () const
  {
    using legacy_ranges_conversion::to_vector;
    return data | std::ranges::views::keys | to_vector ();
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  std::size_t IntervalDictExp<Key, Value, Interval, Impl>::size () const
  {
    return data.size ();
  }

  /*
   * subset
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  template<typename KeyRange, typename ValRange>
  IntervalDictExp<Key, Value, Interval, Impl>
  IntervalDictExp<Key, Value, Interval, Impl>::subset (
    const KeyRange &keys_subset,
    const ValRange &values_subset,
    Interval query_interval) const
  {
    if (boost::icl::is_empty (query_interval))
    {
      return {};
    }

    return IntervalDictExp<Key, Value, Interval, Impl> ().insert (
      details::subset_inserts (
        *this, keys_subset, values_subset, query_interval));
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  template<typename KeyRange>
  IntervalDictExp<Key, Value, Interval, Impl>
  IntervalDictExp<Key, Value, Interval, Impl>::subset (
    const KeyRange &keys_subset, Interval query_interval) const
  {
    if (boost::icl::is_empty (query_interval))
    {
      return {};
    }
    return IntervalDictExp<Key, Value, Interval, Impl> ().insert (
      details::subset_inserts (*this, keys_subset, query_interval));
  }

  /*
   * invert
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<
    Value,
    Key,
    Interval,
    typename Implementation<Value, Interval, Impl>::template rebind<Key>::type>
  IntervalDictExp<Key, Value, Interval, Impl>::invert () const
  {
    using InverseDataType =
      typename IntervalDictExp<Value, Key, Interval, InverseImplType>::DataType;
    InverseDataType inverted_data;
    for (const auto &[key, interval_values] : data)
    {
      for (const auto &[value, interval] :
           Implementation<Value, Interval, Impl>::intervals (
             interval_values, interval_extent<Interval>))
      {
        Implementation<Key, Interval, InverseImplType>::insert (
          inverted_data[value], interval, key);
      }
    }
    return IntervalDictExp<Value, Key, Interval, InverseImplType> (
      inverted_data);
  }

  /*
   *  operator members
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::operator-= (
    const IntervalDictExp<Key, Value, Interval, Impl> &other)
  {
    // Iterate using the vector of keys() is a much more conservative choice at
    // the cost of making a copy of the keys:
    // makes sure we never change the std::map in the
    // middle of key iteration for example in the course of `myself -= myself;`
    for (auto &key : keys ())
    {
      if (const auto f = other.data.find (key); f != other.data.end ())
      {
        auto &interval_values = data.find (key)->second;
        Implementation<Value, Interval, Impl>::subtract_by (interval_values,
                                                            f->second);
        // remove empty keys
        if (Implementation<Value, Interval, Impl>::empty (interval_values))
        {
          data.erase (key);
        }
      }
    }
    return *this;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::operator+= (
    const IntervalDictExp<Key, Value, Interval, Impl> &other)
  {
    for (const auto &[key_other, interval_values_other] : other.data)
    {
      auto f = data.find (key_other);
      if (f == data.end ())
      {
        data[key_other] = interval_values_other;
      }
      else
      {
        Implementation<Value, Interval, Impl>::merged_with (
          f->second, interval_values_other);
      }
    }
    return *this;
  }

  // Doxygen has problems matching this with the declaration
  /// @cond Suppress_Doxygen_Warning
  template<typename A, typename B, typename Interval, typename Impl>
  template<typename C, typename OtherImpl>
  IntervalDictExp<
    A,
    C,
    Interval,
    typename Implementation<B, Interval, Impl>::template rebind<C>::type>
  IntervalDictExp<A, B, Interval, Impl>::joined_to (
    const IntervalDictExp<B, C, Interval, OtherImpl> &b_to_c) const
  {
    using ReturnImplType = OtherImplType<C>;
    using ReturnType = IntervalDictExp<A, C, Interval, ReturnImplType>;
    using ReturnDataType = typename ReturnType::DataType;
    ReturnDataType return_data;
    for (const auto &[key_a, interval_values_ab] : data)
    {
      for (const auto &[value_b, interval_ab] :
           Implementation<B, Interval, Impl>::intervals (
             interval_values_ab, interval_extent<Interval>))
      {
        // Ignore values that are missing from the other dictionary
        const auto ii = b_to_c.data.find (value_b);
        if (ii == b_to_c.data.end ())
        {
          continue;
        }

        // look up b->c values that overlap the a->b interval
        const auto &interval_values_bc = ii->second;
        for (const auto &[value_c, interval_bc] :
             Implementation<C, Interval, OtherImpl>::intervals (
               interval_values_bc, interval_ab))
        {
          Implementation<C, Interval, ReturnImplType>::insert (
            return_data[key_a], interval_ab & interval_bc, value_c);
        }
      }
    }
    return ReturnType (return_data);
  }
  /// @endcond

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::fill_gaps_with (
    const IntervalDictExp<Key, Value, Interval, Impl> &other)
  {
    return insert (details::fill_gaps_with_inserts (*this, other));
  }

  /*
   * Gap filling
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::fill_to_start (
    typename IntervalDictExp::BaseType starting_point,
    typename IntervalDictExp::BaseDifferenceType max_extension)
  {
    return insert (
      details::fill_to_start_inserts (*this, starting_point, max_extension));
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::fill_to_end (
    typename IntervalDictExp::BaseType starting_point,
    typename IntervalDictExp::BaseDifferenceType max_extension)
  {
    return insert (
      details::fill_to_end_inserts (*this, starting_point, max_extension));
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::extend_into_gaps (
    GapExtensionDirection gap_extension_direction,
    typename IntervalDictExp::BaseDifferenceType max_extension)
  {
    return insert (details::extend_into_gaps_inserts (
      *this, gap_extension_direction, max_extension));
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> &
  IntervalDictExp<Key, Value, Interval, Impl>::fill_gaps (
    typename IntervalDictExp::BaseDifferenceType max_extension)
  {
    return insert (details::fill_gaps_inserts (*this, max_extension));
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  bool IntervalDictExp<Key, Value, Interval, Impl>::operator== (
    const IntervalDictExp &rhs) const
  {
    return data == rhs.data;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  bool IntervalDictExp<Key, Value, Interval, Impl>::operator!= (
    const IntervalDictExp &rhs) const
  {
    return !(rhs == *this);
  }

  /// @cond Suppress_Doxygen_Warning
  namespace details
  {
    // Helper function for subset()
    // returns a vector for passing to insert()
    template<typename Key,
             typename Value,
             typename Interval,
             typename Impl,
             typename KeyRange,
             typename ValRange>
    Insertions<Key, Value, Interval> subset_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      const KeyRange &keys_subset,
      const ValRange &values_subset,
      Interval query_interval)
    {
      Insertions<Key, Value, Interval> results;
      std::unordered_set<Key> unique_keys_subset (std::begin (keys_subset),
                                                  std::end (keys_subset));
      std::unordered_set<Value> unique_values_subset (
        std::begin (values_subset), std::end (values_subset));
      for (const auto &[key, interval_values] : interval_dict.data)
      {
        // Ignore non matching keys
        if (unique_keys_subset.count (key))
        {
          for (const auto &[value, interval] :
               Implementation<Value, Interval, Impl>::intervals (
                 interval_values, query_interval))
          {
            // Only add interval if there are any matching specified values
            if (unique_values_subset.count (value) != 0)
            {
              results.push_back ({key, value, interval & query_interval});
            }
          }
        }
      }
      return results;
    }

    // Helper function for subset()
    // returns a vector for passing to insert()
    template<typename Key,
             typename Value,
             typename Interval,
             typename Impl,
             typename KeyRange>
    Insertions<Key, Value, Interval> subset_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      const KeyRange &keys_subset,
      Interval query_interval)
    {
      // Insertions<Key, Value, Interval> results;
      std::vector<std::tuple<Key, Value, Interval>> results;

      std::unordered_set<Key> unique_keys_subset (std::begin (keys_subset),
                                                  std::end (keys_subset));
      for (const auto &[key, interval_values] : interval_dict.data)
      {
        // Ignore non matching keys
        if (unique_keys_subset.count (key))
        {
          for (const auto &[value, interval] :
               Implementation<Value, Interval, Impl>::intervals (
                 interval_values, query_interval))
          {
            results.push_back (
              std::tuple {key, value, interval & query_interval});
          }
        }
      }
      return results;
    }

    // xxx_inserts() are helper functions for implementing xxx() in conjunction
    // with insert()
    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_gaps_with_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      const IntervalDictExp<Key, Value, Interval, Impl> &other)
    {
      Insertions<Key, Value, Interval> results;
      const auto &data = interval_dict.data;
      for (const auto &[key_other, interval_values_other] : other.data)
      {
        // If the key is absent, copy all values willy-nilly from other
        auto f = data.find (key_other);
        if (f == data.end ())
        {
          for (const auto &[value, interval] :
               Implementation<Value, Interval, Impl>::intervals (
                 interval_values_other, interval_extent<Interval>))
          {
            results.push_back ({key_other, value, interval});
          }
          continue;
        }

        // Otherwise only insert values for gaps
        auto &interval_values = f->second;
        for (const auto &gap_interval :
             Implementation<Value, Interval, Impl>::gaps (interval_values))
        {
          for (const auto &[value, interval] :
               Implementation<Value, Interval, Impl>::intervals (
                 interval_values_other, gap_interval))
          {
            results.push_back ({key_other, value, interval & gap_interval});
          }
        }
      }
      return results;
    }

    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_to_start_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      typename IntervalTraits<Interval>::BaseType starting_point,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
    {
      using namespace boost::icl;
      Insertions<Key, Value, Interval> results;
      const auto interval_min = IntervalTraits<Interval>::minimum ();
      const Interval query_interval {interval_min, starting_point};
      for (const auto &[key, interval_values] : interval_dict.data)
      {
        // Make sure first interval is at or before starting_point
        const auto &[values, interval]
          = Implementation<Value, Interval, Impl>::initial_values (
            interval_values);
        if (!comparisons::intersects (query_interval, interval))
        {
          continue;
        }

        // Don't extend fill interval by more than max_extension
        const auto fill_interval = right_subtract (
          operators::left_extend (interval, max_extension), interval);
        for (const auto &val : values)
        {
          results.push_back ({key, val, fill_interval});
        }
      }
      return results;
    }

    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_to_end_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      typename IntervalTraits<Interval>::BaseType starting_point,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
    {
      using namespace boost::icl;
      Insertions<Key, Value, Interval> results;
      const auto interval_max = IntervalTraits<Interval>::maximum ();
      const Interval query_interval {starting_point, interval_max};

      for (const auto &[key, interval_values] : interval_dict.data)
      {
        // Make sure first interval is at or before starting_point
        const auto [values, interval]
          = Implementation<Value, Interval, Impl>::final_values (
            interval_values);
        if (!comparisons::intersects (query_interval, interval))
        {
          continue;
        }

        // Don't extend fill interval by more than max_extension
        auto fill_interval = left_subtract (
          operators::right_extend (interval, max_extension), interval);
        for (const auto &val : values)
        {
          results.push_back ({key, val, fill_interval});
        }
      }
      return results;
    }

    // Helper function for fill_gaps_inserts() and extend_into_gaps_inserts()
    template<typename Key, typename Value, typename Interval>
    void add_values_to_gap (Insertions<Key, Value, Interval> &results,
                            const Key &key,
                            const std::vector<Value> &values,
                            const Interval gap_interval)
    {
      for (const auto &val : values)
      {
        results.push_back ({key, val, gap_interval});
      }
    }

    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> fill_gaps_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
    {
      using namespace boost::icl;
      Insertions<Key, Value, Interval> results;
      for (const auto &[key, interval_values] : interval_dict.data)
      {
        for (const auto &[values1, gap_interval, values2] :
             Implementation<Value, Interval, Impl>::sandwiched_gaps (
               interval_values))
        {
          // only fill if there are the common values that are on both sides
          std::vector<Value> common_values;
          std::set_intersection (
            values1.begin (),
            values1.end (),
            values2.begin (),
            values2.end (),
            std::inserter (common_values, common_values.begin ()));
          if (common_values.empty ())
          {
            continue;
          }

          // Fill short gaps in their entirety
          if (length (gap_interval) <= max_extension)
          {
            add_values_to_gap (results, key, common_values, gap_interval);
          }

          // Fill from both sides
          // Make fill segments by adjusting the gap segment (adjustment)
          // to avoid being mired in open/close interval creation semantics
          else
          {
            const auto adjustment = length (gap_interval) - max_extension;
            const auto lower_gap = Interval {lower (gap_interval),
                                             upper (gap_interval) - adjustment};
            add_values_to_gap (results, key, common_values, lower_gap);
            const auto upper_gap = Interval {lower (gap_interval) + adjustment,
                                             upper (gap_interval)};
            add_values_to_gap (results, key, common_values, upper_gap);
          }
        }
      }
      return results;
    }

    template<typename Key, typename Value, typename Interval, typename Impl>
    Insertions<Key, Value, Interval> extend_into_gaps_inserts (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      GapExtensionDirection gap_extension_direction,
      typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
    {
      using namespace boost::icl;
      Insertions<Key, Value, Interval> results;
      for (const auto &[key, interval_values] : interval_dict.data)
      {
        for (const auto &[values1, gap_interval, values2] :
             Implementation<Value, Interval, Impl>::sandwiched_gaps (
               interval_values))
        {
          // fill short gaps in their entirety
          if (length (gap_interval) <= max_extension)
          {
            if (gap_extension_direction != GapExtensionDirection::Backwards)
            {
              add_values_to_gap (results, key, values1, gap_interval);
            }
            if (gap_extension_direction != GapExtensionDirection::Forwards)
            {
              add_values_to_gap (results, key, values2, gap_interval);
            }
            continue;
          }

          // Make fill segments by adjusting the gap segment (adjustment)
          // to avoid being mired in open/close interval creation semantics
          const auto adjustment = length (gap_interval) - max_extension;

          // Fill forward
          if (gap_extension_direction != GapExtensionDirection::Backwards)
          {
            const auto lower_gap = Interval {lower (gap_interval),
                                             upper (gap_interval) - adjustment};
            add_values_to_gap (results, key, values1, lower_gap);
          }
          // Fill backwards
          if (gap_extension_direction != GapExtensionDirection::Forwards)
          {
            const auto upper_gap = Interval {lower (gap_interval) + adjustment,
                                             upper (gap_interval)};
            add_values_to_gap (results, key, values2, upper_gap);
          }
        }
      }
      return results;
    }

  } // namespace details
  /// @endcond

  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
             std::vector<Key> keys,
             const Interval query_interval)
  {
    // Return sorted by keys
    std::ranges::sort (keys);

    for (const auto &key : keys)
    {
      // Do not assume key is valid
      const auto ff = interval_dict.data.find (key);
      if (ff == interval_dict.data.end ())
      {
        continue;
      }

      for (const auto &[value, interval] :
           Implementation<Value, Interval, Impl>::intervals (ff->second,
                                                             query_interval))
      {
        co_yield std::tuple {key, value, interval};
      }
    }
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
             const Key &key,
             const Interval query_interval)
  {
    return intervals (interval_dict, std::vector {key}, query_interval);
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValueInterval<Key, Value, Interval>>
  intervals (const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
             const Interval query_interval)
  {
    const auto keys = interval_dict.keys ();
    return intervals (interval_dict, keys, query_interval);
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
    std::vector<Key> keys,
    const Interval query_interval)
  {
    // Return sorted by keys
    std::ranges::sort (keys);

    for (const auto &key : keys)
    {
      // Do not assume key is valid
      const auto ff = interval_dict.data.find (key);
      if (ff == interval_dict.data.end ())
      {
        continue;
      }

      for (const auto &[vec_values, interval] :
           Implementation<Value, Interval, Impl>::disjoint_intervals (
             ff->second, query_interval))
      {
        co_yield KeyValuesDisjointInterval {key, vec_values, interval};
      }
    }
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
    const Key &key,
    const Interval query_interval)
  {
    return disjoint_intervals (
      interval_dict, std::vector {key}, query_interval);
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  cppcoro::generator<KeyValuesDisjointInterval<Key, Value, Interval>>
  disjoint_intervals (
    const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
    const Interval query_interval)
  {
    const auto keys = interval_dict.keys ();
    return disjoint_intervals (interval_dict, keys, query_interval);
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  subtract (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
            const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)
  {
    dict_1 -= dict_2;
    return dict_1;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  operator- (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
             const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)
  {
    dict_1 -= dict_2;
    return dict_1;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  operator+ (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
             const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)
  {
    dict_1 += dict_2;
    return dict_1;
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl>
  merge (IntervalDictExp<Key, Value, Interval, Impl> dict_1,
         const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)
  {
    dict_1 += dict_2;
    return dict_1;
  }

  /*
   * stream output operator
   */
  template<typename Key, typename Value, typename Interval, typename Impl>
  std::ostream &
  operator<< (std::ostream &ostream,
              const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict)
  {
    for (const auto &[key, values, interval] :
         disjoint_intervals (interval_dict))
    {
      ostream << key << "\t[";
      bool not_first = false;
      for (const auto &val : values)
      {
        if (not_first)
        {
          ostream << ", ";
        }
        not_first = true;
        ostream << val;
      }
      ostream << "]\t" << interval << "\n";
    }
    return ostream;
  }

  template<typename Key, typename Value, typename Interval>
  using FlattenPolicy
    = std::function<std::optional<Value> (const std::optional<Value> &,
                                          Interval,
                                          const Key &,
                                          const std::vector<Value> &)>;

  /*
   * Flatten
   */

  // Use template member function of struct so we don't have to
  // explicitly specify type parameters
  template<typename Key, typename Value, typename Interval>
  std::optional<Value>
  FlattenPolicyDiscard::operator() (const std::optional<Value> &,
                                    Interval,
                                    const Key &,
                                    const std::vector<Value> &)
  {
    return {};
  }

  inline FlattenPolicyDiscard flatten_policy_discard ()
  {
    return FlattenPolicyDiscard ();
  }

  template<typename FlattenPolicy>
  FlattenPolicyPreferStatusQuo<FlattenPolicy>
  flatten_policy_prefer_status_quo (FlattenPolicy fallback_policy)
  {
    return FlattenPolicyPreferStatusQuo (fallback_policy);
  }

  /// @cond Suppress_Doxygen_Warning
  namespace details
  {
    template<typename Key, typename Value, typename Interval, typename Impl>
    std::tuple<Insertions<Key, Value, Interval>, Erasures<Key, Value, Interval>>
    flatten_actions (
      const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict,
      FlattenPolicy<Key, Value, Interval> keep_one_value)
    {
      using namespace boost::icl;

      Insertions<Key, Value, Interval> deferred_insertions;
      Erasures<Key, Value, Interval> deferred_erasures;

      for (const auto &[key, interval_values] : interval_dict.data)
      {
        Interval status_quo_interval;
        std::optional<Value> status_quo;
        for (const auto &[values, interval] :
             Implementation<Value, Interval, Impl>::disjoint_intervals (
               interval_values, interval_extent<Interval>))
        {
          // Save as status_quo if key-value 1:1
          if (values.size () == 1)
          {
            status_quo_interval = interval;
            status_quo = *values.begin ();
            continue;
          }

          // Only use the previous status quo if intervals are adjacent
          if (status_quo.has_value ()
              && !boost::icl::touches (status_quo_interval, interval))
          {
            status_quo.reset ();
          }

          using legacy_ranges_conversion::to_vector;
          status_quo
            = keep_one_value (status_quo, interval, key, values | to_vector ());

          // Empty return: mark everything for erasure
          if (!status_quo.has_value ())
          {
            for (const auto &value : values)
            {
              deferred_erasures.push_back ({key, value, interval});
            }
          }
          else
          {
            // Mark non-matching for erasure
            bool matching_value = false;
            for (const auto &value : values)
            {
              if (value != *status_quo)
              {
                deferred_erasures.push_back ({key, value, interval});
              }
              else
              {
                matching_value = true;
              }
            }
            // The new status_quo wasn't in the list:
            // Mark it for insertion
            if (!matching_value)
            {
              deferred_insertions.push_back ({key, *status_quo, interval});
            }
          }
        }
      }
      return {deferred_insertions, deferred_erasures};
    }

  } // namespace details
  /// @endcond

  template<typename FlattenPolicy>
  template<typename Key, typename Value, typename Interval>
  std::optional<Value> FlattenPolicyPreferStatusQuo<FlattenPolicy>::operator() (
    const std::optional<Value> &status_quo,
    Interval interval,
    const Key &key,
    const std::vector<Value> &values)
  {
    assert (values.size ());
    if (status_quo.has_value ()
        && std::find (values.begin (), values.end (), status_quo.value ())
             != values.end ())
    {
      return status_quo.value ();
    }
    // There is no status quo: use backup plan ('fallback_policy')
    return fallback_policy (status_quo, interval, key, values);
  }

  template<typename Key, typename Value, typename Interval, typename Impl>
  IntervalDictExp<Key, Value, Interval, Impl> flattened (
    IntervalDictExp<Key, Value, Interval, Impl> interval_dict,
    FlattenPolicy<typename details::identity<Key>::type,
                  typename details::identity<Value>::type,
                  typename details::identity<Interval>::type> keep_one_value)
  {
    const auto [insertions, erasures]
      = details::flatten_actions (interval_dict, keep_one_value);
    interval_dict.insert (insertions);
    interval_dict.erase (erasures);
    return interval_dict;
  }

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVALDICT_H
