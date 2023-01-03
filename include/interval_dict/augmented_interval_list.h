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
/*
 * \file augmented_interval_list.h
 * \brief Definitions of functions to implement Augmented Interval List
 *
 * Original idea described in
 * "Augmented Interval List: a novel data structure for efficient
 * genomic interval search" Feng, Ratan & Sheffield (2019)
 * Bioinformatics 35:4907–4911
 *
 * Algorithm extensively revised so that the actual lists of intervals
 * may be quite different from Feng et. al. All bugs are my own.
 *
 * \author Leo Goodstadt
 *
 * Contact intervaldict@llew.org.uk
 */

#ifndef INCLUDE_INTERVAL_DICT_AUGMENTED_INTERVAL_LIST_H
#define INCLUDE_INTERVAL_DICT_AUGMENTED_INTERVAL_LIST_H

#include "default_init_allocator.h"
#include "disjoint_adaptor.h"
#include "interval_compare.h"
#include "interval_operators.h"
#include "interval_overlaps.h"
#include "interval_traits.h"
#include "std_ranges_23_patch.h"
#include "value_interval.h"

#include <boost/icl/concept/interval.hpp>
#include <boost/icl/interval_traits.hpp>
#include <boost/icl/type_traits/is_asymmetric_interval.hpp>
#include <boost/icl/type_traits/is_continuous_interval.hpp>
#include <boost/icl/type_traits/is_discrete_interval.hpp>

#include <cppcoro/generator.hpp>

#include <array>
#include <cstdint>
#include <set>
#include <vector>

namespace interval_dict::augmented_interval_list
{
  /**
   * A sorted run of intervals denoted by begin and end indices
   * Note that end points to one past the last index
   */
  struct Run
  {
    Run (int_fast32_t begin, int_fast32_t end)
      : begin (begin)
      , end (end)
    {
    }
    auto operator<=> (const Run &) const = default;

    int_fast32_t begin;
    int_fast32_t end;
  };

  /**
   * Streaming operator for Run
   */
  inline std::ostream &operator<< (std::ostream &os, const Run &run)
  {
    os << "[" << run.begin << " - " << run.end << "]";
    return os;
  }

  /**
   * Streaming operator for a vector of Runs
   */
  inline std::ostream &operator<< (std::ostream &os,
                                   const std::vector<Run> &runs)
  {
    for (const auto &run : runs)
    {
      os << "  " << run << "\n";
    }
    return os;
  }

  /// @cond Suppress_Doxygen_Warning
  namespace details
  {
    // Set of touching intervals used by insert_helper()
    template<typename Value, typename Interval>
    using TouchingSet = std::set<ValueInterval<Value, Interval>,
                                 comparisons::CompareValIntervalTouches>;

    template<typename Value, typename Interval>
    void remove_empty (ValueIntervals<Value, Interval> &value_intervals)
    {
      // remove empty intervals
      std::erase_if (value_intervals,
                     [] (const auto &iv)
                     {
                       return boost::icl::is_empty (iv.interval);
                     });
    }

    /**
     * Sort and merge overlapping intervals with the same value
     */
    template<typename Value, typename Interval>
    void
    sort_combine_overlapping (ValueIntervals<Value, Interval> &value_intervals)
    {
      std::ranges::sort (value_intervals, comparisons::CompareValInterval {});
      const auto [l, e] = std::ranges::unique (
        value_intervals,
        [] (auto &lhs, const auto &rhs)
        {
          /*
           * Only discard rhs (return true) if
           * 1. values are the identical
           * 2. intervals overlap
           * In which case, also extend lhs to
           * hull of both
           */
          if (lhs.value != rhs.value)
          {
            return false;
          }

          if (!comparisons::exclusive_less (lhs.interval, rhs.interval))
          {
            lhs.interval = boost::icl::hull (lhs.interval, rhs.interval);
            return true;
          }
          return false;
        });
      value_intervals.erase (l, e);
    }

  } // namespace details
  /// @endcond

  /**
   * \brief Implements an augmented interval list of intervals and values
   * \tparam Value value type
   * \tparam Interval interval type
   */

  /*
   * An AugmentedIntervalList (AIL) stores intervals and associated values like
   * an interval tree but in a vector. Intersection queries involve
   * 1) Using the right edge of the query and binary search on the left edge of
   * the stored intervals. O(log N)
   * 2) Linear search on the left query edge, iterating through overlapping
   * intervals until indicated by the cumulative maximum of the stored right
   * edges. O(k)
   * Overall Performance is O(k + log N) depending on the frequency of
   * overlaps.
   *
   * AILs queries have but have much higher practical query performance than
   * interval trees by
   * 1) using std::vector for better cache locality
   * 2) reducing interval overlaps by iteratively "promoting" overlapping
   * intervals up a hierarchy of interval lists
   *
   * AILs holds a std::vector of ValueIntervals comprising a series of "Run"s
   * each sorted by interval left edge.
   * Intervals that span too many neighbours are promoted to the next
   * run iteratively until
   * 1) The intervals in a run no longer overlap excessively, or
   * 2) The run is small enough to scan manually
   * To prevent highly overlapping interval series from breaking this scheme,
   * we never promote more than 50% of intervals so that Run sizes decrease
   * exponentially
   * See AugmentedIntervalList::decompose_into_runs()
   *
   * The use of std::vector means that random insertions and deletions are
   * relatively expensive. To offset this, costs are amortized by postponing
   * deletions (using tombstone values) and insertions (added to a separate
   * list of runs) until indels reach a specified proportion of stored
   * intervals.
   */
  template<typename Value, typename Interval>
  struct AugmentedIntervalList
  {
    // Dynamic bounds are converted into static bounds for the query algorithm
    // using boost::icl::first() / last(). These overloads are not available for
    // continuous intervals
    static_assert (
      !(boost::icl::is_continuous_interval<Interval>::value
        && boost::icl::has_dynamic_bounds<Interval>::value),
      "This library does not support continuous intervals (e.g. float) "
      "with dynamic bounds (that can be set to open/closed at run time)");

    using ValueIntervalType = ValueInterval<Value, Interval>;

    // Matching indices type driven by efficiency savings in query()
    // with a non-default initialising allocator
    using VecIndices
      = std::vector<int_fast32_t, DefaultInitAllocator<int_fast32_t>>;

