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
/// \file intervaldicticl.h
/// \brief Declaration of the IntervalDictICLExp / IntervalDictICL classes
//
// Provides interval associative dictionaries implemented using
// boost::icl::interval_map storing values in disjoint intervals
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_INTERVALDICTICL_H
#define INCLUDE_INTERVAL_DICT_INTERVALDICTICL_H

#include <interval_dict/icl_interval_map_adaptor.h>
#include <interval_dict/intervaldict.h>
#include <interval_dict/intervaldict_func.h>

namespace interval_dict
{
/// IntervalDictICLExp
/// \brief one-to-many interval dictionary powered by boost::icl::interval_map
///
/// Usually, it is more convenient to use a default interval of Time or
/// Date etc. (See `IntervalDictICL`).
///
/// IntervalDictICLExp allows you to specify the exact interval type,
/// be that inclusive/exclusive.
///
/// \tparam Key Type of keys
/// \tparam Val Type of Values
/// \tparam Interval Interval Type. E.g. boost::icl::right_open_interval<Date>
template <typename Key, typename Val, typename Interval>
using IntervalDictICLExp =
    IntervalDictExp<Key,
                    Val,
                    Interval,
                    implementation::IntervalDictICLSubMap<Val, Interval>>;

/// IntervalDictICL
/// \brief one-to-many interval dictionary powered by boost::icl::interval_map
///
/// \tparam Key Type of keys
/// \tparam Val Type of Values
/// \tparam BaseType The base type of the iterval: Date or Posix Time etc.
template <typename Key, typename Val, typename BaseType>
using IntervalDictICL =
    IntervalDictICLExp<Key, Val, typename boost::icl::interval<BaseType>::type>;

} // namespace interval_dict

#endif