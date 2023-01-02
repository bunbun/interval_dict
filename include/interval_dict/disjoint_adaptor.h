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
/// \file disjoint_adaptor.h
/// \brief Definitions of functions to layer disjoint operations on top of
/// non-disjoint intervals
///
/// Disjoint operations include:
///   - gaps (...)
///   - sandwiched_gaps (...)
///   - values (...)
///   - disjoint_intervals (...)
///   - initial_values (...)
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_DISJOINT_ADAPTOR_H
#define INCLUDE_INTERVAL_DICT_DISJOINT_ADAPTOR_H

#include "interval_compare.h"
#include "std_ranges_23_patch.h"
#include "value_interval.h"

#include <cppcoro/generator.hpp>

#include <algorithm>
#include <set>
#include <tuple>
#include <utility>

/*
 * For boost::icl::is_empty(), intersects(), hull(), inner_complement(),
 * left_subtract(), right_subtract()
 */
#include <boost/icl/concept/interval.hpp>
/*
 * For boost::icl::interval_traits
 */
#include <boost/icl/interval_traits.hpp>

namespace interval_dict
{
  namespace disjoint_adaptor
  {
    /// @return the gaps between intervals
    template<typename Interval, typename Range>
    cppcoro::generator<Interval> gaps (Range range);

    /// Returns the first disjoint interval (possibly containing multiple
    /// values)
    template<typename Value, typename Interval, typename Range>
    ValuesDisjointInterval<Value, Interval> initial_values (Range range);

    /// @return the gaps between intervals and the values on either side
    template<typename Value, typename Interval, typename Range>
    SandwichedGaps<Value, Interval> sandwiched_gaps (Range range);

    /// @return coroutine enumerating all interval/values over @p query_interval
    template<typename Value, typename Interval, typename Range>
    cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
    disjoint_intervals (Range range);

    template<typename Value, typename Range>
    std::vector<Value> values (Range range);

  } // namespace disjoint_adaptor
} // namespace interval_dict

namespace interval_dict
{
  namespace disjoint_adaptor
  {
    template<typename Interval, typename Range>
    cppcoro::generator<Interval> gaps (Range range)
    {
      Interval current_interval;
      for (const auto &value_interval : range)
      {
        if (boost::icl::is_empty (current_interval))
        {
          current_interval = value_interval.interval;
          continue;
        }

        /* extend gapless interval */
        if (boost::icl::intersects (current_interval, value_interval.interval))
        {
          current_interval
            = boost::icl::hull (current_interval, value_interval.interval);
          continue;
        }

        /* Make sure gap is not empty: intervals on either side may touch */
        auto gap_interval = boost::icl::inner_complement (
          current_interval, value_interval.interval);
        if (!boost::icl::is_empty (gap_interval))
        {
          co_yield gap_interval;
        }

        current_interval = value_interval.interval;
      }
    }

    template<typename Value, typename Interval, typename Range>
    ValuesDisjointInterval<Value, Interval> initial_values (Range range)
    {
      std::set<Value> values;
      Interval interval;
      typename boost::icl::interval_traits<Interval>::domain_type lower_edge;
      for (const auto &value_interval : range)
      {
        if (boost::icl::is_empty (interval))
        {
          interval = value_interval.interval;
          values.insert (value_interval.value);
          lower_edge = comparisons::lower_edge (interval);
          continue;
        }

        /*
         * get all values / intersecting intervals for all nodes
         * that touch the lower edge
         */
        if (comparisons::lower_edge (value_interval.interval) == lower_edge)
        {
          interval = interval & value_interval.interval;
          values.insert (value_interval.value);
        }
        else
        {
          break;
        }
      }

      if (boost::icl::is_empty (interval))
      {
        return {};
      }

      return {values | legacy_ranges_conversion::to_vector (), interval};
    }

    namespace details
    {
      /*
       * Class to sort nodes on their lower or upper edges
       */
      template<typename IntervalValueType>
      struct ValueIntervalEdge
      {
        using IntervalType = IntervalValueType::IntervalType;
        using ValueType = IntervalValueType::ValueType;
        using Basetype =
          typename boost::icl::interval_traits<IntervalType>::domain_type;
        Basetype edge;
        const IntervalValueType *p_interval_value;
        ValueIntervalEdge (const IntervalValueType *p_interval_value,
                           Basetype edge)
          : edge (edge)
          , p_interval_value (p_interval_value)
        {
        }
      };

