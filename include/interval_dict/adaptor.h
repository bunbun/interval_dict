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
/// \file adaptor.h
/// \brief Abstract trait definition to implement IntervalDict
/// \author Leo Goodstadt

/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_ADAPTOR_H
#define INCLUDE_INTERVAL_DICT_ADAPTOR_H

#include "interval_traits.h"

#include <boost/icl/interval_map.hpp>
#include <cppcoro/generator.hpp>
#include <range/v3/view/subrange.hpp>

namespace interval_dict
{

///  Abstract trait definition to implement IntervalDict
template <typename Val, typename Interval, typename T, typename enable = void>
struct Implementation
{

    /// Type manipulating function for obtaining the same implementation
    /// underlying an IntervalDict that is uses the same Interval type but
    /// "rebased" with a new Val type.
    ///
    /// The return type is `::type` as per C++ convention.
    ///
    /// This is used to create types to hold data in the `invert()`
    /// orientation or after joining with a compatible IntervalDict
    /// with possibly different Key / Value types. See `joined_to()`.
    template <typename NewVal> struct rebind
    {
        /// Holds type of the implementation in the inverse() direction
        using type = void;
    };

    /// @return coroutine enumerating gaps between intervals
    cppcoro::generator<Interval> gaps(const T&)
    {
    }

    /// @return coroutine enumerating gaps between intervals and the values on either side
    static cppcoro::generator<std::tuple<const std::vector<Val>&,
                                         const Interval&,
                                         const std::vector<Val>&>>
    sandwiched_gaps(const T&)
    {
    }

    /// erase @p value for @p query_interval
    static void erase(T&, const Interval&, const Val&)
    {
    }

    /// erase all values for @p query_interval
    static void erase(T&, const Interval&)
    {
    }

    /// insert @p value for @p query_interval
    static void insert(T&, const Interval&, const Val&)
    {
    }

    /// @return coroutine enumerating all interval/values over @p query_interval
    static cppcoro::generator<std::tuple<const Interval&, const Val&>>
    intervals(const T&, const Interval&)
    {
    }

    /// @return coroutine enumerating all disjoint interval/values over @p
    /// query_interval
    static cppcoro::generator<std::tuple<const Interval&, const std::set<Val>&>>
    disjoint_intervals(const T&, const Interval&)
    {
    }

    /// @return whether there are no values
    static bool empty(const T&)
    {
        return false;
    }

    /// @return the union with another set of interval-values
    static T& merged_with(T&, const T&)
    {
    }

    /// @return the asymmetrical difference with another set of
    /// interval-values
    static T& subtract_by(T&, const T&)
    {
    }

    /// @return first disjoint interval (with one or more values)
    static std::tuple<const Interval&, const std::set<Val>&>
    initial_values(const T&)
    {
    }

    /// @return last disjoint interval (with one or more values)
    static std::tuple<const Interval&, const std::set<Val>&>
    final_values(const T&)
    {
    }
};

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_ADAPTOR_H
