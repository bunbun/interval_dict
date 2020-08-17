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
/// \brief Definitions of free standing functions operating on IntervalDict
//
// Some IntervalDict functionality is implemented in two steps:
// 1) functions in the detail namespace that return a list of insertion or
//    erase operations
// 2) Calling insert/erase with these lists
// This is less efficient because it requires storage for the intermediate
// results but make the operations much more straightforward. It also allows
// bidirectional maps to be implemented using one set of operations.
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_INTERVALDICT_FUNC_H
#define INCLUDE_INTERVAL_DICT_INTERVALDICT_FUNC_H

#include "intervaldict.h"
#include <cppcoro/generator.hpp>
#include <range/v3/algorithm/sort.hpp>
#include <tuple>

namespace interval_dict
{
/// @cond Suppress_Doxygen_Warning
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
               Interval query_interval)
{
    Insertions<Key, Val, Interval> results;
    std::unordered_set<Key> unique_keys_subset(std::begin(keys_subset),
                                               std::end(keys_subset));
    std::unordered_set<Val> unique_values_subset(std::begin(values_subset),
                                                 std::end(values_subset));
    for (const auto& [key, interval_values] : interval_dict.data)
    {
        // Ignore non matching keys
        if (unique_keys_subset.count(key))
        {
            for (const auto& [interval, value] :
                 implementation::intervals(interval_values, query_interval))
            {
                // Only add interval if there are any matching specified values
                if (unique_values_subset.count(value) != 0)
                {
                    results.push_back({key, value, interval & query_interval});
                }
            }
        }
    }
    return results;
}

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
               Interval query_interval)
{
    // Insertions<Key, Val, Interval> results;
    std::vector<std::tuple<Key, Val, Interval>> results;

    std::unordered_set<Key> unique_keys_subset(std::begin(keys_subset),
                                               std::end(keys_subset));
    for (const auto& [key, interval_values] : interval_dict.data)
    {
        // Ignore non matching keys
        if (unique_keys_subset.count(key))
        {
            for (const auto& [interval, value] :
                 implementation::intervals(interval_values, query_interval))
            {
                results.push_back(
                    std::tuple{key, value, interval & query_interval});
            }
        }
    }
    return results;
}

template <typename Interval>
Interval
right_extend(Interval interval,
             typename IntervalTraits<Interval>::BaseDifferenceType size)
{
    using namespace boost::icl;
    const auto left = lower(interval);
    const auto right = upper(interval);

    // Don't extend interval by more than max()
    if (size == IntervalTraits<Interval>::max_size() ||
        right > IntervalTraits<Interval>::max() - size)
    {
        return Interval{left, IntervalTraits<Interval>::max()};
    }

    return Interval{lower(interval), right + size};
}

template <typename Interval>
Interval left_extend(Interval interval,
                     typename IntervalTraits<Interval>::BaseDifferenceType size)
{
    using namespace boost::icl;
    const auto left = lower(interval);
    const auto right = upper(interval);

    // Don't extend interval by more than max()
    if (size == IntervalTraits<Interval>::max_size() ||
        left < IntervalTraits<Interval>::lowest() + size)
    {
        return Interval{IntervalTraits<Interval>::lowest(), right};
    }

    return Interval{lower(interval) - size, right};
}

// xxx_inserts() are helper functions for implementing xxx() in conjunction
// with insert()
template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_gaps_with_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    const IntervalDictExp<Key, Val, Interval, Impl>& other)
{
    Insertions<Key, Val, Interval> results;
    const auto& data = interval_dict.data;
    for (const auto& [key_other, interval_values_other] : other.data)
    {
        // If the key is absent, copy all values willy-nilly from other
        auto f = data.find(key_other);
        if (f == data.end())
        {
            for (const auto& [interval, value] : implementation::intervals(
                     interval_values_other, interval_extent<Interval>))
            {
                results.push_back({key_other, value, interval});
            }
            continue;
        }

        // Otherwise only insert values for gaps
        auto& interval_values = f->second;
        for (const auto& gap_interval : implementation::gaps(interval_values))
        {
            for (const auto& [interval, value] :
                 implementation::intervals(interval_values_other, gap_interval))
            {
                results.push_back({key_other, value, interval & gap_interval});
            }
        }
    }
    return results;
}