      // Default sort by edge
      // Used for lower edges to sort the leftmost nodes first
      template<typename IntervalValueType>
      bool operator<(const ValueIntervalEdge<IntervalValueType> &a,
                     const ValueIntervalEdge<IntervalValueType> &b)
      {
        if (a.edge == b.edge)
        {
          return *a.p_interval_value < *b.p_interval_value;
        }
        return a.edge < b.edge;
        //        return std::tie (a.edge, *a.p_interval_value)
        //             < std::tie (b.edge, *b.p_interval_value);
      }

      // Sort in reverse order
      // Used for upper edges to sort the rightmost nodes first
      struct Greater
      {
        template<typename IntervalValueType>
        bool operator() (const ValueIntervalEdge<IntervalValueType> &a,
                         const ValueIntervalEdge<IntervalValueType> &b) const
        {
          return std::tie (a.edge, *a.p_interval_value)
                 > std::tie (b.edge, *b.p_interval_value);
        }
      };

    } // namespace details

    template<typename Value,
             typename Interval,
             typename ElementType,
             typename Range>
    cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
    disjoint_intervals (Range range, Interval query_interval)
    {
      using interval_dict::comparisons::lower_edge;
      using interval_dict::comparisons::upper_edge;
      using legacy_ranges_conversion::to_vector;
      // right edges of intervals that are in play
      std::set<details::ValueIntervalEdge<ElementType>> right_edges;
      std::set<Value> values;
      Interval total_interval;
      const ElementType *prev_node = nullptr;
      for (const auto &value_interval : range)
      {
        assert (!boost::icl::is_empty (value_interval.interval));
        const auto node_left_edge = lower_edge (value_interval.interval);
        const auto prev_node_edge = prev_node == nullptr
                                      ? node_left_edge
                                      : lower_edge (prev_node->interval);
        prev_node = &value_interval;

        // new or value_interval overlapping on the left edge:
        // push and handle together
        if (node_left_edge == prev_node_edge)
        {
          values.insert (value_interval.value);
          right_edges.insert (
            {&value_interval, upper_edge (value_interval.interval)});
          total_interval
            = boost::icl::hull (total_interval, value_interval.interval);
          continue;
        }

        // Handle all intervals whose right edge before the value_interval's
        // left
        while (right_edges.size ()
               && right_edges.begin ()->edge <= node_left_edge)
        {
          auto right_edge_node = right_edges.begin ()->p_interval_value;

          // Disjoint interval is the intersection between all the intervals
          // in flight and the value_interval that will form the right edge
          // For dynamic intervals, the right edge will have correct type
          // Ignore empty intervals, including where multiple intervals
          // have the same right edge
          auto interval = total_interval & right_edge_node->interval;
          if (!boost::icl::is_empty (interval))
          {
            co_yield ValuesDisjointInterval {values | to_vector (),
                                             interval & query_interval};
          }
          values.erase (right_edge_node->value);
          right_edges.erase (right_edges.begin ());
          total_interval = boost::icl::left_subtract (
            total_interval, right_edge_node->interval);
        }

        /*
         * Interval from previous right edge to the left edge of the new
         * value_interval
         */
        if (!values.empty ())
        {
          auto interval = boost::icl::right_subtract (total_interval,
                                                      value_interval.interval);
          if (!boost::icl::is_empty (interval))
          {
            co_yield {values | to_vector (), interval & query_interval};
            // total_interval now starts off with new open
            total_interval
              = boost::icl::hull (value_interval.interval,
                                  boost::icl::left_subtract (
                                    total_interval, value_interval.interval));
          }
        }
        else
        {
          total_interval = value_interval.interval;
        }

        values.insert (value_interval.value);
        right_edges.insert (
          {&value_interval, upper_edge (value_interval.interval)});
        total_interval
          = boost::icl::hull (total_interval, value_interval.interval);
      }
      while (right_edges.size ())
      {
        auto right_edge_node = right_edges.begin ()->p_interval_value;
        auto interval = total_interval & right_edge_node->interval;
        if (!boost::icl::is_empty (interval))
        {
          co_yield std::tuple {values | to_vector (),
                               interval & query_interval};
        }
        assert (values.count (right_edge_node->value));
        values.erase (right_edge_node->value);
        total_interval = boost::icl::left_subtract (total_interval,
                                                    right_edge_node->interval);
        right_edges.erase (right_edges.begin ());
      }
    }

