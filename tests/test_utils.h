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

#ifndef TESTS_TEST_UTILS_H
#define TESTS_TEST_UTILS_H

#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/intervaldictitree.h>
#include <interval_dict/ptime.h>

template <typename Key, typename Val, typename Interval, typename Impl>
interval_dict::IntervalDictExp<Key, Val, Interval, Impl>
copy(interval_dict::IntervalDictExp<Key, Val, Interval, Impl> data)
{
    return data;
}
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
interval_dict::BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
copy(interval_dict::BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
         data)
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

// Return the results of identifier_dict::intervals() as a vector
template <typename Key, typename Val, typename Interval, typename Impl>
std::vector<std::tuple<Key, Val, Interval>>
intervals_as_vec(interval_dict::IntervalDictExp<Key, Val, Interval, Impl> idict,
                 Interval interval = interval_dict::interval_extent<Interval>)
{
    std::vector<std::tuple<Key, Val, Interval>> results;
    for (const auto& [key, value, interval] :
         interval_dict::intervals(idict, interval))
    {
        results.push_back(std::tuple{key, value, interval});
    }
    return results;
}

// Return the results of identifier_dict::intervals() as a vector of key /
// values
template <typename Key, typename Val, typename Interval, typename Impl>
std::set<std::tuple<Key, Val>> intervals_as_key_values(
    interval_dict::IntervalDictExp<Key, Val, Interval, Impl> idict,
    Interval interval = interval_dict::interval_extent<Interval>)
{
    std::set<std::tuple<Key, Val>> results;
    for (const auto& [key, value, _] :
         interval_dict::intervals(idict, interval))
    {
        results.insert(std::tuple{key, value});
    }
    return results;
}

// Return the results of identifier_dict::intervals() as a vector
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::vector<std::tuple<Key, Val, Interval>> intervals_as_vec(
    interval_dict::BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
        idict,
    Interval interval = interval_dict::interval_extent<Interval>)
{
    std::vector<std::tuple<Key, Val, Interval>> results;
    for (const auto& [key, value, interval] :
         interval_dict::intervals(idict, interval))
    {
        results.push_back(std::tuple{key, value, interval});
    }
    return results;
}

// Return the results of identifier_dict::intervals() as a vector of key /
// values
template <typename Key,
          typename Val,
          typename Interval,
          typename Impl,
          typename InverseImpl>
std::set<std::tuple<Key, Val>> intervals_as_key_values(
    interval_dict::BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl>
        idict,
    Interval interval = interval_dict::interval_extent<Interval>)
{
    std::set<std::tuple<Key, Val>> results;
    for (const auto& [key, value, _] :
         interval_dict::intervals(idict, interval))
    {
        results.insert(std::tuple{key, value});
    }
    return results;
}

#endif // TESTS_TEST_UTILS_H
