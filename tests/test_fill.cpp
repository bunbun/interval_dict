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
#include "test_utils.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldictail.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/intervaldictitree.h>
#include <interval_dict/ptime.h>
#include <concepts>
#include <vector>

// N.B. Continuous types like float have
// no sensible semantics for adjacent intervals
// when open/closed coordinates used so can't be used
// in interval maps
// boost::icl::closed_interval<float>
// boost::icl::open_interval<float>
TEMPLATE_TEST_CASE ("Test gap filling to start for different interval types",
                    "[fill_to_start]",
                    boost::icl::interval<int>::type,
                    boost::icl::left_open_interval<int>,
                    boost::icl::right_open_interval<int>,
                    boost::icl::open_interval<int>,
                    boost::icl::closed_interval<int>,
//                    boost::icl::interval<float>::type,
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
  using namespace boost::posix_time;
  using Interval = TestType;
  using BaseType = typename Interval::domain_type;
  using Key = std::string;
  using Value = int;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
    static_assert(std::same_as<Interval, typename IDict::IntervalType>);
  using ImportData = std::vector<std::tuple<Key, Value, Interval>>;

  /*
   * TestData
   */
  GIVEN ("An IntervalDict with overlapping intervals")
  {
    TestData<Interval> test_data;
    const IDict test_dict (test_data.initial ());
    const auto adjust = Adjust<Interval> {};
    const auto MIN = interval_dict::IntervalTraits<Interval>::minimum ();
    const auto MAXSZ = interval_dict::IntervalTraits<Interval>::max_size ();
    const auto all_keys = std::vector {"aa"s, "bb"s, "cc"s, "dd"s, "ee"s};
    const auto values = TestDataValues<BaseType>::values ();
    const auto test_pos = values[2];
    const auto test_len = test_data.test_duration;

    WHEN ("The IntervalDict is filled to start")
    {
      THEN ("Some keys are extended to the start of time.")
      {
        // Extending only for keys whose first interval is AFTER
        // test_len
        REQUIRE (copy (test_dict).fill_to_start (test_pos) - test_dict
                 == IDict (ImportData {
                   {"aa", 0, adjust.upper ({MIN, values[0]})},
                   {"bb", 1, adjust.upper ({MIN, values[1]})},
                 }));
        // Maximum extension is the default
        REQUIRE (copy (test_dict).fill_to_start (test_pos)
                 == copy (test_dict).fill_to_start (test_pos, MAXSZ));
        // Only extending by test_len
        REQUIRE (copy (test_dict).fill_to_start (test_pos, test_len) - test_dict
                 == IDict (ImportData {
                   {"aa", 0, adjust.upper ({values[0] - test_len, values[0]})},
                   {"bb", 1, adjust.upper ({values[1] - test_len, values[1]})},
                 }));
      }
    }
  }
}