    /**
     * Wrapper for providing an iterator for all intervals in sorted order
     *
     * N.B. Lifetimes for iterators returned by begin() and end() can be used
     * after the enclosing AllIntervalsRange dies since the iterators really
     * point to the state of the entire AIL.
     *
     * However, the iterators become invalid obviously after the death of the
     * AIL into which we are really iterating.
     *
     * Changing the AIL underhandedly mid-iteration is undefined behaviour.
     */
    struct AllIntervalsRange
    {
      public:
      /**
       * Iterator over all intervals in sorted order
       * The implementation exploits the way AugmentedIntervalList is
       * broken down into multiple sorted runs and iterates through these
       * in parallel, using "m_runs" internally to decide which is the
       * next interval:
       *
       * 1) keep a list of indices to each run
       * 2) Return the index into the run with the smallest interval.
       * 3) Keep the indices list sorted in interval-sorted order using
       *    bubble sort(!) after each iteration. This works because m_runs
       *    is small and most of the time no-resorting is needed.
       */
      class ConstIterator
      {
        /// @cond Suppress_Doxygen_Warning
        public:
        using difference_type = ptrdiff_t;
        using value_type = ValueInterval<Value, Interval>;
        using const_reference = const value_type &;
        using const_pointer = const value_type *;
        using iterator_category = std::input_iterator_tag;

        ConstIterator (const ValueIntervals<Value, Interval> &intervals,
                       std::vector<Run> runs)
          : m_value_intervals (intervals)
          , m_runs (std::move (runs))
        {
        }

        ConstIterator (const ConstIterator &other)
          : m_value_intervals (other.m_value_intervals)
          , m_runs (other.m_runs)
        {
        }

        ~ConstIterator () = default;

        ConstIterator &operator= (const ConstIterator &other);

        bool operator== (const ConstIterator &other) const;
        bool operator!= (const ConstIterator &other) const;

        ConstIterator &operator++ ();
        ConstIterator operator++ (int);
        const_reference operator* () const;
        const_pointer operator->() const;

        private:
        const ValueIntervals<Value, Interval> &m_value_intervals;
        std::vector<Run> m_runs;
      };

      AllIntervalsRange (const ValueIntervals<Value, Interval> &intervals,
                         std::vector<Run> runs);

      ConstIterator begin () const;
      ConstIterator end () const;

      private:
      const ValueIntervals<Value, Interval> &m_value_intervals;
      std::vector<Run> m_runs;
    };
    /// @endcond

    /**
     * Wrapper for providing an iterator for a sub set of sorted intervals
     * The sub-set and sorting has to be done before hand.
     *
     * N.B. The subrange holds the data for the selected intervals and
     * therefore must outlive the iterators returned by begin() and end().
     *
     * Changing the AIL underhandedly mid-iteration is undefined behaviour.
     */
    struct IntervalsRange
    {
      public:
      class ConstIterator
      {
        /// @cond Suppress_Doxygen_Warning
        public:
        using difference_type = ptrdiff_t;
        using value_type = ValueInterval<Value, Interval>;
        using const_reference = const value_type &;
        using const_pointer = const value_type *;
        using iterator_category = std::input_iterator_tag;

        ConstIterator (const ValueIntervals<Value, Interval> &intervals,
                       const VecIndices &indices,
                       std::size_t pos = 0)
          : m_value_intervals (intervals)
          , m_indices (indices)
          , m_pos (pos)
        {
        }

        ConstIterator (const ConstIterator &other)
          : m_value_intervals (other.m_value_intervals)
          , m_indices (other.m_indices)
          , m_pos (other.m_pos)
        {
        }

        ~ConstIterator () = default;

        ConstIterator &operator= (const ConstIterator &other);
        bool operator== (const ConstIterator &other) const;
        bool operator!= (const ConstIterator &other) const;

        ConstIterator &operator++ ();
        ConstIterator operator++ (int);
        const_reference operator* () const;
        const_pointer operator->() const;

        private:
        // Stores reference to
        const ValueIntervals<Value, Interval> &m_value_intervals;
        const VecIndices &m_indices;
        std::size_t m_pos = 0;
      };

      IntervalsRange (const ValueIntervals<Value, Interval> &intervals,
                      VecIndices indices)
        : m_value_intervals (intervals)
        , m_indices (indices)
      {
      }

      ConstIterator begin () const;
      ConstIterator end () const;

      private:
      const ValueIntervals<Value, Interval> &m_value_intervals;
      VecIndices m_indices;
    };
    /// @endcond

    /**
     * \brief Construct augmented list from intervals
     * \param intervals The vector of values and intervals for the container.
     * \param max_overlapping_neighbours Threshold for promoting intervals
     * \param min_run_length Runs with fewer intervals will not be promoted.
     *
     * max_fraction_promoted_per_run prevents too many runs from being promoted
     * if, for example, all the intervals are highly overlapping.
     * This sets a practical limit on the number of runs
     * The minimum number of intervals is on the order of 2^runs.size() if
     * max_fraction_promoted_per_run is 0.5.
     *
     * In simulations, the chosen parameters are near optimal for different
     * data sets of widely varying sizes.
     */
    explicit AugmentedIntervalList (ValueIntervals<Value, Interval> intervals,
                                    int max_overlapping_neighbours = 30,
                                    int min_run_length = 64,
                                    double max_fraction_promoted_per_run = 0.50)
      : m_max_overlapping_neighbours (
        static_cast<int_fast32_t> (max_overlapping_neighbours))
      , m_min_run_length (static_cast<int_fast32_t> (min_run_length))
      , m_max_fraction_promoted_per_run (max_fraction_promoted_per_run)
    {
      details::remove_empty (intervals);
      details::sort_combine_overlapping (intervals);
      insert (intervals);
    }

    /**
     * Default Constructor
     */
    AugmentedIntervalList () = default;

    /**
     * Copy constructor
     */
    AugmentedIntervalList (const AugmentedIntervalList &) = default;

    /**
     * Copy assignment operator
     */
    AugmentedIntervalList &operator= (const AugmentedIntervalList &other)
      = default;

