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
/// \file intervaldictail.h
/// \brief Declaration of the IntervalDictAILExp / IntervalDictAIL classes
//
// Provides interval associative dictionaries implemented using
// Augmented Interval Lists storing values in non-disjoint intervals
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_INTERVALDICTAIL_H
#define INCLUDE_INTERVAL_DICT_INTERVALDICTAIL_H

#include "adaptor_ail.h"
#include "intervaldict.h"

namespace interval_dict
{
  /**
   * @brief one-to-many interval dictionary powered by
   * an Augmented Interval List stroing values in non-disjoint intervals
   *
   *  Typically used for time-varying dictionaries.
   *
   *  `IntervalDictAILExp` is useful for specifying the exact inclusive or
   * exclusive interval type.
   *
   * Choices are [boost::icl intervals
   * ](https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html#boost_icl.introduction.icl_s_class_templates)
   * :
   *
   *  - `left_open_interval<BaseType>`
   *  - `right_open_interval<BaseType>`
   *  - `open_interval<BaseType>`
   *  - `closed_interval<BaseType>`
   *
   *  `BaseType` can be a Date or Time type, for example.
   *
   * @tparam Key Type of keys
   * @tparam Value Type of Values
   * @tparam Interval Interval Type. E.g. boost::icl::right_open_interval<Date>
   */
  template<typename Key, typename Value, typename Interval>
  using IntervalDictAILExp
    = IntervalDictExp<Key,
                      Value,
                      Interval,
                      implementation::AugmentedIntervalList<Value, Interval>>;

  /// \brief one-to-many interval dictionary powered by boost::icl::interval_map
  ///
  /// \tparam Key Type of keys
  /// \tparam Value Type of Values
  /// \tparam BaseType The base type of the interval: Date or Posix Time etc.
  template<typename Key, typename Value, typename BaseType>
  using IntervalDictAIL
    = IntervalDictAILExp<Key,
                         Value,
                         typename boost::icl::interval<BaseType>::type>;

} // namespace interval_dict

#endif