TEMPLATE_TEST_CASE ("Test filling gaps to end for different interval types",
                    "[fill_to_end]",
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
  using namespace boost::posix_time;
  using Interval = TestType;
  using BaseType = typename Interval::domain_type;
  using Key = std::string;
  using Value = int;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
  static_assert(std::same_as<Interval, typename IDict::IntervalType>);
  using ImportData = std::vector<std::tuple<Key, Value, Interval>>;

  /*
   * TestData
   */
  GIVEN ("An IntervalDict with overlapping intervals")
  {
    TestData<Interval> test_data;
    const IDict test_dict (test_data.initial ());
    const auto adjust = Adjust<Interval> {};
    const auto MAX = interval_dict::IntervalTraits<Interval>::maximum ();
    const auto MAXSZ = interval_dict::IntervalTraits<Interval>::max_size ();
    const auto all_keys = std::vector {"aa"s, "bb"s, "cc"s, "dd"s, "ee"s};
    const auto values = TestDataValues<BaseType>::values ();
    const auto test_pos = values[10];
    const auto test_len = test_data.test_duration;

    WHEN ("The IntervalDict is filled to end")
    {
      THEN ("Some keys are extended to the end of time.")
      {
        // Extending only for keys whose last interval is BEFORE
        // test_pos
        REQUIRE (copy (test_dict).fill_to_end (test_pos) - test_dict
                 == IDict (ImportData {
                   {"aa", 0, adjust.lower ({values[17], MAX})},
                   {"cc", 3, adjust.lower ({values[11], MAX})},
                   {"dd", 5, adjust.lower ({values[15], MAX})},
                 }));
        // Maximum extension is the default
        REQUIRE (copy (test_dict).fill_to_end (test_pos, MAXSZ)
                   - copy (test_dict).fill_to_end (test_pos)
                 == IDict ());
        // Only extending by test_len
        REQUIRE (
          copy (test_dict).fill_to_end (test_pos, test_len) - test_dict
          == IDict (ImportData {
            {"aa", 0, adjust.lower ({values[17], values[17] + test_len})},
            {"cc", 3, adjust.lower ({values[11], values[11] + test_len})},
            {"dd", 5, adjust.lower ({values[15], values[15] + test_len})},
          }));
      }
    }
  }
}

TEMPLATE_TEST_CASE ("Test gap filling for different interval types",
                    "[fill_gaps]",
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
  using BaseType = typename Interval::domain_type;
  using Key = std::string;
  using Value = int;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
    static_assert(std::same_as<Interval, typename IDict::IntervalType>);
  using ImportData = std::vector<std::tuple<Key, Value, Interval>>;

  /*
   * TestData
   */
  GIVEN ("An IntervalDict with overlapping intervals")
  {
    TestData<Interval> test_data;
    const IDict test_dict (test_data.initial ());
    const auto adjust = Adjust<Interval> {};
    const auto all_keys = std::vector {"aa"s, "bb"s, "cc"s, "dd"s, "ee"s};
    const auto values = TestDataValues<BaseType>::values ();
    const auto test_len = test_data.test_duration;

    WHEN ("The IntervalDict gaps are filled safely")
    {
      THEN ("All gaps of interrupted associations are filled.")
      {
        // Check interval is the intersection of the different sorts of
        // Interval
        REQUIRE ((copy (test_dict).fill_gaps () - test_dict)
                 == IDict (ImportData {
                   {"aa", 0, adjust.both ({values[1], values[8]})},
                   {"bb", 1, adjust.both ({values[3], values[4]})},
                 }));

        // Maximum extension is the default
        REQUIRE (copy (test_dict).fill_gaps (
                   interval_dict::IntervalTraits<Interval>::max_size ())
                   - copy (test_dict).fill_gaps ()
                 == IDict ());
        // Only extending by 3
        REQUIRE ((copy (test_dict).fill_gaps (test_len) - test_dict)
                 == IDict (ImportData {
                   {"aa", 0, adjust.lower ({values[1], values[1] + test_len})},
                   {"aa", 0, adjust.upper ({values[8] - test_len, values[8]})},
                   {"bb", 1, adjust.both ({values[3], values[4]})},
                 }));
      }
    }
  }
}

