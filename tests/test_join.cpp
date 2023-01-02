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
/// \file test_icl_join.cpp
/// \brief Test IntervalDict::joined_to()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "print_tuple.h"
#include "test_data.h"
#include "test_utils.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldictail.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/intervaldictitree.h>
#include <interval_dict/ptime.h>
#include <concepts>
#include <vector>

TEMPLATE_TEST_CASE ("Test joining for different interval types",
                    "[joined_to]",
                    boost::icl::interval<int>::type,
                    boost::icl::left_open_interval<int>,
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
  using namespace interval_dict::date_literals;
  using namespace boost::gregorian;
  using Interval = TestType;
  using BaseType = typename Interval::domain_type;
  using Key = std::string;
  using Value = int;
  using Val2 = double;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
  using IDict2 = interval_dict::INTERVALDICTTESTTYPE<Value, Val2, Interval>;
  static_assert(std::same_as<Interval, typename IDict::IntervalType>);
  TestData<Interval> test_data;

  /*
   * TestData
   */
  GIVEN ("An IntervalDict")
  {
    const IDict test_dict (test_data.initial ());
    const IDict2 test_dict2 (test_data.initial2 ());

    const auto joined_dict = test_dict.joined_to (test_dict2);
    WHEN ("we join up the subsets")
    {
      THEN ("you expect the following key values:")
      {
        REQUIRE (intervals_as_key_values (joined_dict)
                 == std::set<std::tuple<std::string, double>> {{"aa"s, 3.1},
                                                               {"aa"s, 4.1},
                                                               {"bb"s, 1.1},
                                                               {"bb"s, 2.1},
                                                               {"bb"s, 5.1},
                                                               {"cc"s, 4.1},
                                                               {"cc"s, 9.1},
                                                               {"dd"s, 2.1},
                                                               {"dd"s, 6.1},
                                                               {"dd"s, 7.1},
                                                               {"dd"s, 8.1}});
      }
    }

    WHEN ("we join up the subsets after filling in gaps")
    {
      const auto joined_after_fill = copy (test_dict).fill_gaps ().joined_to (
        copy (test_dict2).fill_gaps ());

      THEN ("you expect some links unconstrained by the original "
            "intervals.")
      {
        REQUIRE (intervals_as_key_values (joined_after_fill - joined_dict)
                 == std::set<std::tuple<std::string, double>> {{"aa"s, 3.1}});
      }
    }

    WHEN ("we join up the subsets after filling in gaps and to both ends")
    {
      const auto joined_after_fill
        = copy (test_dict)
            .fill_gaps ()
            .fill_to_start ()
            .fill_to_end ()
            .joined_to (copy (test_dict2).fill_gaps ());

      THEN ("you expect more links unconstrained by the original "
            "intervals.")
      {
        const auto diff_dict
          = joined_after_fill
            - copy (joined_dict).fill_to_start ().fill_to_end ();
        REQUIRE (intervals_as_key_values (diff_dict)
                 == std::set<std::tuple<std::string, double>> {{"aa"s, 3.1},
                                                               {"bb"s, 6.1}});
      }
    }

    WHEN ("we join up the subsets after extending neighbouring values into "
          "gaps willy-nilly")
    {
      const auto joined_after_fill
        = copy (test_dict).extend_into_gaps ().joined_to (
          copy (test_dict2).extend_into_gaps ());

      THEN ("you expect even more links unconstrained by the original "
            "intervals.")
      {
        const auto diff_dict = joined_after_fill - joined_dict;
        REQUIRE (intervals_as_key_values (diff_dict)
                 == std::set<std::tuple<std::string, double>> {{"aa"s, 3.1},
                                                               {"aa"s, 4.1},
                                                               {"bb"s, 1.1},
                                                               {"bb"s, 2.1},
                                                               {"bb"s, 5.1},
                                                               {"cc"s, 4.1},
                                                               {"cc"s, 9.1},
                                                               {"dd"s, 2.1},
                                                               {"dd"s, 3.1},
                                                               {"dd"s, 6.1},
                                                               {"dd"s, 7.1},
                                                               {"dd"s, 8.1}});
      }
    }
    WHEN ("we join up the test data in the inverse direction")
    {
      const auto inv_test_dict = test_dict.invert ();
      const auto inv_test_dict2 = test_dict2.invert ();
      const auto inv_joined_dict = inv_test_dict2.joined_to (inv_test_dict);
      THEN ("you expect the same results as the inverse join:")
      {
        REQUIRE (inv_joined_dict == joined_dict.invert ());
      }
    }

    WHEN ("we join up a dictionary with itself inverted")
    {
      const auto joined_after_fill
        = copy (test_dict).extend_into_gaps ().joined_to (
          copy (test_dict2).extend_into_gaps ());

      const auto self_joined1
        = joined_after_fill.joined_to (joined_after_fill.invert ());
      const auto self_joined2
        = joined_after_fill.invert ().joined_to (joined_after_fill);
      THEN ("You get the keys linked by common values in the same "
            "intervals.")
      {
        // Note that self self references are always present by
        // definition
        REQUIRE (
          intervals_as_key_values (self_joined1)
          == std::set<std::tuple<std::string, std::string>> {{"aa"s, "aa"s},
                                                             {"aa"s, "cc"s},
                                                             {"aa"s, "dd"s},

                                                             {"bb"s, "bb"s},

                                                             {"cc"s, "aa"s},
                                                             {"cc"s, "cc"s},

                                                             {"dd"s, "aa"s},
                                                             {"dd"s, "dd"s}});

        std::set<std::tuple<double, double>> expected_self_joined2 {
          {1.1, 1.1}, {1.1, 2.1}, {1.1, 5.1},

          {2.1, 1.1}, {2.1, 2.1}, {2.1, 3.1}, {2.1, 5.1}, {2.1, 6.1},
          {2.1, 7.1}, {2.1, 8.1},

          {3.1, 2.1}, {3.1, 3.1}, {3.1, 4.1}, {3.1, 6.1}, {3.1, 8.1},

          {4.1, 3.1}, {4.1, 4.1}, {4.1, 9.1},

          {5.1, 1.1}, {5.1, 2.1}, {5.1, 5.1},

          {6.1, 2.1}, {6.1, 3.1}, {6.1, 6.1}, {6.1, 7.1}, {6.1, 8.1},

          {7.1, 2.1}, {7.1, 6.1}, {7.1, 7.1}, {7.1, 8.1},

          {8.1, 2.1}, {8.1, 3.1}, {8.1, 6.1}, {8.1, 7.1}, {8.1, 8.1},

          {9.1, 4.1}, {9.1, 9.1}};

        if constexpr (std::is_same_v<Interval,
                                     boost::icl::open_interval<BaseType>>)
        {
          expected_self_joined2.insert (std::tuple {3.1, 7.1});
          expected_self_joined2.insert (std::tuple {7.1, 3.1});
        }
        REQUIRE (intervals_as_key_values (self_joined2)
                 == expected_self_joined2);
      }
    }
  }
}
