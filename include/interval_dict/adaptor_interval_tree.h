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
/// \file adaptor_interval_tree.h
/// \brief Definitions of functions to implement IntervalDict with an
/// Interval Tree (currently using tinloaf/ygg and boost::intrusive)
///
/// This stores values as non-disjoint intervals
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_ADAPTOR_INTERVAL_TREE_H
#define INCLUDE_INTERVAL_DICT_ADAPTOR_INTERVAL_TREE_H

#include "adaptor.h"
#include "interval_traits.h"
#include "interval_tree.h"

#include <cppcoro/generator.hpp>
#include <range/v3/view/subrange.hpp>

namespace interval_dict
{
/*
 * _____________________________________________________________________________
 *
 * Functions to handle IntervalTree<Val, Interval>
 *
 */

namespace implementation
{
template <typename Val, typename Interval>
using IntervalTree = interval_dict::IntervalTree<Val, Interval>;
}


template <typename Val, typename Interval, typename Impl>
struct Implementation<
    Val,
    Interval,
    Impl,
    typename std::enable_if_t<
        std::is_same_v<Impl, implementation::IntervalTree<Val, Interval>>>>
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
        using type = implementation::IntervalTree<NewVal, Interval>;
    };

    /*
     * _____________________________________________________________________________
     *
     * Functions to handle interval_dict::IntervalTree<Val, Interval>
     *
     */
    /// @return coroutine enumerating gaps between intervals
    static cppcoro::generator<Interval> gaps(const Impl& interval_values)
    {
        return interval_values.gaps();
    }

    /// @return coroutine enumerating gaps between intervals and the values on either side
    static cppcoro::generator<std::tuple<const std::vector<Val>&,
                                         const Interval&,
                                         const std::vector<Val>&>>
    sandwiched_gaps(const Impl& interval_values)
    {
        return interval_values.sandwiched_gaps();
    }

    /// erase @p value for @p query_interval
    static void erase(Impl& interval_values,
                      const Interval& query_interval,
                      const Val& value)
    {
        interval_values.erase(query_interval, value);
    }

    /// erase all values for @p query_interval
    static void erase(Impl& interval_values, const Interval& query_interval)
    {
        interval_values.erase(query_interval);
    }

    /// insert @p value for @p query_interval
    static void insert(Impl& interval_values,
                       const Interval& query_interval,
                       const Val& value)
    {
        interval_values.insert(query_interval, value);
    }

    /// @return coroutine enumerating all interval/values over @p query_interval
    static cppcoro::generator<std::tuple<const Interval&, const Val&>>
    intervals(const Impl& interval_values, const Interval& query_interval)
    {
        return interval_values.intervals(query_interval);
    }

    /// @return coroutine enumerating all disjoint interval/values over @p
    /// query_interval
    static cppcoro::generator<std::tuple<const Interval&, const std::set<Val>&>>
    disjoint_intervals(const Impl& interval_values,
                       const Interval& query_interval)
    {
        return interval_values.disjoint_intervals(query_interval);
    }

    /// @return whether there are no values
    static bool is_empty(const Impl& interval_values)
    {
        // return interval_values.iterative_size() == 0;
        return interval_values.is_empty();
    }

    /// @return the union with another set of interval-values
    static Impl& merged_with(Impl& interval_values, const Impl& other)
    {
        return interval_values.merged_with(other);
    }

    /// @return the asymmetrical difference with another set of
    /// interval-values
    static Impl& subtract_by(Impl& interval_values, const Impl& other)
    {
        return interval_values.subtract_by(other);
    }

    /// @return first disjoint interval (with one or more values)
    static std::tuple<Interval, std::set<Val>>
    initial_values(const Impl& interval_values)
    {
        assert(!interval_values.is_empty());
        return interval_values.initial_values();
    }

    /// @return last disjoint interval (with one or more values)
    static std::tuple<Interval, std::set<Val>>
    final_values(const Impl& interval_values)
    {
        assert(!interval_values.is_empty());
        return interval_values.final_values();
    }
};

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_INTERVAL_TREE_ADAPTOR_H
