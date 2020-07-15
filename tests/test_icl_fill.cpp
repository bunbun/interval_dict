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
#include "test_data.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/ptime.h>
#include <vector>

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
    using namespace interval_dict::date_literals;
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
        TestData<Val, Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto date_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();

        WHEN("The IntervalDict is filled to start")
        {
            THEN("Some keys are extended to the start of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100110
                REQUIRE(copy(test_dict).fill_to_start(20100110_dt) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa"s, 0, adjust.upper({date_MIN, 20100101_dt})},
                            {"bb"s, 1, adjust.upper({date_MIN, 20100105_dt})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_start(20100110_dt) ==
                        copy(test_dict).fill_to_start(20100110_dt, date_MAXSZ));

                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_start(20100110_dt, days(30)) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.upper({20091202_dt, 20100101_dt})},
                            {"bb", 1, adjust.upper({20091206_dt, 20100105_dt})},
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
    using namespace interval_dict::date_literals;
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
        TestData<Val, Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto date_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100415
                REQUIRE(copy(test_dict).fill_to_end(20100415_dt) - test_dict ==
                        IDict(ImportData{
                            {"bb"s, 2, adjust.lower({20100615_dt, date_MAX})},
                            {"dd"s, 5, adjust.lower({20100715_dt, date_MAX})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_end(20100415_dt, date_MAXSZ) -
                            copy(test_dict).fill_to_end(20100415_dt) ==
                        IDict());
                // Only extending by 30
                REQUIRE(
                    copy(test_dict).fill_to_end(20100415_dt, days(30)) -
                        test_dict ==
                    IDict(ImportData{
                        {"bb"s, 2, adjust.lower({20100615_dt, 20100715_dt})},
                        {"dd"s, 5, adjust.lower({20100715_dt, 20100814_dt})},
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
    using namespace interval_dict::date_literals;
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
        TestData<Val, Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto date_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();
        WHEN("The IntervalDict gaps are filled safely")
        {
            THEN("All gaps of interrupted associations are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE(copy(test_dict).fill_gaps() - test_dict ==
                        IDict(ImportData{
                            {"aa"s, 0, adjust.both({20100115_dt, 20100201_dt})},
                            {"bb"s, 1, adjust.both({20100331_dt, 20100501_dt})},
                        }));

                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_gaps(date_MAXSZ) -
                            copy(test_dict).fill_gaps() ==
                        IDict());
                // Only extending by 3
                REQUIRE(
                    (copy(test_dict).fill_gaps(days(3)) - test_dict) ==
                    IDict(ImportData{
                        {"aa"s, 0, adjust.lower({20100115_dt, 20100118_dt})},
                        {"aa"s, 0, adjust.upper({20100129_dt, 20100201_dt})},
                        {"bb"s, 1, adjust.lower({20100331_dt, 20100403_dt})},
                        {"bb"s, 1, adjust.upper({20100428_dt, 20100501_dt})},
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
    using namespace interval_dict::date_literals;
    using namespace interval_dict;
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    using Key = std::string;
    using Val = int;
    using IDict = IntervalDictICLExp<Key, Val, Interval>;
    using Interval = typename IDict::Interval;
    using Impl = typename IDict::ImplType;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        using namespace boost::gregorian;
        TestData<Val, Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = IntervalTraits<Interval>::lowest();
        const auto date_MAX = IntervalTraits<Interval>::max();
        const auto date_MAXSZ = IntervalTraits<Interval>::max_size();
        WHEN("The IntervalDict are extended willy-nilly into the gaps")
        {
            const auto filled = copy(test_dict).fill_gaps();
            THEN("All gaps are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE((copy(filled).extend_into_gaps() - filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.both({20100315_dt, 20100415_dt})},
                            {"dd", 6, adjust.both({20100315_dt, 20100415_dt})},
                            {"dd", 7, adjust.both({20100315_dt, 20100415_dt})},
                        }));

                // Maximum extension is the default
                REQUIRE(
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both,
                                                  date_MAXSZ) ==
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both));
                // Only extending by 3 in both directions
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Both, days(3)) -
                         filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.upper({20100412_dt, 20100415_dt})},
                            {"dd", 6, adjust.lower({20100315_dt, 20100318_dt})},
                            {"dd", 7, adjust.lower({20100315_dt, 20100318_dt})},
                        }));
                // Only extending by 3 forwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Forwards, days(3)) -
                         filled) ==
                        IDict(ImportData{
                            {"dd", 6, adjust.lower({20100315_dt, 20100318_dt})},
                            {"dd", 7, adjust.lower({20100315_dt, 20100318_dt})},
                        }));
                // Only extending by 3 backwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Backwards, days(3)) -
                         filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.upper({20100412_dt, 20100415_dt})},
                        }));
            }
        }
    }
}
/*
 * posix_time
 */
