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
/// \file bi_intervaldictitree.h
/// \brief Declaration of the BiIntervalDictITreeExp / BiIntervalDictITree
/// classes
//
// Provides bidirectional interval associative dictionaries implemented using
// an Interval Tree storing values in non-disjoint intervals
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_BI_INTERVALDICTITREE_H
#define INCLUDE_INTERVAL_DICT_BI_INTERVALDICTITREE_H

#include "adaptor_interval_tree.h"

#include "intervaldict.h"

#include "bi_intervaldict.h"

namespace interval_dict
{
  /**
   * @brief Bidirectional interval dictionary powered by
   * interval_dict::IntervalTree in both directions
   *
   * Typically used for time-varying dictionaries.
   *
   * `BiIntervalDictITreeExp` is useful for specifying the exact inclusive or
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
  using BiIntervalDictITreeExp
    = BiIntervalDictExp<Key,
                        Value,
                        Interval,
                        tree::IntervalTree<Value, Interval>,
                        tree::IntervalTree<Key, Interval>>;

  /// \brief Bidirectional interval dictionary powered by
  /// boost::icl::interval_map in both directions
  ///
  /// \tparam Key Type of keys
  /// \tparam Value Type of Values
  /// \tparam BaseType The base type of the interval: Date or Posix Time etc.
  template<typename Key, typename Value, typename BaseType>
  using BiIntervalDictITree
    = BiIntervalDictITreeExp<Key,
                             Value,
                             typename boost::icl::interval<BaseType>::type>;

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_BI_INTERVALDICTITREE_H
