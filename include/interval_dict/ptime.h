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
/// \file ptime.h
/// \brief Adapts Posix time (boost::posix_time::ptime) for IntervalDict
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_PTIME_H
#define INCLUDE_INTERVAL_DICT_PTIME_H

//#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/icl/ptime.hpp>

#include "interval_traits.h"

namespace interval_dict::ptime_literals
{
/// Convenience function to make dates
inline boost::posix_time::ptime operator"" _pt(const char* time_str,
                                               std::size_t)
{
    return boost::posix_time::from_iso_string(time_str);
}
} // namespace interval_dict::ptime_literals

namespace interval_dict
{
/*
 * For all intervals domain_type == boost::posix_time::ptime
 */
template <typename Interval>
class IntervalTraits<
    Interval,
    typename std::enable_if<std::is_same<
        typename boost::icl::interval_traits<Interval>::domain_type,
        boost::posix_time::ptime>::value>::type>
{
public:
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;
    using BaseDifferenceType = boost::posix_time::time_duration;

    static boost::posix_time::ptime lowest() noexcept
    {
        return boost::posix_time::ptime{boost::date_time::min_date_time};
    }

    static boost::posix_time::ptime max() noexcept
    {
        return boost::posix_time::ptime{boost::date_time::max_date_time};
    }

    static boost::posix_time::time_duration max_size() noexcept
    {
        return boost::posix_time::time_duration{
            boost::date_time::max_date_time};
    }

    static Interval max_range() noexcept
    {
        Interval{lowest(), max()};
    }
};
} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_PTIME_H
