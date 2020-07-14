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
/// \file test_icl_fill.cpp
/// \brief Test IntervalDict::fill_xxxx() and extend_into_gaps()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "test_icl.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <vector>

/*
 * TODO: Test Time
 */
TEMPLATE_TEST_CASE("Test gap filling to start for different interval types",
                   "[fill_to_start]",
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
        using namespace boost::gregorian;
        const auto test_dict = test_data_date<Val, Interval, Impl>();
        const auto adjust = Adjust{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();

        WHEN("The IntervalDict is filled to start")
        {
            THEN("Some keys are extended to the start of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100110
                REQUIRE(copy(test_dict).fill_to_start({2010, 1, 10}) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa"s,
                             0,
                             adjust.upper(Interval{date_MIN, {2010, 1, 1}})},
                            {"bb"s,
                             1,
                             adjust.upper(Interval{date_MIN, {2010, 1, 5}})},
                        }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_to_start({2010, 1, 10}) ==
                    copy(test_dict).fill_to_start(
                        {2010, 1, 10},
                        interval_dict::IntervalTraits<Interval>::max_size()));

                // Only extending by 30
                REQUIRE(
                    copy(test_dict).fill_to_start({2010, 1, 10}, days(30)) -
                        test_dict ==
                    IDict(ImportData{
                        {"aa",
                         0,
                         adjust.upper(Interval{{2009, 12, 2}, {2010, 1, 1}})},
                        {"bb",
                         1,
                         adjust.upper(Interval{{2009, 12, 6}, {2010, 1, 5}})},
                    }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test filling gaps to end for different interval types",
                   "[fill_to_end]",
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
        using namespace boost::gregorian;
        const auto test_dict = test_data_date<Val, Interval, Impl>();
        const auto adjust = Adjust{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100415
                REQUIRE(copy(test_dict).fill_to_end({2010, 4, 15}) -
                            test_dict ==
                        IDict(ImportData{
                            {"bb"s,
                             2,
                             adjust.lower(Interval{{2010, 6, 15}, date_MAX})},
                            {"dd"s,
                             5,
                             adjust.lower(Interval{{2010, 7, 15}, date_MAX})},
                        }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_to_end(
                        {2010, 04, 15},
                        interval_dict::IntervalTraits<Interval>::max_size()) -
                        copy(test_dict).fill_to_end({2010, 4, 15}) ==
                    IDict());
                // Only extending by 30
                REQUIRE(
                    copy(test_dict).fill_to_end({2010, 4, 15}, days(30)) -
                        test_dict ==
                    IDict(ImportData{
                        {"bb"s,
                         2,
                         adjust.lower(Interval{{2010, 6, 15}, {2010, 7, 15}})},
                        {"dd"s,
                         5,
                         adjust.lower(Interval{{2010, 7, 15}, {2010, 8, 14}})},
                    }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test gap filling for different interval types",
                   "[fill_gaps]",
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
        using namespace boost::gregorian;
        const auto test_dict = test_data_date<Val, Interval, Impl>();
        const auto adjust = Adjust{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        WHEN("The IntervalDict gaps are filled safely")
        {
            THEN("All gaps of interrupted associations are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE(
                    copy(test_dict).fill_gaps() - test_dict ==
                    IDict(ImportData{
                        {"aa"s,
                         0,
                         adjust.both(Interval{{2010, 1, 15}, {2010, 2, 1}})},
                        {"bb"s,
                         1,
                         adjust.both(Interval{{2010, 3, 31}, {2010, 5, 1}})},
                    }));

                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_gaps(
                        interval_dict::IntervalTraits<Interval>::max_size()) -
                        copy(test_dict).fill_gaps() ==
                    IDict());
                // Only extending by 3
                REQUIRE(
                    (copy(test_dict).fill_gaps(days(3)) - test_dict) ==
                    IDict(ImportData{
                        {"aa"s,
                         0,
                         adjust.lower(Interval{{2010, 1, 15}, {2010, 1, 18}})},
                        {"aa"s,
                         0,
                         adjust.upper(Interval{{2010, 1, 29}, {2010, 2, 1}})},
                        {"bb"s,
                         1,
                         adjust.lower(Interval{{2010, 3, 31}, {2010, 4, 03}})},
                        {"bb"s,
                         1,
                         adjust.upper(Interval{{2010, 4, 28}, {2010, 5, 1}})},
                    }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test extension into gaps for different interval types",
                   "[extend_into_gaps]",
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
        using namespace boost::gregorian;
        const auto test_dict = test_data_date<Val, Interval, Impl>();
        const auto adjust = Adjust{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        WHEN("The IntervalDict are extended willy-nilly into the gaps")
        {
            const auto filled = copy(test_dict).fill_gaps();
            THEN("All gaps are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE(
                    (copy(filled).extend_into_gaps() - filled) ==
                    IDict(ImportData{
                        {"dd",
                         5,
                         adjust.both(Interval{{2010, 3, 15}, {2010, 4, 15}})},
                        {"dd",
                         6,
                         adjust.both(Interval{{2010, 3, 15}, {2010, 4, 15}})},
                        {"dd",
                         7,
                         adjust.both(Interval{{2010, 3, 15}, {2010, 4, 15}})},
                    }));

                // Maximum extension is the default
                REQUIRE(
                    copy(filled).extend_into_gaps(
                        interval_dict::GapExtensionDirection::Both,
                        interval_dict::IntervalTraits<Interval>::max_size()) ==
                    copy(filled).extend_into_gaps(
                        interval_dict::GapExtensionDirection::Both));
                // Only extending by 3 in both directions
                REQUIRE(
                    (copy(filled).extend_into_gaps(
                         interval_dict::GapExtensionDirection::Both, days(3)) -
                     filled) ==
                    IDict(ImportData{
                        {"dd",
                         5,
                         adjust.upper(Interval{{2010, 4, 12}, {2010, 4, 15}})},
                        {"dd",
                         6,
                         adjust.lower(Interval{{2010, 3, 15}, {2010, 3, 18}})},
                        {"dd",
                         7,
                         adjust.lower(Interval{{2010, 3, 15}, {2010, 3, 18}})},
                    }));
                // Only extending by 3 forwards
                REQUIRE(
                    (copy(filled).extend_into_gaps(
                         interval_dict::GapExtensionDirection::Forwards,
                         days(3)) -
                     filled) ==
                    IDict(ImportData{
                        {"dd",
                         6,
                         adjust.lower(Interval{{2010, 3, 15}, {2010, 3, 18}})},
                        {"dd",
                         7,
                         adjust.lower(Interval{{2010, 3, 15}, {2010, 3, 18}})},
                    }));
                // Only extending by 3 backwards
                REQUIRE(
                    (copy(filled).extend_into_gaps(
                         interval_dict::GapExtensionDirection::Backwards,
                         days(3)) -
                     filled) ==
                    IDict(ImportData{
                        {"dd",
                         5,
                         adjust.upper(Interval{{2010, 4, 12}, {2010, 4, 15}})},
                    }));
            }
        }
    }
}

// N.B. Continuous types like float have
// no sensible semantics for adjacent intervals
// when open/closed coordinates used so can't be used
// in interval maps
// boost::icl::closed_interval<float>
// boost::icl::open_interval<float>
TEMPLATE_TEST_CASE("Test gap filling to start for different interval types",
                   "[fill_to_start]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        const auto test_dict = test_data1<Val, Interval, Impl>();
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust{};

        WHEN("The IntervalDict is filled to start")
        {
            THEN("Some keys are extended to the start of time.")
            {
                const auto MIN = std::numeric_limits<BaseType>::lowest();
                // Extending only for keys whose first interval is AFTER 10
                REQUIRE(copy(test_dict).fill_to_start(10) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.upper(Interval{MIN, 0})},
                            {"bb", 1, adjust.upper(Interval{MIN, 5})},
                        }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_to_start(10) ==
                    copy(test_dict).fill_to_start(
                        10,
                        interval_dict::IntervalTraits<Interval>::max_size()));

                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_start(10, 30) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.upper(Interval{-30, 0})},
                            {"bb", 1, adjust.upper(Interval{-25, 5})},
                        }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test filling gaps to end for different interval types",
                   "[fill_to_end]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        const auto test_dict = test_data1<Val, Interval, Impl>();
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust{};

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                const auto MAX = std::numeric_limits<BaseType>::max();
                // Extending only for keys whose last interval is BEFORE 50
                REQUIRE(copy(test_dict).fill_to_end(50) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.lower(Interval{85, MAX})},
                            {"cc", 3, adjust.lower(Interval{55, MAX})},
                            {"dd", 5, adjust.lower(Interval{75, MAX})},
                        }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_to_end(
                        50,
                        interval_dict::IntervalTraits<Interval>::max_size()) -
                        copy(test_dict).fill_to_end(50) ==
                    IDict());
                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_end(50, 30) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.lower(Interval{85, 115})},
                            {"cc", 3, adjust.lower(Interval{55, 85})},
                            {"dd", 5, adjust.lower(Interval{75, 105})},
                        }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test gap filling for different interval types",
                   "[fill_gaps]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        const auto test_dict = test_data1<Val, Interval, Impl>();
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust{};

        WHEN("The IntervalDict gaps are filled safely")
        {
            THEN("All gaps of interrupted associations are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE((copy(test_dict).fill_gaps() - test_dict) ==
                        IDict(ImportData{
                            {"aa", 0, adjust.both(Interval{5, 40})},
                            {"bb", 1, adjust.both(Interval{15, 20})},
                        }));

                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_gaps(
                        interval_dict::IntervalTraits<Interval>::max_size()) -
                        copy(test_dict).fill_gaps() ==
                    IDict());
                // Only extending by 3
                REQUIRE((copy(test_dict).fill_gaps(3) - test_dict) ==
                        IDict(ImportData{
                            {"aa", 0, adjust.lower(Interval{5, 8})},
                            {"aa", 0, adjust.upper(Interval{37, 40})},
                            {"bb", 1, adjust.both(Interval{15, 20})},
                        }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test extension into gaps for different interval types",
                   "[extend_into_gaps]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        const auto test_dict = test_data1<Val, Interval, Impl>();
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust{};

        WHEN("The IntervalDict are extended willy-nilly into the gaps")
        {
            const auto filled = copy(test_dict).fill_gaps();
            THEN("All gaps are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE((copy(filled).extend_into_gaps() - filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.both(Interval{35, 45})},
                            {"dd", 6, adjust.both(Interval{35, 45})},
                            {"dd", 7, adjust.both(Interval{35, 45})},
                        }));

                // Maximum extension is the default
                REQUIRE(
                    copy(filled).extend_into_gaps(
                        interval_dict::GapExtensionDirection::Both,
                        interval_dict::IntervalTraits<Interval>::max_size()) ==
                    copy(filled).extend_into_gaps(
                        interval_dict::GapExtensionDirection::Both));
                // Only extending by 3 in both directions
                REQUIRE((copy(filled).extend_into_gaps(
                             interval_dict::GapExtensionDirection::Both, 3) -
                         filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.upper(Interval{42, 45})},
                            {"dd", 6, adjust.lower(Interval{35, 38})},
                            {"dd", 7, adjust.lower(Interval{35, 38})},
                        }));
                // Only extending by 3 forwards
                REQUIRE(
                    (copy(filled).extend_into_gaps(
                         interval_dict::GapExtensionDirection::Forwards, 3) -
                     filled) == IDict(ImportData{
                                    {"dd", 6, adjust.lower(Interval{35, 38})},
                                    {"dd", 7, adjust.lower(Interval{35, 38})},
                                }));
                // Only extending by 3 backwards
                REQUIRE(
                    (copy(filled).extend_into_gaps(
                         interval_dict::GapExtensionDirection::Backwards, 3) -
                     filled) == IDict(ImportData{
                                    {"dd", 5, adjust.upper(Interval{42, 45})},
                                }));
            }
        }
    }
}
