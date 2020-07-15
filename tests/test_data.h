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

/*
 * Normalised data for int/float
 */
template <typename Val, typename Interval, typename enable = void>
struct TestData
{
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
    Interval query_interval() const
    {
        return {5, 50};
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
        return {
            // aa-0 gap
            {"aa"s, 0, {20100101_dt, 20100115_dt}},
            {"aa"s, 0, {20100201_dt, 20100215_dt}},

            // Gap of bb-1 {15-20}
            {"bb"s, 1, {20100105_dt, 20100331_dt}},
            {"bb"s, 1, {20100501_dt, 20100531_dt}},
            {"bb"s, 2, {20100501_dt, 20100615_dt}},

            // overlap ignored
            {"cc"s, 3, {20100115_dt, 20100131_dt}},
            {"cc"s, 3, {20100120_dt, 20100215_dt}},

            // dd-6/7 over {30-35}
            {"dd"s, 5, {20100201_dt, 20100301_dt}},
            {"dd"s,
                    6,
                       boost::icl::left_subtract(Interval{20100228_dt, 20100315_dt},
                                                 Interval{20100201_dt, 20100301_dt})},
            {"dd"s,
                    7,
                       boost::icl::left_subtract(Interval{20100228_dt, 20100315_dt},
                                                 Interval{20100201_dt, 20100301_dt})},
            {"dd"s, 5, {20100415_dt, 20100715_dt}},
        };
    }

    std::vector<std::tuple<std::string, Val, Interval>> intervals() const
    {
        using namespace interval_dict::date_literals;
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", 0, {20100101_dt, 20100115_dt}},
            {"aa", 0, {20100201_dt, 20100215_dt}},
            {"bb", 1, {20100105_dt, 20100331_dt}},
            {"bb", 1, {20100501_dt, 20100531_dt}},
            {"bb", 2, {20100501_dt, 20100615_dt}},
            {"cc", 3, {20100115_dt, 20100215_dt}},
            {"dd", 5, {20100201_dt, 20100301_dt}},
            {"dd", 5, {20100415_dt, 20100715_dt}},
            {"dd", 6, adjust.lower({20100301_dt, 20100315_dt})},
            {"dd", 7, adjust.lower({20100301_dt, 20100315_dt})},
        };
    }
    Interval query_interval() const
    {
        using namespace interval_dict::date_literals;
        return {20100215_dt, 20100515_dt};
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
            {"aa"s, 0, {"20100201T180000"_pt, "20100215T180000"_pt}},

            // Gap of bb-1 {15-20}
            {"bb"s, 1, {"20100105T180000"_pt, "20100331T180000"_pt}},
            {"bb"s, 1, {"20100501T180000"_pt, "20100531T180000"_pt}},
            {"bb"s, 2, {"20100501T180000"_pt, "20100615T180000"_pt}},

            // overlap ignored
            {"cc"s, 3, {"20100115T180000"_pt, "20100131T180000"_pt}},
            {"cc"s, 3, {"20100120T180000"_pt, "20100215T180000"_pt}},

            // dd-6/7 over {30-35}
            {"dd"s, 5, {"20100201T180000"_pt, "20100301T180000"_pt}},
            {"dd"s,
                    6,
                       boost::icl::left_subtract(
                           Interval{"20100228T180000"_pt, "20100315T180000"_pt},
                           Interval{"20100201T180000"_pt, "20100301T180000"_pt})},
            {"dd"s,
                    7,
                       boost::icl::left_subtract(
                           Interval{"20100228T180000"_pt, "20100315T180000"_pt},
                           Interval{"20100201T180000"_pt, "20100301T180000"_pt})},
            {"dd"s, 5, {"20100415T180000"_pt, "20100715T180000"_pt}},
        };
    }

    std::vector<std::tuple<std::string, Val, Interval>> intervals() const
    {
        using namespace interval_dict::ptime_literals;
        const auto adjust = Adjust<Interval>{};
        return {
            {"aa", 0, {"20100101T180000"_pt, "20100115T180000"_pt}},
            {"aa", 0, {"20100201T180000"_pt, "20100215T180000"_pt}},
            {"bb", 1, {"20100105T180000"_pt, "20100331T180000"_pt}},
            {"bb", 1, {"20100501T180000"_pt, "20100531T180000"_pt}},
            {"bb", 2, {"20100501T180000"_pt, "20100615T180000"_pt}},
            {"cc", 3, {"20100115T180000"_pt, "20100215T180000"_pt}},
            {"dd", 5, {"20100201T180000"_pt, "20100301T180000"_pt}},
            {"dd", 5, {"20100415T180000"_pt, "20100715T180000"_pt}},
            {"dd",
                   6,
                      adjust.lower({"20100301T180000"_pt, "20100315T180000"_pt})},
            {"dd",
                   7,
                      adjust.lower({"20100301T180000"_pt, "20100315T180000"_pt})},
        };
    }
    Interval query_interval() const
    {
        using namespace interval_dict::ptime_literals;
        return {"20100215T180000"_pt, "20100515T180000"_pt};
    }
};


#endif //TESTS_TEST_DATA_H
