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
/// \file test_interval_overlaps.cpp
/// \brief Test count_overlap()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include <boost/icl/concept/interval.hpp>
#include <boost/icl/interval.hpp>
#include <interval_dict/gregorian.h>
#include <interval_dict/interval_overlaps.h>
#include <interval_dict/ptime.h>
#include <interval_dict/value_interval.h>
#include <numeric>

const int COUNT_EDGES = 14;
const int COUNT_INTERVALS = COUNT_EDGES / 2;
using Val = int;

// Adapted from the answer submitted by gatoatigrado at
// https://stackoverflow.com/questions/5360220/how-to-split-a-list-into-pairs-in-all-possible-ways
class PermuteIntervals
{
public:
    PermuteIntervals(int count_elements)
        : m_count_elements(count_elements), m_state(count_elements / 2, 0),
          m_reverse_indices(m_count_elements)
    {
        for (int i = 0; i < count_elements; i += 2)
        {
            m_limits.push_back(i);
        }
        std::reverse(m_limits.begin(), m_limits.end());
        std::iota(m_reverse_indices.begin(), m_reverse_indices.end(), 0);
        std::reverse(m_reverse_indices.begin(), m_reverse_indices.end());
        m_state.back() = -1;
    }
    std::vector<std::tuple<int, int>> permute()
    {
        if (m_state == m_limits)
        {
            return {};
        }
        for (int i = m_limits.size() - 1; i >= 0; --i)
        {
            if (++m_state[i] <= m_limits[i])
            {
                break;
            }
            m_state[i] = 0;
        }

        std::vector<std::tuple<int, int>> result;
        auto reverse_indices = m_reverse_indices;
        for (const auto choice : m_state)
        {
            auto rev_choice = reverse_indices.size() - choice - 2;
            result.push_back(
                {reverse_indices.back(), reverse_indices[rev_choice]});
            reverse_indices.erase(reverse_indices.begin() + rev_choice);
            reverse_indices.pop_back();
        }
        return result;
    }

private:
    int m_count_elements;
    std::vector<int> m_limits;
    std::vector<int> m_state;
    std::vector<int> m_reverse_indices;
};

/// Get indices that are doubled
/// from 0,0,1,1.. to count_intervals - 1,count_intervals - 1
/// These are used as the begin and end of intervals
/// The actual value of the intervals are in a separate "loci" parameterised
/// by time, int, double, date.
///
/// The doubling means that we are guaranteed to create intervals
/// in some of the permutations [0-10], [10-20]
/// Note how "10" appears twice
std::vector<int> get_repeating_indices(int count_intervals)
{
    std::vector<int> indices(count_intervals * 2);
    std::iota(indices.begin(), indices.begin() + count_intervals, 0);
    std::iota(indices.begin() + count_intervals, indices.end(), 0);
    sort(indices.begin(), indices.end());
    return indices;
}

/// Interval values (loci)
/// parameterised by time, int, double, date.
void get_loci(std::vector<boost::gregorian::date>& loci, int count)
{
    using namespace interval_dict::date_literals;
    auto pos = 20100101_dt;
    for (int i = 0; i < count; ++i, pos += boost::gregorian::days(10))
    {
        loci.push_back(pos);
    }
}

void get_loci(std::vector<boost::posix_time::ptime>& loci, int count)
{
    using namespace interval_dict::ptime_literals;
    auto pos = "20100101T180000"_pt;
    for (int i = 0; i < count; ++i, pos += boost::posix_time::hours(10 * 24))
    {
        loci.push_back(pos);
    }
}

template <typename T> void get_loci(std::vector<T>& loci, int count)
{
    T pos = 0;
    for (int i = 0; i < count; ++i, pos += 10)
    {
        loci.push_back(pos);
    }
}

