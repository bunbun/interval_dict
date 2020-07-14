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
/// \file intervaldict_func.h
/// \brief Forward declarations of functions / associated types of IntervalDict
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_INTERVALDICT_FORWARD_H
#define INCLUDE_INTERVAL_DICT_INTERVALDICT_FORWARD_H

#include <boost/icl/interval_map.hpp>
#include <cppcoro/generator.hpp>

#include "interval_traits.h"
#include <tuple>

namespace interval_dict
{

/// \brief Set of disjoint intervals
template <typename Interval>
using Intervals = boost::icl::interval_set<
    typename boost::icl::interval_traits<Interval>::domain_type,
    std::less,
    Interval>;

/*
 * Insertion and Erasure types
 */
template <typename Key, typename Val, typename Interval>
using Insertions = std::vector<std::tuple<Key, Val, Interval>>;
template <typename Key, typename Val, typename Interval>
using Erasures = std::vector<std::tuple<Key, Val, Interval>>;

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
template <typename Key, typename Val, typename Interval, typename Impl>
class IntervalDictExp;

/* _____________________________________________________________________________
 *
 * Forward declarations of associated functions whose definitions are in
 * intervaldict_func.h
 *
 */

/// subtract()
/// \return The asymmetrical difference between two interval dictionaries
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
subtract(const IntervalDictExp<Key, Val, Interval, Impl>& dict_a,
         const IntervalDictExp<Key, Val, Interval, Impl>& dict_b);

/// merge()
/// \return The union of two interval dictionaries
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
merge(const IntervalDictExp<Key, Val, Interval, Impl>& dict_a,
      const IntervalDictExp<Key, Val, Interval, Impl>& dict_b);

/// intervals()
///
/// returns constituent intervals and their values for the
/// specified \p key. <br>Silently ignores missing keys
///
/// \param interval_dict The input interval dictionary
/// \param key Only show intervals for the specified key
/// \param query_interval Restrict to intervals intersecting with the query
/// \return A sorted std::vector of std::tuple<key, value, interval>
template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
          std::vector<Key> keys,
          Interval query_interval = interval_extent<Interval>);

/// intervals()
///
/// returns constituent intervals for specified \p keys
/// Silently ignores missing keys
/// \param interval_dict
/// \param keys Only show intervals for the specified keys
/// \param query_interval Restrict to intervals intersecting with the query
/// \return sorted std::vector of std::tuple<key, value, interval>
template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
          const Key& key,
          Interval query_interval = interval_extent<Interval>);

/// intervals()
///
/// returns all constituent intervals
/// \param interval_dict
/// \param query_interval Restrict to intervals intersecting with the query
/// \return sorted std::vector of std::tuple<key, value, interval>
template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
          Interval query_interval = interval_extent<Interval>);

/// disjoint_intervals()
///
/// returns all mapped values for each disjoint, non-overlapping interval
/// for the specified keys and overlapping the query interval.
/// Silently ignores missing keys.
/// \param interval_dict
/// \param keys Only show intervals for the specified keys
/// \param query_interval Restrict to intervals intersecting with the query
/// \return sorted std::vector of std::tuple<key, std::vector<value>, interval>
template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
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
/// \return sorted std::vector of std::tuple<key, std::vector<value>, interval>
template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    const Key& key,
    Interval query_interval = interval_extent<Interval>);

/// disjoint_intervals()
/// Returns all mapped values for each disjoint, non-overlapping interval
/// for all keys and overlapping the query interval.
/// Silently ignores missing keys.
/// \param interval_dict
/// \param query_interval Restrict to intervals intersecting with the query
/// \return sorted std::vector of std::tuple<key, std::vector<value>, interval>
template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    Interval query_interval = interval_extent<Interval>);

/// binary operator - asymmeterical difference from dict1 to dict2
/// \return a new IntervalDict
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
operator-(IntervalDictExp<Key, Val, Interval, Impl> dict1,
          const IntervalDictExp<Key, Val, Interval, Impl>& dict2);

/// binary operator + merges key values in dict1 with dict2
/// \return a new IntervalDict
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
operator+(IntervalDictExp<Key, Val, Interval, Impl> dict1,
          const IntervalDictExp<Key, Val, Interval, Impl>& dict2);

/// subtract()
/// asymmeterical difference from dict1 to dict2
/// \return a new IntervalDict
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
subtract(IntervalDictExp<Key, Val, Interval, Impl> dict1,
         const IntervalDictExp<Key, Val, Interval, Impl>& dict2);

/// merge() key values in dict1 with dict2
/// \return a new IntervalDict
template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
merge(IntervalDictExp<Key, Val, Interval, Impl> dict1,
      const IntervalDictExp<Key, Val, Interval, Impl>& dict2);

/// \brief Policy for flattening dictionaries used by flatten()
template <typename Key, typename Val, typename Interval>
using FlattenPolicy = std::function<std::optional<Val>(
    const std::optional<Val>&, Interval, const Key&, const std::vector<Val>&)>;

