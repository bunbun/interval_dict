#include "catch.hpp"
#include <interval_dict/intervaldicticl.h>
#include "test_icl.h"
#include <vector>

/*
 * TODO
 * custom flatten policy
 */
// template <typename Key, typename Val, typename Interval, typename Impl>
// IntervalDictExp<Key, Val, Interval, Impl>
// flattened(IntervalDictExp<Key, Val, Interval, Impl> interval_dict,
//           FlattenPolicy<Key, Val, Interval> keep_one_value =
//           flatten_policy_prefer_status_quo<Key, Val, Interval>());

TEMPLATE_TEST_CASE("Test one way ints flattening for different interval types",
                   "[flattened]",
                   boost::icl::closed_interval<int>,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::interval<int>::type,
                   boost::icl::open_interval<int>)
{
    using namespace std::string_literals;
    using Interval = TestType;
    using BaseType = typename Interval::domain_type;
    using Key = std::string;
    using Val = int;
    using IDict = interval_dict::IntervalDictICLExp<Key, Val, Interval>;
    using Interval = typename IDict::Interval;
    using Impl = typename IDict::ImplType;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;

    GIVEN("An IntervalDict with overlapping intervals")
    {
        auto test_dict = test_data1<Key, Val, Interval, Impl>();

        WHEN("Should be equivalent to inserting resolved data")
        {
            ImportData expected_data{
                {"aa", 0, Interval{0, 5}},
                {"aa", 0, Interval{40, 85}},
                {"bb", 1, Interval{5, 10}},
                {"bb", 1, Interval{10, 15}},
                {"bb", 2, Interval{10, 15}},
                {"bb", 2, Interval{15, 20}},
                {"bb", 1, Interval{20, 25}},
                {"bb", 2, Interval{20, 25}},
                {"bb", 2, Interval{25, 30}},
                {"cc", 3, Interval{15, 55}},
                {"dd", 5, Interval{20, 30}},
                {"dd", 6, Interval{30, 35}},
                {"dd", 7, Interval{30, 35}},
                {"dd", 5, Interval{45, 75}},
            };
            REQUIRE(test_dict == IDict(expected_data));
            REQUIRE(test_dict == IDict().insert(expected_data));
        }

        WHEN("The IntervalDict is flattened by preferring the status quo")
        {
            THEN("Many cases of 1:m can be resolved.")
            {
                // Resolved cases
                REQUIRE(test_dict - flattened(test_dict) ==
                        IDict(ImportData{
                            // keep status_quo
                            {"bb", 2, Interval{10, 15}},
                            {"bb", 1, Interval{20, 25}},
                            // no status_quo: discard
                            {"dd", 6, Interval{30, 35}},
                            {"dd", 7, Interval{30, 35}},
                        }));
            }
        }

        WHEN("The IntervalDict is flattened by deletions")
        {
            const auto flattened_preferring_status_quo = flattened(test_dict);
            const auto flattened_discard = flattened(
                test_dict,
                interval_dict::flatten_policy_discard<Key, Val, Interval>());
            THEN("Fewer cases of 1:m can be resolved.")
            {
                // everything is discarded
                REQUIRE(flattened_preferring_status_quo - flattened_discard ==
                        IDict(ImportData{
                            {"bb", 1, Interval{10, 15}},
                            {"bb", 2, Interval{20, 25}},
                        }));
            }
        }
    }

}
