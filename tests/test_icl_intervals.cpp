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
/// \file test_icl_intervals.cpp
/// \brief Test IntervalDict::intervals()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "print_tuple.h"
#include "test_data.h"
#include "test_icl.h"

#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/ptime.h>
#include <vector>

TEMPLATE_TEST_CASE("Test intervals for different interval types",
                   "[intervals]",
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
    using namespace std::string_literals;
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    using Key = std::string;
    using Val = int;
    using IDict = interval_dict::IntervalDictICLExp<Key, Val, Interval>;
    using Interval = typename IDict::Interval;
    using Impl = typename IDict::ImplType;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        TestData<Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust<Interval>{};
        const auto expected = test_data.intervals();
        const auto query = test_data.query_interval();

        WHEN("All data is retrieved via intervals()")
        {
            auto vec_intervals = interval_dict::intervals(test_dict, all_keys);
            ImportData intervals_data;
            for (const auto& [key, value, interval] : vec_intervals)
            {
                intervals_data.push_back({key, value, interval});
            }
            THEN("Expect")
            {
                REQUIRE(intervals_data == expected);
            }
        }

        WHEN("Only data within a query interval is retrieved via intervals()")
        {
            auto vec_intervals =
                interval_dict::intervals(test_dict, all_keys, query);
            ImportData intervals_data;
            for (const auto& [key, value, interval] : vec_intervals)
            {
                intervals_data.push_back({key, value, interval});
            }
            THEN("Expect to match only a subset")
            {
                ImportData expected_subset;
                for (const auto& [key, value, interval] : expected)
                {
                    if (boost::icl::intersects(interval, query))
                    {
                        expected_subset.push_back(
                            {key, value, interval & query});
                    }
                }
                REQUIRE(intervals_data == expected_subset);
            }
        }
        WHEN("Only data matching keys within a query interval is retrieved via "
             "intervals()")
        {
            auto vec_intervals = interval_dict::intervals(
                test_dict, std::vector{"bb"s, "dd"s}, query);
            ImportData intervals_data;
            for (const auto& [key, value, interval] : vec_intervals)
            {
                intervals_data.push_back({key, value, interval});
            }
            THEN("Expect to match only a subset")
            {
                ImportData expected_subset;
                auto keys = std::set{"bb"s, "dd"s};
                for (const auto& [key, value, interval] : expected)
                {
                    if (keys.count(key) &&
                        boost::icl::intersects(interval, query))
                    {
                        expected_subset.push_back(
                            {key, value, interval & query});
                    }
                }
                REQUIRE(intervals_data == expected_subset);
            }
        }
        WHEN("Only data matching a key within a query interval is retrieved "
             "via intervals()")
        {
            auto vec_intervals =
                interval_dict::intervals(test_dict, "dd"s, query);
            ImportData intervals_data;
            for (const auto& [key, value, interval] : vec_intervals)
            {
                intervals_data.push_back({key, value, interval});
            }
            THEN("Expect to match only a subset")
            {
                ImportData expected_subset;
                auto keys = std::set{"bb"s, "dd"s};
                for (const auto& [key, value, interval] : expected)
                {
                    if (key == "dd"s && boost::icl::intersects(interval, query))
                    {
                        expected_subset.push_back(
                            {key, value, interval & query});
                    }
                }
                REQUIRE(intervals_data == expected_subset);
            }
        }
    }
}
