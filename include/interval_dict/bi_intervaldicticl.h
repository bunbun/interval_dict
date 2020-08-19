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
/// \file bi_intervaldicticl.h
/// \brief Declaration of the BiIntervalDictICLExp / BiIntervalDictICL classes
//
// Provides bidirectional interval associative dictionaries implemented using
// boost::icl::interval_map storing values in disjoint intervals
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H
#define INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H

#include <interval_dict/icl_interval_map_adaptor.h>

#include <interval_dict/intervaldict.h>

#include <interval_dict/bi_intervaldict.h>

namespace interval_dict
{
/**
 * @brief Bidirectional interval dictionary powered by boost::icl::interval_map
 * in both directions
 *
 * Typically used for time-varying dictionaries.
 *
 * `BiIntervalDictICLExp` is useful for specifying the exact inclusive or exclusive interval type.
 *
 * Choices are [boost::icl intervals
 * ](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html#boost_icl.introduction.icl_s_class_templates) :
 *
 *  - `left_open_interval<BaseType>`
 *  - `right_open_interval<BaseType>`
 *  - `open_interval<BaseType>`
 *  - `closed_interval<BaseType>`
 *
 *  `BaseType` can be a Date or Time type, for example.
 *
 * @tparam Key Type of keys
 * @tparam Val Type of Values
 * @tparam Interval Interval Type. E.g. boost::icl::right_open_interval<Date>
 */
template <typename Key, typename Val, typename Interval>
using BiIntervalDictICLExp =
    BiIntervalDictExp<Key,
                      Val,
                      Interval,
                      implementation::IntervalDictICLSubMap<Val, Interval>,
                      implementation::IntervalDictICLSubMap<Key, Interval>>;

/// \brief Bidirectional interval dictionary powered by boost::icl::interval_map
/// in both directions
///
/// \tparam Key Type of keys
/// \tparam Val Type of Values
/// \tparam BaseType The base type of the interval: Date or Posix Time etc.
template <typename Key, typename Val, typename BaseType>
using BiIntervalDictICL =
    BiIntervalDictICLExp<Key,
                         Val,
                         typename boost::icl::interval<BaseType>::type>;

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H
