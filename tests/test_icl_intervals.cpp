// /*
//  * intervals
//  */
// template <typename Key, typename Val, typename Interval, typename Impl>
// cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
// intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
//           std::vector<Key> keys,
//           Interval query_interval = interval_extent<Interval>);
//
// template <typename Key, typename Val, typename Interval, typename Impl>
// cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
// intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
//           const Key& key,
//           Interval query_interval = interval_extent<Interval>);
//
// template <typename Key, typename Val, typename Interval, typename Impl>
// cppcoro::generator<std::tuple<const Key&, const Val&, Interval>>
// intervals(const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
//           Interval query_interval = interval_extent<Interval>);

// std::vector<std::string> test_keys{"aa"s, "bb"s, "cc"s, "dd"s};
// {
//     auto vec_intervals
//         = interval_dict::intervals(test_dict, test_keys,
//                                    IDict::Interval{4, 100});
//     for (const auto&[key, value, interval]: vec_intervals)
//     {
//         std::cout << key << "\t"
//                   << value << "\t"
//                   << interval << "\n";
//     }
//
//     for (const auto&[key, value, interval]:
//     interval_dict::intervals(test_dict))
//     {
//         std::cout << key << "\t" << value << "\t" << interval <<
//         "\n";
//     }
//
//     std::cout << test_dict << "\n";
// }
