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

#include <interval_dict/intervaldicticl.h>

#include "test_icl.h"

#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

// Take the upper value of an interval and construct an empty interval around it
template<typename Interval>
Interval empty_interval_from_upper(Interval orig_interval)
{
    const auto orig_pos = boost::icl::upper(orig_interval);
    Interval interval {orig_pos, orig_pos};
    if (boost::icl::is_empty(interval))
    {
        return interval;
    }
    using interval_dict::operator--;
    auto pos = orig_pos;
    return Interval{orig_pos, --pos};
}

/*
 * Normalised data for int/float
 */
template <typename Val, typename Interval, typename enable = void>
struct TestData
{
    //        0... 05.. 15.. 20.. 25.. 30.. 35.. 40.. 45.. 50.. 55.. 75.. 85..
    // aa     0----0                             0------------------------0
    // bb          1----1    1----1
    // bb                    2---------2
    // cc               3-------------------3
    // cc                         3-----------------------------3
    // dd                    5---------5              5--------------5
    // dd                              6----6
    // dd                              7----7
    // Query        q--------------------------------------q
    // fill to start <-|10
    // fill to end                                       50|->
    // fill_gaps(3)
    std::vector<std::tuple<std::string, Val, Interval>> initial()
    {
        using namespace std::string_literals;
        return {
            // aa-0 gap
            {"aa"s, 0, Interval{0, 5}},
            {"aa"s, 0, Interval{40, 85}},

            // Gap of bb-1 {15-20}
            {"bb"s, 1, Interval{5, 15}},
            {"bb"s, 1, Interval{20, 25}},
            {"bb"s, 2, Interval{20, 30}},

            // overlap ignored
            {"cc"s, 3, Interval{15, 35}},
            {"cc"s, 3, Interval{25, 55}},

            // dd-6/7 over {30-35}
            {"dd"s, 5, Interval{20, 30}},
            {"dd"s,
             6,
             boost::icl::left_subtract(Interval{29, 35}, Interval{20, 30})},
            {"dd"s,
             7,
             boost::icl::left_subtract(Interval{29, 35}, Interval{20, 30})},
            {"dd"s, 5, Interval{45, 75}},
        };
    }

    std::vector<std::tuple<std::string, Val, Interval>> intervals() const
    {
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", 0, {0, 5}},
            {"aa", 0, {40, 85}},
            {"bb", 1, {5, 15}},
            {"bb", 1, {20, 25}},
            {"bb", 2, {20, 30}},
            {"cc", 3, {15, 55}},
            {"dd", 5, {20, 30}},
            {"dd", 5, {45, 75}},
            {"dd", 6, adjust.lower({30, 35})},
            {"dd", 7, adjust.lower({30, 35})},
        };
    }
    std::vector<std::tuple<std::string, std::set<Val>, Interval>>
    disjoint_intervals() const
    {
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", {0}, {0, 5}},
            {"aa", {0}, {40, 85}},
            {"bb", {1}, {5, 15}},
            {"bb", {1, 2}, {20, 25}},
            {"bb", {2}, adjust.lower({25, 30})},
            {"cc", {3}, {15, 55}},
            {"dd", {5}, {20, 30}},
            {"dd", {6, 7}, adjust.lower({30, 35})},
            {"dd", {5}, {45, 75}},
        };
    }
    Interval query_interval() const
    {
        return {5, 50};
    }
    Interval empty_interval() const
    {
        return empty_interval_from_upper(query_interval());
    }
    Interval query_interval_for_find() const
    {
        return {5, 22};
    }
    interval_dict::Intervals<Interval> query_intervals_for_find() const
    {
        interval_dict::Intervals<Interval> intervals;
        intervals.add({2, 8});
        intervals.add({33, 36});
        return intervals;
    }
};

/*
 * Normalised data for date
 */
