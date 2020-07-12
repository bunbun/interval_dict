#ifndef TESTS_TEST_ICL_H
#define TESTS_TEST_ICL_H

template <typename Key, typename Val, typename Interval, typename Impl>
interval_dict::IntervalDictExp<Key, Val, Interval, Impl>
copy(interval_dict::IntervalDictExp<Key, Val, Interval, Impl> data)
{
    return data;
}

template <typename Key, typename Val, typename Interval, typename Impl>
interval_dict::IntervalDictExp<Key, Val, Interval, Impl> test_data1()
{
    using namespace std::string_literals;
    using ImportData = std::vector<std::tuple<Key, Val, Interval>>;
    return ImportData{
        // aa-0 gap
        {"aa"s, 0, Interval{0, 5}},
        {"aa"s, 0, Interval{40, 85}},

        // Gap of bb-1 {15-20}
        {"bb"s, 1, Interval{5, 15}},
        {"bb"s, 1, Interval{20, 25}},
        {"bb"s, 2, Interval{10, 30}},

        // overlap ignored
        {"cc"s, 3, Interval{15, 35}},
        {"cc"s, 3, Interval{25, 55}},

        // dd-6/7 over {30-35}
        {"dd"s, 5, Interval{20, 30}},
        {"dd"s, 6, Interval{30, 35}},
        {"dd"s, 7, Interval{30, 35}},
        {"dd"s, 5, Interval{45, 75}},
    };
}


#endif //TESTS_TEST_ICL_H
