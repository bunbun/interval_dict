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

#ifndef INCLUDE_INTERVAL_DICT_ADAPTOR_AIL_H
#define INCLUDE_INTERVAL_DICT_ADAPTOR_AIL_H

#include "adaptor.h"
#include "augmented_interval_list.h"
#include "interval_traits.h"
#include "value_interval.h"

#include <cppcoro/generator.hpp>

#include <ranges>

namespace interval_dict
{
  /*
   * _____________________________________________________________________________
   *
   * Functions to handle boost::icl::interval_map of std::set<Value>
   *
   */

  namespace implementation
  {
    /// boost icl interval_map associating each Interval with a std::set of
    /// values
    template<typename Value, typename Interval>
    using AugmentedIntervalList
      = augmented_interval_list::AugmentedIntervalList<Value, Interval>;
  } // namespace implementation

  template<typename Impl, typename Value, typename Interval>
  concept AugmentedIntervalListConcept
    = std::is_same_v<Impl,
                     implementation::AugmentedIntervalList<Value, Interval>>;

  template<typename Value,
           typename Interval,
           AugmentedIntervalListConcept<Value, Interval> Impl>
  struct Implementation<Value, Interval, Impl>
  {
    /// Type manipulating function for obtaining the same implementation
    /// underlying an IntervalDict (IntervalDictICLSubMap in this case) that
    /// uses the same Interval but "rebased" with a new Value type.
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
      using type = implementation::AugmentedIntervalList<NewVal, Interval>;
    };

    /*
     * _____________________________________________________________________________
     *
     * Functions to handle interval_dict::IntervalTree<Value, Interval>
     *
     */
    /// @return coroutine enumerating gaps between intervals
    static cppcoro::generator<Interval> gaps (const Impl &interval_values)
    {
      return interval_values.gaps ();
    }

    /// @return coroutine enumerating gaps between intervals and the values on
    /// either side
    static SandwichedGaps<Value, Interval>
    sandwiched_gaps (const Impl &interval_values)
    {
      return interval_values.sandwiched_gaps ();
    }

    /// erase @p value for @p query_interval
    static void erase (Impl &interval_values,
                       const Interval &query_interval,
                       const Value &value)
    {
      interval_values.erase (query_interval, value);
    }

    /// erase all values for @p query_interval
    static void erase (Impl &interval_values, const Interval &query_interval)
    {
      interval_values.erase (query_interval);
    }

    /// insert @p value for @p query_interval
    static void insert (Impl &interval_values,
                        const Interval &query_interval,
                        const Value &value)
    {
      interval_values.insert (query_interval, value);
    }

    /// @return coroutine enumerating all interval/values over @p query_interval
    static auto intervals (const Impl &interval_values,
                           const Interval &query_interval)
    {
      /// TODO
      return interval_values.intervals (query_interval);
    }

    /// @return coroutine enumerating all disjoint interval/values over @p
    /// query_interval
    static cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
    disjoint_intervals (const Impl &interval_values,
                        const Interval &query_interval)
    {
      return interval_values.disjoint_intervals (query_interval);
    }

    /// @return whether there are no values
    static bool empty (const Impl &interval_values)
    {
      // return interval_values.iterative_size() == 0;
      return interval_values.empty ();
    }

    /// @return the union with another set of interval-values
    static Impl &merged_with (Impl &interval_values, const Impl &other)
    {
      return interval_values.merged_with (other);
    }

    /// @return the asymmetrical difference with another set of
    /// interval-values
    static Impl &subtract_by (Impl &interval_values, const Impl &other)
    {
      return interval_values.subtract_by (other);
    }

    /// @return first disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval>
    initial_values (const Impl &interval_values)
    {
      assert (!interval_values.empty ());
      return interval_values.initial_values ();
    }

    /// @return last disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval>
    final_values (const Impl &interval_values)
    {
      assert (!interval_values.empty ());
      return interval_values.final_values ();
    }
  };

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_ADAPTOR_AIL_H
