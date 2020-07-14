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
    std::vector<std::tuple<std::string, Val, Interval>> import_data{
        // aa-0 gap
        {"aa"s, 0, Interval{from_int(20100101), from_int(20100115)}},
        {"aa"s, 0, Interval{from_int(20100201), from_int(20100215)}},

        // Gap of bb-1 {15-20}
        {"bb"s, 1, Interval{from_int(20100105), from_int(20100331)}},
        {"bb"s, 1, Interval{from_int(20100501), from_int(20100531)}},
        {"bb"s, 2, Interval{from_int(20100501), from_int(20100615)}},

        // overlap ignored
        {"cc"s, 3, Interval{from_int(20100115), from_int(20100131)}},
        {"cc"s, 3, Interval{from_int(20100120), from_int(20100215)}},

        // dd-6/7 over {30-35}
        {"dd"s, 5, Interval{from_int(20100201), from_int(20100301)}},
        {"dd"s, 6, boost::icl::left_subtract(Interval{from_int(20100228), from_int(20100315)},
                                             Interval{from_int(20100201), from_int(20100301)})},
        {"dd"s, 7, boost::icl::left_subtract(Interval{from_int(20100228), from_int(20100315)},
                                             Interval{from_int(20100201), from_int(20100301)})},
        {"dd"s, 5, Interval{from_int(20100415), from_int(20100715)}},
    };
    return import_data;
}


struct Adjust
{
    template<typename Interval>
    auto lower(Interval interval) const
    { return interval; }
    template<typename Interval>
    auto upper(Interval interval) const
    { return interval; }
    template<typename Interval>
    auto both(Interval interval) const
    { return interval; }

    template<typename T>
    auto lower(typename boost::icl::open_interval<T> interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return boost::icl::open_interval<T>{--l, u};
    }
    template<typename T>
    auto upper(typename boost::icl::open_interval<T> interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return boost::icl::open_interval<T>{l, ++u};
    }
    template<typename T>
    auto both(typename boost::icl::open_interval<T> interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return boost::icl::open_interval<T>{--l, ++u};
    }

    template<typename T>
    auto lower(typename boost::icl::closed_interval<T> interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return boost::icl::closed_interval<T>{++l, u};
    }
    template<typename T>
    auto upper(typename boost::icl::closed_interval<T> interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return boost::icl::closed_interval<T>{l, --u};
    }
    template<typename T>
    auto both(typename boost::icl::closed_interval<T> interval) const
    {
        using boost::icl::operator++;
        using boost::icl::operator--;
        auto l = boost::icl::lower(interval);
        auto u = boost::icl::upper(interval);
        return boost::icl::closed_interval<T>{++l, --u};
    }
};




#endif //TESTS_TEST_ICL_H
