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
#include "std_ranges_23_patch.h"
#include "value_interval.h"

#include <boost/icl/interval_map.hpp>
#include <cppcoro/generator.hpp>

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
    template<typename Val, typename Interval>
    using IntervalDictICLSubMap
      = boost::icl::interval_map<typename IntervalTraits<Interval>::BaseType,
                                 std::set<Val>,
                                 boost::icl::partial_absorber,
                                 std::less,
                                 boost::icl::inplace_plus,
                                 boost::icl::inplace_et,
                                 Interval>;
  } // namespace implementation

  template<typename Impl, typename Value, typename Interval>
  concept IntervalDictICLSubMapConcept
    = std::is_same_v<Impl,
                     implementation::IntervalDictICLSubMap<Value, Interval>>;

  template<typename Value,
           typename Interval,
           IntervalDictICLSubMapConcept<Value, Interval> Impl>
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
      using type = implementation::IntervalDictICLSubMap<NewVal, Interval>;
    };

    /*
     * _____________________________________________________________________________
     *
     * Functions to handle boost::icl::interval_map of std::set<Value>
     *
     */
    /// @return coroutine enumerating gaps between intervals
    static cppcoro::generator<Interval> gaps (const Impl &impl)
    {
      // Need two intervals for gaps between disjoint intervals
      if (impl.iterative_size () >= 2)
      {
        auto first = impl.begin ();
        auto next = std::next (first);
        auto last = impl.end ();
        for (; next != last; ++next, ++first)
        {
          auto gap_interval
            = boost::icl::inner_complement (first->first, next->first);
          if (!boost::icl::is_empty (gap_interval))
          {
            co_yield boost::icl::inner_complement (first->first, next->first);
          }
        }
      }
    }

    /// @return coroutine enumerating gaps between intervals and the values on
    /// either side
    static SandwichedGaps<Value, Interval> sandwiched_gaps (const Impl &impl)
    {
      using legacy_ranges_conversion::to_vector;
      SandwichedGaps<Value, Interval> results;
      // Need two intervals for gaps between disjoint intervals
      if (impl.iterative_size () >= 2)
      {
        auto first = impl.begin ();
        auto next = std::next (first);
        auto last = impl.end ();
        for (; next != last; ++next, ++first)
        {
          auto gap_interval
            = boost::icl::inner_complement (first->first, next->first);
          if (!boost::icl::is_empty (gap_interval))
          {
            /*Previously used coroutines here (co_yield) for efficiency*/
            results.push_back (
              {first->second | to_vector (),
               boost::icl::inner_complement (first->first, next->first),
               next->second | to_vector ()});
          }
        }
      }
      return results;
    }

    /// erase @p value for @p query_interval
    static void
    erase (Impl &impl, const Interval &query_interval, const Value &value)
    {
      using ValueSet = typename Impl::codomain_type;
      impl -= std::pair (query_interval, ValueSet {value});
    }

    /// erase all values for @p query_interval
    static void erase (Impl &impl, const Interval &query_interval)
    {
      impl.set ({query_interval, {}});
    }

    /// insert @p value for @p query_interval
    static void
    insert (Impl &impl, const Interval &query_interval, const Value &value)
    {
      using ValueSet = typename Impl::codomain_type;
      impl += std::pair (query_interval, ValueSet {value});
    }

    /// @return coroutine enumerating all interval/values over @p query_interval
    static cppcoro::generator<ValueInterval<Value, Interval>>
    intervals (const Impl &impl, const Interval &query_interval)
    {
      using Intervals = boost::icl::interval_set<
        typename boost::icl::interval_traits<Interval>::domain_type,
        std::less,
        Interval>;

      // Underlying storage is disjoint and must be combined
      std::map<Value, Intervals> intervals_per_value;
      const auto itpair = impl.equal_range (query_interval);
      for (const auto &[interval, values] :
           std::ranges::subrange (itpair.first, itpair.second))
      {
        for (const auto &value : values)
        {
          intervals_per_value[value] += interval & query_interval;
        }
      }

      std::vector<ValueInterval<Value, Interval>> vec_interval_values;
      // Sorted by interval then by value
      for (const auto &[value, intervals] : intervals_per_value)
      {
        for (const auto &interval : intervals)
        {
          vec_interval_values.push_back ({value, interval});
        }
      }
      std::sort (vec_interval_values.begin (), vec_interval_values.end ());
      for (auto &i : vec_interval_values)
      {
        co_yield i;
      }
    }

    /// @return coroutine enumerating all disjoint interval/values over @p
    /// query_interval
    static cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
    disjoint_intervals (const Impl &impl, const Interval &query_interval)
    {
      // Naturally sorted by interval then value
      const auto itpair = impl.equal_range (query_interval);
      for (const auto &[interval, values] :
           std::ranges::subrange (itpair.first, itpair.second))
      {
        const auto intersection = query_interval & interval;
        using namespace legacy_ranges_conversion;
        co_yield ValuesDisjointInterval {values | to_vector (), intersection};
      }
    }

    /// @return whether there are no values
    static bool empty (const Impl &impl)
    {
      // return impl.iterative_size() == 0;
      return impl.empty ();
    }

    /// @return the union with another set of interval-values
    static Impl &merged_with (Impl &impl, const Impl &other)
    {
      impl += other;
      return impl;
    }

    /// @return the asymmetrical difference with another set of
    /// interval-values
    static Impl &subtract_by (Impl &impl, const Impl &other)
    {
      impl -= other;
      return impl;
    }

    /// @return first disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval>
    initial_values (const Impl &impl)
    {
      assert (impl.iterative_size () > 0);
      const auto it = impl.begin ();
      return {it->second | legacy_ranges_conversion::to_vector (), it->first};
    }

    /// @return last disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval>
    final_values (const Impl &impl)
    {
      assert (impl.iterative_size ());
      const auto it = impl.rbegin ();
      return {it->second | legacy_ranges_conversion::to_vector (), it->first};
    }
  };

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_ICL_INTERVAL_MAP_ADAPTOR_H
