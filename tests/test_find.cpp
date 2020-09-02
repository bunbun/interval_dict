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
/// \file test_icl_find.cpp
/// \brief Test IntervalDict::find()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "test_data.h"
#include "test_utils.h"

#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/intervaldictitree.h>
#include <interval_dict/ptime.h>
#include <vector>

TEMPLATE_TEST_CASE("Test find() for different interval types",
                   "[find]",
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
    using Key = std::string;
    using Val = int;
    using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Val, Interval>;
    using Interval = typename IDict::IntervalType;

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        TestData<Interval> test_data;
        const IDict test_dict(test_data.initial());
        // N.B. "ee" should be ignored
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s, "ee"s};
        const auto adjust = Adjust<Interval>{};
        const auto expected = test_data.intervals();
        const auto query = test_data.query_interval_for_find();
        const auto queries = test_data.query_intervals_for_find();
        const auto query_end =
            Interval{boost::icl::upper(query), boost::icl::upper(query)};
        const auto query_max =
            Interval{interval_dict::IntervalTraits<Interval>::max(),
                     interval_dict::IntervalTraits<Interval>::max()};

        WHEN("Calling find() the largest extent")
        {
            const auto max_ext = interval_dict::interval_extent<Interval>;
            THEN("Expect all corresponding values for those keys")
            {
                REQUIRE(test_dict.find(all_keys, max_ext) ==
                        std::vector{0, 1, 2, 3, 5, 6, 7});
                REQUIRE(test_dict.find({"bb"s, "dd"s}, max_ext) ==
                        std::vector{1, 2, 5, 6, 7});
                REQUIRE(test_dict.find("bb"s, max_ext) == std::vector{1, 2});
                REQUIRE(test_dict.find("bb"s,
                                       boost::icl::lower(max_ext),
                                       boost::icl::upper(max_ext)) ==
                        std::vector{1, 2});
                REQUIRE(test_dict.find("not a key"s,
                                       boost::icl::lower(max_ext),
                                       boost::icl::upper(max_ext)) ==
                        std::vector<Val>());
            }
        }

        WHEN("Calling find() with a query extent")
        {
            THEN("Expect all corresponding values for those keys only over "
                 "that interval")
            {
                REQUIRE(test_dict.find(all_keys, adjust.both(query)) ==
                        std::vector{1, 2, 3, 5});
                REQUIRE(test_dict.find({"bb"s, "dd"s}, query) ==
                        std::vector{1, 2, 5});
                REQUIRE(test_dict.find("bb"s, query) == std::vector{1, 2});
                // nothing for this key
                REQUIRE(test_dict.find("aa"s, adjust.both(query)) ==
                        std::vector<Val>());
                // not a key
                REQUIRE(test_dict.find("not a key"s, query) ==
                        std::vector<Val>());
            }
        }

        WHEN("Calling find() with a single points")
        {
            THEN("Expect all corresponding values for those keys")
            {
                REQUIRE(test_dict.find("bb"s,
                                       boost::icl::lower(query),
                                       boost::icl::upper(query)) ==
                        std::vector{1, 2});
                // Inclusive single point
                if (!boost::icl::is_empty(query_end))
                {
                    REQUIRE(test_dict.find(all_keys, query_end) ==
                            std::vector{1, 2, 3, 5});
                    REQUIRE(
                        test_dict.find("bb"s, boost::icl::lower(query_end)) ==
                        std::vector{1, 2});
                }
            }
        }

        WHEN("Calling find() with a multiple intervals")
        {
            THEN("Expect all corresponding values for those keys")
            {
                REQUIRE(test_dict.find("aa"s, queries) == std::vector{0});
                REQUIRE(test_dict.find("bb"s, queries) == std::vector{1});
                REQUIRE(test_dict.find("dd"s, queries) == std::vector{6, 7});
                // not a key
                REQUIRE(test_dict.find("not a key"s, queries) ==
                        std::vector<Val>());
                // Empty intervals
                REQUIRE(test_dict.find("dd"s,
                                       interval_dict::Intervals<Interval>()) ==
                        std::vector<Val>{});
                REQUIRE(test_dict.find("not a key"s,
                                       interval_dict::Intervals<Interval>()) ==
                        std::vector<Val>{});
            }
        }

        WHEN("Calling find() with an empty query extent")
        {
            THEN("Expect all empty results")
            {
                // Empty query interval:
                if (boost::icl::is_empty(query_end))
                {
                    REQUIRE(test_dict.find(all_keys, query_end) ==
                            std::vector<Val>{});
                }
                REQUIRE(test_dict.find(all_keys, query_max) ==
                        std::vector<Val>{});
                REQUIRE(test_dict.find("bb"s, query_max) == std::vector<Val>{});

                // Empty query interval: Empty result
                REQUIRE(test_dict.find("bb"s,
                                       boost::icl::upper(query_max),
                                       boost::icl::upper(query_max)) ==
                        std::vector<Val>{});
                REQUIRE(test_dict.find("bb"s, boost::icl::upper(query_max)) ==
                        std::vector<Val>{});
            }
        }
    }
}
