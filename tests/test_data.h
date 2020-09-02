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
/// \file test_data.h
/// \brief Provide test data set
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef TESTS_TEST_DATA_H
#define TESTS_TEST_DATA_H

#include <interval_dict/gregorian.h>
#include <interval_dict/ptime.h>

#include <interval_dict/bi_intervaldicticl.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/bi_intervaldictitree.h>
#include <interval_dict/intervaldictitree.h>

#include "test_utils.h"

#include <boost/lexical_cast.hpp>

#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

// Take the upper value of an interval and construct an empty interval around it
template <typename Interval>
Interval empty_interval_from_upper(Interval orig_interval)
{
    const auto orig_pos = boost::icl::upper(orig_interval);
    Interval interval{orig_pos, orig_pos};
    if (boost::icl::is_empty(interval))
    {
        return interval;
    }
    auto pos = orig_pos;
    //using boost::icl::operator--;
    --pos;
    return Interval{orig_pos, pos};
}

//        0....1....2....3....4....5....6....7....8....9...10...11...12...13...14...15...16...17
// aa     0----0                                  0-------------------------------------------0
// bb          1---------1    1----1
// bb                         2---------2
// cc                    3-------------------3
// cc                              3-----------------------------3
// dd                         5---------5              5----------------------------5
// dd                                   6----6
// dd                                   7----7
// Query        q--------------------------------------q
// fill to start <-|10
// fill to end                                       50|->
// fill_gaps(3)

