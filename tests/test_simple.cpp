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
/// \file test_icl_simple.cpp
/// \brief Test IntervalDict::fill_xxxx() and extend_into_gaps()
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk
#include "catch.hpp"


// ValueIntervalRef
// ValueInterval
#include <value_interval.h>

#include <std_ranges_23_patch.h>

#include <limits>

// IntervalTraits<Interval> for minimum() maximum() max_size()
//                              BaseDifferenceType, BaseType
// KeyValueInterval
// KeyValuesDisjointInterval
// ValuesDisjointInterval
// SandwichedGap / SandwichedGap
#include <interval_traits.h>

// Overload of IntervalTraits struct for ICL Posix Time and Gregorian Date
// and increment/decrement operators to allow these types to be used
// with interval_dict

#include <ptime.h>
#include <gregorian.h>

#include <boost/icl/interval.hpp>

#include <map>
#include <vector>
#include <ranges>
#include <iostream>
#include <concepts>

// N.B. Continuous types like float have
// no sensible semantics for adjacent intervals
// when open/closed coordinates used so can't be used
// in interval maps
// boost::icl::open_interval<float>
// boost::icl::closed_interval<float>
// boost::icl::interval<float>::type,
using namespace interval_dict;

TEMPLATE_TEST_CASE ("Test simple cases",
                    "[simple_test]",
                    boost::icl::interval<int>::type,
                    boost::icl::left_open_interval<int>,
                    boost::icl::right_open_interval<int>,
                    boost::icl::closed_interval<int>,
                    boost::icl::open_interval<int>,
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
                    boost::icl::discrete_interval<boost::gregorian::date>
                      )
{
  using Interval = TestType;
//  using Key = std::string;
  using Value = int;
  using namespace std::string_literals;

  GIVEN ("Type = "s + typeid(Interval).name())
  {
    const ValueInterval<Value, Interval> vi{3,
                                       {IntervalTraits<Interval>::minimum(),
                                        IntervalTraits<Interval>::maximum()}};
    REQUIRE(vi.interval == interval_extent<Interval>);
    REQUIRE(vi.value == 3);
  }

}

TEST_CASE ("Test to_set / to_vector",
           "[legacy_ranges_conversion]")
{
  using namespace legacy_ranges_conversion;
  using namespace std::string_literals;
  namespace ranges = std::ranges;
  namespace views = std::ranges::views;
  {
    auto iota_range = views::iota(4,1000)
                      | views::filter([](int x) {return (x % 23) == 0; })
                      | views::reverse
                      | views::transform([](int x){return std::to_string(x);})
      ;

    REQUIRE((iota_range| views::take(7) | to_vector())
              == std::vector{"989"s, "966"s, "943"s, "920"s,
                             "897"s, "874"s, "851"s});

    REQUIRE((iota_range| views::take(7) | to_set())
              == std::set{"989"s, "966"s, "943"s, "920"s,
                          "897"s, "874"s, "851"s});

    REQUIRE((iota_range| views::take(7) | to_unordered_set())
              == std::unordered_set{"989"s, "966"s, "943"s, "920"s,
                                    "897"s, "874"s, "851"s});
  }}
