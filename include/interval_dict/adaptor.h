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
#include "value_interval.h"

#include <boost/icl/interval_map.hpp>
#include <cppcoro/generator.hpp>
#include <ranges>

namespace interval_dict
{
  ///  Abstract trait definition to implement IntervalDict
  template<typename Value, typename Interval, typename Impl>
  struct Implementation
  {
    /// Type manipulating function for obtaining the same implementation
    /// underlying an IntervalDict that is uses the same Interval type but
    /// "rebased" with a new Value type.
    ///
    /// The return type is `::type` as per C++ convention.
    ///
    /// This is used to create types to hold data in the `invert()`
    /// orientation or after joining with a compatible IntervalDict
    /// with possibly different Key / Value types. See `joined_to()`.
    template<typename NewVal>
    struct rebind
    {
      /// Holds type of the implementation in the inverse() direction
      using type = void;
    };

    /// @return coroutine enumerating gaps between intervals and the values on
    /// either side
    static SandwichedGaps<Value, Interval> sandwiched_gaps (const Impl &);

    /// @return coroutine enumerating gaps between intervals
    static cppcoro::generator<Interval> gaps (const Impl &);

    /// @return coroutine enumerating all interval/values over @p query_interval
    static cppcoro::generator<ValueInterval<Value, Interval>>
    intervals (const Impl &, const Interval &);

    /// @return coroutine enumerating all disjoint interval/values over @p
    /// query_interval
    static cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
    disjoint_intervals (const Impl &, const Interval &);

    /// @return first disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval>
    initial_values (const Impl &);

    /// @return last disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval> final_values (const Impl &);

    /// erase @p value for @p query_interval
    static void erase (Impl &, const Interval &, const Value &);

    /// erase all values for @p query_interval
    static void erase (Impl &, const Interval &);

    /// insert @p value for @p query_interval
    static void insert (Impl &, const Interval &, const Value &);

    /// @return whether there are no values
    static bool empty (const Impl &);

    /// @return the union with another set of interval-values
    static Impl &merged_with (Impl &, const Impl &);

    /// @return the asymmetrical difference with another set of
    /// interval-values
    static Impl &subtract_by (Impl &, const Impl &);
  };

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_ADAPTOR_H
