//
// Created by lg on 10/07/2020.
//
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
/// \file test_icl_member_functions.cpp
/// \brief Test IntervalDict memeber functions
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

TEMPLATE_TEST_CASE("Test member functions different interval types",
                   "[keys][empty][count][count][contains][clear][invert][size]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::closed_interval<int>,
                   boost::icl::open_interval<int>,
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
    using namespace interval_dict::date_literals;
    using namespace boost::gregorian;
    using Interval = TestType;
    using Key = std::string;
    using Val = int;
    using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Val, Interval>;
    using Interval = typename IDict::IntervalType;
    TestData<Interval> test_data;
    auto import_data = test_data.intervals();

    /*
     * TestData
     */
    GIVEN("An IntervalDict divided into subsets by keys")
    {
        // keys
        const IDict test_dict(test_data.initial());
        const auto all_keys = test_dict.keys();
        WHEN("we look at the keys of each subset")
        {
            THEN("They need to have the keys they were subset() with")
            {
                for (const auto& key : all_keys)
                {
                    const auto subset_dict = test_dict.subset(std::vector{key});
                    // only has one key as keys()
                    REQUIRE(subset_dict.keys() == std::vector{key});

                    // check not empty but has size == 1
                    REQUIRE(!subset_dict.is_empty());
                    REQUIRE(subset_dict.size() == 1);

                    // but is empty after removing that key
                    REQUIRE(copy(subset_dict).erase(key).is_empty());

                    for (const auto& check_key : all_keys)
                    {
                        // count () == 1 and contains() == true only for the
                        // matching key
                        REQUIRE(subset_dict.count(check_key) ==
                                (key == check_key ? 1 : 0));
                        REQUIRE(subset_dict.contains(check_key) ==
                                (key == check_key));
                    }
                }
            }
        }

        WHEN("we look at a cleared dictionary")
        {
            auto cleared_dict = copy(test_dict);
            REQUIRE(!cleared_dict.is_empty());
            REQUIRE(cleared_dict.size() != 0);
            REQUIRE(cleared_dict.size() == all_keys.size());
            cleared_dict.clear();
            THEN("It should be empty")
            {
                REQUIRE(cleared_dict.is_empty());
                REQUIRE(cleared_dict.size() == 0);
                for (const auto& key : all_keys)
                {
                    REQUIRE(cleared_dict.contains(key) == false);
                }
            }
        }

        GIVEN("An inverted IntervalDict divided into subsets by inverted keys")
        {
            // keys
            const auto inverted_dict = test_dict.invert();
            const auto all_values = inverted_dict.keys();
            WHEN("The examining each subset")
            {
                THEN("They need to match the subset divided by values")
                {
                    for (const auto& value : all_values)
                    {
                        const auto value_subset_dict1 =
                            inverted_dict.subset(std::vector{value}).invert();
                        const auto value_subset_dict2 =
                            test_dict.subset(all_keys, std::vector{value});
                        REQUIRE(value_subset_dict1 == value_subset_dict2);
                    }
                }
            }
        }
    }
}