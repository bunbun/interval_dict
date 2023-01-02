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
/// \file test_icl_intervals.cpp
/// \brief Test IntervalDict::intervals()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "print_set.h"
#include "print_tuple.h"
#include "print_vector.h"
#include "test_data.h"
#include "test_utils.h"

#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldictail.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/intervaldictitree.h>
#include <interval_dict/ptime.h>
#include <concepts>
#include <vector>

TEMPLATE_TEST_CASE ("Test disjoint intervals for different interval types",
                    "[disjoint_intervals]",
                    boost::icl::interval<int>::type,
                    boost::icl::left_open_interval<int>,
                    boost::icl::right_open_interval<int>,
                    boost::icl::open_interval<int>,
                    boost::icl::closed_interval<int>,
//                  boost::icl::interval<float>::type,
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
  using Interval = TestType;
  using Key = std::string;
  using Value = int;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
  static_assert(std::same_as<Interval, typename IDict::IntervalType>);
  using ImportData = std::vector<std::tuple<Key, std::vector<Value>, Interval>>;

  /*
   * TestData
   */
  GIVEN ("An IntervalDict with overlapping intervals")
  {
    TestData<Interval> test_data;
    const IDict test_dict (test_data.initial ());
    // N.B. "ee" should be ignored
    const auto all_keys = std::vector {"aa"s, "bb"s, "cc"s, "dd"s, "ee"s};
    const auto expected = test_data.disjoint_intervals ();
    const auto query = test_data.query_interval ();

    WHEN ("All data is retrieved via disjoint_intervals() specified with "
          "keys")
    {
      auto vec_intervals
        = interval_dict::disjoint_intervals (test_dict, all_keys);
      ImportData intervals_data;
      for (const auto &[key, values, interval] : vec_intervals)
      {
        intervals_data.push_back ({key, values, interval});
      }
      THEN ("Expect the same values")
      {
        REQUIRE (intervals_data == expected);
      }
    }

    WHEN ("Only data within a query interval is retrieved via intervals()")
    {
      auto vec_intervals
        = interval_dict::disjoint_intervals (test_dict, all_keys, query);
      ImportData intervals_data;
      for (const auto &[key, values, interval] : vec_intervals)
      {
        intervals_data.push_back ({key, values, interval});
      }
      THEN ("Expect to match only a subset")
      {
        ImportData expected_subset;
        for (const auto &[key, values, interval] : expected)
        {
          if (boost::icl::intersects (interval, query))
          {
            expected_subset.push_back ({key, values, interval & query});
          }
        }
        REQUIRE (intervals_data == expected_subset);
      }
    }
    WHEN ("Only data matching keys within a query interval is retrieved via "
          "intervals()")
    {
      auto vec_intervals = interval_dict::disjoint_intervals (
        test_dict, std::vector {"bb"s, "dd"s}, query);
      ImportData intervals_data;
      for (const auto &[key, values, interval] : vec_intervals)
      {
        intervals_data.push_back ({key, values, interval});
      }
      THEN ("Expect to match only a subset")
      {
        ImportData expected_subset;
        auto keys = std::set {"bb"s, "dd"s};
        for (const auto &[key, values, interval] : expected)
        {
          if (keys.count (key) && boost::icl::intersects (interval, query))
          {
            expected_subset.push_back ({key, values, interval & query});
          }
        }
        REQUIRE (intervals_data == expected_subset);
      }
    }
    WHEN ("Only data matching a key within a query interval is retrieved "
          "via intervals()")
    {
      auto vec_intervals
        = interval_dict::disjoint_intervals (test_dict, "dd"s, query);
      ImportData intervals_data;
      for (const auto &[key, values, interval] : vec_intervals)
      {
        intervals_data.push_back ({key, values, interval});
      }
      THEN ("Expect to match only a subset")
      {
        ImportData expected_subset;
        auto keys = std::set {"bb"s, "dd"s};
        for (const auto &[key, values, interval] : expected)
        {
          if (key == "dd"s && boost::icl::intersects (interval, query))
          {
            expected_subset.push_back ({key, values, interval & query});
          }
        }
        REQUIRE (intervals_data == expected_subset);
      }
    }
  }
}
