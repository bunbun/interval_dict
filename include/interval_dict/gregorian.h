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
/// \brief Adapts boost gregorian dates for IntervalDict
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef INCLUDE_INTERVAL_DICT_GREGORIAN_H
#define INCLUDE_INTERVAL_DICT_GREGORIAN_H

#include "interval_traits.h"

#include <boost/icl/gregorian.hpp>

#include <concepts>

namespace interval_dict::date_literals
{
  /// Convenience function to make dates
  inline boost::gregorian::date operator"" _dt (unsigned long long int date_int)
  {
    const short unsigned int year = date_int / 10000;
    const short unsigned int month = date_int / 100 - year * 100;
    const short unsigned int day = date_int - date_int / 100 * 100;
    return {year, month, day};
  }
} // namespace interval_dict::date_literals

namespace interval_dict
{
  template<typename Interval>
  concept GregorianInterval
    = std::is_same_v<typename ICLTraits<Interval>::BaseType,
                     boost::gregorian::date>;

  /// Traits specialised to adapt boost::gregorian::date for use with
  /// IntervalDict
  template<GregorianInterval Interval>
  class IntervalTraits<Interval>
  {
    public:
    /// The underlying type for the interval
    using BaseType = boost::gregorian::date;

    /// The type for interval differences.
    using BaseDifferenceType = boost::gregorian::date_duration;

    /// The earliest possible date
    static boost::gregorian::date minimum () noexcept
    {
      return boost::gregorian::date {boost::date_time::min_date_time};
    }

    /// The latest possible date
    static boost::gregorian::date maximum () noexcept
    {
      return boost::gregorian::date {boost::date_time::max_date_time};
    }

    /// The maximum size of a date interval
    static boost::gregorian::date_duration max_size () noexcept
    {
      return boost::gregorian::date_duration {boost::date_time::max_date_time};
    }
  };

} // namespace interval_dict

namespace boost::gregorian
{
  using boost::icl::operator--;
  using boost::icl::operator++;
} // namespace boost::gregorian

namespace std
{
  /// \brief Hash function for boost::gregorian::date
  template<>
  struct hash<boost::gregorian::date>
  {
    size_t operator() (const boost::gregorian::date &date) const
    {
      return std::hash<decltype (date.julian_day ())> () (date.julian_day ());
    }
  };
} // namespace std

#endif // INCLUDE_INTERVAL_DICT_GREGORIAN_H
