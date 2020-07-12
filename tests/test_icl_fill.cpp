#include "catch.hpp"
#include <interval_dict/intervaldicticl.h>
#include "test_icl.h"
#include <vector>

// N.B. Continuous types like float have
// no sensible semantics for adjacent intervals
// when open/closed coordinates used so can't be used
// in interval maps
//boost::icl::closed_interval<float>
//boost::icl::open_interval<float>

TEMPLATE_TEST_CASE("Test one way ints gap filling for different interval types",
                   "[fill_to_start][fill_to_end][fill_gaps][extend_into_gaps]",
                   boost::icl::interval<int>::type,
                   boost::icl::left_open_interval<int>,
                   boost::icl::right_open_interval<int>,
                   boost::icl::open_interval<int>,
                   boost::icl::closed_interval<int>,
                   boost::icl::interval<float>::type,
                   boost::icl::left_open_interval<float>,
                   boost::icl::right_open_interval<float>
                   )
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

    /*
     * TestData
     */
    GIVEN("An IntervalDict with overlapping intervals")
    {
        const auto test_dict = test_data1<Key, Val, Interval, Impl>();
        const auto all_keys = std::vector{"aa"s, "bb"s, "cc"s, "dd"s};

        WHEN("The IntervalDict is filled to start")
        {
            THEN("Some keys are extended to the start of time.")
            {
                const Interval check_interval{
                    std::numeric_limits<BaseType>::lowest(), 5};
                // Start extending only for keys whose first interval is AFTER
                // 10
                REQUIRE(copy(test_dict).fill_to_start(10).subset(
                    all_keys, check_interval) ==
                        IDict(ImportData{
                            {"aa", 0, check_interval},
                            {"bb", 1, check_interval},
                        }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict)
                        .fill_to_start(
                            17,
                            interval_dict::IntervalTraits<Interval>::max_size())
                        .subset(all_keys, check_interval) ==
                    IDict(ImportData{
                        {"aa", 0, check_interval},
                        {"bb", 1, check_interval},
                        {"cc", 3, check_interval},
                    }));
                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_start(10, 30).subset(
                    all_keys, check_interval) ==
                        IDict(ImportData{
                            {"aa", 0, Interval{-30, 5}},
                            {"bb", 1, Interval{-25, 5}},
                        }));
            }
        }

        WHEN("The IntervalDict is filled to end")
        {
            THEN("Some keys are extended to the end of time.")
            {
                const Interval check_interval{
                    60, std::numeric_limits<BaseType>::max()};
                // Start extending only for keys whose first interval is AFTER
                // 10
                REQUIRE(copy(test_dict).fill_to_end(50).subset(all_keys,
                                                               check_interval)

                        == IDict(ImportData{
                    {"aa", 0, check_interval},
                    {"cc", 3, check_interval},
                    {"dd", 5, check_interval},
                }));
                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_to_end(
                        50,
                        interval_dict::IntervalTraits<Interval>::max_size()) -
                    copy(test_dict).fill_to_end(50) ==
                    IDict());
                // Only extending by 30
                REQUIRE(copy(test_dict).fill_to_end(50, 30).subset(
                    all_keys, check_interval) ==
                        IDict(ImportData{
                            {"aa", 0, Interval{60, 115}},
                            {"cc", 3, Interval{60, 85}},
                            {"dd", 5, Interval{60, 105}},
                        }));
            }
        }

        WHEN("The IntervalDict gaps are filled safely")
        {
            THEN("All gaps of interrupted associations are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                const Interval check_interval{6, 39};
                REQUIRE((copy(test_dict).fill_gaps() - test_dict)
                            .subset(all_keys, check_interval) ==
                        IDict(ImportData{
                            {"aa", 0, check_interval},
                        }));

                // Maximum extension is the default
                REQUIRE(
                    copy(test_dict).fill_gaps(
                        interval_dict::IntervalTraits<Interval>::max_size()) -
                    copy(test_dict).fill_gaps() ==
                    IDict());
                // Only extending by 3
                REQUIRE((copy(test_dict).fill_gaps(3) - test_dict)
                            .subset(all_keys, check_interval) ==
                        IDict(ImportData{
                            {"aa", 0, {6, 8}},
                            {"aa", 0, {37, 39}},
                        }));
            }
        }

        WHEN("The IntervalDict are extended willy-nilly into the gaps")
        {
            const auto filled = copy(test_dict).fill_gaps();
            THEN("All gaps are filled.")
            {
                // Check interval is the intersection of the different sorts of
                // Interval
                const Interval check_interval{36, 44};
                REQUIRE((copy(filled).extend_into_gaps() - filled)
                            .subset(all_keys, check_interval)
                        == IDict(ImportData{
                    {"dd", 5, {36,44}},
                    {"dd", 6, {36,44}},
                    {"dd", 7, {36,44}},
                }));

                // Maximum extension is the default
                REQUIRE(
                    copy(filled).extend_into_gaps(interval_dict::GapExtensionDirection::Both,
                                                  interval_dict::IntervalTraits<Interval>::max_size()) -
                    copy(filled).extend_into_gaps(interval_dict::GapExtensionDirection::Both) ==
                    IDict());
                // Only extending by 3 in both directions
                REQUIRE((copy(filled).extend_into_gaps
                                         (interval_dict::GapExtensionDirection::Both,3) - filled)
                            .subset(all_keys, check_interval)
                        == IDict(ImportData{
                    {"dd", 5, {42,44}},
                    {"dd", 6, {36,38}},
                    {"dd", 7, {36,38}},
                }));
                // Only extending by 3 forwards
                REQUIRE((copy(filled).extend_into_gaps
                                         (interval_dict::GapExtensionDirection::Forwards,3) - filled)
                            .subset(all_keys, check_interval)
                        == IDict(ImportData{
                    {"dd", 6, {36,38}},
                    {"dd", 7, {36,38}},
                }));
                // Only extending by 3 backwards
                REQUIRE((copy(filled).extend_into_gaps
                                         (interval_dict::GapExtensionDirection::Backwards,3) - filled)
                            .subset(all_keys, check_interval)
                        == IDict(ImportData{
                    {"dd", 5, {42,44}},
                }));
            }
        }
    }
}