template <typename Val, typename Interval>
std::vector<interval_dict::ValueInterval<Val, Interval>>
make_intervals(const std::vector<std::tuple<int, int>>& pairs,
               const std::vector<int>& interval_indices,
               const std::vector<typename Interval::domain_type>& loci)
{
    std::vector<interval_dict::ValueInterval<Val, Interval>> m_intervals;
    for (const auto& [i, j] : pairs)
    {
        int first = interval_indices[i];
        int second = interval_indices[j];
        if (first > second)
        {
            std::swap(first, second);
        }
        Interval interval{loci[first], loci[second]};
        if (!boost::icl::is_empty(interval))
        {
            m_intervals.push_back({i, interval});
        }
    }
    std::sort(m_intervals.begin(),
              m_intervals.end(),
              interval_dict::comparisons::CompareInterval{});
    return m_intervals;
}

/// Count number of subsequent intervals each interval overlaps
template <typename Val, typename Interval>
std::vector<int_fast32_t> manual_count_overlapping(
    const std::vector<interval_dict::ValueInterval<Val, Interval>>& m_intervals)
{
    const int count_intervals = m_intervals.size();
    std::vector<int_fast32_t> overlap_counts(count_intervals, 0);
    for (int i = 0; i < count_intervals; ++i)
    {
        for (int j = i + 1; j < count_intervals; ++j)
        {
            if (m_intervals[i].interval == m_intervals[j].interval)
            {
                continue;
            }
            if (boost::icl::intersects(m_intervals[i].interval,
                                       m_intervals[j].interval))
            {
                ++overlap_counts[i];
            }
        }
    }
    return overlap_counts;
}

template <typename Val, typename Interval>
auto calculate_max_right_edges(
    const std::vector<interval_dict::ValueInterval<Val, Interval>>& m_intervals)
{
    using BaseType = typename Interval::domain_type;
    std::vector<BaseType> m_max_right_edges;
    m_max_right_edges.resize(m_intervals.size());

    // calculate max_right_edges
    auto max_end =
        interval_dict::comparisons::upper_edge(m_intervals[0].interval);
    m_max_right_edges[0] = max_end;
    for (auto i = 1; i < m_intervals.size(); ++i)
    {
        max_end = std::max(
            interval_dict::comparisons::upper_edge(m_intervals[i].interval),
            max_end);
        m_max_right_edges[i] = max_end;
    }
    return m_max_right_edges;
};

template <typename Val, typename Interval>
std::vector<int> binary_search(
    const Interval& query_interval,
    const std::vector<interval_dict::ValueInterval<Val, Interval>>& m_intervals,
    const std::vector<typename Interval::domain_type>& m_max_right_edges)
{
    const auto query_start =
        interval_dict::comparisons::lower_edge(query_interval);
    const auto query_end =
        interval_dict::comparisons::upper_edge(query_interval);
    std::vector<int> matching_indices;
    // Binary search for the last item that we should start search from
    // N.B. Extra -1 because lower_bound returns the "open" end
    // of our search from begin->end, and we actually want the "last" index
    int_fast32_t i =
        std::lower_bound(m_intervals.begin(),
                         m_intervals.end(),
                         query_end,
                         // query_interval.right >= m_intervals[i].left
                         [](const auto& iv, auto q) {
                             return !interval_dict::comparisons::exclusive_less(
                                 q, iv.interval);
                         }) -
        m_intervals.begin() - 1;

    while (i >= 0 && m_max_right_edges[i] >= query_start)
    {
        // m_intervals[i].interval.right >= query_interval.left
        if (!interval_dict::comparisons::exclusive_less(m_intervals[i].interval,
                                                        query_interval))
        {
            matching_indices.push_back(i);
        }
        --i;
    }
    std::sort(matching_indices.begin(), matching_indices.end());
    return matching_indices;
}

