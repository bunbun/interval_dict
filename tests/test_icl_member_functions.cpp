//
// Created by lg on 10/07/2020.
//
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
/// \file test_icl_member_functions.cpp
/// \brief Test IntervalDict memeber functions
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#include "catch.hpp"
#include "test_data.h"
#include "test_icl.h"
#include <interval_dict/gregorian.h>
#include <interval_dict/intervaldicticl.h>
#include <interval_dict/ptime.h>
#include <vector>

TEMPLATE_TEST_CASE(
    "Test member functions for different interval types",
    "[memberfunc]",
    boost::icl::interval<int>::type,
    boost::icl::left_open_interval<int>
    // , boost::icl::right_open_interval<int>
    // , boost::icl::closed_interval<int>
    // , boost::icl::open_interval<int>
    // , boost::icl::interval<float>::type
    // , boost::icl::left_open_interval<float>
    // , boost::icl::right_open_interval<float>
    // , boost::icl::interval<boost::posix_time::ptime>::type
    // , boost::icl::left_open_interval<boost::posix_time::ptime>
    // , boost::icl::right_open_interval<boost::posix_time::ptime>
    // , boost::icl::open_interval<boost::posix_time::ptime>
    // , boost::icl::closed_interval<boost::posix_time::ptime>
    // , boost::icl::discrete_interval<boost::posix_time::ptime>
    // , boost::icl::interval<boost::gregorian::date>::type
    // , boost::icl::left_open_interval<boost::gregorian::date>
    // , boost::icl::right_open_interval<boost::gregorian::date>
    // , boost::icl::open_interval<boost::gregorian::date>
    // , boost::icl::closed_interval<boost::gregorian::date>
    // , boost::icl::discrete_interval<boost::gregorian::date>
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
        const auto interval_min =
            interval_dict::IntervalTraits<Interval>::lowest();
        const auto interval_max =
            interval_dict::IntervalTraits<Interval>::max();
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
            query_end = Interval{boost::icl::upper(query), --u};
            REQUIRE(boost::icl::is_empty(query_end));
        }
        const auto query_max = Interval{interval_max, interval_max};

        // reverse key value order
        std::vector<std::tuple<Val, Key, Interval>> inverse_import_data;
        inverse_import_data.reserve(import_data.size());
        for (const auto& [key, value, interval] : import_data)
        {
            inverse_import_data.push_back(std::tuple{value, key, interval});
        }

        WHEN("Inserting data in permuted order")
        {
            THEN("Should always give the same result.")
            {
            }
        }
    }
}

// /*joined_to*/
// template <typename OtherVal>
// [[nodiscard]] IntervalDictExp<Key, OtherVal, Interval, Impl>
// joined_to(
//     const IntervalDictExp<Val, OtherVal, Interval, Impl>& b_to_c) const;
//

// [[nodiscard]] std::vector<Key> keys() const;
// [[nodiscard]] bool empty() const;
// [[nodiscard]] std::size_t count(const Key& key) const;
// [[nodiscard]] bool contains(const Key& key) const;
// void clear();
// /*Invert*/
// [[nodiscard]] IntervalDictExp<Val, Key, Interval, Impl>
// invert() const;
// /*size*/
// [[nodiscard]] std::size_t size() const;
