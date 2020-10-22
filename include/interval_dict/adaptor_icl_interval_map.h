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
/// \file adaptor_icl_interval_map.h
/// \brief Definitions of functions to implement IntervalDict with
/// boost::icl::interval_map
//
// This allows interval_map storing values as disjoint intervals to be used to
// implement interval associative dictionaries
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_ADAPTOR_ICL_INTERVAL_MAP_H
#define INCLUDE_INTERVAL_DICT_ADAPTOR_ICL_INTERVAL_MAP_H

#include "adaptor.h"
#include "interval_traits.h"

#include <boost/icl/interval_map.hpp>
#include <cppcoro/generator.hpp>
#include <range/v3/view/subrange.hpp>

namespace interval_dict
{

/*
 * _____________________________________________________________________________
 *
 * Functions to handle boost::icl::interval_map of std::set<Val>
 *
 */

namespace implementation
{
/// boost icl interval_map associating each Interval with a std::set of values
template <typename Val, typename Interval>
using IntervalDictICLSubMap =
    boost::icl::interval_map<typename IntervalTraits<Interval>::BaseType,
                             std::set<Val>,
                             boost::icl::partial_absorber,
                             std::less,
                             boost::icl::inplace_plus,
                             boost::icl::inplace_et,
                             Interval>;
} // namespace implementation

template <typename Val, typename Interval, typename Impl>
struct Implementation<
    Val,
    Interval,
    Impl,
    typename std::enable_if_t<
        std::is_same_v<Impl,
                       implementation::IntervalDictICLSubMap<Val, Interval>>>>
{

    /// Type manipulating function for obtaining the same implementation
    /// underlying an IntervalDict (IntervalDictICLSubMap in this case) that
    /// uses the same Interval but "rebased" with a new Val type.
    ///
    /// The return type is `::type` as per C++ convention.
    ///
    /// This is used to create types to hold data in the `invert()`
    /// orientation or after joining with a compatible IntervalDict
    /// with possibly different Key / Value types. See `joined_to()`.
    template <typename NewVal> struct rebind
    {
        /// Holds type of the implementation in the inverse() direction
        using type = implementation::IntervalDictICLSubMap<NewVal, Interval>;
    };

    /*
     * _____________________________________________________________________________
     *
     * Functions to handle boost::icl::interval_map of std::set<Val>
     *
     */
    /// @return coroutine enumerating gaps between intervals
    static cppcoro::generator<Interval> gaps(const Impl& interval_values)
    {
        if (interval_values.iterative_size() >= 2)
        {
            auto first = interval_values.begin();
            auto next = std::next(first);
            auto last = interval_values.end();
            for (; next != last; ++next, ++first)
            {
                auto gap_interval =
                    boost::icl::inner_complement(first->first, next->first);
                if (!boost::icl::is_empty(gap_interval))
                {
                    co_yield boost::icl::inner_complement(first->first,
                                                          next->first);
                }
            }
        }
    }

    /// @return coroutine enumerating gaps between intervals and the values on
    /// either side
    static cppcoro::generator<std::tuple<const std::vector<Val>&,
                                         const Interval&,
                                         const std::vector<Val>&>>
    sandwiched_gaps(const Impl& interval_values)
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
                auto gap_interval =
                    boost::icl::inner_complement(first->first, next->first);
                if (!boost::icl::is_empty(gap_interval))
                {
                    co_yield {
                        before,
                        boost::icl::inner_complement(first->first, next->first),
                        after};
                }
            }
        }
    }

    /// erase @p value for @p query_interval
    static void erase(Impl& interval_values,
                      const Interval& query_interval,
                      const Val& value)
    {
        using ValueSet = typename Impl::codomain_type;
        interval_values -= std::pair(query_interval, ValueSet{value});
    }

    /// erase all values for @p query_interval
    static void erase(Impl& interval_values, const Interval& query_interval)
    {
        interval_values.set({query_interval, {}});
    }

    /// insert @p value for @p query_interval
    static void insert(Impl& interval_values,
                       const Interval& query_interval,
                       const Val& value)
    {
        using ValueSet = typename Impl::codomain_type;
        interval_values += std::pair(query_interval, ValueSet{value});
    }

    /// @return coroutine enumerating all interval/values over @p query_interval
    static cppcoro::generator<std::tuple<const Interval&, const Val&>>
    intervals(const Impl& interval_values, const Interval& query_interval)
    {
        using Intervals = boost::icl::interval_set<
            typename boost::icl::interval_traits<Interval>::domain_type,
            std::less,
            Interval>;

        // Underlying storage is disjoint and must be combined
        std::map<Val, Intervals> intervals_per_value;
        const auto itpair = interval_values.equal_range(query_interval);
        for (const auto& [interval, values] :
             ranges::subrange(itpair.first, itpair.second))
        {
            for (const auto& value : values)
            {
                intervals_per_value[value] += interval & query_interval;
            }
        }

        std::vector<std::tuple<Interval, Val>> vec_interval_values;
        // Sorted by interval then by value
        for (const auto& [value, intervals] : intervals_per_value)
        {
            for (const auto& interval : intervals)
            {
                vec_interval_values.push_back({interval, value});
            }
        }
        std::sort(vec_interval_values.begin(), vec_interval_values.end());
        for (const auto& i : vec_interval_values)
        {
            co_yield std::tie(std::get<0>(i), std::get<1>(i));
        }
    }

    /// @return coroutine enumerating all disjoint interval/values over @p
    /// query_interval
    static cppcoro::generator<std::tuple<const Interval&, const std::set<Val>&>>
    disjoint_intervals(const Impl& interval_values,
                       const Interval& query_interval)
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

    /// @return whether there are no values
    static bool is_empty(const Impl& interval_values)
    {
        // return interval_values.iterative_size() == 0;
        return interval_values.empty();
    }

    /// @return the union with another set of interval-values
    static Impl& merged_with(Impl& interval_values, const Impl& other)
    {
        interval_values += other;
        return interval_values;
    }

    /// @return the asymmetrical difference with another set of
    /// interval-values
    static Impl& subtract_by(Impl& interval_values, const Impl& other)
    {
        interval_values -= other;
        return interval_values;
    }

    /// @return first disjoint interval (with one or more values)
    static std::tuple<Interval, std::set<Val>>
    initial_values(const Impl& interval_values)
    {
        assert(interval_values.iterative_size() > 0);
        const auto it = interval_values.begin();
        return {it->first, it->second};
    }

    /// @return last disjoint interval (with one or more values)
    static std::tuple<Interval, std::set<Val>>
    final_values(const Impl& interval_values)
    {
        assert(interval_values.iterative_size());
        const auto it = interval_values.rbegin();
        return {it->first, it->second};
    }
};

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_ICL_INTERVAL_MAP_ADAPTOR_H