template <typename Val, typename Interval>
std::vector<int> binary_search_touch(
    const Interval& query_interval,
    const std::vector<interval_dict::ValueInterval<Val, Interval>>& m_intervals,
    const std::vector<typename Interval::domain_type>& m_max_right_edges)
{
    const auto query_start =
        interval_dict::comparisons::lower_edge(query_interval);
    const auto query_start_touches =
        boost::icl::domain_prior<Interval>(query_start);
    std::vector<int> matching_indices;
    // Binary search for the last item that we should start search from
    // N.B. Extra -1 because lower_bound returns the "open" end
    // of our search from begin->end, and we actually want the "last" index
    int_fast32_t i =
        std::lower_bound(m_intervals.begin(),
                         m_intervals.end(),
                         query_interval,
                         [](const auto& iv, const auto& q) {
                             return interval_dict::comparisons::more_or_touches(
                                 q, iv.interval);
                         }) -
        m_intervals.begin() - 1;

    while (i >= 0 && m_max_right_edges[i] >= query_start_touches)
    {
        // m_intervals[i].interval.right >= query_interval.left
        if (interval_dict::comparisons::more_or_touches(m_intervals[i].interval,
                                                        query_interval))
        {
            matching_indices.push_back(i);
        }
        --i;
    }
    std::sort(matching_indices.begin(), matching_indices.end());
    return matching_indices;
}

template <typename Val, typename Interval>
std::vector<int> linear_search_touch(
    const Interval& query_interval,
    const std::vector<interval_dict::ValueInterval<Val, Interval>>& m_intervals)
{
    std::vector<int> matching_indices;
    for (int i = 0; i < m_intervals.size(); ++i)
    {
        if (boost::icl::intersects(m_intervals[i].interval, query_interval) ||
            boost::icl::touches(query_interval, m_intervals[i].interval) ||
            boost::icl::touches(m_intervals[i].interval, query_interval))
        {
            matching_indices.push_back(i);
        }
    }
    return matching_indices;
}

template <typename Val, typename Interval>
std::vector<int> linear_search(
    const Interval& query_interval,
    const std::vector<interval_dict::ValueInterval<Val, Interval>>& m_intervals)
{
    std::vector<int> matching_indices;
    for (int i = 0; i < m_intervals.size(); ++i)
    {
        if (boost::icl::intersects(m_intervals[i].interval, query_interval))
        {
            matching_indices.push_back(i);
        }
    }
    return matching_indices;
}

TEMPLATE_TEST_CASE("Test unsorted_interval_indices",
                   "[unsorted_interval_indices]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
                   boost::icl::interval<float>::type,
                   boost::icl::left_open_interval<float>,
                   boost::icl::right_open_interval<float>,
                   boost::icl::interval<boost::posix_time::ptime>::type,
                   boost::icl::left_open_interval<boost::posix_time::ptime>,
                   boost::icl::right_open_interval<boost::posix_time::ptime>,
                   boost::icl::open_interval<boost::posix_time::ptime>,
                   boost::icl::closed_interval<boost::posix_time::ptime>,
                   boost::icl::discrete_interval<boost::posix_time::ptime>,
                   boost::icl::interval<boost::gregorian::date>::type,
                   boost::icl::left_open_interval<boost::gregorian::date>,
                   boost::icl::right_open_interval<boost::gregorian::date>,
                   boost::icl::open_interval<boost::gregorian::date>,
                   boost::icl::closed_interval<boost::gregorian::date>,
                   boost::icl::discrete_interval<boost::gregorian::date>)
{
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    std::vector<BaseType> loci;
    get_loci(loci, COUNT_EDGES);
    auto interval_indices = get_repeating_indices(COUNT_INTERVALS);
    PermuteIntervals permute_intervals(COUNT_EDGES);

    size_t max_count_overlaps = 0;

    while (1)
    {
        auto pairs = permute_intervals.permute();
        if (pairs.empty())
        {
            break;
        }

        const auto m_intervals =
            make_intervals<Val, Interval>(pairs, interval_indices, loci);
        if (m_intervals.empty())
        {
            continue;
        }
        const auto m_max_right_edges = calculate_max_right_edges(m_intervals);

        for (int i = 0; i < COUNT_INTERVALS; ++i)
        {
            Interval query_interval{loci[i], loci[i + 1]};
            const auto ls = linear_search(query_interval, m_intervals);
            const auto bs =
                binary_search(query_interval, m_intervals, m_max_right_edges);
            REQUIRE(ls == bs);
            max_count_overlaps = std::max(max_count_overlaps, ls.size());
        }
    }
    std::cerr << "max_count_overlaps = " << max_count_overlaps << "\n";
}

