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
// subset(const KeyRange& keys, const ValRange& values, Interval interval)
// const;
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
