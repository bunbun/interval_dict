// /*
//  * disjoint_intervals
//  */
// template <typename Key, typename Val, typename Interval, typename Impl>
// cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
// disjoint_intervals(
//     const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
//     std::vector<Key> keys,
//     Interval query_interval = interval_extent<Interval>);
//
// cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
// disjoint_intervals(
//     const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
//     const Key& key,
//     Interval query_interval = interval_extent<Interval>);
//
// template <typename Key, typename Val, typename Interval, typename Impl>
// cppcoro::generator<std::tuple<const Key&, const std::set<Val>&, Interval>>
// disjoint_intervals(
//     const IntervalDictExp<Key, Val, Interval, Impl>& interval_dict,
//     Interval query_interval = interval_extent<Interval>);
