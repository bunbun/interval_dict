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
/// \file test_icl.h
/// \brief Common test data and adjustments to handle different interval types
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef TESTS_TEST_ICL_H
#define TESTS_TEST_ICL_H

#include <interval_dict/gregorian.h>
#include <interval_dict/ptime.h>
#include <interval_dict/intervaldicticl.h>

template <typename Key, typename Val, typename Interval, typename Impl>
interval_dict::IntervalDictExp<Key, Val, Interval, Impl>
copy(interval_dict::IntervalDictExp<Key, Val, Interval, Impl> data)
{
    return data;
}

template <typename Val, typename Interval, typename Impl>
interval_dict::IntervalDictExp<std::string, Val, Interval, Impl> test_data1()
{
    using namespace std::string_literals;
    std::vector<std::tuple<std::string, Val, Interval>> import_data{
        // aa-0 gap
        {"aa"s, 0, Interval{0, 5}},
        {"aa"s, 0, Interval{40, 85}},

        // Gap of bb-1 {15-20}
        {"bb"s, 1, Interval{5, 15}},
        {"bb"s, 1, Interval{20, 25}},
        {"bb"s, 2, Interval{20, 30}},

        // overlap ignored
        {"cc"s, 3, Interval{15, 35}},
        {"cc"s, 3, Interval{25, 55}},

        // dd-6/7 over {30-35}
        {"dd"s, 5, Interval{20, 30}},
        {"dd"s, 6, boost::icl::left_subtract(Interval{29, 35}, Interval{20,30})},
        {"dd"s, 7, boost::icl::left_subtract(Interval{29, 35}, Interval{20,30})},
        {"dd"s, 5, Interval{45, 75}},
    };
    return import_data;
}

template <typename Val, typename Interval, typename Impl>
interval_dict::IntervalDictExp<std::string, Val, Interval, Impl> test_data_date()
{
    static_assert(std::is_same<typename Interval::domain_type, boost::gregorian::date>::value,
                 "Must be an interval of dates");
    using namespace std::string_literals;
    using namespace interval_dict::date_literals;
    std::vector<std::tuple<std::string, Val, Interval>> import_data{
        // aa-0 gap
        {"aa"s, 0, {20100101_dt, 20100115_dt}},
        {"aa"s, 0, {20100201_dt, 20100215_dt}},

        // Gap of bb-1 {15-20}
        {"bb"s, 1, {20100105_dt, 20100331_dt}},
        {"bb"s, 1, {20100501_dt, 20100531_dt}},
        {"bb"s, 2, {20100501_dt, 20100615_dt}},

        // overlap ignored
        {"cc"s, 3, {20100115_dt, 20100131_dt}},
        {"cc"s, 3, {20100120_dt, 20100215_dt}},

        // dd-6/7 over {30-35}
        {"dd"s, 5, {20100201_dt, 20100301_dt}},
        {"dd"s, 6, boost::icl::left_subtract(Interval{20100228_dt, 20100315_dt},
                                             Interval{20100201_dt, 20100301_dt})},
        {"dd"s, 7, boost::icl::left_subtract(Interval{20100228_dt, 20100315_dt},
                                             Interval{20100201_dt, 20100301_dt})},
        {"dd"s, 5, {20100415_dt, 20100715_dt}},
    };
    return import_data;
}

template <typename Val, typename Interval, typename Impl>
interval_dict::IntervalDictExp<std::string, Val, Interval, Impl> test_data_ptime()
{
    static_assert(std::is_same<typename Interval::domain_type, boost::posix_time::ptime>::value,
                  "Must be an interval of posix time");
    using namespace std::string_literals;
    using namespace interval_dict::ptime_literals;
    std::vector<std::tuple<std::string, Val, Interval>> import_data{
        // aa-0 gap
        {"aa"s, 0, {"20100101T180000"_pt, "20100115T180000"_pt}},
        {"aa"s, 0, {"20100201T180000"_pt, "20100215T180000"_pt}},

        // Gap of bb-1 {15-20}
        {"bb"s, 1, {"20100105T180000"_pt, "20100331T180000"_pt}},
        {"bb"s, 1, {"20100501T180000"_pt, "20100531T180000"_pt}},
        {"bb"s, 2, {"20100501T180000"_pt, "20100615T180000"_pt}},

        // overlap ignored
        {"cc"s, 3, {"20100115T180000"_pt, "20100131T180000"_pt}},
        {"cc"s, 3, {"20100120T180000"_pt, "20100215T180000"_pt}},

        // dd-6/7 over {30-35}
        {"dd"s, 5, {"20100201T180000"_pt, "20100301T180000"_pt}},
        {"dd"s, 6, boost::icl::left_subtract(Interval{"20100228T180000"_pt, "20100315T180000"_pt},
                                             Interval{"20100201T180000"_pt, "20100301T180000"_pt})},
        {"dd"s, 7, boost::icl::left_subtract(Interval{"20100228T180000"_pt, "20100315T180000"_pt},
                                             Interval{"20100201T180000"_pt, "20100301T180000"_pt})},
        {"dd"s, 5, {"20100415T180000"_pt, "20100715T180000"_pt}},
    };
    return import_data;
}

/*
 * Adjust boundaries for open and closed intervals only
 */
template <typename Interval, typename enable = void>
struct Adjust
{
    auto lower(Interval interval) const
    {
        return interval;
    }
    auto upper(Interval interval) const
    {
        return interval;
    }
    auto both(Interval interval) const
    {
        return interval;
    }
};

template <typename Interval>
struct Adjust<Interval, typename std::enable_if<
    std::is_same<typename boost::icl::open_interval<typename Interval::domain_type>,
                 Interval>::value>::type>
{
    Interval lower(Interval interval) const
    {
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return {--l, u};
    }

    Interval upper(Interval interval) const
    {
        using boost::icl::operator++;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return {l, ++u};
    }

    Interval both(Interval interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return {--l, ++u};
    }
};

template <typename Interval>
struct Adjust<Interval,
    typename std::enable_if<
        std::is_same<typename boost::icl::closed_interval<typename Interval::domain_type>,
                     Interval
                     >::value
                          >::type>
{
    Interval lower(Interval interval) const
    {
        using boost::icl::operator++;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return {++l, u};
    }
    Interval upper(Interval interval) const
    {
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return {l, --u};
    }
    Interval both(Interval interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return {++l, --u};
    }
};




#endif //TESTS_TEST_ICL_H
