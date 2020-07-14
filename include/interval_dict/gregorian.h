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
/// \file gregorian.h
/// \brief Allow use of boost gregorian dates in IntervalDict
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_GREGORIAN_H
#define INCLUDE_INTERVAL_DICT_GREGORIAN_H

#include "interval_traits.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/icl/gregorian.hpp>

inline boost::gregorian::date from_int(int date_int)
{
    int year = date_int / 10000;
    int month = date_int / 100 - year * 100;
    int day = date_int - date_int / 100 * 100;
    return {year, month, day};
}

namespace interval_dict
{
/*
 * For all intervals domain_type == boost::gregorian::date
 */
template <typename Interval>
class IntervalTraits<
    Interval,
    typename std::enable_if<std::is_same<
        typename boost::icl::interval_traits<Interval>::domain_type,
        boost::gregorian::date>::value>::type>
{
public:
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;
    using BaseDifferenceType = boost::gregorian::date_duration;

    static boost::gregorian::date lowest() throw()
    {
        return boost::gregorian::date{boost::date_time::min_date_time};
    }

    static boost::gregorian::date max() throw()
    {
        return boost::gregorian::date{boost::date_time::max_date_time};
    }

    static boost::gregorian::date_duration max_size() throw()
    {
        return boost::gregorian::date_duration{boost::date_time::max_date_time};
    }

    static Interval max_range() throw()
    {
        Interval{lowest(), max()};
    }
};
} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_GREGORIAN_H