// Use template member function of struct so we don't have to
// explicitly specify type parameters
struct FlattenPolicyDiscard
{
    template <typename Key, typename Val, typename Interval>
    std::optional<Val> operator()(const std::optional<Val>&,
                                  Interval,
                                  const Key&,
                                  const std::vector<Val>&);
};

/// \brief Policy that simple discards data for intervals where a key maps to
/// multiple values
// template <typename Key, typename Val, typename Interval>
// FlattenPolicy<Key, Val, Interval> flatten_policy_discard();
FlattenPolicyDiscard flatten_policy_discard();

// Use template member function of struct so we don't have to
// explicitly specify type parameters
template <typename FlattenPolicy> struct FlattenPolicyPreferStatusQuo
{
    template <typename Key, typename Val, typename Interval>
    std::optional<Val> operator()(const std::optional<Val>&,
                                  Interval,
                                  const Key&,
                                  const std::vector<Val>&);
    FlattenPolicyPreferStatusQuo(FlattenPolicy fallback_policy)
        : fallback_policy(fallback_policy)
    {
    }
    FlattenPolicy fallback_policy;
};

/// \brief Policy that simple discards data for intervals where a key maps to
/// multiple values
// template <typename Key, typename Val, typename Interval>
// FlattenPolicy<Key, Val, Interval> flatten_policy_discard();
FlattenPolicyDiscard flatten_policy_discard();

/// \brief Policy that prefers to extend the preceding interval if possible.
/// Otherwise, uses the supplied (nested) fallback policy
// template <typename Key, typename Val, typename Interval>
// FlattenPolicy<Key, Val, Interval> flatten_policy_prefer_status_quo(
//     FlattenPolicy<Key, Val, Interval> fallback_policy =
//         flatten_policy_discard());
template <typename FlattenPolicy = FlattenPolicyDiscard>
FlattenPolicyPreferStatusQuo<FlattenPolicy> flatten_policy_prefer_status_quo(
    FlattenPolicy fallback_policy = flatten_policy_discard());

namespace detail
{
// Exclude specific arguments from deduction for flattened()
// (available as std::type_identity as of C++20)
// Otherwise, we would, for example, have to use a cast from lambda to
// std::function
template <typename T> struct identity
{
    typedef T type;
};
} // namespace detail

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
template <typename Key, typename Val, typename Interval, typename Impl>
[[nodiscard]] IntervalDictExp<Key, Val, Interval, Impl> flattened(
    IntervalDictExp<Key, Val, Interval, Impl> interval_dict,
    FlattenPolicy<typename detail::identity<Key>::type,
                  typename detail::identity<Val>::type,
                  typename detail::identity<Interval>::type> keep_one_value =
        // flatten_policy_prefer_status_quo<Key, Val, Interval>());
    flatten_policy_prefer_status_quo());

namespace detail
{

// Helper function for subset()
// returns a vector for passing to insert()
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename KeyRange,
          typename ValRange>
Insertions<Key, Val, Interval>
subset_inserts(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
               const KeyRange& keys_subset,
               const ValRange& values_subset,
               Interval query_interval);

// Helper function for subset()
// returns a vector for passing to insert()
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename KeyRange>
Insertions<Key, Val, Interval>
subset_inserts(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
               const KeyRange& keys_subset,
               Interval query_interval);

// Helper function for fill_gaps_with()
// returns a vector for passing to insert()
template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_gaps_with_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    const IntervalDictExp<Key, Val, Interval, Impl>& other);

// fill_to_start_inserts()
// Helper function for fill_to_start()
// returns a vector for passing to insert()
template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_to_start_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    typename IntervalTraits<Interval>::BaseType starting_point =
        IntervalTraits<Interval>::max(),
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
        IntervalTraits<Interval>::max_size());

// fill_to_end_inserts()
// Helper function for fill_to_end()
// returns a vector for passing to insert()
template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_to_end_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    typename IntervalTraits<Interval>::BaseType starting_point =
        IntervalTraits<Interval>::lowest(),
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
        IntervalTraits<Interval>::max_size());

// extend_into_gaps_inserts()
// Helper function for extend_into_gaps()
// returns a vector for passing to insert()
template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> extend_into_gaps_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    GapExtensionDirection gap_extension_direction = GapExtensionDirection::Both,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
        IntervalTraits<Interval>::max_size());

// fill_gaps_inserts()
// Helper function for fill_gaps()
// returns a vector for passing to insert()
template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_gaps_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension =
        IntervalTraits<Interval>::max_size());

// flatten_actions()
// Helper function for flatten()
// returns a tuple of vectors for passing to insert() and erase()
template <typename Key, typename Val, typename Interval, typename Impl>
std::tuple<Insertions<Key, Val, Interval>, Erasures<Key, Val, Interval>>
flatten_actions(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
                FlattenPolicy<Key, Val, Interval> keep_one_value);

} // namespace detail

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVALDICT_FORWARD_H