template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_to_start_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    using namespace boost::icl;
    Insertions<Key, Val, Interval> results;
    const auto interval_min = IntervalTraits<Interval>::lowest();
    const Interval query_interval{interval_min, starting_point};
    for (const auto& [key, interval_values] : interval_dict.data)
    {
        // Make sure first interval is at or before starting_point
        const auto& [interval, values] =
            implementation::first_disjoint_interval(interval_values);
        if (!intersects(query_interval, interval))
        {
            continue;
        }

        // Don't extend fill interval by more than max_extension
        auto fill_interval =
            right_subtract(left_extend(interval, max_extension), interval);
        for (const auto& val : values)
        {
            results.push_back({key, val, fill_interval});
        }
    }
    return results;
}

template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_to_end_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    typename IntervalTraits<Interval>::BaseType starting_point,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    using namespace boost::icl;
    Insertions<Key, Val, Interval> results;
    const auto interval_max = IntervalTraits<Interval>::max();
    const Interval query_interval{starting_point, interval_max};

    for (const auto& [key, interval_values] : interval_dict.data)
    {
        // Make sure first interval is at or before starting_point
        const auto& [interval, values] =
            implementation::last_disjoint_interval(interval_values);
        if (!intersects(query_interval, interval))
        {
            continue;
        }

        // Don't extend fill interval by more than max_extension
        auto fill_interval =
            left_subtract(right_extend(interval, max_extension), interval);
        for (const auto& val : values)
        {
            results.push_back({key, val, fill_interval});
        }
    }
    return results;
}

// Helper function for fill_gaps_inserts() and extend_into_gaps_inserts()
template <typename Key, typename Val, typename Interval>
void add_values_to_gap(Insertions<Key, Val, Interval>& results,
                       const Key& key,
                       const std::vector<Val>& values,
                       const Interval gap_interval)
{
    for (const auto& val : values)
    {
        results.push_back({key, val, gap_interval});
    }
}

template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> fill_gaps_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    using namespace boost::icl;
    Insertions<Key, Val, Interval> results;
    for (const auto& [key, interval_values] : interval_dict.data)
    {
        for (const auto& [values1, gap_interval, values2] :
             implementation::sandwiched_gaps(interval_values))
        {
            // only fill if there are the common values that are on both sides
            std::vector<Val> common_values;
            std::set_intersection(
                values1.begin(),
                values1.end(),
                values2.begin(),
                values2.end(),
                std::inserter(common_values, common_values.begin()));
            if (common_values.empty())
            {
                continue;
            }

            // Fill short gaps in their entirety
            if (length(gap_interval) <= max_extension)
            {
                add_values_to_gap(results, key, common_values, gap_interval);
            }

            // Fill from both sides
            // Make fill segments by adjusting the gap segment (adjustment)
            // to avoid being mired in open/close interval creation semantics
            else
            {
                const auto adjustment = length(gap_interval) - max_extension;
                const auto lower_gap = Interval{
                    lower(gap_interval), upper(gap_interval) - adjustment};
                add_values_to_gap(results, key, common_values, lower_gap);
                const auto upper_gap = Interval{
                    lower(gap_interval) + adjustment, upper(gap_interval)};
                add_values_to_gap(results, key, common_values, upper_gap);
            }
        }
    }
    return results;
}

