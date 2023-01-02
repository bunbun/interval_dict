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

// #include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/icl/ptime.hpp>

#include "interval_traits.h"

namespace interval_dict::ptime_literals
{
  /// Convenience function to make dates
  inline boost::posix_time::ptime operator"" _pt (const char *time_str,
                                                  std::size_t)
  {
    return boost::posix_time::from_iso_string (time_str);
  }
} // namespace interval_dict::ptime_literals

namespace interval_dict
{
  template<typename Interval>
  concept PTimeInterval = std::is_same_v<typename ICLTraits<Interval>::BaseType,
                                         boost::posix_time::ptime>;
  /// Traits specialised to adapt boost::posix_time::ptime for use with
  /// IntervalDict
  template<PTimeInterval Interval>
  class IntervalTraits<Interval>
  {
    public:
    /// The underlying type for the interval
    using BaseType = boost::posix_time::ptime;

    /// The type for interval differences.
    using BaseDifferenceType = boost::posix_time::time_duration;

    /// The earliest time
    static boost::posix_time::ptime minimum () noexcept
    {
      return boost::posix_time::ptime {boost::date_time::min_date_time};
    }

    /// The latest possible time
    static boost::posix_time::ptime maximum () noexcept
    {
      return boost::posix_time::ptime {boost::date_time::max_date_time};
    }

    /// The maximum size of a time interval
    static boost::posix_time::time_duration max_size () noexcept
    {
      return boost::posix_time::time_duration {boost::date_time::max_date_time};
    }
  };
} // namespace interval_dict

namespace interval_dict
{
  namespace
  {
    // Code from boost
    // Reciprocal of the golden ratio helps spread entropy
    //     and handles duplicates.
    // See Mike Seymour in magic-numbers-in-boosthash-combine:
    //     http://stackoverflow.com/questions/4948780
    template<class T>
    inline void hash_combine (std::size_t &seed, T const &v)
    {
      seed ^= std::hash<T> {}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  } // namespace
} // namespace interval_dict

namespace boost::posix_time
{
  using boost::icl::operator--;
  using boost::icl::operator++;
} // namespace boost::posix_time

namespace std
{
  /// \brief Hash function for boost::gregorian::date
  template<>
  struct hash<boost::posix_time::ptime>
  {
    size_t operator() (const boost::posix_time::ptime &ptime) const
    {
      size_t hash_value = 0;

      auto tod = ptime.time_of_day ().total_nanoseconds ();
      auto jd = ptime.date ().julian_day ();

      interval_dict::hash_combine (hash_value, tod);
      interval_dict::hash_combine (hash_value, jd);
      return hash_value;
    }
  };

} // namespace std

#endif // INCLUDE_INTERVAL_DICT_PTIME_H
