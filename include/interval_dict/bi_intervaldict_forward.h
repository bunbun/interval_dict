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
/// \file bi_intervaldict_forward.h
/// \brief Forward declarations of functions / associated types of IntervalDict
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_FORWARD_H
#define INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_FORWARD_H

#include "adaptor_traits.h"
#include "intervaldict.h"

namespace interval_dict
{

// forward declaration of BiIntervalDictExp
template <typename Key,
          typename Val,
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
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
subtract(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
         const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2);

/// merge()
/// \return The union of two interval dictionaries
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
merge(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
      const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2);

/// intervals()
///
/// returns constituent intervals for specified \p keys
/// Silently ignores missing keys
/// \param interval_dict
/// \param keys Only show intervals for the specified keys
/// \param query_interval Restrict to intervals intersecting with the query
/// \return sorted std::vector of std::tuple<key, value, interval>
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
              interval_dict,
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
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
              interval_dict,
          const Key& key,
          Interval query_interval = interval_extent<Interval>);

/// intervals()
///
/// returns all constituent intervals
/// \param interval_dict
/// \param query_interval Restrict to intervals intersecting with the query
/// \return sorted std::vector of std::tuple<key, value, interval>
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
              interval_dict,
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
    Interval query_interval = interval_extent<Interval>);

/// disjoint_intervals()
/// Returns all mapped values for each disjoint, non-overlapping interval
/// for all keys and overlapping the query interval.
/// Silently ignores missing keys.
/// \param interval_dict
/// \param query_interval Restrict to intervals intersecting with the query
/// \return sorted std::vector of std::tuple<key, std::vector<value>, interval>
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
        interval_dict,
    Interval query_interval = interval_extent<Interval>);

/// binary operator - asymmeterical difference from dict_1 to dict_2
/// \return a new BiIntervalDict
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> operator-(
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2);

/// binary operator + merges key values in dict_1 with dict_2
/// \return a new BiIntervalDict
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> operator+(
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
    const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2);

/// subtract()
/// asymmeterical difference from dict_1 to dict_2
/// \return a new BiIntervalDict
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
subtract(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
         const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2);

/// merge() key values in dict_1 with dict_2
/// \return a new BiIntervalDict
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
merge(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1,
      const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>& dict_2);

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
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> flattened(
    BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> interval_dict,
    FlattenPolicy<typename detail::identity<Key>::type,
                  typename detail::identity<Val>::type,
                  typename detail::identity<Interval>::type> keep_one_value =
        flatten_policy_prefer_status_quo());

/// output streaming operator: prints disjoint intervals
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::ostream&
operator<<(std::ostream& ostream,
           const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>&
               interval_dict);

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_BI_INTERVALDICT_FORWARD_H
