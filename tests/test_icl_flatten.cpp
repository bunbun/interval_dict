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

#include "catch.hpp"
#include "test_icl.h"
#include "test_data.h"
#include <interval_dict/intervaldicticl.h>
#include <vector>

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
    using namespace interval_dict::date_literals;
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    using Key = std::string;
    using Val = int;
    using IDict = interval_dict::IntervalDictICLExp<Key, Val, Interval>;
    using Interval = typename IDict::Interval;
    using Impl = typename IDict::ImplType;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;
    const auto adjust = Adjust<Interval>{};

    using namespace boost::gregorian;

    GIVEN("An IntervalDict with overlapping intervals")
    {
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());

        WHEN("Should be equivalent to inserting resolved data")
        {
            REQUIRE(test_dict == IDict(test_data.intervals()));
            REQUIRE(test_dict == IDict().insert(test_data.intervals()));
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
                            {"bb", 2, {20100501_dt, 20100531_dt}},
                            // no status_quo: discard
                            {"dd", 6, adjust.lower({20100301_dt, 20100315_dt})},
                            {"dd", 7, adjust.lower({20100301_dt, 20100315_dt})},
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
                            {"bb", 1, {20100501_dt, 20100531_dt}},
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
                        {"bb", 11, {20100501_dt, 20100531_dt}},
                        {"dd", 42, adjust.lower({20100301_dt, 20100315_dt})},
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
                        {"bb", 1, {20100501_dt, 20100531_dt}},
                        // this is resolved via our custom function
                        {"dd", 42, adjust.lower({20100301_dt, 20100315_dt})},
                    }));
            }
        }
    }
}

/*
 * posix_time
 */
TEMPLATE_TEST_CASE("Test flattening for different interval types",
                   "[flattened]",
                   boost::icl::interval<boost::posix_time::ptime>::type,
                   boost::icl::left_open_interval<boost::posix_time::ptime>,
                   boost::icl::right_open_interval<boost::posix_time::ptime>,
                   boost::icl::open_interval<boost::posix_time::ptime>,
                   boost::icl::closed_interval<boost::posix_time::ptime>,
                   boost::icl::discrete_interval<boost::posix_time::ptime>)
{
    using namespace std::string_literals;
    using namespace interval_dict::ptime_literals;
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    using Key = std::string;
    using Val = int;
    using IDict = interval_dict::IntervalDictICLExp<Key, Val, Interval>;
    using Interval = typename IDict::Interval;
    using Impl = typename IDict::ImplType;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;
    const auto adjust = Adjust<Interval>{};

    using namespace boost::gregorian;

    GIVEN("An IntervalDict with overlapping intervals")
    {
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());

        WHEN("Should be equivalent to inserting resolved data")
        {
            REQUIRE(test_dict == IDict(test_data.intervals()));
            REQUIRE(test_dict == IDict().insert(test_data.intervals()));
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
                        {"bb", 2, {"20100501T180000"_pt, "20100531T180000"_pt}},
                        // no status_quo: discard
                        {"dd",
                         6,
                         adjust.lower(
                             {"20100301T180000"_pt, "20100315T180000"_pt})},
                        {"dd",
                         7,
                         adjust.lower(
                             {"20100301T180000"_pt, "20100315T180000"_pt})},
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
                REQUIRE(
                    flattened_preferring_status_quo - flattened_discard ==
                    IDict(ImportData{
                        {"bb", 1, {"20100501T180000"_pt, "20100531T180000"_pt}},
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
                            {"bb",
                             11,
                             {"20100501T180000"_pt, "20100531T180000"_pt}},
                            {"dd",
                             42,
                             adjust.lower(
                                 {"20100301T180000"_pt, "20100315T180000"_pt})},
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
                        {"bb", 1, {"20100501T180000"_pt, "20100531T180000"_pt}},
                        // this is resolved via our custom function
                        {"dd",
                         42,
                         adjust.lower(
                             {"20100301T180000"_pt, "20100315T180000"_pt})},
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
    const auto adjust = Adjust<Interval>{};

    GIVEN("An IntervalDict with overlapping intervals")
    {
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());

        WHEN("Should be equivalent to inserting resolved data")
        {
            REQUIRE(test_dict == IDict(test_data.intervals()));
            REQUIRE(test_dict == IDict().insert(test_data.intervals()));
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
                            {"bb", 2, {20, 25}},
                            // no status_quo: discard
                            {"dd", 6, adjust.lower({30, 35})},
                            {"dd", 7, adjust.lower({30, 35})},
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
                            {"bb", 1, {20, 25}},
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
                            {"bb", 11, {20, 25}},
                            {"dd", 42, adjust.lower({30, 35})},
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
                            {"bb", 1, {20, 25}},
                            // this is resolved via our custom function
                            {"dd", 42, adjust.lower({30, 35})},
                        }));
            }
        }
    }
}
