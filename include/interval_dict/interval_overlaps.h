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
/// \file interval_overlaps.h
/// \brief Definition of function to calculate the number of successive
/// intervals that an interval overlaps with.
/// This measures how much a vector of intervals is non-disjoint
/// This is of particular use in augmented_interval_list.
/// Both edges of an interval are added to a list and sorted, and the
/// difference between the count of "left" edges and the index of the
/// right edge is the overlap.
/// The algorithm is slightly complicated by the fact that we don't
/// consider identical intervals as being overlapping.

#ifndef INCLUDE_INTERVAL_DICT_INTERVAL_OVERLAPS_H
#define INCLUDE_INTERVAL_DICT_INTERVAL_OVERLAPS_H

#include "interval_compare.h"
#include "value_interval.h"
#include <iostream>
#include <vector>

namespace interval_dict
{

namespace details
{

/// Data structure to calculate the number of subsequent intervals that
/// each interval overlaps with
template <typename Edge> struct OverlapCounter
{
    OverlapCounter(Edge edge,
                   int_fast32_t beg_index,
                   int_fast32_t end_index,
                   int_fast32_t index)
        : edge(edge), beg_index(beg_index), end_index(end_index), index(index)
    {
    }

    Edge edge;
    // = end_index if start else 0
    int_fast32_t beg_index = 0;
    int_fast32_t end_index = 0;
    int_fast32_t index;

    bool operator<(const OverlapCounter& rhs) const
    {
        if (edge < rhs.edge)
            return true;
        if (rhs.edge < edge)
            return false;
        if (beg_index > rhs.beg_index)
            return true;
        if (rhs.beg_index > beg_index)
            return false;
        if (end_index < rhs.end_index)
            return true;
        if (rhs.end_index < end_index)
            return false;
        return index < rhs.index;
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const OverlapCounter& counter)
    {
        os << "edge: " << counter.edge << " beg_index: " << counter.beg_index
           << " end_index: " << counter.end_index
           << " index: " << counter.index;
        return os;
    }
};
} // namespace details

// If there are no overlaps, then the number of starting points encountered
// before the end of the interval is equal to the index of the interval
// in sorted order.
// If there are 7 starts before the end of interval[5], then interval[5]
// intersects 2 subsequent intervals
// We need to have an sorted array of both interval begins and ends, tagged
// by their indices in begin-sorted order
// 1) From end:
//    end_index = (interval == next_interval) ? next_end_index : index
//    begin_index = is_begin ? end_index : 0
// 2) Sort by pos (begin and end)
// 3) max_beg_index = max(beg_index, max_beg_index)
// 4) diff = max_beg_index - end_index
// 5) assign diff to index.
//    In theory, only update if (beg_index==0), i.e. ends. However, if we allow
//    and ignore updates for starts, we save a conditional
/// @return The number of subsequent intervals each interval overlaps with.

template <typename Interval> class CountOverlap
{
public:
    /// Uses first and last to accurate calulate overlaps
    /// If these functions are not available (for continuous intervals)
    /// we fake them with upper and lower. Note this means that the counts
    /// may be off by one, depending on how open/closed intervals are mixed
    /// together. There is no way to mix the two edges of continuous intervals
    /// so this is an acceptable compromise
    template <typename Val>
    void update(const std::vector<ValueInterval<Val, Interval>>& intervals);
    std::vector<int_fast32_t> m_counts;

    // private:
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;
    std::vector<details::OverlapCounter<BaseType>> m_overlap_counters;
};

template <typename, typename = void> struct has_icl_first : std::false_type
{
};
template <typename T>
struct has_icl_first<
    T,
    std::void_t<decltype(boost::icl::first(std::declval<T>()))>>
    : std::true_type
{
};

template <typename, typename = void> struct has_icl_last : std::false_type
{
};
template <typename T>
struct has_icl_last<T,
                    std::void_t<decltype(boost::icl::last(std::declval<T>()))>>
    : std::true_type
{
};

template <typename Interval>
template <typename Val>
void CountOverlap<Interval>::update(
    const std::vector<ValueInterval<Val, Interval>>& intervals)
{
    m_counts.resize(intervals.size());
    m_overlap_counters.clear();
    m_overlap_counters.reserve(intervals.size() * 2);

    // Edges of last interval
    int_fast32_t next_index = intervals.size() - 1;
    BaseType next_left;
    BaseType next_right;
    if constexpr (has_icl_first<Interval>::value)
    {
        next_left = boost::icl::first(intervals[next_index].interval);
    }
    else
    {
        next_left = boost::icl::lower(intervals[next_index].interval);
    }
    if constexpr (has_icl_last<Interval>::value)
    {
        next_right = boost::icl::last(intervals[next_index].interval);
    }
    else
    {
        next_right = boost::icl::upper(intervals[next_index].interval);
    }

    m_overlap_counters.emplace_back(
        next_left, next_index, next_index, next_index);
    m_overlap_counters.emplace_back(next_right, 0, next_index, next_index);

    // Edges of all but last interval
    for (int_fast32_t index = intervals.size() - 2; index >= 0; --index)
    {
        BaseType left;
        BaseType right;
        if constexpr (has_icl_first<Interval>::value)
        {
            left = boost::icl::first(intervals[index].interval);
        }
        else
        {
            left = boost::icl::lower(intervals[index].interval);
        }
        if constexpr (has_icl_last<Interval>::value)
        {
            right = boost::icl::last(intervals[index].interval);
        }
        else
        {
            right = boost::icl::upper(intervals[index].interval);
        }
        // update index if interval is not identical to the next one
        if (left != next_left || right != next_right)
        {
            next_index = index;
        }
        m_overlap_counters.emplace_back(left, next_index, next_index, index);
        m_overlap_counters.emplace_back(right, 0, next_index, index);
        next_left = left;
        next_right = right;
    }

    // Sort all edges regardless of begin and end
    std::sort(m_overlap_counters.begin(), m_overlap_counters.end());

    int_fast32_t max_beg_index = 0;
    for (const auto& oc : m_overlap_counters)
    {
        max_beg_index = std::max(max_beg_index, oc.beg_index);
        m_counts[oc.index] = max_beg_index - oc.end_index;
    }
}

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVAL_OVERLAPS_H
