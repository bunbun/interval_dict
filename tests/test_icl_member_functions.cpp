//
// Created by lg on 10/07/2020.
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



// /*Subset*/
// template <typename KeyRange>
// [[nodiscard]] IntervalDictExp<Key, Val, Interval, Impl>
// subset(const KeyRange& keys, Interval interval) const;
//
// template <typename KeyRange, typename ValRange>
// [[nodiscard]] IntervalDictExp<Key, Val, Interval, Impl>
// subset(const KeyRange& keys, const ValRange& values, Interval interval) const;
//
// /*joined_to*/
// template <typename OtherVal>
// [[nodiscard]] IntervalDictExp<Key, OtherVal, Interval, Impl>
// joined_to(
//     const IntervalDictExp<Val, OtherVal, Interval, Impl>& b_to_c) const;
//
// /*
//  * operator -= +=
//  */
// IntervalDictExp<Key, Val, Interval, Impl>&
// operator-=(const IntervalDictExp<Key, Val, Interval, Impl>& other);
// IntervalDictExp<Key, Val, Interval, Impl>&
// operator+=(const IntervalDictExp<Key, Val, Interval, Impl>& other);
//
//
//
//
//
// namespace interval_dict
// {
//
// /*subtract*/
// template <typename Key, typename Val, typename Interval, typename Impl>
// IntervalDictExp<Key, Val, Interval, Impl>
// subtract(const IntervalDictExp<Key, Val, Interval, Impl>& dict_a,
//          const IntervalDictExp<Key, Val, Interval, Impl>& dict_b);
//
// /*
//  * merge
//  */
// template <typename Key, typename Val, typename Interval, typename Impl>
// IntervalDictExp<Key, Val, Interval, Impl>
// merge(const IntervalDictExp<Key, Val, Interval, Impl>& dict_a,
//       const IntervalDictExp<Key, Val, Interval, Impl>& dict_b);
//
//
//
// /*
//  * operators
//  */
// template <typename Key, typename Val, typename Interval, typename Impl>
// IntervalDictExp<Key, Val, Interval, Impl>
// operator-(IntervalDictExp<Key, Val, Interval, Impl> dict1,
//           const IntervalDictExp<Key, Val, Interval, Impl>& dict2);
//
// template <typename Key, typename Val, typename Interval, typename Impl>
// IntervalDictExp<Key, Val, Interval, Impl>
// operator+(IntervalDictExp<Key, Val, Interval, Impl> dict1,
//           const IntervalDictExp<Key, Val, Interval, Impl>& dict2);
//
// /*
//  * subtract
//  */
// template <typename Key, typename Val, typename Interval, typename Impl>
// IntervalDictExp<Key, Val, Interval, Impl>
// subtract(IntervalDictExp<Key, Val, Interval, Impl> dict1,
//          const IntervalDictExp<Key, Val, Interval, Impl>& dict2);
//
// /*
//  * merge
//  */
// template <typename Key, typename Val, typename Interval, typename Impl>
// IntervalDictExp<Key, Val, Interval, Impl>
// merge(IntervalDictExp<Key, Val, Interval, Impl> dict1,
//       const IntervalDictExp<Key, Val, Interval, Impl>& dict2);
//
//
//
// } // namespace interval_dict
//
//


// TEST_CASE( "Test one way float", "[float]" )
// {
// //     using FDict = interval_dict::IntervalDictICLExp<std::string, int,
// //         boost::icl::closed_interval<float>>;
// //         boost::icl::left_open_interval<float>>;
// //         boost::icl::right_open_interval<float>>;
// //         boost::icl::interval<float>::type>;
// //         boost::icl::open_interval<float>>;
//
//     // FDict test_dict_float;
//     using namespace std::string_literals;
//     using FDict = interval_dict::IntervalDictICL<std::string, int, float>;
//     using Interval = FDict::Interval;
//
//     using BaseType = float;
//     using KeyType = std::string;
//     using ValueType = int;
//
//     /*
//      * TestData
//      */
//     std::vector<std::tuple<std::string, int, Interval>>
//         test_vec{{"aa"s, 0, Interval{0.0, 5.0}},
//                  {"bb"s, 1, Interval{5.0, 15.0}},
//                  {"bb"s, 2, Interval{10.0, 25.0}},
//                  {"cc"s, 3, Interval{15.0, 35.0}},
//                  {"dd"s, 4, Interval{20.0, 45.0}},
//                  {"ee"s, 5, Interval{25.0, 55.0}},
//                  {"ff"s, 6, Interval{30.0, 65.0}},
//                  {"gg"s, 7, Interval{35.0, 75.0}},
//                  {"hh"s, 8, Interval{40.0, 85.0}},
//     };
//
//     FDict test_dict_float;
//     test_dict_float.insert(test_vec);
//     {
//         std::vector<std::string> test_keys{"aa"s, "bb"s, "cc"s, "dd"s};
//         auto vec_intervals = interval_dict::intervals(test_dict_float,
//                                                       test_keys,
//                                                       FDict::Interval{5.5,
//                                                       100.0});
//         for (const auto&[key, value, interval]: vec_intervals)
//         {
//             std::cout << key << "\t"\
//                       << value << "\t"
//                       << interval << "\n";
//         }
//         std::cout << "Before fill:\n" << test_dict_float << "\n";
//         test_dict_float.fill_to_end(6.0, 30.0).fill_to_start(6.0, 30.0);
//         std::cout << "After fill:\n" << test_dict_float << "\n";
//     }
//     auto test_dict_float2 = test_dict_float;
//     std::cout << "Empty:\n" << (test_dict_float2 - test_dict_float) << "\n";
//     test_dict_float2 -= test_dict_float;
//     std::cout << "Empty:\n" << test_dict_float2 << "\n";
//     test_dict_float.fill_gaps_with(test_dict_float2);
//     test_dict_float = test_dict_float.subset(std::vector{"bb"s, "cc"s},
//     FDict::Interval{5.0, 7.0});
// }
