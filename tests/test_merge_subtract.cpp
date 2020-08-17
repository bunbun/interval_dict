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
/// \file test_icl_merge_subtract.cpp
/// \brief Test IntervalDict::merge() and ::subtract()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "test_data.h"
#include "test_utils.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/ptime.h>
#include <vector>

TEMPLATE_TEST_CASE("Test merging for different interval types",
                   "[merge][subtract]",
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
    GIVEN("An IntervalDict divided into subsets")
    {
        const IDict test_dict(test_data.initial());
        const auto all_keys = test_dict.keys();
        std::map<Key, IDict> dict_by_keys;
        for (const auto& key : all_keys)
        {
            dict_by_keys[key] = test_dict.subset(std::vector{key});
        }

        WHEN("we add up the subsets")
        {
            IDict new_dict1;
            IDict new_dict2;
            for (const auto& key : all_keys)
            {
                // add twice for good measure
                new_dict1 += dict_by_keys[key];
                new_dict1 += dict_by_keys[key];
                new_dict2 = merge(new_dict2, dict_by_keys[key]);
                new_dict2 = merge(new_dict2, dict_by_keys[key]);
            }
            THEN("we get the original.")
            {
                REQUIRE(new_dict1 == test_dict);
                REQUIRE(new_dict2 == test_dict);
            }
        }

        WHEN("we remove all the subsets")
        {
            IDict new_dict1 = test_dict;
            IDict new_dict2 = test_dict;
            for (const auto& key : all_keys)
            {
                // subtract twice for good measure
                new_dict1 = subtract(new_dict1, dict_by_keys[key]);
                new_dict1 = subtract(new_dict1, dict_by_keys[key]);
                new_dict2 -= dict_by_keys[key];
                new_dict2 -= dict_by_keys[key];
            }
            THEN("we get an empty dictionary.")
            {
                REQUIRE(new_dict1 == IDict());
                REQUIRE(new_dict2 == IDict());
            }
        }

        WHEN("we compare subset()s")
        {
            THEN("we can reassemble the original dict from subset by keys")
            {
                REQUIRE(test_dict == test_dict.subset(std::vector{"aa"s}) +
                                         test_dict.subset(std::vector{"bb"s}) +
                                         test_dict.subset(std::vector{"cc"s}) +
                                         test_dict.subset(std::vector{"dd"s}));
                REQUIRE(test_dict - test_dict.subset(std::vector{"aa"s}) -
                            test_dict.subset(std::vector{"bb"s}) -
                            test_dict.subset(std::vector{"cc"s}) -
                            test_dict.subset(std::vector{"dd"s}) ==
                        IDict());
            }

            THEN("we can reassemble the original dict from subset by values")
            {
                const auto values_subset1 = std::vector{0, 1, 2, 3};
                const auto subset_dict1 =
                    test_dict.subset(all_keys, values_subset1);
                REQUIRE(!subset_dict1.is_empty());

                const auto values_subset2 = std::vector{5, 6, 7, 8};
                const auto subset_dict2 =
                    test_dict.subset(all_keys, values_subset2);
                REQUIRE(!subset_dict2.is_empty());

                // check they are not the same but can be put together to
                // the original
                REQUIRE(subset_dict1 != subset_dict2);
                REQUIRE(subset_dict1 + subset_dict2 == test_dict);
            }
        }
        WHEN("we add or remove empty dictionaries")
        {
            THEN("Nothing happens")
            {
                REQUIRE(copy(test_dict) - IDict() == test_dict);
                REQUIRE(copy(test_dict) + IDict() == test_dict);
                REQUIRE((copy(test_dict) -= IDict()) == test_dict);
                REQUIRE((copy(test_dict) += IDict()) == test_dict);
            }
        }
    }
}