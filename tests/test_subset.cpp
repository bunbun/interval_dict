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
/// \file test_icl_subset.cpp
/// \brief Test IntervalDict::subset
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "test_data.h"
#include "test_utils.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/ptime.h>
#include <tuple>
#include <vector>

TEMPLATE_TEST_CASE("Test subsetting for different interval types",
                   "[subset]",
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
    using Interval = typename IDict::Interval;
    TestData<Interval> test_data;
    auto import_data = test_data.intervals();

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        using namespace boost::gregorian;
        const IDict test_dict(test_data.initial());
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto query = test_data.query_interval();
        const auto empty_query = test_data.empty_interval();

        WHEN("we compare subset() vs intervals() for all keys")
        {
            THEN("they should always give the same result.")
            {
                const auto externity = interval_dict::interval_extent<Interval>;
                REQUIRE(intervals_as_vec(test_dict, query) ==
                        intervals_as_vec(test_dict.subset(all_keys, query),
                                         externity));
            }
        }

        WHEN("we compare subset()s")
        {
            THEN("we can reassemble the original dict from subset by keys")
            {
                const auto keys_subset1 = std::vector{"cc"s, "dd"s};
                const auto subset_dict1 = test_dict.subset(keys_subset1);
                REQUIRE(subset_dict1.keys() == keys_subset1);

                const auto keys_subset2 = std::vector{"aa"s, "bb"s};
                const auto subset_dict2 = test_dict.subset(keys_subset2);
                REQUIRE(subset_dict2.keys() == keys_subset2);

                REQUIRE(subset_dict1 + subset_dict2 == test_dict);
            }

            THEN("we can reassemble the original dict from subset by values")
            {
                const auto values_subset1 = std::vector{0, 1, 2, 3};
                const auto subset_dict1 =
                    test_dict.subset(all_keys, values_subset1);
                REQUIRE(!subset_dict1.empty());

                const auto values_subset2 = std::vector{5, 6, 7, 8};
                const auto subset_dict2 =
                    test_dict.subset(all_keys, values_subset2);
                REQUIRE(!subset_dict2.empty());

                // check they are not the same but can be put together to
                // the original
                REQUIRE(subset_dict1 != subset_dict2);
                REQUIRE(subset_dict1 + subset_dict2 == test_dict);
            }
        }
        WHEN("we look at empty subset()s")
        {
            THEN("we should expect empty interval dicts")
            {
                // Empty keys
                REQUIRE(test_dict.subset(std::vector<Key>{}).empty());
                // Empty values
                REQUIRE(test_dict.subset(all_keys, std::vector<Val>{}).empty());
                // Empty interval
                REQUIRE(test_dict.subset(all_keys, empty_query) == IDict());
                REQUIRE(test_dict.subset(
                            all_keys,
                            std::vector<Val>{0, 1, 2, 3, 4, 5, 6, 7, 8},
                            empty_query) == IDict());
            }
        }
    }
}