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
/// \file test_icl_flatten.cpp
/// \brief Test IntervalDict::flattened()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "test_data.h"
#include "test_utils.h"
#include <interval_dict/intervaldictail.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/intervaldictitree.h>
#include <concepts>
#include <vector>

TEMPLATE_TEST_CASE ("Test flattening for different interval types",
                    "[flattened]",
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
  using ImportData = std::vector<std::tuple<Key, Value, Interval>>;
  static_assert(std::same_as<Interval, typename IDict::IntervalType>);

  GIVEN ("An IntervalDict with overlapping intervals")
  {
    TestData<Interval> test_data;
    IDict test_dict (test_data.initial ());
    const auto adjust = Adjust<Interval> {};
    const auto values = TestDataValues<BaseType>::values ();
    const auto midvalues = TestDataValues<BaseType>::midvalues ();

    WHEN ("Should be equivalent to inserting resolved data")
    {
      REQUIRE (test_dict == IDict (test_data.intervals ()));
      REQUIRE (test_dict == IDict ().insert (test_data.intervals ()));
    }
    test_dict = test_dict.fill_gaps ();

    WHEN ("The IntervalDict is flattened by preferring the status quo")
    {
      THEN ("Many cases of 1:m can be resolved.")
      {
        IDict expected {ImportData {
          // keep status_quo
          {"bb", 2, {values[4], values[5]}},
          // no status_quo: discard
          {"dd", 6, adjust.lower ({values[6], values[7]})},
          {"dd", 7, adjust.lower ({values[6], values[7]})},
        }};
        // Resolved cases
        REQUIRE (test_dict - flattened (test_dict) == expected);

        // Resolved the same way even when non-flat region is not
        // contiguous with the flat one.
        REQUIRE (test_dict
                   - flattened (copy (test_dict).erase (
                     "dd"s, adjust.lower (Interval {values[6], midvalues[6]})))
                 == expected);
      }
    }

    WHEN ("The IntervalDict is flattened by deletions")
    {
      const auto flattened_preferring_status_quo = flattened (test_dict);
      const auto flattened_discard
        = flattened (test_dict, interval_dict::flatten_policy_discard ());
      THEN ("Fewer cases of 1:m can be resolved.")
      {
        // everything is discarded
        REQUIRE (flattened_preferring_status_quo - flattened_discard
                 == IDict (ImportData {
                   {"bb", 1, {values[4], values[5]}},
                 }));
      }
    }

    auto custom_policy
      = [] (const std::optional<Value> &status_quo,
            Interval,
            const Key &key,
            const std::vector<Value> &values) -> const std::optional<Value>
    {
      if (status_quo.has_value ()
          && std::find (values.begin (), values.end (), status_quo.value ())
               != values.end ())
      {
        return status_quo.value () + 10;
      }
      else
      {
        assert (key == "dd");
        return 42;
      }
    };

    WHEN ("The IntervalDict is flattened by a custom function")
    {
      using namespace interval_dict;
      const auto flattened_custom = flattened (test_dict, custom_policy);
      const auto flattened_discard
        = flattened (test_dict, interval_dict::flatten_policy_discard ());
      THEN ("More cases of 1:m can be resolved in curious ways.")
      {
        REQUIRE (flattened_custom - flattened_discard
                 == IDict (ImportData {
                   {"bb", 11, {values[4], values[5]}},
                   {"dd", 42, adjust.lower ({values[6], values[7]})},
                 }));
      }
    }

    WHEN ("The IntervalDict is flattened by prefering status quo then a "
          "custom function")
    {
      using namespace interval_dict;
      const auto flattened_custom = flattened (
        test_dict,
        interval_dict::flatten_policy_prefer_status_quo (custom_policy));
      const auto flattened_discard
        = flattened (test_dict, interval_dict::flatten_policy_discard ());
      THEN ("More not 'status quo' cases can still be resolved.")
      {
        REQUIRE (flattened_custom - flattened_discard
                 == IDict (ImportData {
                   // Note this is resolved by keeping the status quo
                   // so the value is 1 not 11
                   {"bb", 1, {values[4], values[5]}},
                   // this is resolved via our custom function
                   {"dd", 42, adjust.lower ({values[6], values[7]})},
                 }));
      }
    }
  }
}
