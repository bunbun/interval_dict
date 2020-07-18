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
#include "test_data.h"
#include "test_icl.h"
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
    using namespace boost::gregorian;
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
        TestData<Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};
        const auto test_pos = 20100120_dt;
        const auto test_len = days(10);

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
                REQUIRE(copy(test_dict).fill_to_start(test_pos) - test_dict ==
                        IDict(ImportData{
                            {"aa"s, 0, adjust.upper({date_MIN, 20100101_dt})},
                            {"bb"s, 1, adjust.upper({date_MIN, 20100115_dt})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_start(test_pos) ==
                        copy(test_dict).fill_to_start(test_pos, date_MAXSZ));

                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_start(test_pos, test_len) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.upper({20091222_dt, 20100101_dt})},
                            {"bb", 1, adjust.upper({20100105_dt, 20100115_dt})},
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
    using namespace boost::gregorian;
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
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto date_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();
        const auto test_pos = 20100515_dt;
        const auto test_len = days(10);

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100415
                REQUIRE(copy(test_dict).fill_to_end(test_pos) - test_dict ==
                        IDict(ImportData{
                            {"aa"s, 0, adjust.lower({20100701_dt, date_MAX})},
                            {"cc"s, 3, adjust.lower({20100601_dt, date_MAX})},
                            {"dd"s, 5, adjust.lower({20100615_dt, date_MAX})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_end(test_pos, date_MAXSZ) -
                            copy(test_dict).fill_to_end(test_pos) ==
                        IDict());
                // Only extending by test_len
                REQUIRE(
                    copy(test_dict).fill_to_end(test_pos, test_len) -
                        test_dict ==
                    IDict(ImportData{
                        {"aa"s, 0, adjust.lower({20100701_dt, 20100711_dt})},
                        {"cc"s, 3, adjust.lower({20100601_dt, 20100611_dt})},
                        {"dd"s, 5, adjust.lower({20100615_dt, 20100625_dt})},
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
    using namespace boost::gregorian;
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
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto date_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();
        const auto test_len = days(10);
        WHEN("The IntervalDict gaps are filled safely")
        {
            THEN("All gaps of interrupted associations are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE(copy(test_dict).fill_gaps() - test_dict ==
                        IDict(ImportData{
                            {"aa"s, 0, adjust.both({20100115_dt, 20100415_dt})},
                            {"bb"s, 1, adjust.both({20100201_dt, 20100215_dt})},
                        }));

                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_gaps(date_MAXSZ) -
                            copy(test_dict).fill_gaps() ==
                        IDict());
                // Only extending by 3
                REQUIRE(
                    (copy(test_dict).fill_gaps(test_len) - test_dict) ==
                    IDict(ImportData{
                        {"aa"s, 0, adjust.lower({20100115_dt, 20100125_dt})},
                        {"aa"s, 0, adjust.upper({20100405_dt, 20100415_dt})},
                        {"bb"s, 1, adjust.both({20100201_dt, 20100215_dt})},
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
    using namespace boost::gregorian;
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
        TestData<Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = IntervalTraits<Interval>::lowest();
        const auto date_MAX = IntervalTraits<Interval>::max();
        const auto date_MAXSZ = IntervalTraits<Interval>::max_size();
        const auto test_len = days(10);
        WHEN("The IntervalDict are extended willy-nilly into the gaps")
        {
            const auto filled = copy(test_dict).fill_gaps();
            THEN("All gaps are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                REQUIRE((copy(filled).extend_into_gaps() - filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.both({20100401_dt, 20100501_dt})},
                            {"dd", 6, adjust.both({20100401_dt, 20100501_dt})},
                            {"dd", 7, adjust.both({20100401_dt, 20100501_dt})},
                        }));

                // Maximum extension is the default
                REQUIRE(
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both,
                                                  date_MAXSZ) ==
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both));
                // Only extending by 3 in both directions
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Both, test_len) -
                         filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.upper({20100421_dt, 20100501_dt})},
                            {"dd", 6, adjust.lower({20100401_dt, 20100411_dt})},
                            {"dd", 7, adjust.lower({20100401_dt, 20100411_dt})},
                        }));
                // Only extending by test_len forwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Forwards, test_len) -
                         filled) ==
                        IDict(ImportData{
                            {"dd", 6, adjust.lower({20100401_dt, 20100411_dt})},
                            {"dd", 7, adjust.lower({20100401_dt, 20100411_dt})},
                        }));
                // Only extending by test_len backwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Backwards, test_len) -
                         filled) ==
                        IDict(ImportData{
                            {"dd", 5, adjust.upper({20100421_dt, 20100501_dt})},
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
    using namespace boost::posix_time;
    using namespace interval_dict::date_literals;
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
        using namespace interval_dict::ptime_literals;
        TestData<Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};
        const auto test_pos = "20100120T180000"_pt;
        const auto test_len = hours(10 * 24);

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
                REQUIRE(copy(test_dict).fill_to_start(test_pos) - test_dict ==
                        IDict(ImportData{
                            {"aa"s,
                             0,
                             adjust.upper({date_MIN, "20100101T180000"_pt})},
                            {"bb"s,
                             1,
                             adjust.upper({date_MIN, "20100115T180000"_pt})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_start(test_pos) ==
                        copy(test_dict).fill_to_start(test_pos, date_MAXSZ));

                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_start(test_pos, test_len) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa",
                             0,
                             adjust.upper(
                                 {"20091222T180000"_pt, "20100101T180000"_pt})},
                            {"bb",
                             1,
                             adjust.upper(
                                 {"20100105T180000"_pt, "20100115T180000"_pt})},
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
    using namespace interval_dict::date_literals;
    using namespace interval_dict::ptime_literals;
    using namespace boost::posix_time;
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
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto date_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();
        const auto test_pos = "20100515T180000"_pt;
        const auto test_len = hours(10 * 24);

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                // Extending only for keys whose first interval is AFTER
                // 20100415
                REQUIRE(copy(test_dict).fill_to_end(test_pos) - test_dict ==
                        IDict(ImportData{
                            {"aa"s,
                             0,
                             adjust.lower({"20100701T180000"_pt, date_MAX})},
                            {"cc"s,
                             3,
                             adjust.lower({"20100601T180000"_pt, date_MAX})},
                            {"dd"s,
                             5,
                             adjust.lower({"20100615T180000"_pt, date_MAX})},
                        }));
                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_to_end(test_pos, date_MAXSZ) -
                            copy(test_dict).fill_to_end(test_pos) ==
                        IDict());
                // Only extending by test_len
                REQUIRE(copy(test_dict).fill_to_end(test_pos, test_len) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa"s,
                             0,
                             adjust.lower(
                                 {"20100701T180000"_pt, "20100711T180000"_pt})},
                            {"cc"s,
                             3,
                             adjust.lower(
                                 {"20100601T180000"_pt, "20100611T180000"_pt})},
                            {"dd"s,
                             5,
                             adjust.lower(
                                 {"20100615T180000"_pt, "20100625T180000"_pt})},
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
    using namespace boost::posix_time;
    using namespace interval_dict::date_literals;
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
        TestData<Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = interval_dict::IntervalTraits<Interval>::lowest();
        const auto date_MAX = interval_dict::IntervalTraits<Interval>::max();
        const auto date_MAXSZ =
            interval_dict::IntervalTraits<Interval>::max_size();
        const auto test_len = hours(10 * 24);
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
                                 {"20100115T180000"_pt, "20100415T180000"_pt})},
                            {"bb"s,
                             1,
                             adjust.both(
                                 {"20100201T180000"_pt, "20100215T180000"_pt})},
                        }));

                // Maximum extension is the default
                REQUIRE(copy(test_dict).fill_gaps(date_MAXSZ) -
                            copy(test_dict).fill_gaps() ==
                        IDict());
                // Only extending by 3
                REQUIRE((copy(test_dict).fill_gaps(test_len) - test_dict) ==
                        IDict(ImportData{
                            {"aa"s,
                             0,
                             adjust.lower(
                                 {"20100115T180000"_pt, "20100125T180000"_pt})},
                            {"aa"s,
                             0,
                             adjust.upper(
                                 {"20100405T180000"_pt, "20100415T180000"_pt})},
                            {"bb"s,
                             1,
                             adjust.both(
                                 {"20100201T180000"_pt, "20100215T180000"_pt})},
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
    using namespace boost::posix_time;
    using namespace interval_dict::date_literals;
    using namespace interval_dict;
    using namespace interval_dict::ptime_literals;
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
        using namespace interval_dict::ptime_literals;
        TestData<Interval> test_data;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};

        const auto date_MIN = IntervalTraits<Interval>::lowest();
        const auto date_MAX = IntervalTraits<Interval>::max();
        const auto date_MAXSZ = IntervalTraits<Interval>::max_size();
        const auto test_len = hours(10 * 24);
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
                                 {"20100401T180000"_pt, "20100501T180000"_pt})},
                            {"dd",
                             6,
                             adjust.both(
                                 {"20100401T180000"_pt, "20100501T180000"_pt})},
                            {"dd",
                             7,
                             adjust.both(
                                 {"20100401T180000"_pt, "20100501T180000"_pt})},
                        }));

                // Maximum extension is the default
                REQUIRE(
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both,
                                                  date_MAXSZ) ==
                    copy(filled).extend_into_gaps(GapExtensionDirection::Both));
                // Only extending by 3 in both directions
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Both, test_len) -
                         filled) == IDict(ImportData{
                                        {"dd",
                                         5,
                                         adjust.upper({"20100421T180000"_pt,
                                                       "20100501T180000"_pt})},
                                        {"dd",
                                         6,
                                         adjust.lower({"20100401T180000"_pt,
                                                       "20100411T180000"_pt})},
                                        {"dd",
                                         7,
                                         adjust.lower({"20100401T180000"_pt,
                                                       "20100411T180000"_pt})},
                                    }));
                // Only extending by test_len forwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Forwards, test_len) -
                         filled) == IDict(ImportData{
                                        {"dd",
                                         6,
                                         adjust.lower({"20100401T180000"_pt,
                                                       "20100411T180000"_pt})},
                                        {"dd",
                                         7,
                                         adjust.lower({"20100401T180000"_pt,
                                                       "20100411T180000"_pt})},
                                    }));
                // Only extending by test_len backwards
                REQUIRE((copy(filled).extend_into_gaps(
                             GapExtensionDirection::Backwards, test_len) -
                         filled) == IDict(ImportData{
                                        {"dd",
                                         5,
                                         adjust.upper({"20100421T180000"_pt,
                                                       "20100501T180000"_pt})},
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
    using namespace boost::posix_time;
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
        const auto test_pos = 10;
        const auto test_len = 3;

        WHEN("The IntervalDict is filled to start")
        {
            THEN("Some keys are extended to the start of time.")
            {
                const auto MIN = std::numeric_limits<BaseType>::lowest();
                // Extending only for keys whose first interval is AFTER
                // test_len
                REQUIRE(copy(test_dict).fill_to_start(test_pos) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.upper({MIN, 0})},
                            {"bb", 1, adjust.upper({MIN, 5})},
                        }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_to_start(test_pos) ==
                    copy(test_dict).fill_to_start(
                        test_pos,
                        interval_dict::IntervalTraits<Interval>::max_size()));

                // Only extending by test_len
                REQUIRE(copy(test_dict).fill_to_start(test_pos, test_len) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.upper({-3, 0})},
                            {"bb", 1, adjust.upper({2, 5})},
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
    using namespace boost::posix_time;
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
        IDict test_dict(test_data.initial());
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto adjust = Adjust<Interval>{};
        const auto test_pos = 50;
        const auto test_len = 3;

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                const auto MAX = std::numeric_limits<BaseType>::max();
                // Extending only for keys whose last interval is BEFORE
                // test_pos
                REQUIRE(copy(test_dict).fill_to_end(test_pos) - test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.lower({85, MAX})},
                            {"cc", 3, adjust.lower({55, MAX})},
                            {"dd", 5, adjust.lower({75, MAX})},
                        }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_to_end(
                        test_pos,
                        interval_dict::IntervalTraits<Interval>::max_size()) -
                        copy(test_dict).fill_to_end(test_pos) ==
                    IDict());
                // Only extending by test_len
                REQUIRE(copy(test_dict).fill_to_end(test_pos, test_len) -
                            test_dict ==
                        IDict(ImportData{
                            {"aa", 0, adjust.lower({85, 88})},
                            {"cc", 3, adjust.lower({55, 58})},
                            {"dd", 5, adjust.lower({75, 78})},
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
        TestData<Interval> test_data;
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
        TestData<Interval> test_data;
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
