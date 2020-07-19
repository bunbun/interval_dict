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
/// \file test_data2.h
/// \brief Provide second test data set. From int to floats
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef TESTS_TESTS_DATA2_H
#define TESTS_TESTS_DATA2_H

#include <interval_dict/gregorian.h>
#include <interval_dict/ptime.h>

#include <interval_dict/intervaldicticl.h>

#include "test_utils.h"

#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

/*
 * Normalised data for int->float
 */
template <typename Interval, typename enable = void> struct TestData2
{
    //     0... 05.. 15.. 20.. 25.. 30.. 35.. 40.. 45.. 50.. 55.. 75.. 85..
    //     0101 0115 0201 0215 0301 0315 0401 0415 0501 0515 0601 0615 0701
    // 0   3.0-----------------3.0            |    4.0-----------------4.0
    // 1        1.0--1.0  2.0------------2.0  |    |
    // 2        5.0-----------------5.0       6.0--|-------------------6.0
    // 3             4.0----------------------4.0  9.0-------9.0
    // 4                       1.0------------|----|--------------1.0
    // 5   7.0---------------------------7.0  2.0--|---------2.0
    // 6   6.0--------------------------------6.0  3.0-----------------3.0
    // 7   8.0--------------------------------8.0  6.0-----------------6.0
    // 8                            9.0--9.0
    // Query     q--------------------------------------q
    std::vector<std::tuple<int, double, Interval>> initial()
    {
        using namespace interval_dict::date_literals;
        return {
            {0, 3.1, {0, 25}},
            {0, 4.1, {45, 85}},
            {1, 1.1, {5, 15}},
            {1, 2.1, {20, 35}},
            {2, 5.1, {5, 30}},
            {2, 6.1, {40, 85}},
            {3, 4.1, {15, 40}},
            {3, 9.1, {45, 55}},
            {4, 1.1, {25, 60}},
            {5, 7.1, {0, 35}},
            {5, 2.1, {40, 55}},
            {6, 6.1, {0, 40}},
            {6, 3.1, {45, 85}},
            {7, 8.1, {0, 35}},
            {7, 6.1, {45, 85}},
            {8, 9.1, {30, 35}},
        };
    }
};

/*
 * Normalised data for date
 */
template <typename Interval>
struct TestData2<
    Interval,
    typename std::enable_if<std::is_same<typename Interval::domain_type,
                                         boost::gregorian::date>::value>::type>
{
    std::vector<std::tuple<int, double, Interval>> initial()
    {
        using namespace interval_dict::date_literals;
        return {
            {0, 3.1, {20100101_dt, 20100301_dt}},
            {0, 4.1, {20100501_dt, 20100701_dt}},
            {1, 1.1, {20100115_dt, 20100201_dt}},
            {1, 2.1, {20100215_dt, 20100401_dt}},
            {2, 5.1, {20100115_dt, 20100315_dt}},
            {2, 6.1, {20100415_dt, 20100701_dt}},
            {3, 4.1, {20100201_dt, 20100415_dt}},
            {3, 9.1, {20100501_dt, 20100601_dt}},
            {4, 1.1, {20100301_dt, 20100615_dt}},
            {5, 7.1, {20100101_dt, 20100401_dt}},
            {5, 2.1, {20100415_dt, 20100601_dt}},
            {6, 6.1, {20100101_dt, 20100415_dt}},
            {6, 3.1, {20100501_dt, 20100701_dt}},
            {7, 8.1, {20100101_dt, 20100401_dt}},
            {7, 6.1, {20100501_dt, 20100701_dt}},
            {8, 9.1, {20100315_dt, 20100401_dt}},
        };
    }
};

/*
 * Normalised data for posix time
 */
template <typename Interval>
struct TestData2<Interval,
                 typename std::enable_if<
                     std::is_same<typename Interval::domain_type,
                                  boost::posix_time::ptime>::value>::type>
{
    std::vector<std::tuple<int, double, Interval>> initial()
    {
        using namespace interval_dict::ptime_literals;
        return {
            {0, 3.1, {"20100101T180000"_pt, "20100301T180000"_pt}},
            {0, 4.1, {"20100501T180000"_pt, "20100701T180000"_pt}},
            {1, 1.1, {"20100115T180000"_pt, "20100201T180000"_pt}},
            {1, 2.1, {"20100215T180000"_pt, "20100401T180000"_pt}},
            {2, 5.1, {"20100115T180000"_pt, "20100315T180000"_pt}},
            {2, 6.1, {"20100415T180000"_pt, "20100701T180000"_pt}},
            {3, 4.1, {"20100201T180000"_pt, "20100415T180000"_pt}},
            {3, 9.1, {"20100501T180000"_pt, "20100601T180000"_pt}},
            {4, 1.1, {"20100301T180000"_pt, "20100615T180000"_pt}},
            {5, 7.1, {"20100101T180000"_pt, "20100401T180000"_pt}},
            {5, 2.1, {"20100415T180000"_pt, "20100601T180000"_pt}},
            {6, 6.1, {"20100101T180000"_pt, "20100415T180000"_pt}},
            {6, 3.1, {"20100501T180000"_pt, "20100701T180000"_pt}},
            {7, 8.1, {"20100101T180000"_pt, "20100401T180000"_pt}},
            {7, 6.1, {"20100501T180000"_pt, "20100701T180000"_pt}},
            {8, 9.1, {"20100315T180000"_pt, "20100401T180000"_pt}},
        };
    }
};

#endif // TESTS_TESTS_DATA2_H