enum EIntervalType
{
    LEFT_OPEN,
    RIGHT_OPEN,
    OPEN,
    CLOSED
};

template <typename T>
std::tuple<std::string, typename boost::icl::interval<T>::type>
create_value(T left, T right, EIntervalType type_enum)
{
    using Interval = typename boost::icl::interval<T>::type;
    switch (type_enum)
    {
    case LEFT_OPEN:
        return {"LEFT_OPEN", Interval::left_open(left, right)};
    case RIGHT_OPEN:
        return {"RIGHT_OPEN", Interval::right_open(left, right)};
    case OPEN:
        return {"OPEN", Interval::open(left, right)};
    case CLOSED:
        return {"CLOSED", Interval::closed(left, right)};
    }
}

TEST_CASE("Test int unsorted_interval_indices",
          "[int unsorted_interval_indices]")
{
    using Interval = boost::icl::interval<int>::type;
    using BaseType = typename Interval::domain_type;
    std::vector<BaseType> loci;
    get_loci(loci, COUNT_EDGES);
    auto interval_indices = get_repeating_indices(COUNT_INTERVALS);
    PermuteIntervals permute_intervals(COUNT_EDGES);

    size_t max_count_overlaps = 0;

    while (1)
    {
        auto pairs = permute_intervals.permute();
        if (pairs.empty())
        {
            break;
        }

        const auto m_intervals =
            make_intervals<Val, Interval>(pairs, interval_indices, loci);
        if (m_intervals.empty())
        {
            continue;
        }
        const auto m_max_right_edges = calculate_max_right_edges(m_intervals);

        for (int j = 0; j < 4; ++j)
        {
            for (int i = 0; i < COUNT_INTERVALS; ++i)
            {
                auto [name1, query_interval] =
                    create_value(loci[i], loci[i + 1], EIntervalType(j));
                const auto ls = linear_search(query_interval, m_intervals);
                const auto bs = binary_search(
                    query_interval, m_intervals, m_max_right_edges);
                REQUIRE(ls == bs);
                max_count_overlaps = std::max(max_count_overlaps, ls.size());
            }
        }
    }
    std::cerr << "max_count_overlaps = " << max_count_overlaps << "\n";
}

