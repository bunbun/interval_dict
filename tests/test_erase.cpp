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

TEMPLATE_TEST_CASE ("Test erasing for different interval types",
                    "[erase]",
                    boost::icl::interval<int>::type,
                    boost::icl::left_open_interval<int>,
                    boost::icl::right_open_interval<int>,
                    boost::icl::closed_interval<int>,
                    boost::icl::open_interval<int>,
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
  using Interval = TestType;
  using Key = std::string;
  using Value = int;
  using IDict = interval_dict::INTERVALDICTTESTTYPE<Key, Value, Interval>;
  TestData<Interval> test_data;
  auto import_data = test_data.intervals ();

  /*
   * TestData
   */
  GIVEN ("An IntervalDict with overlapping intervals")
  {
    const IDict test_dict (test_data.initial ());
    const auto interval_max = interval_dict::IntervalTraits<Interval>::maximum ();
    const auto all_keys = std::vector {"aa"s, "bb"s, "cc"s, "dd"s};
    const auto query = test_data.query_interval ();
    const auto empty_query = test_data.empty_interval ();

    // reverse key value order
    std::vector<std::tuple<Value, Key, Interval>> inverse_import_data;
    inverse_import_data.reserve (import_data.size ());
    for (const auto &[key, value, interval] : import_data)
    {
      inverse_import_data.push_back (std::tuple {value, key, interval});
    }

    WHEN ("The IntervalDict is entirely erased using the underlying data")
    {
      THEN ("The resulting dictionary will be empty however the erases "
            "are ordered.")
      {
        for (int i = import_data.size () * 2; i >= 0; --i)
        {
          std::next_permutation (import_data.begin (), import_data.end ());
          REQUIRE (copy (test_dict).erase (import_data) == IDict ());
        }
      }
    }

    WHEN ("The IntervalDict is inversely erased using the underlying data")
    {
      THEN ("The resulting dictionary will be empty however the erases "
            "are ordered.")
      {
        for (int i = import_data.size () * 2; i >= 0; --i)
        {
          std::next_permutation (inverse_import_data.begin (),
                                 inverse_import_data.end ());
          REQUIRE (copy (test_dict).inverse_erase (inverse_import_data)
                   == IDict ());
        }
      }
    }

    WHEN ("The IntervalDict is entirely erased using its own intervals")
    {
      auto test_dict_copy1 = test_dict;
      auto test_dict_copy2 = test_dict;
      for (const auto &[key, value, interval] :
           interval_dict::intervals (test_dict))
      {
        test_dict_copy1.erase ({std::pair {key, value}}, interval);
        test_dict_copy2.inverse_erase ({std::pair {value, key}}, interval);
      }
      THEN ("The resulting dictionary will be empty.")
      {
        REQUIRE (test_dict_copy1 == IDict ());
        REQUIRE (test_dict_copy2 == IDict ());
      }
    }

    WHEN ("The IntervalDict is entirely erased using its own disjoint "
          "intervals")
    {
      auto test_dict_copy1 = test_dict;
      auto test_dict_copy2 = test_dict;
      for (const auto &[key, values, interval] :
           interval_dict::disjoint_intervals (test_dict))
      {
        for (const auto &value : values)
        {
          test_dict_copy1.erase ({std::pair {key, value}}, interval);
          test_dict_copy2.inverse_erase ({std::pair {value, key}}, interval);
        }
      }
      THEN ("The resulting dictionary will be empty.")
      {
        REQUIRE (test_dict_copy1 == IDict ());
        REQUIRE (test_dict_copy2 == IDict ());
      }
    }

    WHEN ("The IntervalDict is erased within an interval")
    {
      const auto erased_dict = copy (test_dict).erase (query);

      THEN ("It will the same as erasing key values en bulk.")
      {
        std::vector<std::pair<Key, Value>> pairs;
        pairs.reserve (import_data.size ());
        for (const auto &[key, value, _] : import_data)
        {
          pairs.push_back (std::pair {key, value});
        }
        REQUIRE (copy (test_dict).erase (pairs, query) == erased_dict);
      }

      THEN ("It will the same as erasing matching intervals manually.")
      {
        auto test_dict_copy1 = test_dict;
        auto test_dict_copy2 = test_dict;
        for (const auto &[key, value, interval] :
             interval_dict::intervals (test_dict))
        {
          if (boost::icl::intersects (interval, query))
          {
            const auto erase_interval = query & interval;
            test_dict_copy1.erase ({std::pair {key, value}}, erase_interval);
            test_dict_copy2.erase ({std::pair {key, value}},
                                   boost::icl::lower (erase_interval),
                                   boost::icl::upper (erase_interval));
          }
        }
        REQUIRE (test_dict_copy1 == test_dict_copy2);
        REQUIRE (test_dict_copy1 == erased_dict);
        REQUIRE (test_dict_copy2
                 == copy (test_dict).erase (boost::icl::lower (query),
                                            boost::icl::upper (query)));
      }
      THEN ("The interval will be empty")
      {
        // Check some values have been removed from the interval
        REQUIRE (erased_dict.find (all_keys, query)
                 != test_dict.find (all_keys, query));
        // Check all values have been removed from the interval
        REQUIRE (erased_dict.find (all_keys, query).empty ());
      }
      THEN ("Adding back data for the interval will restore the dictionary")
      {
        auto restored_dict = erased_dict;
        for (const auto &[key, value, interval] :
             interval_dict::intervals (test_dict))
        {
          if (boost::icl::intersects (interval, query))
          {
            restored_dict.insert ({std::pair {key, value}}, query & interval);
          }
        }
        REQUIRE (test_dict == restored_dict);
      }
    }

    WHEN ("An empty interval is erased")
    {
      auto test_dict_copy1 = test_dict;
      auto test_dict_copy2 = test_dict;
      for (const auto &[key, value, _] : interval_dict::intervals (test_dict))
      {
        test_dict_copy1.erase ({std::pair {key, value}}, empty_query);
        test_dict_copy2.inverse_erase ({std::pair {value, key}}, empty_query);
      }
      THEN ("Nothing changes")
      {
        // erase(vector<pair<key,value>>, empty_interval)
        // inverse_erase(vector<pair<value, value>>, empty_interval)
        REQUIRE (test_dict_copy1 == test_dict);
        REQUIRE (test_dict_copy2 == test_dict);

        // Empty query
        REQUIRE (copy (test_dict).erase ("aa", empty_query) == test_dict);
        REQUIRE (copy (test_dict).erase (empty_query) == test_dict);

        // query intersecting nothing
        REQUIRE (copy (test_dict).erase ("aa", interval_max) == test_dict);
        REQUIRE (copy (test_dict).erase (interval_max) == test_dict);
      }
    }

    WHEN ("An unknown key is erased")
    {
      THEN ("Nothing changes")
      {
        REQUIRE (copy (test_dict).erase ("zz", query) == test_dict);
        REQUIRE (copy (test_dict).erase (
                   "zz", boost::icl::lower (query), boost::icl::upper (query))
                 == test_dict);
      }
    }

    WHEN ("Unknown values are erased")
    {
      auto test_dict_copy1 = test_dict;
      auto test_dict_copy2 = test_dict;
      for (const auto &[key, _, interval] :
           interval_dict::intervals (test_dict))
      {
        test_dict_copy1.erase ({std::pair {key, -1}}, interval);
        test_dict_copy2.inverse_erase ({std::pair {-1, key}}, interval);
      }
      THEN ("Nothing changes")
      {
        REQUIRE (test_dict_copy1 == test_dict);
        REQUIRE (test_dict_copy2 == test_dict);
      }
    }

    WHEN ("Single key are erased over an interval")
    {
      THEN ("It will the same as erasing matching intervals manually.")
      {
        for (const auto &erase_key : std::vector {"bb"s, "cc"s, "dd"s})
        {
          auto test_dict_copy = test_dict;

          for (const auto &[key, value, interval] :
               interval_dict::intervals (test_dict))
          {
            if (key == erase_key && boost::icl::intersects (interval, query))
            {
              const auto erase_interval = query & interval;
              test_dict_copy.erase ({std::pair {key, value}}, erase_interval);
            }
          }
          REQUIRE (copy (test_dict).erase (erase_key, query) == test_dict_copy);
          REQUIRE (copy (test_dict).erase (erase_key,
                                           boost::icl::lower (query),
                                           boost::icl::upper (query))
                   == test_dict_copy);
        }
      }
      THEN ("Erasing all keys in turn will the same as erasing them all "
            "at once.")
      {
        auto test_dict_erase = test_dict;
        for (const auto &key : all_keys)
        {
          test_dict_erase.erase (key, query);
        }
        // Check that something has been erased and it is the right
        // thing that has been erased
        REQUIRE (test_dict_erase != test_dict);
        REQUIRE (test_dict_erase == copy (test_dict).erase (query));
      }
    }
  }
}
