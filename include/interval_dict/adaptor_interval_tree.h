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
#include "value_interval.h"

#include <cppcoro/generator.hpp>

namespace interval_dict
{
  /*
   * _____________________________________________________________________________
   *
   * Functions to handle IntervalTree<Value, Interval>
   *
   */

  //  Convenient alias
  namespace implementation
  {
    template<typename Value, typename Interval>
    using IntervalTree = interval_dict::tree::IntervalTree<Value, Interval>;
  }

  template<typename Impl, typename Value, typename Interval>
  concept IntervalTreeConcept
    = std::is_same_v<Impl, implementation::IntervalTree<Value, Interval>>;

  // The various Implementation are an extra level of indirection
  // that provide a common API for implementing IdentifierMap
  template<typename Value,
           typename Interval,
           IntervalTreeConcept<Value, Interval> Impl>
  struct Implementation<Value, Interval, Impl>
  {
    /// Type manipulating function for obtaining the same implementation
    /// underlying an IntervalDict (IntervalDictICLSubMap in this case) that
    /// uses the same Interval but "rebased" with a new Value type.
    ///
    /// This is used to create types to hold data in the `invert()`
    /// orientation or after joining with a compatible IntervalDict
    /// with possibly different Key / Value types. See `joined_to()`.
    template<typename NewVal>
    struct rebind
    {
      /// Holds type of the implementation in the inverse() direction
      using type = implementation::IntervalTree<NewVal, Interval>;
    };

    /*
     * _____________________________________________________________________________
     *
     * Functions to handle interval_dict::IntervalTree<Value, Interval>
     *
     */
    /// @return coroutine enumerating gaps between intervals and the values on
    /// either side
    static SandwichedGaps<Value, Interval> sandwiched_gaps (const Impl &impl)
    {
      return impl.sandwiched_gaps ();
    }

    /// @return coroutine enumerating gaps between intervals
    static cppcoro::generator<Interval> gaps (const Impl &impl)
    {
      return impl.gaps ();
    }

    /// @return coroutine enumerating all interval/values
    static auto intervals (const Impl &impl, const Interval &query_interval)
    {
      return impl.intervals (query_interval);
    }

    /// @return coroutine enumerating all disjoint interval/values
    static cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
    disjoint_intervals (const Impl &impl, const Interval &query_interval)
    {
      return impl.disjoint_intervals (query_interval);
    }

    /// @return first disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval>
    initial_values (const Impl &impl)
    {
      assert (!impl.empty ());
      return impl.initial_values ();
    }

    /// @return last disjoint interval (with one or more values)
    static ValuesDisjointInterval<Value, Interval>
    final_values (const Impl &impl)
    {
      assert (!impl.empty ());
      return impl.final_values ();
    }

    /// erase @p value for @p query_interval
    static void
    erase (Impl &impl, const Interval &query_interval, const Value &value)
    {
      impl.erase (query_interval, value);
    }

    /// erase all values
    static void erase (Impl &impl, const Interval &query_interval)
    {
      impl.erase (query_interval);
    }

    /// insert @p value
    static void
    insert (Impl &impl, const Interval &query_interval, const Value &value)
    {
      impl.insert (query_interval, value);
    }

    /// @return whether there are no values
    static bool empty (const Impl &impl)
    {
      return impl.empty ();
    }

    /// @return the union with other
    static Impl &merged_with (Impl &impl, const Impl &other)
    {
      return impl.merged_with (other);
    }

    /// @return the asymmetrical difference with other
    static Impl &subtract_by (Impl &impl, const Impl &other)
    {
      return impl.subtract_by (other);
    }
  };

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT_ADAPTOR_INTERVAL_TREE_H
