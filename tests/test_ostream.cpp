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
/// \file test_ostream.cpp
/// \brief Test os << IntervalDict()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "test_data.h"
#include "test_utils.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldictail.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/intervaldictitree.h>
#include <interval_dict/ptime.h>

#include <concepts>
#include <vector>

TEMPLATE_TEST_CASE ("Test stream output for different interval types",
                    "[ostream<<]",
                    boost::icl::left_open_interval<int>,
                    boost::icl::interval<int>::type,
                    boost::icl::right_open_interval<int>,
                    boost::icl::closed_interval<int>,
                    boost::icl::open_interval<int>,
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
  TestData<Interval> test_data;
  // Unadjusted disjoint intervals
  const auto import_data = test_data.disjoint_intervals ();

  /*
   * TestData
   */
  GIVEN ("An IntervalDict with overlapping intervals")
  {
    IDict test_dict;
    for (const auto &[key, values, interval] : import_data)
    {
      for (const auto &value : values)
      {
        test_dict.insert ({std::pair {key, value}}, interval);
      }
    }
    WHEN ("Output to a string stream")
    {
      std::ostringstream os;
      os << test_dict;
      std::string result = os.str ();
      // Replace () with [] so that we don't need to test separately
      // for open/close intervals
      std::replace (result.begin (), result.end (), ')', ']');
      std::replace (result.begin (), result.end (), '(', '[');
      THEN ("Expect the following strings")
      {
        REQUIRE (result == test_data.str ());
      }
    }
  }
}
