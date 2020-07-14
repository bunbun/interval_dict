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
/// \file icl_interval_map_adaptor.h
/// \brief Definitions of functions to implement IntervalDict with
/// boost::icl::interval_map
//
// This allows interval_map storing values as disjoint intervals to be used to
// implement interval associative dictionaries
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_ICL_INTERVAL_MAP_ADAPTOR_H
#define INCLUDE_INTERVAL_DICT_ICL_INTERVAL_MAP_ADAPTOR_H

#include "interval_traits.h"

#include <boost/icl/interval_map.hpp>
#include <cppcoro/generator.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/map.hpp>

namespace interval_dict
{

/*
 * _____________________________________________________________________________
 *
 * Abstract functions to handle boost::icl::interval_map of std::set<Val>
 *
 */
namespace implementation
{

// boost icl interval_map associating each Interval with a std::set of values
template <typename Val, typename Interval>
using IntervalDictICLSubMap =
    boost::icl::interval_map<typename IntervalTraits<Interval>::BaseType,
                             std::set<Val>,
                             boost::icl::partial_absorber,
                             std::less,
                             boost::icl::inplace_plus,
                             boost::icl::inplace_et,
                             Interval>;

/*
 * _____________________________________________________________________________
 *
 * Abstract functions to handle boost::icl::interval_map of std::set<Val>
 *
 */
// Returns the gaps between intervals
// template <typename Val, typename Interval>
// auto overlapping(
//     const IntervalDictICLSubMap<Val, Interval>& values_per_interval,
//     Interval interval)
// {
//     const auto itpair = values_per_interval.equal_range(interval);
//     return ranges::subrange(itpair.first, itpair.second);
// }

// Returns the gaps between intervals
template <typename Val, typename Interval>
cppcoro::generator<Interval>
gaps(const IntervalDictICLSubMap<Val, Interval>& interval_values)
{
    if (interval_values.iterative_size() >= 2)
    {
        auto first = interval_values.begin();
        auto next = std::next(first);
        auto last = interval_values.end();
        for (; next != last; ++next, ++first)
        {
            co_yield boost::icl::inner_complement(first->first, next->first);
        }
    }
}

// Returns the gaps between intervals and the values on either side
template <typename Val, typename Interval>
cppcoro::generator<std::tuple<const std::vector<Val>&,
                              const Interval&,
                              const std::vector<Val>&>>
sandwiched_gaps(const IntervalDictICLSubMap<Val, Interval>& interval_values)
{
    // Reuse vectors for efficiency
    std::vector<Val> before;
    std::vector<Val> after;
    if (interval_values.iterative_size() >= 2)
    {
        auto first = interval_values.begin();
        auto next = std::next(first);
        auto last = interval_values.end();
        for (; next != last; ++next, ++first)
        {
            before.assign(first->second.begin(), first->second.end());
            after.assign(next->second.begin(), next->second.end());
            co_yield {before,
                      boost::icl::inner_complement(first->first, next->first),
                      after};
        }
    }
}

template <typename Val, typename Interval>
void erase(IntervalDictICLSubMap<Val, Interval>& interval_values,
           const Interval& query_interval,
           const Val& value)
{
    using ValueSet =
        typename IntervalDictICLSubMap<Val, Interval>::codomain_type;
    interval_values -= std::pair(query_interval, ValueSet{value});
}

template <typename Val, typename Interval>
void erase(IntervalDictICLSubMap<Val, Interval>& interval_values,
           const Interval& query_interval)
{
    interval_values.set({query_interval, {}});
}

template <typename Val, typename Interval>
void insert(IntervalDictICLSubMap<Val, Interval>& interval_values,
            const Interval& query_interval,
            const Val& value)
{
    using ValueSet =
        typename IntervalDictICLSubMap<Val, Interval>::codomain_type;
    interval_values += std::pair(query_interval, ValueSet{value});
}

template <typename Val, typename Interval>
cppcoro::generator<std::tuple<const Interval&, const Val&>>
intervals(const IntervalDictICLSubMap<Val, Interval>& interval_values,
          const Interval query_interval = interval_extent<Interval>)
{
    // Underlying storage is disjoint and must be combined
    std::map<Val, Intervals<Interval>> intervals_per_value;
    const auto itpair = interval_values.equal_range(query_interval);
    for (const auto& [interval, values] :
         ranges::subrange(itpair.first, itpair.second))
    {
        for (const auto& value : values)
        {
            intervals_per_value[value] += interval & query_interval;
        }
    }

    // Naturally sorted by value then by interval
    for (const auto& [value, intervals] : intervals_per_value)
    {
        for (const auto& interval : intervals)
        {
            co_yield std::tuple{interval, value};
        }
    }
}

template <typename Val, typename Interval>
cppcoro::generator<std::tuple<const Interval&, const std::set<Val>&>>
disjoint_intervals(const IntervalDictICLSubMap<Val, Interval>& interval_values,
                   const Interval query_interval = interval_extent<Interval>)
{
    // Naturally sorted by interval then value
    const auto itpair = interval_values.equal_range(query_interval);
    for (const auto& [interval, values] :
         ranges::subrange(itpair.first, itpair.second))
    {
        const auto intersection = query_interval & interval;
        co_yield std::tie(intersection, values);
    }
}

template <typename Val, typename Interval>
bool empty(const IntervalDictICLSubMap<Val, Interval>& interval_values)
{
    // return interval_values.iterative_size() == 0;
    return interval_values.empty();
}

/// Returns the union
template <typename Val, typename Interval>
IntervalDictICLSubMap<Val, Interval>&
merged_with(IntervalDictICLSubMap<Val, Interval>& interval_values,
            const IntervalDictICLSubMap<Val, Interval>& other)
{
    interval_values += other;
    return interval_values;
}

/// Returns the asymmetrical difference
template <typename Val, typename Interval>
IntervalDictICLSubMap<Val, Interval>&
subtract_by(IntervalDictICLSubMap<Val, Interval>& interval_values,
            const IntervalDictICLSubMap<Val, Interval>& other)
{
    interval_values -= other;
    return interval_values;
}

template <typename Val, typename Interval>
std::tuple<const Interval&, const std::set<Val>&> first_disjoint_interval(
    const IntervalDictICLSubMap<Val, Interval>& interval_values)
{
    assert(interval_values.iterative_size() > 0);
    const auto it = interval_values.begin();
    return {it->first, it->second};
}

template <typename Val, typename Interval>
std::tuple<const Interval&, const std::set<Val>&> last_disjoint_interval(
    const IntervalDictICLSubMap<Val, Interval>& interval_values)
{
    assert(interval_values.iterative_size());
    const auto it = interval_values.rbegin();
    return {it->first, it->second};
}

} // namespace implementation

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_ICL_INTERVAL_MAP_ADAPTOR_H
