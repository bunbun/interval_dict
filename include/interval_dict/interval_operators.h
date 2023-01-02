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
/// \file interval_compare.h
/// \brief Definitions of functions to mutate intervals
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_INTERVAL_OPERATORS_H
#define INCLUDE_INTERVAL_DICT_INTERVAL_OPERATORS_H

#include "interval_traits.h"
#include <boost/icl/concept/interval.hpp>
#include <boost/icl/interval_traits.hpp>

namespace interval_dict
{
  namespace operators
  {
    template<typename Interval>
    Interval
    right_extend (Interval interval,
                  typename IntervalTraits<Interval>::BaseDifferenceType size)
    {
      using namespace boost::icl;
      const auto left = lower (interval);
      const auto right = upper (interval);

      // Don't extend interval edge past maximum()
      if (size == IntervalTraits<Interval>::max_size ()
          || right > IntervalTraits<Interval>::maximum () - size)
      {
        return Interval {left, IntervalTraits<Interval>::maximum ()};
      }

      return Interval {lower (interval), right + size};
    }

    template<typename Interval>
    Interval
    left_extend (Interval interval,
                 typename IntervalTraits<Interval>::BaseDifferenceType size)
    {
      using namespace boost::icl;
      const auto left = lower (interval);
      const auto right = upper (interval);

      // Don't extend interval to more than maximum()
      if (size == IntervalTraits<Interval>::max_size ()
          || left < IntervalTraits<Interval>::minimum () + size)
      {
        return Interval {IntervalTraits<Interval>::minimum (), right};
      }

      return Interval {lower (interval) - size, right};
    }

    template<typename Interval>
    Interval tombstone (Interval interval)
    {
      using namespace boost::icl;
      return Interval {lower (interval), IntervalTraits<Interval>::minimum ()};
    }

  } // namespace operators

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVAL_OPERATORS_H