namespace test_detail
{
// Initial test data to construct a single interval_dict
template <typename Interval>
std::vector<std::tuple<std::string, int, Interval>> initial(
    const std::vector<typename Interval::domain_type>& values)
{
    using namespace std::string_literals;
    return {
        {"aa"s, 0, Interval{values[0], values[1]}},
        {"aa"s, 0, Interval{values[8], values[17]}},
        {"bb"s, 1, Interval{values[1], values[3]}},
        {"bb"s, 1, Interval{values[4], values[5]}},
        {"bb"s, 2, Interval{values[4], values[6]}},
        {"cc"s, 3, Interval{values[3], values[7]}},
        {"cc"s, 3, Interval{values[5], values[11]}},
        {"dd"s, 5, Interval{values[4], values[6]}},
        {"dd"s, 6, boost::icl::left_subtract(Interval{values[5], values[7]},
                                             Interval{values[4], values[6]})},
        {"dd"s, 7, boost::icl::left_subtract(Interval{values[5], values[7]},
                                             Interval{values[4], values[6]})},
        {"dd"s, 5, Interval{values[9], values[15]}},
    };
}

// Expected changes after fill_gaps
template <typename Interval>
std::vector<std::tuple<std::string, int, Interval>> fill_gaps_with_data(
    const std::vector<typename Interval::domain_type>& values)
{
    using namespace std::string_literals;
    return {
        {"aa"s, 25, Interval{values[0], values[17]}},
        {"ff"s, 26, Interval{values[0], values[1]}},
    };
}

// Initial test data to construct another interval_dict to join to the first
template <typename Interval>
std::vector<std::tuple<int, double, Interval>> initial2(
    const std::vector<typename Interval::domain_type>& values)
{
    using namespace std::string_literals;
    return {
        {0, 3.1, {values[0], values[5]}},
        {0, 4.1, {values[9], values[17]}},
        {1, 1.1, {values[1], values[3]}},
        {1, 2.1, {values[4], values[7]}},
        {2, 5.1, {values[1], values[6]}},
        {2, 6.1, {values[8], values[17]}},
        {3, 4.1, {values[3], values[8]}},
        {3, 9.1, {values[9], values[11]}},
        {4, 1.1, {values[5], values[12]}},
        {5, 7.1, {values[0], values[7]}},
        {5, 2.1, {values[8], values[11]}},
        {6, 6.1, {values[0], values[8]}},
        {6, 3.1, {values[9], values[17]}},
        {7, 8.1, {values[0], values[7]}},
        {7, 6.1, {values[9], values[17]}},
        {8, 9.1, {values[6], values[7]}},
    };
}

// Expected results from calling intervals()
template <typename Interval>
std::vector<std::tuple<std::string, int, Interval>> intervals(
    const std::vector<typename Interval::domain_type>& values)
{
    using namespace std::string_literals;
    const auto adjust = Adjust<Interval>{};
    return {
        {"aa"s, 0, {values[0], values[1]}},
        {"aa"s, 0, {values[8], values[17]}},
        {"bb"s, 1, {values[1], values[3]}},
        {"bb"s, 1, {values[4], values[5]}},
        {"bb"s, 2, {values[4], values[6]}},
        {"cc"s, 3, {values[3], values[11]}},
        {"dd"s, 5, {values[4], values[6]}},
        {"dd"s, 6, adjust.lower({values[6], values[7]})},
        {"dd"s, 7, adjust.lower({values[6], values[7]})},
        {"dd"s, 5, {values[9], values[15]}},
    };
}

// Expected results from calling disjoint_intervals()
template <typename Interval>
std::vector<std::tuple<std::string, std::set<int>, Interval>>
disjoint_intervals(
    const std::vector<typename Interval::domain_type>& values)
{
    using namespace std::string_literals;
    const auto adjust = Adjust<Interval>{};
    return {
        {"aa"s, {0},    {values[0], values[1]}},
        {"aa"s, {0},    {values[8], values[17]}},
        {"bb"s, {1},    {values[1], values[3]}},
        {"bb"s, {1, 2}, {values[4], values[5]}},
        {"bb"s, {2},    adjust.lower({values[5], values[6]})},
        {"cc"s, {3},    {values[3], values[11]}},
        {"dd"s, {5},    {values[4], values[6]}},
        {"dd"s, {6, 7}, adjust.lower({values[6], values[7]})},
        {"dd"s, {5},    {values[9], values[15]}},
    };
}

template <typename Interval>
std::string
to_str(Interval interval)
{
    return boost::lexical_cast<std::string>(interval);
}

// Expected string representation
template <typename Interval>
std::string
to_str(const std::vector<typename Interval::domain_type>& values)
{
    using namespace std::string_literals;
    const auto adjust = Adjust<Interval>{};
    auto result =
        "aa\t[0]\t"s +    to_str(Interval{values[0], values[1]}) + "\n"
        "aa\t[0]\t"s +    to_str(Interval{values[8], values[17]}) + "\n"
        "bb\t[1]\t"s +    to_str(Interval{values[1], values[3]}) + "\n"
        "bb\t[1, 2]\t"s + to_str(Interval{values[4], values[5]}) + "\n"
        "bb\t[2]\t"s +    to_str(adjust.lower({values[5], values[6]}))
        + "\n"
        "cc\t[3]\t"s +    to_str(Interval{values[3], values[11]}) + "\n"
        "dd\t[5]\t"s +    to_str(Interval{values[4], values[6]}) + "\n"
        "dd\t[6, 7]\t"s + to_str(adjust.lower({values[6], values[7]}))
        + "\n"
        "dd\t[5]\t"s +    to_str(Interval{values[9], values[15]}) + "\n";

    // deliberately ignore open / closed notation for comparisons
    std::replace(result.begin(), result.end(), ')', ']');
    std::replace(result.begin(), result.end(), '(', '[');
    return result;
}



}

template <typename domain_type> struct TestDataValues
{
    static std::vector<domain_type> values()
    {        // 0  1  2   3   4   5   6   7   8   9   10  11  12  13  14  15, 16, 17
        return {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85};
    }
    static std::vector<domain_type> midvalues()
    {
        return {3, 8, 13, 18, 23, 28, 33, 38, 43, 48, 53, 58, 63, 68, 73, 78, 83, 88};
    }

    static domain_type test_duration()
    {
        return 3;
    }

};

template<> struct TestDataValues<boost::gregorian::date>
{
    static boost::gregorian::date_duration test_duration()
    {
        return boost::gregorian::days(10);
    }

    static std::vector<boost::gregorian::date> values()
    {
        //     0101 0115 0201 0215 0301 0315 0401 0415 0501 0515 0601 0615 0701
        using namespace interval_dict::date_literals;
        return{20100101_dt,
               20100115_dt,
               20100201_dt,
               20100215_dt,
               20100301_dt,
               20100315_dt,
               20100401_dt,
               20100415_dt,
               20100501_dt,
               20100515_dt,
               20100601_dt,
               20100615_dt,
               20100701_dt,
               20100715_dt,
               20100801_dt,
               20100815_dt,
               20100901_dt,
               20100915_dt,
        };
    }

    static std::vector<boost::gregorian::date> midvalues()
    {
        using namespace interval_dict::date_literals;
        return{20100103_dt,
               20100118_dt,
               20100203_dt,
               20100218_dt,
               20100303_dt,
               20100318_dt,
               20100403_dt,
               20100418_dt,
               20100503_dt,
               20100518_dt,
               20100603_dt,
               20100618_dt,
               20100703_dt,
               20100718_dt,
               20100803_dt,
               20100818_dt,
               20100903_dt,
               20100918_dt,
        };
    }
};

