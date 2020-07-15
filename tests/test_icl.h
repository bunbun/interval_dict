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

/*
 * Adjust boundaries for open and closed intervals only
 */
template <typename Interval, typename enable = void> struct Adjust
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
struct Adjust<
    Interval,
    typename std::enable_if<std::is_same<
        typename boost::icl::open_interval<typename Interval::domain_type>,
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
struct Adjust<
    Interval,
    typename std::enable_if<std::is_same<
        typename boost::icl::closed_interval<typename Interval::domain_type>,
        Interval>::value>::type>
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


#endif // TESTS_TEST_ICL_H
