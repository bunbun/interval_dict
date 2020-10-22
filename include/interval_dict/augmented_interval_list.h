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
/// \file augmented_interval_list.h
/// \brief Definitions of functions to implement Augmented Interval List
///
/// Original idea described in
/// "Augmented Interval List: a novel data structure for efficient
/// genomic interval search" Feng, Ratan & Sheffield (2019)
/// Bioinformatics 35:4907–4911
///
/// Algorithm extensively revised so that the actual lists of intervals
/// may be quite different from Feng et. al. All bugs are my own.
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_AUGMENTED_INTERVAL_LIST_H
#define INCLUDE_INTERVAL_DICT_AUGMENTED_INTERVAL_LIST_H

#include "default_init_allocator.h"
#include "interval_compare.h"
#include "interval_operators.h"
#include "interval_overlaps.h"
#include "interval_traits.h"
#include "non_disjoint.h"
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

namespace interval_dict
{

namespace augmented_interval_list
{

// A sorted run of intervals denoted by begin and end indices
// Note that end points to one past the last index
struct Run
{
    Run(int_fast32_t begin, int_fast32_t end) : begin(begin), end(end)
    {
    }

    int_fast32_t begin;
    int_fast32_t end;
};

std::ostream& operator<<(std::ostream& os, const Run& run)
{
    os << "[" << run.begin << " - " << run.end << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Run>& runs)
{
    for (const auto& run : runs)
    {
        os << "  " << run << "\n";
    }
    return os;
}

namespace details
{
template <typename Val, typename Interval>
using TouchingSet =
    std::set<ValueInterval<Val, Interval>,
             interval_dict::comparisons::CompareValIntervalTouches>;

} // namespace details

template <typename Val, typename Interval> struct AugmentedIntervalList
{
    static_assert(
        !(boost::icl::is_continuous_interval<Interval>::value &&
          boost::icl::has_dynamic_bounds<Interval>::value),
        "This library does not support continuous intervals (e.g. float) "
        "with dynamic bounds (that can be set to open/closed at run time)");

    using ValueIntervalType = ValueInterval<Val, Interval>;
    // Matching indices type driven by efficiency savings in query()
    // with a non-default initialising allocator
    using VecIndices =
        std::vector<int_fast32_t, DefaultInitAllocator<int_fast32_t>>;

    struct AllIntervals
    {
    public:
        class const_iterator
        {
        public:
            using difference_type = ptrdiff_t;
            using value_type = ValueInterval<Val, Interval>;
            using const_reference = const value_type&;
            using const_pointer = const value_type*;
            using iterator_category = std::input_iterator_tag;

            const_iterator(
                const std::vector<ValueInterval<Val, Interval>>& intervals,
                std::vector<Run> runs)
                : m_intervals(intervals), m_runs(std::move(runs))
            {
            }

            const_iterator(const const_iterator& other)
                : m_intervals(other.m_intervals), m_runs(other.m_runs)
            {
            }

            ~const_iterator() = default;

            const_iterator& operator=(const const_iterator& other);

            bool operator==(const const_iterator& other) const;
            bool operator!=(const const_iterator& other) const;

            // use bubble sort to keep m_runs inverse sorted by the interval at
            // the head of each run.
            const_iterator& operator++();
            const_iterator operator++(int);
            const_reference operator*() const;
            const_pointer operator->() const;

        private:
            const std::vector<ValueInterval<Val, Interval>>& m_intervals;
            std::vector<Run> m_runs;
        };

        AllIntervals(const std::vector<ValueInterval<Val, Interval>>& intervals,
                     std::vector<Run> runs);

        const_iterator begin() const;
        const_iterator end() const;

    private:
        const std::vector<ValueInterval<Val, Interval>>& m_intervals;
        std::vector<Run> m_runs;
    };

    AugmentedIntervalList();

    explicit AugmentedIntervalList(
        std::vector<ValueInterval<Val, Interval>> intervals,
        int max_overlapping_neighbours = 20,
        int min_run_length = 256,
        int max_runs_count = 30,
        double max_fraction_promoted_per_run = 0.20)
        : m_max_overlapping_neighbours(
              static_cast<int_fast32_t>(max_overlapping_neighbours)),
          m_min_run_length(static_cast<int_fast32_t>(min_run_length)),
          m_max_runs_count(static_cast<int_fast32_t>(max_runs_count)),
          m_max_fraction_promoted_per_run(max_fraction_promoted_per_run)
    {
        insert(intervals);
        decompose_into_runs();
    }

    /// Copy constructor
    AugmentedIntervalList(const AugmentedIntervalList&) = default;
    /// Copy assignment operator
    AugmentedIntervalList&
    operator=(const AugmentedIntervalList& other) = default;
    /// Default move constructor
    AugmentedIntervalList(AugmentedIntervalList&& other) noexcept = default;
    /// Default move assignment operator
    AugmentedIntervalList&
    operator=(AugmentedIntervalList&& other) noexcept = default;

    /// @return true if tree is empty
    [[nodiscard]] bool is_empty() const;

    /// @return coroutine enumerating all interval/values
    AllIntervals all_intervals() const
    {
        return {m_intervals, m_runs};
    }

    /// @return coroutine enumerating all interval/values over @p query_interval
    cppcoro::generator<std::tuple<Interval, const Val&>>
    intervals(const Interval& query_interval) const;

    /// @return the gaps between intervals
    cppcoro::generator<Interval> gaps() const;

    /// @return the gaps between intervals and the values on either side
    cppcoro::generator<
        std::tuple<const std::vector<Val>&, Interval, const std::vector<Val>&>>
    sandwiched_gaps() const;

    /// @return coroutine enumerating all interval/values over @p query_interval
    cppcoro::generator<std::tuple<Interval, const std::set<Val>&>>
    disjoint_intervals(const Interval& query_interval) const;

    /// Returns the first disjoint interval (possibly containing multiple
    /// values)
    std::tuple<Interval, std::set<Val>> initial_values() const;

    /// add values and intervals, merging overlaps intervals with the same value
    void insert(std::vector<ValueInterval<Val, Interval>>& value_intervals);

    /// @return last disjoint interval (possibly containing multiple values)
    std::tuple<Interval, std::set<Val>> final_values() const;

    /// @erase specified value over interval
    void erase(const Interval& query_interval, const Val& value);

    /// @erase all values over interval
    void erase(const Interval& query_interval);

    /// Merges intervals from another tree into this
    AugmentedIntervalList& merged_with(const AugmentedIntervalList& other);

    /// Returns the asymmetrical difference with another set of
    /// implementation-dependent interval-values
    AugmentedIntervalList& subtract_by(const AugmentedIntervalList& other);

    /// @return all sorted unique values over the specified interval
    std::vector<Val> values(const Interval& query_interval) const;

    /// For range iteration of const
    auto begin() const
    {
        AllIntervals(m_intervals, m_runs).begin();
    }

    /// For range iteration of const
    auto end() const
    {
        AllIntervals(m_intervals, m_runs).end();
    }

    //____________________TODO__________________________________________________

    /// equality operator
    bool operator==(const AugmentedIntervalList& rhs) const;

    void sorted_match_indices(const Interval& query,
                              VecIndices& matching_indices) const;
    void unsorted_match_indices(const Interval& query,
                                VecIndices& matching_indices) const;
    void unsorted_match_value_indices(const Interval& query_interval,
                                      const Val& query_value,
                                      VecIndices& matching_indices) const;

    std::vector<ValueInterval<Val, Interval>> m_intervals;
    std::vector<typename ValueInterval<Val, Interval>::BaseType>
        m_max_right_edges;
    std::vector<Run> m_runs;

private:
    // split m_intervals into Spans each of which is sorted by beg
    // has its cummulative (running) maximum right edge in m_max_right_edges
    void decompose_into_runs();

    // add value and interval to last run
    void
    insert_helper(const ValueInterval<Val, Interval>& value_interval,
                  details::TouchingSet<Val, Interval>& new_value_intervals);

    // mark intervals with indices as erased by setting their end to the
    // minimum value
    void mark_as_erased(const VecIndices& indices);

    int_fast32_t m_max_overlapping_neighbours = 20;
    int_fast32_t m_min_run_length = 256;
    int_fast32_t m_max_runs_count = 30;
    double m_max_fraction_promoted_per_run = 0.20;
    int_fast32_t m_count_removed = 0;
    int_fast32_t m_count_inserted = 0;
    // runs excluding adhoc insertions
    int_fast32_t m_optimal_runs = 0;

    void calculate_running_max_end(int intervals_offset = 0);
};

template <typename Val, typename Interval>
AugmentedIntervalList<Val, Interval>::AugmentedIntervalList()
{
    m_intervals.reserve(64);
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::calculate_running_max_end(
    int intervals_offset)
{
    m_max_right_edges.resize(m_intervals.size());
    for (const auto& [begin, end] : m_runs)
    {
        if (end < intervals_offset)
        {
            continue;
        }
        assert(end - begin > 1);
        auto max_end =
            interval_dict::comparisons::upper_edge(m_intervals[begin].interval);
        m_max_right_edges[begin] = max_end;
        for (auto i = begin + 1; i < end; ++i)
        {
            max_end = std::max(
                interval_dict::comparisons::upper_edge(m_intervals[i].interval),
                max_end);
            m_max_right_edges[i] = max_end;
        }
    }
}

namespace details
{

int_fast32_t quantile(const std::vector<int_fast32_t>& values, double fraction)
{
    if (values.empty())
    {
        return 0;
    }
    auto scratch = values;
    std::sort(scratch.begin(), scratch.end());
    return scratch[int_fast32_t(scratch.size() * fraction)];
}

} // namespace details

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::decompose_into_runs()
{
    if ((m_count_inserted + m_count_removed) > 0.2 * m_intervals.size() ||
        m_intervals.size() < m_min_run_length)
    {
        m_count_inserted = 0;
        m_count_removed = 0;
        m_optimal_runs = 0;
    }

    if (m_optimal_runs < m_runs.size())
    {
        m_runs.erase(m_runs.begin() + m_optimal_runs, m_runs.end());
    }

    // offset to first interval to decompose
    int intervals_offset = m_runs.empty() ? 0 : m_runs.back().end;

    // all intervals sorted
    if (intervals_offset == m_intervals.size())
    {
        if (m_count_inserted + m_count_removed == 0)
        {
            m_optimal_runs = m_runs.size();
        }
        return;
    }

    // Remove intervals marked as erased that are not part of a Run
    std::remove_if(
        m_intervals.begin() + intervals_offset,
        m_intervals.end(),
        [](const auto& iv) { return boost::icl::is_empty(iv.interval); });
    std::sort(m_intervals.begin() + intervals_offset,
              m_intervals.end(),
              interval_dict::comparisons::CompareInterval());

    if (m_intervals.size() - intervals_offset <= m_min_run_length)
    {
        m_runs.emplace_back(intervals_offset, (int_fast32_t)m_intervals.size());
        calculate_running_max_end(intervals_offset);
        if (m_count_inserted + m_count_removed == 0)
        {
            m_optimal_runs = m_runs.size();
        }
        return;
    }

    // scratch space
    auto unresolved = m_intervals;

    // intervals that cover more than 'm_max_overlapping_neighbours' subsequent
    // intervals
    std::vector<ValueInterval<Val, Interval>> enclosing;

    int_fast32_t runs_count = 0;
    int_fast32_t pos = intervals_offset;
    int_fast32_t run_start = intervals_offset;
    CountOverlap<Interval> overlap_counter;
    while (unresolved.size())
    {
        int unresolved_size = unresolved.size();
        // short last run: save verbatim
        if (unresolved_size < m_min_run_length ||
            m_runs.size() == m_max_runs_count - 1)
        {
            std::copy(unresolved.begin(),
                      unresolved.end(),
                      m_intervals.begin() + pos);
            m_runs.emplace_back(pos, pos + unresolved.size());
            break;
        }

        // calculate how many neighbours each interval overlaps
        // intervals which overlap too many neighbours are promoted
        overlap_counter.update(unresolved);

        // Make sure we use a level of overlapping_threshold that
        // doesn't promote too much of the list
        int_fast32_t overlapping_threshold =
            std::max(m_max_overlapping_neighbours,
                     details::quantile(overlap_counter.m_counts,
                                       1.0 - m_max_fraction_promoted_per_run));

        // short last run: save verbatim
        if (unresolved_size < overlapping_threshold)
        {
            std::copy(unresolved.begin(),
                      unresolved.end(),
                      m_intervals.begin() + pos);
            m_runs.emplace_back(pos, pos + unresolved.size());
            break;
        }

        for (int_fast32_t i = 0; i < unresolved_size - overlapping_threshold;
             i++)
        {
            // Overlapping too many neighbours copy to enclosing
            if (overlap_counter.m_counts[i] > overlapping_threshold)
            {
                enclosing.push_back(unresolved[i]);
            }
            // Not much overlapping: Resolve / copy to m_intervals
            else
            {
                m_intervals[pos] = unresolved[i];
                ++pos;
            }
        }
        // last overlapping_threshold items by definition do not have
        // too many overlapping neightbours and can be resolved to
        // m_intervals
        std::copy(unresolved.begin() + unresolved_size - overlapping_threshold,
                  unresolved.begin() + unresolved_size,
                  m_intervals.begin() + pos);
        pos += overlapping_threshold;
        assert(run_start >= 0);
        m_runs.emplace_back(run_start, pos);
        run_start = pos;
        runs_count++;

        std::swap(enclosing, unresolved);
        enclosing.clear();
    }

    calculate_running_max_end(intervals_offset);
    if (m_count_inserted + m_count_removed == 0)
    {
        m_optimal_runs = m_runs.size();
    }
}

/// @return coroutine enumerating all interval/values over @p query_interval
template <typename Val, typename Interval>
cppcoro::generator<std::tuple<Interval, const Val&>>
AugmentedIntervalList<Val, Interval>::intervals(
    const Interval& query_interval) const
{
    VecIndices matching_indices;
    sorted_match_indices(query_interval, matching_indices);
    for (const auto i : matching_indices)
    {
        co_yield std::tuple{m_intervals[i].interval & query_interval,
                            m_intervals[i].value};
    }
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::sorted_match_indices(
    const Interval& query_interval, VecIndices& matching_indices) const
{
    unsorted_match_indices(query_interval, matching_indices);
    std::sort(
        matching_indices.begin(), matching_indices.end(), [&](auto a, auto b) {
            return m_intervals[a].interval < m_intervals[b].interval;
        });
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::unsorted_match_indices(
    const Interval& query_interval, VecIndices& matching_indices) const
{
    if (m_runs.empty())
    {
        return;
    }
    matching_indices.clear();
    const auto query_start =
        interval_dict::comparisons::lower_edge(query_interval);
    const auto query_end =
        interval_dict::comparisons::upper_edge(query_interval);
    int cnt_elements = 0;
    // Buffer of uninitialised int indexed by cnt_elements gives a considerable
    // speed up compared with std::vector push_back
    for (const auto& [begin, end] : m_runs)
    {
        matching_indices.resize(matching_indices.size() + end - begin);

        // brute force small runs
        if (end - begin <= 15)
        {
            for (int_fast32_t i = begin; i < end; ++i)
            {
                if (boost::icl::intersects(m_intervals[i].interval,
                                           query_interval))
                {
                    matching_indices[cnt_elements++] = i;
                }
            }
            continue;
        }

        // Binary search for the last item that we should start search from
        // N.B. Extra -1 because lower_bound returns the "open" end
        // of our search from begin->end, and we actually want the "last" index
        int_fast32_t i =
            std::lower_bound(
                m_intervals.begin() + begin,
                m_intervals.begin() + end,
                query_end,
                // query_interval.right >= m_intervals[i].left
                [](const auto& iv, auto q) {
                    return !interval_dict::comparisons::exclusive_less(
                        q, iv.interval);
                }) -
            m_intervals.begin() - 1;

        while (i >= begin && m_max_right_edges[i] >= query_start)
        {
            // m_intervals[i].interval.right >= query_interval.left
            if (!interval_dict::comparisons::exclusive_less(
                    m_intervals[i].interval, query_interval))
            {
                matching_indices[cnt_elements++] = i;
            }
            --i;
        }
    }
    matching_indices.resize(cnt_elements);
}

template <typename Val, typename Interval>
bool AugmentedIntervalList<Val, Interval>::is_empty() const
{
    return m_intervals.empty();
}

template <typename Val, typename Interval>
bool AugmentedIntervalList<Val, Interval>::operator==(
    const AugmentedIntervalList& rhs) const
{
    return std::tie(m_intervals,
                    m_max_right_edges,
                    m_runs,
                    m_max_overlapping_neighbours,
                    m_min_run_length,
                    m_max_runs_count,
                    m_max_fraction_promoted_per_run,
                    m_count_removed,
                    m_count_inserted,
                    m_optimal_runs) ==
           std::tie(rhs.m_intervals,
                    rhs.m_max_right_edges,
                    rhs.m_runs,
                    rhs.m_max_overlapping_neighbours,
                    rhs.m_min_run_length,
                    rhs.m_max_runs_count,
                    rhs.m_max_fraction_promoted_per_run,
                    rhs.m_count_removed,
                    rhs.m_count_inserted,
                    rhs.m_optimal_runs);
}

template <typename Val, typename Interval>
std::tuple<Interval, std::set<Val>>
AugmentedIntervalList<Val, Interval>::initial_values() const
{
    return non_disjoint::initial_values<Val, Interval>(all_intervals());
}

template <typename Val, typename Interval>
cppcoro::generator<std::tuple<Interval, const std::set<Val>&>>
AugmentedIntervalList<Val, Interval>::disjoint_intervals(
    const Interval& query_interval) const
{
    return non_disjoint::disjoint_intervals<Val, Interval, ValueIntervalType>(
        intervals(query_interval), query_interval);
}

template <typename Val, typename Interval>
cppcoro::generator<Interval> AugmentedIntervalList<Val, Interval>::gaps() const
{
    return non_disjoint::gaps<Interval>(all_intervals());
}

template <typename Val, typename Interval>
cppcoro::generator<
    std::tuple<const std::vector<Val>&, Interval, const std::vector<Val>&>>
AugmentedIntervalList<Val, Interval>::sandwiched_gaps() const
{
    return non_disjoint::sandwiched_gaps<Val, Interval, ValueIntervalType>(
        all_intervals());
}

template <typename Val, typename Interval>
std::vector<Val>
AugmentedIntervalList<Val, Interval>::values(const Interval& interval) const
{
    return non_disjoint::values<Val>(intervals(interval));
}

namespace details
{

template <typename Val, typename Interval>
auto find_overlapping(const ValueInterval<Val, Interval>& value_interval,
                      TouchingSet<Val, Interval>& new_value_intervals)
    -> std::array<typename TouchingSet<Val, Interval>::iterator, 2>
{
    return {new_value_intervals.lower_bound(value_interval),
            new_value_intervals.upper_bound(value_interval)};
}

template <typename Val, typename Interval>
std::vector<ValueInterval<Val, Interval>> subtract_intervals(
    const std::vector<ValueInterval<Val, Interval>>& value_intervals,
    const std::vector<ValueInterval<Val, Interval>>& other)
{
    using interval_dict::comparisons::CompareValInterval;
    assert(std::is_sorted(
        value_intervals.begin(), value_intervals.end(), CompareValInterval{}));
    assert(std::is_sorted(other.begin(), other.end(), CompareValInterval{}));

    auto first1 = value_intervals.begin();
    auto last1 = value_intervals.end();
    auto first2 = other.end();
    auto last2 = other.begin();
    std::vector<ValueInterval<Val, Interval>> result;

    while (first1 != last1)
    {
        // nothing more to subtract
        if (first2 == last2)
        {
            result.insert(result.end(), first1, last1);
            break;
        }

        if (first1->value < first2->value ||
            boost::icl::exclusive_less(first1->interval, first2->interval))
        {
            result.push_back(*first1++);
        }
        else if (first2->value < first1->value ||
                 boost::icl::exclusive_less(first2->interval, first1->interval))
        {
            ++first2;
        }

        // same value and overlap
        else
        {
            if (boost::icl::lower(first1->interval) <
                boost::icl::lower(first2->interval))
            {
                result.push_back(boost::icl::right_subtract(first1->interval,
                                                            first2->interval),
                                 first1->value);
            }

            if (boost::icl::upper(first1->interval) >
                boost::icl::upper(first2->interval))
            {
                result.push_back(boost::icl::left_subtract(first1->interval,
                                                           first2->interval),
                                 first1->value);
            }
            ++first1;
        }
    }
    return result;
}

/// Sort and merge overlapping intervals with the same value
template <typename Val, typename Interval>
void join_intervals(std::vector<ValueInterval<Val, Interval>>& value_intervals)
{
    // remove empty intervals
    value_intervals.erase(
        std::remove_if(
            value_intervals.begin(),
            value_intervals.end(),
            [](const auto& iv) { return boost::icl::is_empty(iv.interval); }),
        value_intervals.end());

    using interval_dict::comparisons::CompareValInterval;
    std::sort(
        value_intervals.begin(), value_intervals.end(), CompareValInterval{});
    value_intervals.erase(
        std::unique(value_intervals.begin(),
                    value_intervals.end(),
                    [](auto& lhs, const auto& rhs) {
                        // Only discard rhs (return true) if
                        // 1. values are the identical
                        // 2. intervals overlap
                        // In which case, also extend lhs
                        if (lhs.value != rhs.value)
                        {
                            return false;
                        }
                        // if overlap: extend lhs and discard rhs by
                        if (!interval_dict::comparisons::exclusive_less(
                                lhs.interval, rhs.interval))
                        {
                            lhs.interval =
                                boost::icl::hull(lhs.interval, rhs.interval);
                            return true;
                        }
                        return false;
                    }),
        value_intervals.end());
}

} // namespace details

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::insert_helper(
    const ValueInterval<Val, Interval>& value_interval,
    details::TouchingSet<Val, Interval>& new_value_intervals)
{
    // ignore empty intervals
    if (boost::icl::is_empty(value_interval.interval))
    {
        return;
    }

    VecIndices matching_indices;
    unsorted_match_value_indices(
        value_interval.interval, value_interval.value, matching_indices);

    // If first contains interval, skip: nothing will change on insert
    if (matching_indices.size() == 1 &&
        boost::icl::contains(m_intervals[matching_indices.front()].interval,
                             value_interval.interval))
    {
        return;
    }

    // Remove overlapping or even touching
    auto total_interval = value_interval.interval;
    for (const auto i : matching_indices)
    {
        total_interval =
            boost::icl::hull(m_intervals[i].interval, total_interval);
    }
    m_count_removed += matching_indices.size();
    mark_as_erased(matching_indices);

    // Look inside newly inserted intervals
    auto [i_new, end_new] =
        details::find_overlapping(value_interval, new_value_intervals);
    while (i_new != end_new)
    {
        // If contains interval, skip: nothing will change on insert
        if (boost::icl::contains(i_new->interval, value_interval.interval))
        {
            return;
        }
        total_interval = boost::icl::hull(i_new->interval, total_interval);
        i_new = new_value_intervals.erase(i_new);
    }

    // Add new interval including overlapping/touching
    assert(!boost::icl::is_empty(total_interval));
    new_value_intervals.insert({value_interval.value, total_interval});
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::insert(
    std::vector<ValueInterval<Val, Interval>>& value_intervals)
{
    // Sort and merge overlapping intervals with the same value
    details::join_intervals(value_intervals);
    details::TouchingSet<Val, Interval> new_value_intervals;
    for (const auto& value_interval : value_intervals)
    {
        // add value and interval to last run
        insert_helper(value_interval, new_value_intervals);
    }

    m_count_inserted += new_value_intervals.size();
    m_intervals.reserve(m_intervals.size() + new_value_intervals.size());
    m_intervals.insert(m_intervals.end(),
                       new_value_intervals.begin(),
                       new_value_intervals.end());
    decompose_into_runs();
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::erase(const Interval& interval)
{
    VecIndices matching_indices;
    unsorted_match_indices(interval, matching_indices);
    for (const auto i : matching_indices)
    {
        if (boost::icl::lower(m_intervals[i].interval) <
            boost::icl::lower(interval))
        {
            m_intervals.push_back(
                boost::icl::right_subtract(m_intervals[i].interval, interval),
                m_intervals[i].value);
        }
        if (boost::icl::upper(m_intervals[i].interval) >
            boost::icl::upper(interval))
        {
            m_intervals.push_back(
                boost::icl::left_subtract(m_intervals[i].interval, interval),
                m_intervals[i].value);
        }
    }
    mark_as_erased(matching_indices);
    decompose_into_runs();
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::erase(const Interval& interval,
                                                 const Val& value)
{
    VecIndices matching_indices;
    unsorted_match_value_indices(interval, value, matching_indices);
    for (const auto i : matching_indices)
    {
        if (boost::icl::lower(m_intervals[i].interval) <
            boost::icl::lower(interval))
        {
            m_intervals.push_back(
                boost::icl::right_subtract(m_intervals[i].interval, interval),
                value);
        }
        if (boost::icl::upper(m_intervals[i].interval) >
            boost::icl::upper(interval))
        {
            m_intervals.push_back(
                boost::icl::left_subtract(m_intervals[i].interval, interval),
                value);
        }
    }
    mark_as_erased(matching_indices);
    decompose_into_runs();
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::mark_as_erased(
    const VecIndices& indices)
{
    const auto interval_max = IntervalTraits<Interval>::max();
    using interval_dict::operators::right_extend;
    for (const auto i : indices)
    {
        assert(i < (int_fast32_t)m_intervals.size());
        m_intervals[i].interval =
            right_extend(m_intervals[i].interval, interval_max);
    }
}

template <typename Val, typename Interval>
AugmentedIntervalList<Val, Interval>&
AugmentedIntervalList<Val, Interval>::merged_with(
    const AugmentedIntervalList& other)
{
    // If too few, just pretend it is a normal insert
    if (other.m_intervals.size() + m_count_inserted + m_count_removed <
            0.2 * m_intervals.size() &&
        m_intervals.size() > m_min_run_length)
    {
        insert(other.m_intervals);
        return;
    }

    m_intervals.reserve(m_intervals.size() + other.m_intervals.size());
    m_intervals.insert(
        m_intervals.end(), other.m_intervals.begin(), other.m_intervals.end());
    details::join_intervals(m_intervals);
    m_runs.clear();
    decompose_into_runs();
}

template <typename Val, typename Interval>
AugmentedIntervalList<Val, Interval>&
AugmentedIntervalList<Val, Interval>::subtract_by(
    const AugmentedIntervalList& other)
{
    // If too few, just pretend it is a normal erase
    if (other.m_intervals.size() + m_count_inserted + m_count_removed <
            0.2 * m_intervals.size() &&
        m_intervals.size() > m_min_run_length)
    {
        for (const auto& [value, interval] : other.m_intervals)
        {
            erase(value, interval);
        }
    }

    using interval_dict::comparisons::CompareValInterval;
    std::sort(m_intervals.begin(), m_intervals.end(), CompareValInterval{});
    auto intervals_to_subtract = other.m_intervals;
    std::sort(intervals_to_subtract.begin(),
              intervals_to_subtract.end(),
              CompareValInterval{});
    m_intervals =
        details::subtract_intervals(m_intervals, intervals_to_subtract);
    m_runs.clear();
    decompose_into_runs();
}

template <typename Val, typename Interval>
void AugmentedIntervalList<Val, Interval>::unsorted_match_value_indices(
    const Interval& query_interval,
    const Val& query_value,
    VecIndices& matching_indices) const
{
    if (m_runs.empty())
    {
        return;
    }
    matching_indices.clear();
    const auto query_start =
        interval_dict::comparisons::lower_edge(query_interval);
    const auto query_end =
        interval_dict::comparisons::upper_edge(query_interval);
    int cnt_elements = 0;

    // Buffer of uninitialised int indexed by cnt_elements gives a considerable
    // speed up compared with std::vector push_back
    for (const auto& [begin, end] : m_runs)
    {
        matching_indices.resize(matching_indices.size() + end - begin);

        // brute force small runs
        if (end - begin <= 15)
        {
            for (int_fast32_t i = begin; i < end; ++i)
            {
                if (m_intervals[i].value == query_value &&
                    boost::icl::intersects(m_intervals[i].interval,
                                           query_interval))
                {
                    matching_indices[cnt_elements++] = i;
                }
            }
            continue;
        }

        // Binary search for the last item that we should start search from
        // N.B. Extra -1 because lower_bound returns the "open" end
        // of our search from begin->end, and we actually want the "last" index
        int_fast32_t i =
            std::lower_bound(
                m_intervals.begin() + begin,
                m_intervals.begin() + end,
                query_end,
                // query_interval.right >= m_intervals[i].left
                [](const auto& iv, auto q) {
                    return !interval_dict::comparisons::exclusive_less(
                        q, iv.interval);
                }) -
            m_intervals.begin() - 1;

        while (i >= begin && m_max_right_edges[i] >= query_start)
        {
            // m_intervals[i].interval.right >= query_interval.left
            if (m_intervals[i].value == query_value &&
                !interval_dict::comparisons::exclusive_less(
                    m_intervals[i].interval, query_interval))
            {
                matching_indices[cnt_elements++] = i;
            }
            --i;
        }
    }
    matching_indices.resize(cnt_elements);
}

template <typename Val, typename Interval>
typename AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator&
AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator::operator=(
    const typename AugmentedIntervalList<Val, Interval>::AllIntervals::
        const_iterator& other)
{
    assert(&m_intervals == &other.m_intervals);
    m_runs = other.m_runs;
}

template <typename Val, typename Interval>
bool AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator::
operator==(const typename AugmentedIntervalList<Val, Interval>::AllIntervals::
               const_iterator& other) const
{
    assert(&m_intervals == &other.m_intervals);
    return (other.m_runs.size() == m_runs.size() && other.m_runs == m_runs);
}

template <typename Val, typename Interval>
bool AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator::
operator!=(const typename AugmentedIntervalList<Val, Interval>::AllIntervals::
               const_iterator& other) const
{
    assert(&m_intervals == &other.m_intervals);
    return (other.m_runs.size() != m_runs.size() || other.m_runs != m_runs);
}

template <typename Val, typename Interval>
typename AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator
AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator::operator++(
    int)
{
    typename AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator
        cpy(*this);
    this->operator++();
    return cpy;
}

template <typename Val, typename Interval>
typename AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator&
AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator::operator++()
{
    assert(m_runs.size());
    if (++m_runs.back().begin >= m_runs.back().end)
    {
        m_runs.pop_back();
        return *this;
    }
    if (m_runs.size() <= 1)
    {
        return *this;
    }

    auto new_pos = std::lower_bound(m_runs.begin(),
                                    m_runs.end(),
                                    m_runs.back(),
                                    [this](const auto& a, const auto& b) {
                                        return this->m_intervals[b.begin] <
                                               this->m_intervals[a.begin];
                                    });
    if (new_pos < m_runs.end() - 1)
    {
        std::rotate(new_pos, m_runs.end() - 1, m_runs.end());
    }
    return *this;
}

template <typename Val, typename Interval>
const ValueInterval<Val, Interval>&
AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator::operator*()
    const
{
    assert(m_runs.size());
    assert(m_runs.back().begin != m_runs.back().end);
    return m_intervals[m_runs.back().begin];
}

template <typename Val, typename Interval>
const ValueInterval<Val, Interval>*
AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator::operator->()
    const
{
    assert(m_runs.size());
    assert(m_runs.back().begin != m_runs.back().end);
    return &m_intervals[m_runs.back().begin];
}

template <typename Val, typename Interval>
AugmentedIntervalList<Val, Interval>::AllIntervals::AllIntervals(
    const std::vector<ValueInterval<Val, Interval>>& intervals,
    std::vector<Run> runs)
    : m_intervals(intervals), m_runs(std::move(runs))
{
    // remove empty and sort
    // after this assume runs are always sorted
    std::remove_if(m_runs.begin(), m_runs.end(), [](const auto& a) {
        return a.begin >= a.end;
    });
    std::sort(m_runs.begin(),
              m_runs.end(),
              [&intervals](const auto& a, const auto& b) {
                  return intervals[b.begin] < intervals[a.begin];
              });
}

template <typename Val, typename Interval>
typename AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator
AugmentedIntervalList<Val, Interval>::AllIntervals::begin() const
{
    using const_iterator =
        typename AugmentedIntervalList<Val,
                                       Interval>::AllIntervals::const_iterator;
    return const_iterator{m_intervals, m_runs};
}

template <typename Val, typename Interval>
typename AugmentedIntervalList<Val, Interval>::AllIntervals::const_iterator
AugmentedIntervalList<Val, Interval>::AllIntervals::end() const
{
    using const_iterator =
        typename AugmentedIntervalList<Val,
                                       Interval>::AllIntervals::const_iterator;
    return const_iterator{m_intervals, {}};
}

template <typename Val, typename Interval>
std::tuple<Interval, std::set<Val>>
AugmentedIntervalList<Val, Interval>::final_values() const
{
    if (m_runs.empty())
    {
        return {};
    }

    // get maximum_right_edge in entire array
    typename ValueInterval<Val, Interval>::BaseType max_right_edge =
        m_max_right_edges[m_runs.front().end - 1];
    for (auto [begin, end] : m_runs)
    {
        assert(end > begin);
        max_right_edge = std::max(max_right_edge, m_max_right_edges[end - 1]);
    }

    std::tuple<Interval, std::set<Val>> result;
    Interval interval;
    std::set<Val> values;
    using interval_dict::comparisons::upper_edge;
    for (const auto& [begin, end] : m_runs)
    {
        auto i = end - 1;
        while (i >= begin && m_max_right_edges[i] == max_right_edge)
        {
            if (upper_edge(m_intervals[i].interval) == max_right_edge)
            {
                if (result.values.empty())
                {
                    result.interval = m_intervals[i].interval;
                }
                else
                {
                    result.interval = interval & m_intervals[i].interval;
                }
                result.values.insert(m_intervals[i].value);
            }
        }
    }
    return result;
}

} // namespace augmented_interval_list

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_AUGMENTED_INTERVAL_LIST_H

// TODO: unsorted_match_value_indices