template <typename Val, typename Interval>
struct TestData<
    Val,
    Interval,
    typename std::enable_if<std::is_same<typename Interval::domain_type,
                                         boost::gregorian::date>::value>::type>
{
    std::vector<std::tuple<std::string, Val, Interval>> initial()
    {
        using namespace std::string_literals;
        static_assert(std::is_same<typename Interval::domain_type,
                                   boost::gregorian::date>::value,
                      "Must be an interval of dates");
        using namespace std::string_literals;
        using namespace interval_dict::date_literals;
        //     0101 0115 0201 0215 0301 0315 0401 0415 0501 0515 0601 0615 0701
        // aa  0----0                             0------------------------0
        // bb       1----1    1----1
        // bb                 2---------2
        // cc            3-------------------3
        // cc                      3-----------------------------3
        // dd                 5---------5              5--------------5
        // dd                           6----6
        // dd                           7----7
        // Query     q--------------------------------------q
        // f to start <-|0120
        // f to end                                     0515|->
        // fill_gaps(10)
        return {
            // aa-0 gap
            {"aa"s, 0, {20100101_dt, 20100115_dt}},
            {"aa"s, 0, {20100415_dt, 20100701_dt}},

            // Gap of bb-1 {20100201-20100215}
            {"bb"s, 1, {20100115_dt, 20100201_dt}},
            {"bb"s, 1, {20100215_dt, 20100301_dt}},
            {"bb"s, 2, {20100215_dt, 20100315_dt}},

            // overlap merged
            {"cc"s, 3, {20100201_dt, 20100401_dt}},
            {"cc"s, 3, {20100301_dt, 20100601_dt}},

            // dd-6/7 over {20100315-20100401}
            {"dd"s, 5, {20100215_dt, 20100315_dt}},
            {"dd"s,
             6,
             boost::icl::left_subtract(Interval{20100313_dt, 20100401_dt},
                                       Interval{20100215_dt, 20100315_dt})},
            {"dd"s,
             7,
             boost::icl::left_subtract(Interval{20100313_dt, 20100401_dt},
                                       Interval{20100215_dt, 20100315_dt})},
            {"dd"s, 5, {20100501_dt, 20100615_dt}},
        };
    }

    std::vector<std::tuple<std::string, Val, Interval>> intervals() const
    {
        using namespace interval_dict::date_literals;
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", 0, {20100101_dt, 20100115_dt}},
            {"aa", 0, {20100415_dt, 20100701_dt}},
            {"bb", 1, {20100115_dt, 20100201_dt}},
            {"bb", 1, {20100215_dt, 20100301_dt}},
            {"bb", 2, {20100215_dt, 20100315_dt}},
            {"cc", 3, {20100201_dt, 20100601_dt}},
            {"dd", 5, {20100215_dt, 20100315_dt}},
            {"dd", 5, {20100501_dt, 20100615_dt}},
            {"dd", 6, adjust.lower({20100315_dt, 20100401_dt})},
            {"dd", 7, adjust.lower({20100315_dt, 20100401_dt})},
        };
    }

    std::vector<std::tuple<std::string, std::set<Val>, Interval>>
    disjoint_intervals() const
    {
        using namespace interval_dict::date_literals;
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", {0}, {20100101_dt, 20100115_dt}},
            {"aa", {0}, {20100415_dt, 20100701_dt}},
            {"bb", {1}, {20100115_dt, 20100201_dt}},
            {"bb", {1, 2}, {20100215_dt, 20100301_dt}},
            {"bb", {2}, adjust.lower({20100301_dt, 20100315_dt})},
            {"cc", {3}, {20100201_dt, 20100601_dt}},
            {"dd", {5}, {20100215_dt, 20100315_dt}},
            {"dd", {6, 7}, adjust.lower({20100315_dt, 20100401_dt})},
            {"dd", {5}, {20100501_dt, 20100615_dt}},
        };
    }
    Interval query_interval() const
    {
        using namespace interval_dict::date_literals;
        return {20100215_dt, 20100515_dt};
    }
    Interval empty_interval() const
    {
        return empty_interval_from_upper(query_interval());
    }
    Interval query_interval_for_find() const
    {
        using namespace interval_dict::date_literals;
        return {20100215_dt, 20100220_dt};
    }
    interval_dict::Intervals<Interval> query_intervals_for_find() const
    {
        using namespace interval_dict::date_literals;
        interval_dict::Intervals<Interval> intervals;
        intervals.add({20100110_dt, 20100120_dt});
        intervals.add({20100325_dt, 20100405_dt});
        return intervals;
    }
};

/*
 * Normalised data for posix time
 */
