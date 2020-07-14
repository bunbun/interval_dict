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
/// \file test_icl_flatten.cpp
/// \brief Test IntervalDict::flattened()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

/*
 * TODO: Test Time
 */
#include "catch.hpp"
#include "test_icl.h"
#include <interval_dict/intervaldicticl.h>
#include <vector>

/*
 * TODO: Test Date and Time
 */
TEMPLATE_TEST_CASE("Test flattening for different interval types",
                   "[flattened]",
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

    using namespace boost::gregorian;

    GIVEN("An IntervalDict with overlapping intervals")
    {
        auto test_dict = test_data_date<Val, Interval, Impl>();

        WHEN("Should be equivalent to inserting resolved data")
        {
            ImportData expected_data{
                {"aa", 0, Interval{{2010, 1, 1}, {2010, 1, 15}}},
                {"aa", 0, Interval{{2010, 2, 1}, {2010, 2, 15}}},
                {"bb", 1, Interval{{2010, 1, 5}, {2010, 3, 31}}},
                {"bb", 1, Interval{{2010, 5, 1}, {2010, 5, 31}}},
                {"bb", 2, Interval{{2010, 5, 1}, {2010, 5, 31}}},
                {"bb",
                 2,
                 Adjust{}.lower(Interval{{2010, 5, 31}, {2010, 6, 15}})},
                {"cc", 3, Interval{{2010, 1, 15}, {2010, 2, 15}}},
                {"dd", 5, Interval{{2010, 2, 1}, {2010, 3, 1}}},
                {"dd",
                 6,
                 Adjust{}.lower(Interval{{2010, 3, 1}, {2010, 3, 15}})},
                {"dd",
                 7,
                 Adjust{}.lower(Interval{{2010, 3, 1}, {2010, 3, 15}})},
                {"dd", 5, Interval{{2010, 4, 15}, {2010, 7, 15}}},
            };
            REQUIRE(test_dict == IDict(expected_data));
            REQUIRE(test_dict == IDict().insert(expected_data));
        }
        test_dict = test_dict.fill_gaps();

        WHEN("The IntervalDict is flattened by preferring the status quo")
        {
            THEN("Many cases of 1:m can be resolved.")
            {
                // Resolved cases
                REQUIRE(
                    test_dict - flattened(test_dict) ==
                    IDict(ImportData{
                        // keep status_quo
                        {"bb", 2, Interval{{2010, 5, 1}, {2010, 5, 31}}},
                        // no status_quo: discard
                        {"dd",
                         6,
                         Adjust{}.lower(Interval{{2010, 3, 1}, {2010, 3, 15}})},
                        {"dd",
                         7,
                         Adjust{}.lower(Interval{{2010, 3, 1}, {2010, 3, 15}})},
                    }));
            }
        }

        WHEN("The IntervalDict is flattened by deletions")
        {
            const auto flattened_preferring_status_quo = flattened(
                test_dict, interval_dict::flatten_policy_prefer_status_quo());
            const auto flattened_discard =
                flattened(test_dict, interval_dict::flatten_policy_discard());
            THEN("Fewer cases of 1:m can be resolved.")
            {
                // everything is discarded
                REQUIRE(flattened_preferring_status_quo - flattened_discard ==
                        IDict(ImportData{
                            {"bb", 1, Interval{{2010, 5, 1}, {2010, 5, 31}}},
                        }));
            }
        }

        auto custom_policy =
            [](const std::optional<Val>& status_quo,
               Interval,
               const Key& key,
               const std::vector<Val>& values) -> const std::optional<Val> {
            if (status_quo.has_value() &&
                std::find(values.begin(), values.end(), status_quo.value()) !=
                    values.end())
            {
                return status_quo.value() + 10;
            }
            else if (key == "dd")
            {
                return 42;
            }
            else
            {
                return {};
            }
        };
        WHEN("The IntervalDict is flattened by a custom function")
        {
            using namespace interval_dict;
            const auto flattened_custom = flattened(test_dict, custom_policy);
            const auto flattened_discard =
                flattened(test_dict, interval_dict::flatten_policy_discard());
            THEN("More cases of 1:m can be resolved in curious ways.")
            {
                REQUIRE(
                    flattened_custom - flattened_discard ==
                    IDict(ImportData{
                        {"bb", 11, Interval{{2010, 5, 01}, {2010, 5, 31}}},
                        {"dd",
                         42,
                         Adjust{}.lower(Interval{{2010, 3, 1}, {2010, 3, 15}})},
                    }));
            }
        }
        WHEN("The IntervalDict is flattened by prefering status quo then a "
             "custom function")
        {
            using namespace interval_dict;
            const auto flattened_custom = flattened(
                test_dict,
                interval_dict::flatten_policy_prefer_status_quo(custom_policy));
            const auto flattened_discard =
                flattened(test_dict, interval_dict::flatten_policy_discard());
            THEN("More not 'status quo' cases can still be resolved.")
            {
                REQUIRE(
                    flattened_custom - flattened_discard ==
                    IDict(ImportData{
                        // Note this is resolved by keeping the status quo
                        // so the value is 1 not 11
                        {"bb", 1, Interval{{2010, 5, 01}, {2010, 5, 31}}},
                        // this is resolved via our custom function
                        {"dd",
                         42,
                         Adjust{}.lower(Interval{{2010, 3, 1}, {2010, 3, 15}})},
                    }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test flattening for different interval types",
                   "[flattened]",
                   boost::icl::closed_interval<int>,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::interval<int>::type,
                   boost::icl::open_interval<int>,
                   boost::icl::interval<float>::type,
                   boost::icl::left_open_interval<float>,
                   boost::icl::right_open_interval<float>)
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

    GIVEN("An IntervalDict with overlapping intervals")
    {
        auto test_dict = test_data1<Val, Interval, Impl>();

        WHEN("Should be equivalent to inserting resolved data")
        {
            ImportData expected_data{
                {"aa", 0, Interval{0, 5}},
                {"aa", 0, Interval{40, 85}},
                {"bb", 1, Interval{5, 15}},
                {"bb", 1, Interval{20, 25}},
                {"bb", 2, Interval{20, 25}},
                {"bb", 2, Adjust{}.lower(Interval{25, 30})},
                {"cc", 3, Interval{15, 55}},
                {"dd", 5, Interval{20, 30}},
                {"dd", 6, Adjust{}.lower(Interval{30, 35})},
                {"dd", 7, Adjust{}.lower(Interval{30, 35})},
                {"dd", 5, Interval{45, 75}},
            };
            REQUIRE(test_dict == IDict(expected_data));
            REQUIRE(test_dict == IDict().insert(expected_data));
        }
        test_dict = test_dict.fill_gaps();

        WHEN("The IntervalDict is flattened by preferring the status quo")
        {
            THEN("Many cases of 1:m can be resolved.")
            {
                // Resolved cases
                REQUIRE(test_dict - flattened(test_dict) ==
                        IDict(ImportData{
                            // keep status_quo
                            {"bb", 2, Interval{20, 25}},
                            // no status_quo: discard
                            {"dd", 6, Adjust{}.lower(Interval{30, 35})},
                            {"dd", 7, Adjust{}.lower(Interval{30, 35})},
                        }));
            }
        }

        WHEN("The IntervalDict is flattened by deletions")
        {
            const auto flattened_preferring_status_quo = flattened(test_dict);
            const auto flattened_discard =
                flattened(test_dict, interval_dict::flatten_policy_discard());
            THEN("Fewer cases of 1:m can be resolved.")
            {
                // everything is discarded
                REQUIRE(flattened_preferring_status_quo - flattened_discard ==
                        IDict(ImportData{
                            {"bb", 1, Interval{20, 25}},
                        }));
            }
        }

        auto custom_policy =
            [](const std::optional<Val>& status_quo,
               Interval,
               const Key& key,
               const std::vector<Val>& values) -> const std::optional<Val> {
            if (status_quo.has_value() &&
                std::find(values.begin(), values.end(), status_quo.value()) !=
                    values.end())
            {
                return status_quo.value() + 10;
            }
            else if (key == "dd")
            {
                return 42;
            }
            else
            {
                return {};
            }
        };

        WHEN("The IntervalDict is flattened by a custom function")
        {
            using namespace interval_dict;
            const auto flattened_custom = flattened(test_dict, custom_policy);
            const auto flattened_discard =
                flattened(test_dict, interval_dict::flatten_policy_discard());
            THEN("More cases of 1:m can be resolved in curious ways.")
            {
                REQUIRE(flattened_custom - flattened_discard ==
                        IDict(ImportData{
                            {"bb", 11, Interval{20, 25}},
                            {"dd", 42, Adjust{}.lower(Interval{30, 35})},
                        }));
            }
        }

        WHEN("The IntervalDict is flattened by prefering status quo then a "
             "custom function")
        {
            using namespace interval_dict;
            const auto flattened_custom = flattened(
                test_dict,
                interval_dict::flatten_policy_prefer_status_quo(custom_policy));
            const auto flattened_discard =
                flattened(test_dict, interval_dict::flatten_policy_discard());
            THEN("More not 'status quo' cases can still be resolved.")
            {
                REQUIRE(flattened_custom - flattened_discard ==
                        IDict(ImportData{
                            // Note this is resolved by keeping the status quo
                            // so the value is 1 not 11
                            {"bb", 1, Interval{20, 25}},
                            // this is resolved via our custom function
                            {"dd", 42, Adjust{}.lower(Interval{30, 35})},
                        }));
            }
        }
    }
}