template <typename Key, typename Val, typename Interval, typename Impl>
Insertions<Key, Val, Interval> extend_into_gaps_inserts(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    GapExtensionDirection gap_extension_direction,
    typename IntervalTraits<Interval>::BaseDifferenceType max_extension)
{
    using namespace boost::icl;
    Insertions<Key, Val, Interval> results;
    for (const auto& [key, interval_values] : interval_dict.data)
    {
        for (const auto& [values1, gap_interval, values2] :
             implementation::sandwiched_gaps(interval_values))
        {
            // fill short gaps in their entirety
            if (length(gap_interval) <= max_extension)
            {
                if (gap_extension_direction != GapExtensionDirection::Backwards)
                {
                    add_values_to_gap(results, key, values1, gap_interval);
                }
                if (gap_extension_direction != GapExtensionDirection::Forwards)
                {
                    add_values_to_gap(results, key, values2, gap_interval);
                }
                continue;
            }

            // Make fill segments by adjusting the gap segment (adjustment)
            // to avoid being mired in open/close interval creation semantics
            const auto adjustment = length(gap_interval) - max_extension;

            // Fill forward
            if (gap_extension_direction != GapExtensionDirection::Backwards)
            {
                const auto lower_gap = Interval{
                    lower(gap_interval), upper(gap_interval) - adjustment};
                add_values_to_gap(results, key, values1, lower_gap);
            }
            // Fill backwards
            if (gap_extension_direction != GapExtensionDirection::Forwards)
            {
                const auto upper_gap = Interval{
                    lower(gap_interval) + adjustment, upper(gap_interval)};
                add_values_to_gap(results, key, values2, upper_gap);
            }
        }
    }
    return results;
}

} // namespace detail
/// @endcond

template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
          std::vector<Key> keys,
          const Interval query_interval)
{
    // Return sorted by keys
    ranges::sort(keys);

    for (const auto& key : keys)
    {
        // Do not assume key is valid
        const auto ff = interval_dict.data.find(key);
        if (ff == interval_dict.data.end())
        {
            continue;
        }

        for (const auto& [interval, value] :
             implementation::intervals(ff->second, query_interval))
        {
            co_yield std::tuple{key, value, interval};
        }
    }
}

template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
          const Key& key,
          const Interval query_interval)
{
    return intervals(interval_dict, std::vector{key}, query_interval);
}

template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
          const Interval query_interval)
{
    const auto keys = interval_dict.keys();
    return intervals(interval_dict, keys, query_interval);
}

template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    std::vector<Key> keys,
    const Interval query_interval)
{
    // Return sorted by keys
    ranges::sort(keys);

    std::vector<Val> vec_values;
    for (const auto& key : keys)
    {
        // Do not assume key is valid
        const auto ff = interval_dict.data.find(key);
        if (ff == interval_dict.data.end())
        {
            continue;
        }

        for (const auto& [interval, vec_values] :
             implementation::disjoint_intervals(ff->second, query_interval))
        {
            co_yield std::tie(key, vec_values, interval);
        }
    }
}

template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    const Key& key,
    const Interval query_interval)
{
    return disjoint_intervals(interval_dict, std::vector{key}, query_interval);
}