TEMPLATE_TEST_CASE("Test gap filling to start for different interval types",
                   "[fill_to_start]",
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        using namespace boost::posix_time;
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto ptime_MIN =
            interval_dict::IntervalTraits<Interval>::lowest();
        const auto ptime_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto ptime_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();

        WHEN("The IntervalDict is filled to start")
        {
            THEN("Some keys are extended to the start of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100110
                REQUIRE(copy(test_dict).fill_to_start("20100110T180000"_pt) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa"s,
                             0,
                             adjust.upper({ptime_MIN, "20100101T180000"_pt})},
                            {"bb"s,
                             1,
                             adjust.upper({ptime_MIN, "20100105T180000"_pt})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_start("20100110T180000"_pt) ==
                        copy(test_dict).fill_to_start("20100110T180000"_pt,
                                                      ptime_MAXSZ));

                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_start("20100110T180000"_pt,
                                                      hours(30 * 24)) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa",
                             0,
                             adjust.upper(
                                 {"20091202T180000"_pt, "20100101T180000"_pt})},
                            {"bb",
                             1,
                             adjust.upper(
                                 {"20091206T180000"_pt, "20100105T180000"_pt})},
                        }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test filling gaps to end for different interval types",
                   "[fill_to_end]",
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        using namespace boost::posix_time;
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto ptime_MIN =
            interval_dict::IntervalTraits<Interval>::lowest();
        const auto ptime_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto ptime_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100415
                REQUIRE(copy(test_dict).fill_to_end("20100415T180000"_pt) -
                            test_dict ==
                        IDict(ImportData{
                            {"bb"s,
                             2,
                             adjust.lower({"20100615T180000"_pt, ptime_MAX})},
                            {"dd"s,
                             5,
                             adjust.lower({"20100715T180000"_pt, ptime_MAX})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_end("20100415T180000"_pt,
                                                    ptime_MAXSZ) -
                            copy(test_dict).fill_to_end("20100415T180000"_pt) ==
                        IDict());
                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_end("20100415T180000"_pt,
                                                    hours(30 * 24)) -
                            test_dict ==
                        IDict(ImportData{
                            {"bb"s,
                             2,
                             adjust.lower(
                                 {"20100615T180000"_pt, "20100715T180000"_pt})},
                            {"dd"s,
                             5,
                             adjust.lower(
                                 {"20100715T180000"_pt, "20100814T180000"_pt})},
                        }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test gap filling for different interval types",
                   "[fill_gaps]",
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        using namespace boost::posix_time;
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto ptime_MIN =
            interval_dict::IntervalTraits<Interval>::lowest();
        const auto ptime_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto ptime_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();
        WHEN("The IntervalDict gaps are filled safely")
        {
            THEN("All gaps of interrupted associations are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE(copy(test_dict).fill_gaps() - test_dict ==
                        IDict(ImportData{
                            {"aa"s,
                             0,
                             adjust.both(
                                 {"20100115T180000"_pt, "20100201T180000"_pt})},
                            {"bb"s,
                             1,
                             adjust.both(
                                 {"20100331T180000"_pt, "20100501T180000"_pt})},
                        }));

                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_gaps(ptime_MAXSZ) -
                            copy(test_dict).fill_gaps() ==
                        IDict());
                // Only extending by 3
                REQUIRE(
                    (copy(test_dict).fill_gaps(hours(3 * 24)) - test_dict) ==
                    IDict(ImportData{
                        {"aa"s,
                         0,
                         adjust.lower(
                             {"20100115T180000"_pt, "20100118T180000"_pt})},
                        {"aa"s,
                         0,
                         adjust.upper(
                             {"20100129T180000"_pt, "20100201T180000"_pt})},
                        {"bb"s,
                         1,
                         adjust.lower(
                             {"20100331T180000"_pt, "20100403T180000"_pt})},
                        {"bb"s,
                         1,
                         adjust.upper(
                             {"20100428T180000"_pt, "20100501T180000"_pt})},
                    }));
            }
        }
    }
}

TEMPLATE_TEST_CASE("Test extension into gaps for different interval types",
                   "[extend_into_gaps]",
                   boost::icl::interval<boost::posix_time::ptime>::type,
                   boost::icl::left_open_interval<boost::posix_time::ptime>,
                   boost::icl::right_open_interval<boost::posix_time::ptime>,
                   boost::icl::open_interval<boost::posix_time::ptime>,
                   boost::icl::closed_interval<boost::posix_time::ptime>,
                   boost::icl::discrete_interval<boost::posix_time::ptime>)
{
    using namespace std::string_literals;
    using namespace interval_dict::ptime_literals;
    using namespace interval_dict;
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    using Key = std::string;
    using Val = int;
    using IDict = IntervalDictICLExp<Key, Val, Interval>;
    using Interval = typename IDict::Interval;
    using Impl = typename IDict::ImplType;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        using namespace boost::posix_time;
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto ptime_MIN = IntervalTraits<Interval>::lowest();
        const auto ptime_MAX = IntervalTraits<Interval>::max();
        const auto ptime_MAXSZ = IntervalTraits<Interval>::max_size();
        WHEN("The IntervalDict are extended willy-nilly into the gaps")
        {
            const auto filled = copy(test_dict).fill_gaps();
            THEN("All gaps are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE((copy(filled).extend_into_gaps() - filled) ==
                        IDict(ImportData{
                            {"dd",
                             5,
                             adjust.both(
                                 {"20100315T180000"_pt, "20100415T180000"_pt})},
                            {"dd",
                             6,
                             adjust.both(
                                 {"20100315T180000"_pt, "20100415T180000"_pt})},
                            {"dd",
                             7,
                             adjust.both(
                                 {"20100315T180000"_pt, "20100415T180000"_pt})},
                        }));

                // Maximum extension is the default
                REQUIRE(
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both,
                                                  ptime_MAXSZ) ==
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both));
                // Only extending by 3 in both directions
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Both, hours(3 * 24)) -
                         filled) == IDict(ImportData{
                                        {"dd",
                                         5,
                                         adjust.upper({"20100412T180000"_pt,
                                                       "20100415T180000"_pt})},
                                        {"dd",
                                         6,
                                         adjust.lower({"20100315T180000"_pt,
                                                       "20100318T180000"_pt})},
                                        {"dd",
                                         7,
                                         adjust.lower({"20100315T180000"_pt,
                                                       "20100318T180000"_pt})},
                                    }));
                // Only extending by 3 forwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Forwards, hours(3 * 24)) -
                         filled) == IDict(ImportData{
                                        {"dd",
                                         6,
                                         adjust.lower({"20100315T180000"_pt,
                                                       "20100318T180000"_pt})},
                                        {"dd",
                                         7,
                                         adjust.lower({"20100315T180000"_pt,
                                                       "20100318T180000"_pt})},
                                    }));
                // Only extending by 3 backwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Backwards, hours(3 * 24)) -
                         filled) == IDict(ImportData{
                                        {"dd",
                                         5,
                                         adjust.upper({"20100412T180000"_pt,
                                                       "20100415T180000"_pt})},
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
        TestData<Val, Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust<Interval>{};

        WHEN("The IntervalDict is filled to start")
        {
            THEN("Some keys are extended to the start of time.")
            {
                const auto MIN = std::numeric_limits<BaseType>::lowest();
                // Extending only for keys whose first interval is AFTER 10
                REQUIRE(copy(test_dict).fill_to_start(10) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.upper({MIN, 0})},
                            {"bb", 1, adjust.upper({MIN, 5})},
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
                            {"aa", 0, adjust.upper({-30, 0})},
                            {"bb", 1, adjust.upper({-25, 5})},
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
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust<Interval>{};

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                const auto MAX = std::numeric_limits<BaseType>::max();
                // Extending only for keys whose last interval is BEFORE 50
                REQUIRE(copy(test_dict).fill_to_end(50) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.lower({85, MAX})},
                            {"cc", 3, adjust.lower({55, MAX})},
                            {"dd", 5, adjust.lower({75, MAX})},
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
                            {"aa", 0, adjust.lower({85, 115})},
                            {"cc", 3, adjust.lower({55, 85})},
                            {"dd", 5, adjust.lower({75, 105})},
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
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust<Interval>{};

        WHEN("The IntervalDict gaps are filled safely")
        {
            THEN("All gaps of interrupted associations are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE((copy(test_dict).fill_gaps() - test_dict) ==
                        IDict(ImportData{
                            {"aa", 0, adjust.both({5, 40})},
                            {"bb", 1, adjust.both({15, 20})},
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
                            {"aa", 0, adjust.lower({5, 8})},
                            {"aa", 0, adjust.upper({37, 40})},
                            {"bb", 1, adjust.both({15, 20})},
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
        TestData<Val, Interval> test_data;
        IDict test_dict(test_data.initial());
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust<Interval>{};

        WHEN("The IntervalDict are extended willy-nilly into the gaps")
        {
            const auto filled = copy(test_dict).fill_gaps();
            THEN("All gaps are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE((copy(filled).extend_into_gaps() - filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.both({35, 45})},
                            {"dd", 6, adjust.both({35, 45})},
                            {"dd", 7, adjust.both({35, 45})},
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
                         filled) == IDict(ImportData{
                                        {"dd", 5, adjust.upper({42, 45})},
                                        {"dd", 6, adjust.lower({35, 38})},
                                        {"dd", 7, adjust.lower({35, 38})},
                                    }));
                // Only extending by 3 forwards
                REQUIRE(
                    (copy(filled).extend_into_gaps(
                         interval_dict::GapExtensionDirection::Forwards, 3) -
                     filled) == IDict(ImportData{
                                    {"dd", 6, adjust.lower({35, 38})},
                                    {"dd", 7, adjust.lower({35, 38})},
                                }));
                // Only extending by 3 backwards
                REQUIRE(
                    (copy(filled).extend_into_gaps(
                         interval_dict::GapExtensionDirection::Backwards, 3) -
                     filled) == IDict(ImportData{
                                    {"dd", 5, adjust.upper({42, 45})},
                                }));
            }
        }
    }
}