TEMPLATE_TEST_CASE ("Test extension into gaps for different interval types",
                    "[extend_into_gaps]",
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
  using BaseType = typename Interval::domain_type;
  using Key = std::string;
  using Value = int;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
    static_assert(std::same_as<Interval, typename IDict::IntervalType>);
  using ImportData = std::vector<std::tuple<Key, Value, Interval>>;

  GIVEN ("An IntervalDict with overlapping intervals")
  {
    TestData<Interval> test_data;
    const IDict test_dict (test_data.initial ());
    const auto adjust = Adjust<Interval> {};
    const auto MAXSZ = interval_dict::IntervalTraits<Interval>::max_size ();
    const auto all_keys = std::vector {"aa"s, "bb"s, "cc"s, "dd"s, "ee"s};
    const auto values = TestDataValues<BaseType>::values ();
    const auto test_len = test_data.test_duration;

    WHEN ("The IntervalDict are extended willy-nilly into the gaps")
    {
      const auto filled = copy (test_dict).fill_gaps ();
      THEN ("All gaps are filled.")
      {
        // Check interval is the intersection of the different sorts of
        // Interval
        REQUIRE ((copy (filled).extend_into_gaps () - filled)
                 == IDict (ImportData {
                   {"dd", 5, adjust.both ({values[7], values[9]})},
                   {"dd", 6, adjust.both ({values[7], values[9]})},
                   {"dd", 7, adjust.both ({values[7], values[9]})},
                 }));

        // Maximum extension is the default
        REQUIRE (copy (filled).extend_into_gaps (
                   interval_dict::GapExtensionDirection::Both, MAXSZ)
                 == copy (filled).extend_into_gaps (
                   interval_dict::GapExtensionDirection::Both));
        // Only extending by test_len in both directions
        REQUIRE ((copy (filled).extend_into_gaps (
                    interval_dict::GapExtensionDirection::Both, test_len)
                  - filled)
                 == IDict (ImportData {
                   {"dd", 5, adjust.upper ({values[9] - test_len, values[9]})},
                   {"dd", 6, adjust.lower ({values[7], values[7] + test_len})},
                   {"dd", 7, adjust.lower ({values[7], values[7] + test_len})},
                 }));
        // Only extending by 3 forwards
        REQUIRE ((copy (filled).extend_into_gaps (
                    interval_dict::GapExtensionDirection::Forwards, test_len)
                  - filled)
                 == IDict (ImportData {
                   {"dd", 6, adjust.lower ({values[7], values[7] + test_len})},
                   {"dd", 7, adjust.lower ({values[7], values[7] + test_len})},
                 }));
        // Only extending by 3 backwards
        REQUIRE ((copy (filled).extend_into_gaps (
                    interval_dict::GapExtensionDirection::Backwards, test_len)
                  - filled)
                 == IDict (ImportData {
                   {"dd", 5, adjust.upper ({values[9] - test_len, values[9]})},
                 }));
      }
    }
  }
}

TEMPLATE_TEST_CASE (
  "Test gap filling by using another dictionary as a backstop",
  "[fill_gaps_with]",
  boost::icl::interval<int>::type,
  boost::icl::left_open_interval<int>,
  boost::icl::right_open_interval<int>,
  boost::icl::open_interval<int>,
  boost::icl::closed_interval<int>,
//boost::icl::interval<float>::type,
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
  using BaseType = typename Interval::domain_type;
  using Key = std::string;
  using Value = int;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
  static_assert(std::same_as<Interval, typename IDict::IntervalType>);
  using ImportData = std::vector<std::tuple<Key, Value, Interval>>;

  /*
   * TestData
   */
  GIVEN ("An IntervalDict with overlapping intervals")
  {
    TestData<Interval> test_data;
    const IDict test_dict (test_data.initial ());
    const auto adjust = Adjust<Interval> {};
    const auto values = TestDataValues<BaseType>::values ();

    WHEN ("The IntervalDict is supplemented with a backstop")
    {
      const IDict backstop (test_data.fill_gaps_with_data ());
      const auto gap_filled
        = copy (test_dict).fill_gaps_with (backstop) - test_dict;
      THEN ("Extra data is derived from the backstop only during gaps.")
      {
        REQUIRE (gap_filled
                 == IDict (ImportData {
                   {"aa", 25, adjust.both ({values[1], values[8]})},
                   {"ff", 26, {values[0], values[1]}},
                 }));

        // Only fill_gap_with in empty positions
        for (const auto &[key, _, interval] : intervals (gap_filled))
        {
          REQUIRE (test_dict.find (key, interval).empty ());
        }
      }
    }
  }
}