    namespace details
    {
      template<typename Interval>
      bool gap_between (const Interval &a, const Interval &b)
      {
        return !boost::icl::is_empty (boost::icl::inner_complement (a, b));
      }

      template<typename ElementType>
      auto sandwiched_gap (
        const std::set<details::ValueIntervalEdge<ElementType>> &left_edges,
        const std::set<details::ValueIntervalEdge<ElementType>,
                       details::Greater> &right_edges)
        -> SandwichedGap<typename ElementType::ValueType,
                         typename ElementType::IntervalType>
      {
        using Value = ElementType::ValueType;
        using Interval = ElementType::IntervalType;
        std::set<Value> values_before_set;
        std::set<Value> values_after_set;
        Interval interval_before
          = right_edges.begin ()->p_interval_value->interval;
        Interval interval_after
          = left_edges.begin ()->p_interval_value->interval;
        for (const auto &edge : left_edges)
        {
          values_after_set.insert (edge.p_interval_value->value);
          interval_after = interval_after & edge.p_interval_value->interval;
        }

        for (const auto &edge : right_edges)
        {
          values_before_set.insert (edge.p_interval_value->value);
          interval_before = interval_before & edge.p_interval_value->interval;
        }
        Interval gap_interval
          = boost::icl::inner_complement (interval_before, interval_after);
        assert (!boost::icl::is_empty (gap_interval));
        using legacy_ranges_conversion::to_vector;
        return {values_before_set | to_vector (),
                gap_interval,
                values_after_set | to_vector ()};
      }

    } // namespace details

    template<typename Value,
             typename Interval,
             typename ElementType,
             typename Range>
    SandwichedGaps<Value, Interval> sandwiched_gaps (Range range)
    {
      using interval_dict::comparisons::lower_edge;
      using interval_dict::comparisons::upper_edge;

      // right edges of intervals that are in play
      std::set<details::ValueIntervalEdge<ElementType>> left_edges;
      std::set<details::ValueIntervalEdge<ElementType>, details::Greater>
        right_edges;

      SandwichedGaps<Value, Interval> results;
      for (const auto &value_interval : range)
      {
        assert (!boost::icl::is_empty (value_interval.interval));
        const auto node_left_edge = lower_edge (value_interval.interval);
        const auto node_right_edge = upper_edge (value_interval.interval);

        if (!left_edges.empty ())
        {
          assert (!right_edges.empty ());
          // same left edge: store
          if (left_edges.begin ()->edge == node_left_edge)
          {
            left_edges.insert ({&value_interval, node_left_edge});
          }
          continue;

          // different left edge: process

          results.push_back (details::sandwiched_gap (left_edges, right_edges));
          for (const auto &edge_node : left_edges)
          {
            const auto right_edge
              = lower_edge (edge_node.p_interval_value->interval);
            if (right_edge > right_edges.begin ()->edge)
            {
              right_edges.clear ();
            }
            right_edges.insert ({edge_node.p_interval_value, right_edge});
          }
          left_edges.clear ();
        }

        // process current edge: it is either a left edge or a right edge
        if (right_edges.size ()
            && details::gap_between (
              right_edges.begin ()->p_interval_value->interval,
              value_interval.interval))
        {
          left_edges.insert ({&value_interval, node_left_edge});
          continue;
        }

        if (node_right_edge > right_edges.begin ()->edge)
        {
          right_edges.clear ();
        }
        right_edges.insert ({&value_interval, node_right_edge});
      }

      if (!left_edges.empty ())
      {
        assert (!right_edges.empty ());
        results.push_back (details::sandwiched_gap (left_edges, right_edges));
      }
      return results;
    }

    template<typename Value, typename Range>
    std::vector<Value> values (Range range)
    {
      std::set<Value> results;
      for (const auto &node : range)
      {
        results.insert (node.value);
      }
      return results | legacy_ranges_conversion::to_vector ();
    }

  } // namespace disjoint_adaptor

} // namespace interval_dict

/// @endcond

#endif // INCLUDE_INTERVAL_DICT_DISJOINT_ADAPTOR_H