template <typename Val, typename Interval>
struct TestData<Val,
                Interval,
                typename std::enable_if<
                    std::is_same<typename Interval::domain_type,
                                 boost::posix_time::ptime>::value>::type>
{
    std::vector<std::tuple<std::string, Val, Interval>> initial()
    {
        using namespace std::string_literals;
        static_assert(std::is_same<typename Interval::domain_type,
                                   boost::posix_time::ptime>::value,
                      "Must be an interval of posix time");
        using namespace std::string_literals;
        using namespace interval_dict::ptime_literals;
        return {
            // aa-0 gap
            {"aa"s, 0, {"20100101T180000"_pt, "20100115T180000"_pt}},
            {"aa"s, 0, {"20100415T180000"_pt, "20100701T180000"_pt}},

            // Gap of bb-1 {20100201-20100215}
            {"bb"s, 1, {"20100115T180000"_pt, "20100201T180000"_pt}},
            {"bb"s, 1, {"20100215T180000"_pt, "20100301T180000"_pt}},
            {"bb"s, 2, {"20100215T180000"_pt, "20100315T180000"_pt}},

            // overlap ignored
            {"cc"s, 3, {"20100201T180000"_pt, "20100401T180000"_pt}},
            {"cc"s, 3, {"20100301T180000"_pt, "20100601T180000"_pt}},

            // dd-6/7 over {20100315-20100401}
            {"dd"s, 5, {"20100215T180000"_pt, "20100315T180000"_pt}},
            {"dd"s,
             6,
             boost::icl::left_subtract(
                 Interval{"20100313T180000"_pt, "20100401T180000"_pt},
                 Interval{"20100215T180000"_pt, "20100315T180000"_pt})},
            {"dd"s,
             7,
             boost::icl::left_subtract(
                 Interval{"20100313T180000"_pt, "20100401T180000"_pt},
                 Interval{"20100215T180000"_pt, "20100315T180000"_pt})},
            {"dd"s, 5, {"20100501T180000"_pt, "20100615T180000"_pt}},
        };
    }
    std::vector<std::tuple<std::string, std::set<Val>, Interval>>
    disjoint_intervals() const
    {
        using namespace interval_dict::ptime_literals;
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", {0}, {"20100101T180000"_pt, "20100115T180000"_pt}},
            {"aa", {0}, {"20100415T180000"_pt, "20100701T180000"_pt}},
            {"bb", {1}, {"20100115T180000"_pt, "20100201T180000"_pt}},
            {"bb", {1, 2}, {"20100215T180000"_pt, "20100301T180000"_pt}},
            {"bb",
             {2},
             adjust.lower({"20100301T180000"_pt, "20100315T180000"_pt})},
            {"cc", {3}, {"20100201T180000"_pt, "20100601T180000"_pt}},
            {"dd", {5}, {"20100215T180000"_pt, "20100315T180000"_pt}},
            {"dd",
             {6, 7},
             adjust.lower({"20100315T180000"_pt, "20100401T180000"_pt})},
            {"dd", {5}, {"20100501T180000"_pt, "20100615T180000"_pt}},
        };
    }
    std::vector<std::tuple<std::string, Val, Interval>> intervals() const
    {
        using namespace interval_dict::ptime_literals;
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", 0, {"20100101T180000"_pt, "20100115T180000"_pt}},
            {"aa", 0, {"20100415T180000"_pt, "20100701T180000"_pt}},
            {"bb", 1, {"20100115T180000"_pt, "20100201T180000"_pt}},
            {"bb", 1, {"20100215T180000"_pt, "20100301T180000"_pt}},
            {"bb", 2, {"20100215T180000"_pt, "20100315T180000"_pt}},
            {"cc", 3, {"20100201T180000"_pt, "20100601T180000"_pt}},
            {"dd", 5, {"20100215T180000"_pt, "20100315T180000"_pt}},
            {"dd", 5, {"20100501T180000"_pt, "20100615T180000"_pt}},
            {"dd",
             6,
             adjust.lower({"20100315T180000"_pt, "20100401T180000"_pt})},
            {"dd",
             7,
             adjust.lower({"20100315T180000"_pt, "20100401T180000"_pt})},
        };
    }
    Interval query_interval() const
    {
        using namespace interval_dict::ptime_literals;
        return {"20100215T180000"_pt, "20100515T180000"_pt};
    }
    Interval empty_interval() const
    {
        return empty_interval_from_upper(query_interval());
    }

    Interval query_interval_for_find() const
    {
        using namespace interval_dict::ptime_literals;
        return {"20100215T000000"_pt, "20100220T180000"_pt};
    }
    interval_dict::Intervals<Interval> query_intervals_for_find() const
    {
        using namespace interval_dict::ptime_literals;
        interval_dict::Intervals<Interval> intervals;
        intervals.add({"20100110T000000"_pt, "20100120T000000"_pt});
        intervals.add({"20100325T000000"_pt, "20100405T000000"_pt});
        return intervals;
    }
};

#endif // TESTS_TEST_DATA_H
