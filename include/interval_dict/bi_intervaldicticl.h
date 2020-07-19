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
/// \file biintervaldicticl.h
/// \brief Declaration of the BiIntervalDictICLExp / BiIntervalDictICL classes
//
// Provides bidirectional interval associative dictionaries implemented using
// boost::icl::interval_map storing values in disjoint intervals
//
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H
#define INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H

#include <interval_dict/bi_intervaldict.h>
#include <interval_dict/icl_interval_map_adaptor.h>
#include <interval_dict/intervaldict.h>
#include <interval_dict/intervaldict_func.h>

namespace interval_dict
{
template <typename Key, typename Val, typename Interval>
using BiIntervalDictICLExp =
    BiIntervalDictExp<Key,
                      Val,
                      Interval,
                      implementation::IntervalDictICLSubMap<Val, Interval>,
                      implementation::IntervalDictICLSubMap<Key, Interval>>;

template <typename Key, typename Val, typename BaseType>
using BiIntervalDictICL =
    BiIntervalDictICLExp<Key,
                         Val,
                         typename boost::icl::interval<BaseType>::type>;

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_BI_INTERVALDICTICL_H