    /**
     * Default move constructor
     */
    AugmentedIntervalList (AugmentedIntervalList &&other) noexcept = default;

    /**
     * Default move assignment operator
     */
    AugmentedIntervalList &operator= (AugmentedIntervalList &&other) noexcept
      = default;

    /*
     * @return true if tree is empty
     */
    [[nodiscard]] bool empty () const;

    /**
     * @return iterator for all interval/values
     */
    AllIntervalsRange all_intervals () const
    {
      return {m_value_intervals, m_runs};
    }

    /**
     * @return coroutine enumerating all interval/values over @p query_interval
     */
    IntervalsRange intervals (const Interval &query_interval) const;

    /**
     * @return the gaps between intervals
     */
    cppcoro::generator<Interval> gaps () const;

    /**
     * @return the gaps between intervals and the values on either side
     */
    SandwichedGaps<Value, Interval> sandwiched_gaps () const;

    /**
     * @return coroutine enumerating all interval/values over @p query_interval
     */
    cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
    disjoint_intervals (const Interval &query_interval) const;

    /**
     * @return the first disjoint interval (possibly containing multiple values)
     */
    ValuesDisjointInterval<Value, Interval> initial_values () const;

    /**
     * add values and intervals, merging overlapping intervals with the same
     * value
     */
    void insert (ValueIntervals<Value, Interval> value_intervals);

    /**
     * add value and interval, merging overlapping intervals with the same value
     */
    void insert (Interval interval, const Value &value);

    /**
     * @return last disjoint interval (possibly containing multiple values)
     */
    ValuesDisjointInterval<Value, Interval> final_values () const;

    /**
     * Erase specified value over interval
     */
    void erase (const Interval &query_interval, const Value &value);

    /**
     * Erase all values over interval
     */
    void erase (const Interval &query_interval);

    /**
     * Merges intervals from another tree into this
     */
    AugmentedIntervalList &merged_with (const AugmentedIntervalList &other);

    /**
     * @return the asymmetrical difference with another set of
     * implementation-dependent interval-values
     */
    AugmentedIntervalList &subtract_by (const AugmentedIntervalList &other);

    /**
     * @return all sorted unique values over the specified interval
     */
    std::vector<Value> values (const Interval &query_interval) const;

    /**
     * For range iteration
     */
    auto begin () const
    {
      /*
       * Safe to return iterator from temp object
       */
      AllIntervalsRange (m_value_intervals, m_runs).begin ();
    }

    /**
     * For range iteration
     */
    auto end () const
    {
      /*
       * Safe to return iterator from temp object
       */
      AllIntervalsRange (m_value_intervals, m_runs).end ();
    }

    /**
     * equality operator
     */
    bool operator== (const AugmentedIntervalList &rhs) const;

    /**
     * @return indices of intervals intersecting the query sorted by interval
     */
    void sorted_match_indices (const Interval &query,
                               VecIndices &matching_indices) const;
    /**
     * @return indices of intervals intersecting the query
     */
    void unsorted_match_indices (const Interval &query,
                                 VecIndices &matching_indices) const;

    /**
     * list of intervals and values
     */
    ValueIntervals<Value, Interval> m_value_intervals;

    [[nodiscard]] const std::vector<Run> &runs () const
    {
      return m_runs;
    }

    private:
    /**
     * @return indices of intervals intersecting or touching the query and
     * matching the value
     */
    void unsorted_touching_value_indices (const Interval &query_interval,
                                          const Value &query_value,
                                          VecIndices &matching_indices) const;
    /**
     * @return interval indices intersecting the query and matching query_value.
     * Implemented as unsorted_match_indices() then filtering on query_value
     */
    void unsorted_match_value_indices (const Interval &query_interval,
                                       const Value &query_value,
                                       VecIndices &matching_indices) const;

    /**
     * Cumulative maximum value of the right edge
     * Used to short circuit binary search
     */
    std::vector<typename ValueInterval<Value, Interval>::BaseType>
      m_max_right_edges;

    /**
     * The begin and end indices of each "run" of sorted intervals
     */
    std::vector<Run> m_runs;

    /**
     * Split m_value_intervals into "run"s each of intervals sorted by left edge
     */
    void decompose_into_runs ();

    /**
     * Helper function for inserting multiple value/intervals
     * Erase overlapping intervals with the same value
     * Add consolidated value/intervals for pending insert to
     * new_value_intervals
     */
    void
    insert_helper (const ValueInterval<Value, Interval> &value_interval,
                   details::TouchingSet<Value, Interval> &new_value_intervals);

    /**
     * Mark intervals with indices as erased (set tombstone) by setting their
     * right side to the minimum value
     */
    void mark_as_erased (const VecIndices &indices);

    // Parameters for tuning the decompose_into_runs algorithm
    /// @cond Suppress_Doxygen_Warning
    int_fast32_t m_max_overlapping_neighbours = 20;
    int_fast32_t m_min_run_length = 256;
    double m_max_fraction_promoted_per_run = 0.20;
    /// @endcond

    /**
     * Count number of erased intervals that have yet to be removed
     */
    int_fast32_t m_count_removed = 0;

    /**
     * Count number of inserted intervals that have yet to be integrated
     */
    int_fast32_t m_count_inserted = 0;

    /**
     * Runs excluding pending inserts/erases
     */
    int_fast32_t m_optimal_runs = 0;

    /**
     * Re-calculate running maximum right edges
     * Start from the Nth run where N = run_index
     */
    void calculate_running_max_end (int run_index = 0);
  };

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::calculate_running_max_end (
    int run_index)
  {
    m_max_right_edges.resize (m_value_intervals.size ());
    for (auto i = run_index; i < std::ssize (m_runs); ++i)
    {
      const auto [begin, end] = m_runs[i];
      assert (end - begin > 1);
      auto max_end
        = comparisons::upper_edge (m_value_intervals[begin].interval);
      m_max_right_edges[begin] = max_end;
      for (auto j = begin + 1; j < end; ++j)
      {
        max_end = std::max (
          comparisons::upper_edge (m_value_intervals[j].interval), max_end);
        m_max_right_edges[j] = max_end;
      }
    }
  }