template<> struct TestDataValues<boost::posix_time::ptime>
{
    static std::vector<boost::posix_time::ptime> values()
    {
        //     0101 0115 0201 0215 0301 0315 0401 0415 0501 0515 0601 0615 0701
        using namespace interval_dict::ptime_literals;
        return{"20100101T180000"_pt,
               "20100115T180000"_pt,
               "20100201T180000"_pt,
               "20100215T180000"_pt,
               "20100301T180000"_pt,
               "20100315T180000"_pt,
               "20100401T180000"_pt,
               "20100415T180000"_pt,
               "20100501T180000"_pt,
               "20100515T180000"_pt,
               "20100601T180000"_pt,
               "20100615T180000"_pt,
               "20100701T180000"_pt,
               "20100715T180000"_pt,
               "20100801T180000"_pt,
               "20100815T180000"_pt,
               "20100901T180000"_pt,
               "20100915T180000"_pt,
        };
    }

    static std::vector<boost::posix_time::ptime> midvalues()
    {
        using namespace interval_dict::ptime_literals;
        return{"20100103T180000"_pt,
               "20100118T180000"_pt,
               "20100203T180000"_pt,
               "20100218T180000"_pt,
               "20100303T180000"_pt,
               "20100318T180000"_pt,
               "20100403T180000"_pt,
               "20100418T180000"_pt,
               "20100503T180000"_pt,
               "20100518T180000"_pt,
               "20100603T180000"_pt,
               "20100618T180000"_pt,
               "20100703T180000"_pt,
               "20100718T180000"_pt,
               "20100803T180000"_pt,
               "20100818T180000"_pt,
               "20100903T180000"_pt,
               "20100918T180000"_pt,
        };
    }

    static boost::posix_time::time_duration test_duration()
    {
        return boost::posix_time::hours(10 * 24);
    }
};


/*
 * Normalised data for int/float
 */
template <typename Interval, typename enable = void> struct TestData
{
    std::vector<typename Interval::domain_type> values
     = TestDataValues<typename Interval::domain_type>::values();
        // 0  1  2   3   4   5   6   7   8   9   10  11  12  13  14  15, 16, 17
        //{0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85};
    std::vector<typename Interval::domain_type> midvalues
    = TestDataValues<typename Interval::domain_type>::midvalues();
        //{3, 8, 13, 18, 23, 28, 33, 38, 43, 48, 53, 58, 63, 68, 73, 78, 83, 88};

    typename interval_dict::IntervalTraits<Interval>::BaseDifferenceType test_duration
     = TestDataValues<typename Interval::domain_type>::test_duration();

    std::vector<std::tuple<std::string, int, Interval>> initial()
    {
        return test_detail::initial<Interval>(values);
    }

    std::vector<std::tuple<int, double, Interval>> initial2()
    {
        return test_detail::initial2<Interval>(values);
    }

    std::vector<std::tuple<std::string, int, Interval>> fill_gaps_with_data()
    {
        return test_detail::fill_gaps_with_data<Interval>(values);
    }

    std::vector<std::tuple<std::string, int, Interval>> intervals() const
    {
        return test_detail::intervals<Interval>(values);
    }
    std::string str()
    {
        return test_detail::to_str<Interval>(values);
    }

    std::vector<std::tuple<std::string, std::set<int>, Interval>>
    disjoint_intervals() const
    {
        return test_detail::disjoint_intervals<Interval>(values);
    }

    Interval query_interval() const
    {
        return {values[1], values[10]};
    }
    Interval empty_interval() const
    {
        return empty_interval_from_upper(query_interval());
    }
    Interval query_interval_for_find() const
    {
        return {values[1], midvalues[4]};
    }
    interval_dict::Intervals<Interval> query_intervals_for_find() const
    {
        interval_dict::Intervals<Interval> intervals;
        intervals.add({midvalues[0], midvalues[1]});
        intervals.add({midvalues[6], midvalues[7]});
        return intervals;
    }
};

#endif // TESTS_TEST_DATA_H
