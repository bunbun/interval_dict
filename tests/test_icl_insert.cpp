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
#include "test_icl.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/ptime.h>
#include <interval_dict/intervaldicticl.h>
#include <vector>

TEMPLATE_TEST_CASE("Test inserting for different interval types"
, "[erase]"
, boost::icl::interval<int>::type
, boost::icl::left_open_interval<int>
, boost::icl::right_open_interval<int>
, boost::icl::closed_interval<int>
, boost::icl::open_interval<int>
, boost::icl::interval<float>::type
, boost::icl::left_open_interval<float>
, boost::icl::right_open_interval<float>
, boost::icl::interval<boost::posix_time::ptime>::type
, boost::icl::left_open_interval<boost::posix_time::ptime>
, boost::icl::right_open_interval<boost::posix_time::ptime>
, boost::icl::open_interval<boost::posix_time::ptime>
, boost::icl::closed_interval<boost::posix_time::ptime>
, boost::icl::discrete_interval<boost::posix_time::ptime>
, boost::icl::interval<boost::gregorian::date>::type
, boost::icl::left_open_interval<boost::gregorian::date>
, boost::icl::right_open_interval<boost::gregorian::date>
, boost::icl::open_interval<boost::gregorian::date>
, boost::icl::closed_interval<boost::gregorian::date>
, boost::icl::discrete_interval<boost::gregorian::date>
                  )
{
    using namespace std::string_literals;
    using namespace interval_dict::date_literals;
    using namespace boost::gregorian;
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    using Key = std::string;
    using Val = int;
    using IDict = interval_dict::IntervalDictICLExp<Key, Val, Interval>;
    using Interval = typename IDict::Interval;
    using Impl = typename IDict::ImplType;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;
    TestData<Val, Interval> test_data;
    auto import_data = test_data.intervals();

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        using namespace boost::gregorian;
        const IDict test_dict(test_data.initial());
        const auto adjust = Adjust<Interval>{};
        const auto interval_min = interval_dict::IntervalTraits<Interval>::lowest();
        const auto interval_max = interval_dict::IntervalTraits<Interval>::max();
        const auto interval_maxsz =
            interval_dict::IntervalTraits<Interval>::max_size();
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};
        const auto query = test_data.query_interval();
        auto query_end =
            Interval{boost::icl::upper(query), boost::icl::upper(query)};

        if (!boost::icl::is_empty(query_end))
        {
            auto u = boost::icl::upper(query);
            using interval_dict::operator--;
            query_end =Interval{boost::icl::upper(query), --u};
            REQUIRE(boost::icl::is_empty(query_end));
        }
        const auto query_max = Interval{interval_max, interval_max};

        // reverse key value order
        std::vector<std::tuple<Val, Key, Interval>> inverse_import_data;
        inverse_import_data.reserve(import_data.size());
        for (const auto& [key, value, interval]: import_data)
        {
            inverse_import_data.push_back(std::tuple{value, key, interval});
        }

        WHEN("Inserting data in permuted order")
        {
            IDict test1;
            IDict test2;
            IDict test3;
            IDict test4;
            IDict test5;
            IDict test6;
            THEN("Should always give the same result.")
            {
                for (int i = import_data.size() * 2; i >= 0; --i)
                {
                    std::next_permutation(import_data.begin(), import_data.end());
                    std::next_permutation(inverse_import_data.begin(), inverse_import_data.end());

                    // Import whole scale
                    test1.insert(import_data);
                    test2.inverse_insert(inverse_import_data);

                    for (const auto& [key, value, interval]: import_data)
                    {
                        test3.insert(std::vector{std::pair{key, value}}, interval);
                        test4.insert(std::vector{std::pair{key, value}},
                                     boost::icl::lower(interval),
                                     boost::icl::upper(interval));
                        test5.inverse_insert(std::vector{std::pair{value, key}}, interval);
                        test6.inverse_insert(std::vector{std::pair{value, key}},
                                             boost::icl::lower(interval),
                                             boost::icl::upper(interval));
                    }
                    REQUIRE(test1 == test_dict);
                    REQUIRE(test2 == test_dict);
                    REQUIRE(test3 == test_dict);
                    REQUIRE(test4 == test_dict);
                    REQUIRE(test5 == test_dict);
                    REQUIRE(test6 == test_dict);
                }
            }
        }

        // get all pairs irrespective of intervals
        const auto pairs
            = [&]() -> std::vector<std::pair<Key, Val>>
            {
                std::set<std::pair<Key, Val>> pairs;
                for (const auto& [key, value, _]: import_data)
                {
                    pairs.insert(std::pair{key, value});
                }
                return {pairs.begin(), pairs.end()};
            }();
        const auto inverse_pairs
            = [&]() -> std::vector<std::pair<Val, Key>>
            {
                std::set<std::pair<Val, Key>> pairs;
                for (const auto& [key, value, _]: import_data)
                {
                    pairs.insert(std::pair{value, key});
                }
                return {pairs.begin(), pairs.end()};
            }();

        WHEN("Inserting data for the same data in forward and inverse for a single interval")
        {
            IDict test1;
            test1.insert(pairs, query);
            IDict test2;
            test2.inverse_insert(inverse_pairs, query);
            REQUIRE(test1.size() > 0);
            REQUIRE(test1.keys()  == all_keys);
            REQUIRE(test1 == test2);
        }

        WHEN("Inserting intervals should have no effect")
        {
            IDict test1;
            test1.insert(pairs, query_end);
            IDict test2;
            test2.inverse_insert(inverse_pairs, query_end);
            REQUIRE(test1 == IDict());
            REQUIRE(test2 == IDict());
        }
    }
}