TEMPLATE_TEST_CASE("Test unsorted_interval_indices_touch",
                   "[unsorted_interval_indices_touch]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
                   boost::icl::interval<float>::type,
                   boost::icl::left_open_interval<float>,
                   boost::icl::right_open_interval<float>,
                   boost::icl::interval<boost::posix_time::ptime>::type,
                   boost::icl::left_open_interval<boost::posix_time::ptime>,
                   boost::icl::right_open_interval<boost::posix_time::ptime>,
                   boost::icl::open_interval<boost::posix_time::ptime>,
                   boost::icl::closed_interval<boost::posix_time::ptime>,
                   boost::icl::discrete_interval<boost::posix_time::ptime>,
                   boost::icl::interval<boost::gregorian::date>::type,
                   boost::icl::left_open_interval<boost::gregorian::date>,
                   boost::icl::right_open_interval<boost::gregorian::date>,
                   boost::icl::open_interval<boost::gregorian::date>,
                   boost::icl::closed_interval<boost::gregorian::date>,
                   boost::icl::discrete_interval<boost::gregorian::date>)
{
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    std::vector<BaseType> loci;
    get_loci(loci, COUNT_EDGES);
    auto interval_indices = get_repeating_indices(COUNT_INTERVALS);
    PermuteIntervals permute_intervals(COUNT_EDGES);

    size_t max_count_overlaps = 0;

    int permutation_index = 0;
    while (1)
    {
        auto pairs = permute_intervals.permute();
        if (pairs.empty())
        {
            break;
        }
        const auto m_intervals =
            make_intervals<Val, Interval>(pairs, interval_indices, loci);
        if (m_intervals.empty())
        {
            continue;
        }
        const auto m_max_right_edges = calculate_max_right_edges(m_intervals);

        for (int i = 0; i < COUNT_INTERVALS; ++i)
        {
            Interval query_interval{loci[i], loci[i + 1]};
            const auto ls = linear_search_touch(query_interval, m_intervals);
            const auto bs = binary_search_touch(
                query_interval, m_intervals, m_max_right_edges);
            REQUIRE(ls == bs);
            max_count_overlaps = std::max(max_count_overlaps, ls.size());
        }
        ++permutation_index;
    }
    std::cerr << "max_count_overlaps = " << max_count_overlaps << "\n";
}
TEMPLATE_TEST_CASE("Test count_overlaps",
                   "[count_overlaps]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
                   boost::icl::interval<float>::type,
                   boost::icl::left_open_interval<float>,
                   boost::icl::right_open_interval<float>,
                   boost::icl::interval<boost::posix_time::ptime>::type,
                   boost::icl::left_open_interval<boost::posix_time::ptime>,
                   boost::icl::right_open_interval<boost::posix_time::ptime>,
                   boost::icl::open_interval<boost::posix_time::ptime>,
                   boost::icl::closed_interval<boost::posix_time::ptime>,
                   boost::icl::discrete_interval<boost::posix_time::ptime>,
                   boost::icl::interval<boost::gregorian::date>::type,
                   boost::icl::left_open_interval<boost::gregorian::date>,
                   boost::icl::right_open_interval<boost::gregorian::date>,
                   boost::icl::open_interval<boost::gregorian::date>,
                   boost::icl::closed_interval<boost::gregorian::date>,
                   boost::icl::discrete_interval<boost::gregorian::date>)
{

    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    std::vector<BaseType> loci;
    get_loci(loci, COUNT_EDGES);
    auto interval_indices = get_repeating_indices(COUNT_INTERVALS);
    PermuteIntervals permute_intervals(COUNT_EDGES);

    interval_dict::CountOverlap<Interval> overlap_counter;

    while (1)
    {
        auto pairs = permute_intervals.permute();
        if (pairs.empty())
        {
            break;
        }
        const auto m_intervals =
            make_intervals<Val, Interval>(pairs, interval_indices, loci);
        if (m_intervals.empty())
        {
            continue;
        }

        overlap_counter.update(m_intervals);
        auto bs = overlap_counter.m_counts;
        auto ls = manual_count_overlapping(m_intervals);
        bool identical = (ls == bs);
        if constexpr (std::is_same_v<BaseType, float>)
        {
            if (!identical)
            {
                identical = true;
                for (int i = 0; i < ls.size(); ++i)
                {
                    if (std::abs(ls[i] - bs[i]) > 1)
                    {
                        identical = false;
                    }
                }
            }
        }
        if (!identical)
        {
            REQUIRE(overlap_counter.m_counts ==
                    manual_count_overlapping(m_intervals));
        }
    }
}

// #include <cxxabi.h>
//
// template<typename T>
// std::string get_type_name()
// {
//     char const * mangled = typeid(T).name();
//     int status = 0;
//     char * c_demangled = abi::__cxa_demangle( mangled, nullptr, nullptr,
//     &status);
//
//     if (c_demangled)
//     {
//         std::string res = c_demangled;
//         free(c_demangled);
//         return res;
//     }
//     else
//     {
//         return mangled;
//     }
// }

// Mix of intervals