template <typename Key, typename Val, typename Interval, typename Impl>
cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
disjoint_intervals(
    const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
    const Interval query_interval)
{
    const auto keys = interval_dict.keys();
    return disjoint_intervals(interval_dict, keys, query_interval);
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
subtract(IntervalDictExp<Key, Val, Interval, Impl> dict_1,
         const IntervalDictExp<Key, Val, Interval, Impl>& dict_2)
{
    dict_1 -= dict_2;
    return dict_1;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
operator-(IntervalDictExp<Key, Val, Interval, Impl> dict_1,
          const IntervalDictExp<Key, Val, Interval, Impl>& dict_2)
{
    dict_1 -= dict_2;
    return dict_1;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
operator+(IntervalDictExp<Key, Val, Interval, Impl> dict_1,
          const IntervalDictExp<Key, Val, Interval, Impl>& dict_2)
{
    dict_1 += dict_2;
    return dict_1;
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl>
merge(IntervalDictExp<Key, Val, Interval, Impl> dict_1,
      const IntervalDictExp<Key, Val, Interval, Impl>& dict_2)
{
    dict_1 += dict_2;
    return dict_1;
}

/*
 * stream output operator
 */
template <typename Key, typename Val, typename Interval, typename Impl>
std::ostream&
operator<<(std::ostream& ostream,
           const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict)
{
    for (const auto& [key, values, interval] :
         disjoint_intervals(interval_dict))
    {
        ostream << key << "\t[";
        bool not_first = false;
        for (const auto val : values)
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

template <typename Key, typename Val, typename Interval>
using FlattenPolicy = std::function<std::optional<Val>(
    const std::optional<Val>&, Interval, const Key&, const std::vector<Val>&)>;

/*
 * Flatten
 */

// Use template member function of struct so we don't have to
// explicitly specify type parameters
template <typename Key, typename Val, typename Interval>
std::optional<Val> FlattenPolicyDiscard::operator()(const std::optional<Val>&,
                                                    Interval,
                                                    const Key&,
                                                    const std::vector<Val>&)
{
    return {};
}

inline FlattenPolicyDiscard flatten_policy_discard()
{
    return FlattenPolicyDiscard();
}

template <typename FlattenPolicy>
FlattenPolicyPreferStatusQuo<FlattenPolicy>
flatten_policy_prefer_status_quo(FlattenPolicy fallback_policy)
{
    return FlattenPolicyPreferStatusQuo(fallback_policy);
}

/// @cond Suppress_Doxygen_Warning
namespace detail
{
template <typename Key, typename Val, typename Interval, typename Impl>
std::tuple<Insertions<Key, Val, Interval>, Erasures<Key, Val, Interval>>
flatten_actions(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
                FlattenPolicy<Key, Val, Interval> keep_one_value)
{
    using namespace boost::icl;

    Insertions<Key, Val, Interval> deferred_insertions;
    Erasures<Key, Val, Interval> deferred_erasures;

    for (const auto& [key, interval_values] : interval_dict.data)
    {
        Interval status_quo_interval;
        std::optional<Val> status_quo;
        for (const auto& [interval, values] :
             implementation::disjoint_intervals(interval_values,
                                                interval_extent<Interval>))
        {
            // Save as status_quo if key-value 1:1
            if (values.size() == 1)
            {
                status_quo_interval = interval;
                status_quo = *values.begin();
                continue;
            }

            // Only use the previous status quo if intervals are adjacent
            if (status_quo.has_value() &&
                !boost::icl::touches(status_quo_interval, interval))
            {
                status_quo.reset();
            }

            status_quo =
                keep_one_value(status_quo,
                               interval,
                               key,
                               std::vector(values.begin(), values.end()));

            // Empty return: mark everything for erasure
            if (!status_quo.has_value())
            {
                for (const auto& value : values)
                {
                    deferred_erasures.push_back({key, value, interval});
                }
            }
            else
            {
                // Mark non-matching for erasure
                bool matching_value = false;
                for (const auto& value : values)
                {
                    if (value != *status_quo)
                    {
                        deferred_erasures.push_back({key, value, interval});
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
                    deferred_insertions.push_back({key, *status_quo, interval});
                }
            }
        }
    }
    return {deferred_insertions, deferred_erasures};
}

} // namespace detail
/// @endcond

template <typename FlattenPolicy>
template <typename Key, typename Val, typename Interval>
std::optional<Val> FlattenPolicyPreferStatusQuo<FlattenPolicy>::operator()(
    const std::optional<Val>& status_quo,
    Interval interval,
    const Key& key,
    const std::vector<Val>& values)
{
    assert(values.size());
    if (status_quo.has_value() &&
        std::find(values.begin(), values.end(), status_quo.value()) !=
            values.end())
    {
        return status_quo.value();
    }
    // There is no status quo: use backup plan ('fallback_policy')
    return fallback_policy(status_quo, interval, key, values);
}

template <typename Key, typename Val, typename Interval, typename Impl>
IntervalDictExp<Key, Val, Interval, Impl> flattened(
    IntervalDictExp<Key, Val, Interval, Impl> interval_dict,
    FlattenPolicy<typename detail::identity<Key>::type,
                  typename detail::identity<Val>::type,
                  typename detail::identity<Interval>::type> keep_one_value)
{
    const auto [insertions, erasures] =
        detail::flatten_actions(interval_dict, keep_one_value);
    interval_dict.insert(insertions);
    interval_dict.erase(erasures);
    return interval_dict;
}

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVALDICT_FUNC_H