  namespace details
  {
    /**
     * Calculate value at the specified quantile
     * Rough calculation by sorting and finding value of nearest index for
     * fractile
     */
    inline int_fast32_t quantile (const std::vector<int_fast32_t> &values,
                                  double fraction)
    {
      if (values.empty ())
      {
        return 0;
      }
      auto scratch = values;
      std::sort (scratch.begin (), scratch.end ());
      return scratch[int_fast32_t (scratch.size () * fraction)];
    }

  } // namespace details

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::decompose_into_runs ()
  {
    if ((m_count_inserted + m_count_removed) > 0.2 * m_value_intervals.size ()
        || std::ssize (m_value_intervals) < m_min_run_length)
    {
      m_count_inserted = 0;
      m_count_removed = 0;
      m_optimal_runs = 0;
    }

    if (m_optimal_runs < std::ssize (m_runs))
    {
      m_runs.erase (m_runs.begin () + m_optimal_runs, m_runs.end ());
    }

    // offset to first interval to decompose
    int intervals_offset = m_runs.empty () ? 0 : m_runs.back ().end;

    // If all intervals already sorted, set m_optimal to include all runs
    if (intervals_offset == std::ssize (m_value_intervals))
    {
      if (m_count_inserted + m_count_removed == 0)
      {
        m_optimal_runs = m_runs.size ();
      }
      return;
    }

    // Remove intervals marked as erased that are not part of a Run
    std::remove_if (m_value_intervals.begin () + intervals_offset,
                    m_value_intervals.end (),
                    [] (const auto &iv)
                    {
                      return boost::icl::is_empty (iv.interval);
                    });
    std::sort (m_value_intervals.begin () + intervals_offset,
               m_value_intervals.end (),
               comparisons::CompareInterval ());

    // Index of first interval to decompose runs off the end.
    if (std::ssize (m_value_intervals) - intervals_offset <= m_min_run_length)
    {
      assert (std::ssize (m_value_intervals) > intervals_offset);
      m_runs.emplace_back (intervals_offset, std::ssize (m_value_intervals));
      calculate_running_max_end (m_optimal_runs);
      if (m_count_inserted + m_count_removed == 0)
      {
        m_optimal_runs = std::ssize (m_runs);
      }
      return;
    }

    // scratch space
    auto unresolved = m_value_intervals;

    // intervals that cover more than 'm_max_overlapping_neighbours' subsequent
    // intervals
    ValueIntervals<Value, Interval> overlapping;

    int_fast32_t runs_count = 0;
    int_fast32_t pos = intervals_offset;
    int_fast32_t run_start = intervals_offset;
    CountOverlap<Interval> overlap_counter;
    while (!unresolved.empty ())
    {
      int unresolved_size = std::ssize (unresolved);

      // calculate how many neighbours each interval overlaps
      // intervals which overlap too many neighbours are promoted
      overlap_counter.update (unresolved);

      // Make sure we use a level of overlapping_threshold that
      // doesn't promote too much of the list
      const int_fast32_t overlapping_threshold = std::max (
        m_min_run_length,
        std::max (m_max_overlapping_neighbours,
                  details::quantile (overlap_counter.m_counts,
                                     1.0 - m_max_fraction_promoted_per_run)));

      // short last run: save verbatim
      if (unresolved_size < overlapping_threshold)
      {
        std::copy (unresolved.begin (),
                   unresolved.end (),
                   m_value_intervals.begin () + pos);
        m_runs.emplace_back (pos, pos + std::ssize (unresolved));
        break;
      }

      overlapping.clear ();
      for (int_fast32_t i = 0; i < unresolved_size - overlapping_threshold; i++)
      {
        // If overlapping too many neighbours copy to "overlapping"
        if (overlap_counter.m_counts[i] > overlapping_threshold)
        {
          overlapping.push_back (unresolved[i]);
        }
        // Not much overlapping: Resolve / copy to m_value_intervals
        else
        {
          m_value_intervals[pos++] = unresolved[i];
        }
      }
      // last overlapping_threshold items by definition do not have
      // too many overlapping neighbours and can be resolved by adding to
      // m_value_intervals
      std::copy (unresolved.begin () + unresolved_size - overlapping_threshold,
                 unresolved.begin () + unresolved_size,
                 m_value_intervals.begin () + pos);
      pos += overlapping_threshold;
      assert (run_start >= 0);
      m_runs.emplace_back (run_start, pos);
      run_start = pos;
      runs_count++;

      std::swap (overlapping, unresolved);
    }

    calculate_running_max_end (m_optimal_runs);
    // update m_optimal_runs if we have incorporated all insertion/deletions
    if (m_count_inserted + m_count_removed == 0)
    {
      m_optimal_runs = std::ssize (m_runs);
    }
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::unsorted_match_indices (
    const Interval &query_interval, VecIndices &matching_indices) const
  {
    if (m_runs.empty ())
    {
      return;
    }
    matching_indices.clear ();
    const auto query_start = comparisons::lower_edge (query_interval);
    const auto query_end = comparisons::upper_edge (query_interval);
    int cnt_elements = 0;
    /*
     * Extending "matching_indices" in size gives a considerable
     * speed up in benchmarking compared with std::vector push_back()
     * Requires
     * - over committing (default initialised ints) using resize()
     * - assigning indexed by cnt_elements
     * - resizing back to actually used elements (cnt_elements)
     */
    for (const auto &[begin, end] : m_runs)
    {
      matching_indices.resize (matching_indices.size () + end - begin);

      // brute force small runs
      if (end - begin <= 64)
      {
        for (int_fast32_t i = begin; i < end; ++i)
        {
          // intersects
          if (comparisons::intersects (m_value_intervals[i].interval,
                                       query_interval))
          {
            matching_indices[cnt_elements++] = i;
          }
        }
        continue;
      }

      // Binary search for the last item that can intersect the query
      // N.B. Extra -1 because lower_bound returns the "open" end
      // of our search from begin->end, and we actually want the "last" index
      int_fast32_t i
        = std::lower_bound (
            m_value_intervals.begin () + begin,
            m_value_intervals.begin () + end,
            query_end,
            /*
             * lower bound where query.right >= m_value_intervals.left
             */
            [] (const auto &iv, auto q)
            {
              return !comparisons::exclusive_less (q, iv.interval);
            })
          - m_value_intervals.begin () - 1;

      /*
       * While m_max_right_edges[i] >= query_start, some interval(s) will
       * overlap query.
       *                           <--- query --->
       *   <--- m_value_intervals[7] ->            |
       *   <--- m_value_intervals[6] --------->    |
       *  <---- m_value_intervals[5] ->            |(max_right_edge)
       * <----- m_value_intervals[4] ------------->|
       */
      while (i >= begin && m_max_right_edges[i] >= query_start)
      {
        /*
         * By definition, if we are here m_value_intervals[i].left <= query.left
         * Therefore, query overlaps if m_value_intervals[i].right >= query.left
         *                        |<--- query --->
         *  <--- m_value_intervals[i] --->|
         */
        if (!comparisons::exclusive_less (m_value_intervals[i].interval,
                                          query_interval))
        {
          matching_indices[cnt_elements++] = i;
        }
        --i;
      }
    }
    matching_indices.resize (cnt_elements);
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::unsorted_touching_value_indices (
    const Interval &query_interval,
    const Value &query_value,
    VecIndices &matching_indices) const
  {
    if (m_runs.empty ())
    {
      return;
    }
    matching_indices.clear ();
    const auto query_start = comparisons::lower_edge (query_interval);
    const auto query_start_touches
      = boost::icl::domain_prior<Interval> (query_start);
    for (const auto &[begin, end] : m_runs)
    {
      // Binary search for the last item that may touch the query
      // N.B. Extra -1 because lower_bound returns the "open" end
      // of our search from begin->end, and we actually want the "last" index
      int_fast32_t i
        = std::lower_bound (m_value_intervals.begin () + begin,
                            m_value_intervals.begin () + end,
                            query_interval,
                            // query_interval.right >= m_value_intervals[i].left
                            [] (const auto &iv, const auto &q)
                            {
                              return !comparisons::more_or_touches (
                                q, iv.interval);
                            })
          - m_value_intervals.begin () - 1;

      while (i >= begin && m_max_right_edges[i] >= query_start_touches)
      {
        // m_value_intervals[i].interval.right >= query_interval.left
        if (!comparisons::more_or_touches (m_value_intervals[i].interval,
                                           query_interval)
            && m_value_intervals[i].value == query_value)
        {
          matching_indices.push_back (i);
        }
        --i;
      }
    }
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::unsorted_match_value_indices (
    const Interval &query_interval,
    const Value &query_value,
    VecIndices &matching_indices) const
  {
    VecIndices all_matching_indices;
    unsorted_match_indices (query_interval, all_matching_indices);

    // Filter intersecting indices for just those that contain query_value
    for (const auto i : all_matching_indices)
    {
      if (m_value_intervals[i].value == query_value)
      {
        matching_indices.push_back (i);
      }
    }
  }

  /**
   * @return coroutine enumerating all interval/values over @p query_interval
   */
  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value, Interval>::IntervalsRange
  AugmentedIntervalList<Value, Interval>::intervals (
    const Interval &query_interval) const
  {
    VecIndices matching_indices;
    sorted_match_indices (query_interval, matching_indices);
    return IntervalsRange {m_value_intervals, matching_indices};
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::sorted_match_indices (
    const Interval &query_interval, VecIndices &matching_indices) const
  {
    /*
     * Get matching indices
     * These are interleaved in run order
     */
    unsorted_match_indices (query_interval, matching_indices);

    /*
     * Return matching indices sorted by the intervals they point to
     */
    std::ranges::sort (
      matching_indices,
      [this, compare_interval = comparisons::CompareInterval {}] (auto i,
                                                                  const auto j)
      {
        return compare_interval (m_value_intervals[i].interval,
                                 m_value_intervals[j].interval);
      });
  }

  template<typename Value, typename Interval>
  bool AugmentedIntervalList<Value, Interval>::empty () const
  {
    return m_value_intervals.empty ();
  }

  template<typename Value, typename Interval>
  bool AugmentedIntervalList<Value, Interval>::operator== (
    const AugmentedIntervalList &rhs) const
  {
    return std::tie (m_value_intervals,
                     m_max_right_edges,
                     m_runs,
                     m_max_overlapping_neighbours,
                     m_min_run_length,
                     m_max_fraction_promoted_per_run,
                     m_count_removed,
                     m_count_inserted,
                     m_optimal_runs)
           == std::tie (rhs.m_value_intervals,
                        rhs.m_max_right_edges,
                        rhs.m_runs,
                        rhs.m_max_overlapping_neighbours,
                        rhs.m_min_run_length,
                        rhs.m_max_fraction_promoted_per_run,
                        rhs.m_count_removed,
                        rhs.m_count_inserted,
                        rhs.m_optimal_runs);
  }

  template<typename Value, typename Interval>
  ValuesDisjointInterval<Value, Interval>
  AugmentedIntervalList<Value, Interval>::initial_values () const
  {
    return disjoint_adaptor::initial_values<Value, Interval> (all_intervals ());
  }

  template<typename Value, typename Interval>
  cppcoro::generator<ValuesDisjointInterval<Value, Interval>>
  AugmentedIntervalList<Value, Interval>::disjoint_intervals (
    const Interval &query_interval) const
  {
    // TODO: What is this?
    return disjoint_adaptor::
      disjoint_intervals<Value, Interval, ValueIntervalType> (
        intervals (query_interval), query_interval);
  }

  template<typename Value, typename Interval>
  cppcoro::generator<Interval>
  AugmentedIntervalList<Value, Interval>::gaps () const
  {
    return disjoint_adaptor::gaps<Interval> (all_intervals ());
  }

  template<typename Value, typename Interval>
  SandwichedGaps<Value, Interval>
  AugmentedIntervalList<Value, Interval>::sandwiched_gaps () const
  {
    return disjoint_adaptor::
      sandwiched_gaps<Value, Interval, ValueIntervalType> (all_intervals ());
  }

  template<typename Value, typename Interval>
  std::vector<Value> AugmentedIntervalList<Value, Interval>::values (
    const Interval &interval) const
  {
    return disjoint_adaptor::values<Value> (intervals (interval));
  }

  /// @cond Suppress_Doxygen_Warning
  namespace details
  {
    // Helper function for subtract_by()
    // Subtracts two vectors after sorting in value-interval order
    // returns a vector still in value_interval sorted order
    template<typename Value, typename Interval>
    ValueIntervals<Value, Interval>
    sort_subtract_intervals (ValueIntervals<Value, Interval> &value_intervals,
                             ValueIntervals<Value, Interval> other)
    {
      using comparisons::CompareValInterval;

      std::ranges::sort (value_intervals, CompareValInterval {});
      std::ranges::sort (other, CompareValInterval {});

      auto first1 = value_intervals.begin ();
      auto last1 = value_intervals.end ();
      auto first2 = other.end ();
      auto last2 = other.begin ();
      ValueIntervals<Value, Interval> result;

      while (first1 != last1)
      {
        // nothing more to subtract
        if (first2 == last2)
        {
          result.insert (result.end (), first1, last1);
          break;
        }

        // Different values
        if (first1->value < first2->value)
        {
          result.push_back (*first1++);
        }
        else if (first2->value < first1->value)
        {
          ++first2;
        }

        // Same value
        else
        {
          // non-overlapping
          if (boost::icl::exclusive_less (first1->interval, first2->interval))
          {
            result.push_back (*first1++);
          }
          else if (boost::icl::exclusive_less (first2->interval,
                                               first1->interval))
          {
            ++first2;
          }

          // overlapping
          else
          {
            const auto remainder1
              = boost::icl::right_subtract (first1->interval, first2->interval);
            const auto remainder2
              = boost::icl::left_subtract (first1->interval, first2->interval);
            if (!boost::icl::is_empty (remainder1))
            {
              result.push_back ({first1->value, remainder1});
            }
            if (!boost::icl::is_empty (remainder2))
            {
              result.push_back ({first1->value, remainder2});
            }
            ++first1;
          }
        }
      }
      return result;
    }

  } // namespace details
  /// @endcond

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::insert_helper (
    const ValueInterval<Value, Interval> &value_interval,
    details::TouchingSet<Value, Interval> &new_value_intervals)
  {
    // ignore empty intervals
    if (boost::icl::is_empty (value_interval.interval))
    {
      return;
    }

    VecIndices matching_indices;
    unsorted_touching_value_indices (
      value_interval.interval, value_interval.value, matching_indices);

    // If first contains interval, skip: nothing will change on insert
    if (matching_indices.size () == 1
        && boost::icl::contains (
          m_value_intervals[matching_indices.front ()].interval,
          value_interval.interval))
    {
      return;
    }

    // Remove overlapping or even touching
    auto total_interval = value_interval.interval;
    for (const auto i : matching_indices)
    {
      total_interval
        = boost::icl::hull (m_value_intervals[i].interval, total_interval);
    }
    m_count_removed += std::ssize (matching_indices);
    mark_as_erased (matching_indices);

    // Look inside newly inserted intervals
    auto i_new = new_value_intervals.lower_bound (value_interval);
    const auto end_new = new_value_intervals.upper_bound (value_interval);
    while (i_new != end_new)
    {
      // If contains interval, skip: nothing will change on insert
      if (boost::icl::contains (i_new->interval, value_interval.interval))
      {
        return;
      }
      total_interval = boost::icl::hull (i_new->interval, total_interval);
      i_new = new_value_intervals.erase (i_new);
    }

    // Add pending new intervals including overlapping/touching
    assert (!boost::icl::is_empty (total_interval));
    new_value_intervals.insert ({value_interval.value, total_interval});
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::insert (Interval interval,
                                                       const Value &value)
  {
    // Prepare intervals for insertion
    if (boost::icl::is_empty (interval))
    {
      return;
    }
    ++m_count_inserted;
    m_value_intervals.emplace_back (value, interval);
    decompose_into_runs ();
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::insert (
    ValueIntervals<Value, Interval> value_intervals)
  {
    // Prepare intervals for insertion
    details::remove_empty (value_intervals);
    details::sort_combine_overlapping (value_intervals);

    details::TouchingSet<Value, Interval> new_value_intervals;
    for (const auto &value_interval : value_intervals)
    {
      // add value and interval to last run
      insert_helper (value_interval, new_value_intervals);
    }

    m_count_inserted += std::ssize (new_value_intervals);
    m_value_intervals.reserve (m_value_intervals.size ()
                               + new_value_intervals.size ());
    m_value_intervals.insert (m_value_intervals.end (),
                              new_value_intervals.begin (),
                              new_value_intervals.end ());
    decompose_into_runs ();
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::erase (const Interval &interval)
  {
    VecIndices matching_indices;
    unsorted_match_indices (interval, matching_indices);
    for (const auto i : matching_indices)
    {
      const auto remainder1
        = boost::icl::right_subtract (m_value_intervals[i].interval, interval);
      const auto remainder2
        = boost::icl::left_subtract (m_value_intervals[i].interval, interval);
      if (!boost::icl::is_empty (remainder1))
      {
        m_value_intervals.push_back ({m_value_intervals[i].value, remainder1});
        ++m_count_inserted;
      }
      if (!boost::icl::is_empty (remainder2))
      {
        m_value_intervals.push_back ({m_value_intervals[i].value, remainder2});
        ++m_count_inserted;
      }
    }
    mark_as_erased (matching_indices);
    decompose_into_runs ();
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::erase (const Interval &interval,
                                                      const Value &value)
  {
    VecIndices matching_indices;
    unsorted_match_value_indices (interval, value, matching_indices);
    for (const auto i : matching_indices)
    {
      const auto remainder1
        = boost::icl::right_subtract (m_value_intervals[i].interval, interval);
      const auto remainder2
        = boost::icl::left_subtract (m_value_intervals[i].interval, interval);
      if (!boost::icl::is_empty (remainder1))
      {
        m_value_intervals.emplace_back (value, remainder1);
        ++m_count_inserted;
      }
      if (!boost::icl::is_empty (remainder2))
      {
        m_value_intervals.emplace_back (value, remainder2);
        ++m_count_inserted;
      }
    }
    mark_as_erased (matching_indices);
    decompose_into_runs ();
  }

  template<typename Value, typename Interval>
  void AugmentedIntervalList<Value, Interval>::mark_as_erased (
    const VecIndices &indices)
  {
    using namespace boost::icl;
    /*
     * Set right hand side to minimum value to put this interval out of
     * commission N.B. The interval left side is left untouched so it sorts
     * properly
     */
    const auto tombstone_pos = IntervalTraits<Interval>::minimum ();
    for (const auto i : indices)
    {
      assert (i < std::ssize (m_value_intervals));
      m_value_intervals[i].interval
        = Interval {lower (m_value_intervals[i].interval), tombstone_pos};
    }
  }

  template<typename Value, typename Interval>
  AugmentedIntervalList<Value, Interval> &
  AugmentedIntervalList<Value, Interval>::merged_with (
    const AugmentedIntervalList &other)
  {
    // If too few, just pretend it is a normal insert
    if (other.m_value_intervals.size () + m_count_inserted + m_count_removed
          < int (0.2 * m_value_intervals.size ())
        && std::ssize (m_value_intervals) > m_min_run_length)
    {
      insert (other.m_value_intervals);
      return *this;
    }

    m_value_intervals.reserve (m_value_intervals.size ()
                               + other.m_value_intervals.size ());
    m_value_intervals.insert (m_value_intervals.end (),
                              other.m_value_intervals.begin (),
                              other.m_value_intervals.end ());
    details::sort_combine_overlapping (m_value_intervals);
    m_runs.clear ();
    decompose_into_runs ();
    return *this;
  }

  template<typename Value, typename Interval>
  AugmentedIntervalList<Value, Interval> &
  AugmentedIntervalList<Value, Interval>::subtract_by (
    const AugmentedIntervalList &other)
  {
    // If too few, just pretend it is a normal erase
    if (std::ssize (other.m_value_intervals) + m_count_inserted
            + m_count_removed
          < static_cast<int> (0.2 * m_value_intervals.size ())
        && std::ssize (m_value_intervals) > m_min_run_length)
    {
      for (const auto &[value, interval] : other.m_value_intervals)
      {
        erase (interval, value);
      }
      return *this;
    }

    m_value_intervals = details::sort_subtract_intervals (
      m_value_intervals, other.m_value_intervals);
    m_runs.clear ();
    decompose_into_runs ();
    return *this;
  }

  template<typename Value, typename Interval>
  ValuesDisjointInterval<Value, Interval>
  AugmentedIntervalList<Value, Interval>::final_values () const
  {
    if (m_runs.empty ())
    {
      return {};
    }

    // Get the maximum right edge over all runs
    typename ValueInterval<Value, Interval>::BaseType max_right_edge
      = m_max_right_edges[m_runs.front ().end - 1];
    for (auto [begin, end] : m_runs)
    {
      assert (end > begin);
      max_right_edge = std::max (max_right_edge, m_max_right_edges[end - 1]);
    }

    // The last values have intervals whose right edge match the global
    // maximum right edge
    std::set<Value> values;
    Interval interval;
    using comparisons::upper_edge;
    for (const auto &[begin, end] : m_runs)
    {
      auto i = end - 1;
      while (i >= begin && m_max_right_edges[i] == max_right_edge)
      {
        if (upper_edge (m_value_intervals[i].interval) == max_right_edge)
        {
          if (values.empty ())
          {
            interval = m_value_intervals[i].interval;
          }
          else
          {
            interval = interval & m_value_intervals[i].interval;
          }
          values.insert (m_value_intervals[i].value);
        }
      }
    }
    using namespace legacy_ranges_conversion;
    return {values | to_vector (), interval};
  }

  /*
   * Implementation of AugmentedIntervalList::AllIntervalsRange
   */
  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value,
                                 Interval>::AllIntervalsRange::ConstIterator &
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::ConstIterator::
  operator= (const typename AugmentedIntervalList<Value, Interval>::
               AllIntervalsRange::ConstIterator &other)
  {
    assert (&m_value_intervals == &other.m_value_intervals);
    m_runs = other.m_runs;
  }

  template<typename Value, typename Interval>
  bool
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::ConstIterator::
  operator== (const typename AugmentedIntervalList<Value, Interval>::
                AllIntervalsRange::ConstIterator &other) const
  {
    assert (&m_value_intervals == &other.m_value_intervals);
    return (other.m_runs.size () == m_runs.size () && other.m_runs == m_runs);
  }

  template<typename Value, typename Interval>
  bool
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::ConstIterator::
  operator!= (const typename AugmentedIntervalList<Value, Interval>::
                AllIntervalsRange::ConstIterator &other) const
  {
    assert (&m_value_intervals == &other.m_value_intervals);
    /*
     * The end() iterator has size() == 0 and is a fast compare
     */
    return (other.m_runs.size () != m_runs.size () || other.m_runs != m_runs);
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value,
                                 Interval>::AllIntervalsRange::ConstIterator
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::ConstIterator::
  operator++ (int)
  {
    typename AugmentedIntervalList<Value,
                                   Interval>::AllIntervalsRange::ConstIterator
      cpy (*this);
    this->operator++ ();
    return cpy;
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value,
                                 Interval>::AllIntervalsRange::ConstIterator &
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::ConstIterator::
  operator++ ()
  {
    assert (!m_runs.empty ());
    /*
     * if we get to the end of the run in back() we can just discard it
     * because the remaining runs are also in (inverse) order waiting to
     * step in
     */
    if (++m_runs.back ().begin >= m_runs.back ().end)
    {
      m_runs.pop_back ();
      return *this;
    }
    if (m_runs.size () <= 1)
    {
      return *this;
    }

    // keep m_runs inverse sorted so that the back() run always contains the
    // next interval to return
    auto new_pos
      = std::lower_bound (m_runs.begin (),
                          m_runs.end (),
                          m_runs.back (),
                          [this] (const auto &a, const auto &b)
                          {
                            return this->m_value_intervals[b.begin].interval
                                   < this->m_value_intervals[a.begin].interval;
                          });
    if (new_pos < m_runs.end () - 1)
    {
      std::rotate (new_pos, m_runs.end () - 1, m_runs.end ());
    }
    return *this;
  }

  template<typename Value, typename Interval>
  const ValueInterval<Value, Interval> &AugmentedIntervalList<Value, Interval>::
    AllIntervalsRange::ConstIterator::operator* () const
  {
    assert (!m_runs.empty ());
    assert (m_runs.back ().begin != m_runs.back ().end);
    return m_value_intervals[m_runs.back ().begin];
  }

  template<typename Value, typename Interval>
  const ValueInterval<Value, Interval> *AugmentedIntervalList<Value, Interval>::
    AllIntervalsRange::ConstIterator::operator->() const
  {
    assert (!m_runs.empty ());
    assert (m_runs.back ().begin != m_runs.back ().end);
    return &m_value_intervals[m_runs.back ().begin];
  }

  template<typename Value, typename Interval>
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::AllIntervalsRange (
    const ValueIntervals<Value, Interval> &intervals, std::vector<Run> runs)
    : m_value_intervals (intervals)
    , m_runs (std::move (runs))
  {
    // remove empty runs
    // TODO: Bug? Needs erase?
    std::remove_if (m_runs.begin (),
                    m_runs.end (),
                    [] (const auto &a)
                    {
                      return a.begin >= a.end;
                    });
    // Inverse sort by interval so that the back() run contains the next
    // interval to return
    std::ranges::sort (m_runs,
                       [&intervals] (const auto &a, const auto &b)
                       {
                         return intervals[b.begin].interval
                                < intervals[a.begin].interval;
                       });
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value,
                                 Interval>::AllIntervalsRange::ConstIterator
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::begin () const
  {
    using ConstIterator = typename AugmentedIntervalList<Value, Interval>::
      AllIntervalsRange::ConstIterator;
    return ConstIterator {m_value_intervals, m_runs};
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value,
                                 Interval>::AllIntervalsRange::ConstIterator
  AugmentedIntervalList<Value, Interval>::AllIntervalsRange::end () const
  {
    using ConstIterator = typename AugmentedIntervalList<Value, Interval>::
      AllIntervalsRange::ConstIterator;
    return ConstIterator {m_value_intervals, {}};
  }

  /*
   * Implementation of AugmentedIntervalList::IntervalsRange
   */
  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value,
                                 Interval>::IntervalsRange::ConstIterator &
  AugmentedIntervalList<Value, Interval>::IntervalsRange::ConstIterator::
  operator= (const typename AugmentedIntervalList<Value, Interval>::
               IntervalsRange::ConstIterator &other)
  {
    assert (&m_value_intervals == &other.m_value_intervals);
    assert (&m_indices == &other.m_indices);
    m_pos = other.m_pos;
  }

  template<typename Value, typename Interval>
  bool AugmentedIntervalList<Value, Interval>::IntervalsRange::ConstIterator::
  operator== (const typename AugmentedIntervalList<Value, Interval>::
                IntervalsRange::ConstIterator &other) const
  {
    assert (&m_value_intervals == &other.m_value_intervals);
    assert (&m_indices == &other.m_indices);
    return (other.m_pos == m_pos);
  }

  template<typename Value, typename Interval>
  bool AugmentedIntervalList<Value, Interval>::IntervalsRange::ConstIterator::
  operator!= (const typename AugmentedIntervalList<Value, Interval>::
                IntervalsRange::ConstIterator &other) const
  {
    return (other.m_pos != m_pos);
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value, Interval>::IntervalsRange::ConstIterator
  AugmentedIntervalList<Value, Interval>::IntervalsRange::ConstIterator::
  operator++ (int)
  {
    typename AugmentedIntervalList<Value,
                                   Interval>::IntervalsRange::ConstIterator
      cpy (*this);
    this->operator++ ();
    return cpy;
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value,
                                 Interval>::IntervalsRange::ConstIterator &
  AugmentedIntervalList<Value,
                        Interval>::IntervalsRange::ConstIterator::operator++ ()
  {
    /* Don't increment any more if already at end */
    if (m_pos < std::size (m_indices))
    {
      ++m_pos;
    }
    return *this;
  }

  template<typename Value, typename Interval>
  const ValueInterval<Value, Interval> &AugmentedIntervalList<Value, Interval>::
    IntervalsRange::ConstIterator::operator* () const
  {
    assert (!m_indices.empty ());
    assert (m_pos < std::size (m_indices));
    const auto index = m_indices[m_pos];
    assert (index < std::ssize (m_value_intervals));
    return m_value_intervals[index];
  }

  template<typename Value, typename Interval>
  const ValueInterval<Value, Interval> *AugmentedIntervalList<Value, Interval>::
    IntervalsRange::ConstIterator::operator->() const
  {
    assert (!m_indices.empty ());
    assert (m_pos < std::ssize (m_indices));
    const auto index = m_indices[m_pos];
    return &m_value_intervals[index];
    assert (index < std::ssize (m_value_intervals));
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value, Interval>::IntervalsRange::ConstIterator
  AugmentedIntervalList<Value, Interval>::IntervalsRange::begin () const
  {
    using ConstIterator =
      typename AugmentedIntervalList<Value,
                                     Interval>::IntervalsRange::ConstIterator;
    return ConstIterator {m_value_intervals, m_indices, 0};
  }

  template<typename Value, typename Interval>
  typename AugmentedIntervalList<Value, Interval>::IntervalsRange::ConstIterator
  AugmentedIntervalList<Value, Interval>::IntervalsRange::end () const
  {
    using ConstIterator =
      typename AugmentedIntervalList<Value,
                                     Interval>::IntervalsRange::ConstIterator;
    return ConstIterator {m_value_intervals, m_indices, m_indices.size ()};
  }

} // namespace interval_dict::augmented_interval_list

#endif // INCLUDE_INTERVAL_DICT_AUGMENTED_INTERVAL_LIST_H
// TODO find other lower() and upper() dynamic intervals that break logic
// TODO separate amortisation parameter and logic 0.2 * m_value_intervals.size()
// TODO check calculate running_max_end still works
// TODO tests for insertion deletions
// TODO all intervals should step over tombstoned intervals
// TODO check final_values() works for dynamic intervals
// TODO check touching set works
// TODO int_fast32_t???
// TODO Eytzinger Binary Search

// TODO sandwiched_gaps SandwichedGapCRef
// TODO disjoint_intervals ValuesDisjointIntervalCRef
// TODO intervals ValueInterval or ValueIntervalCRef