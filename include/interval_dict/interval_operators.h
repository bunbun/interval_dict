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

template <typename Interval>
Interval
right_extend(Interval interval,
             typename IntervalTraits<Interval>::BaseDifferenceType size)
{
    using namespace boost::icl;
    const auto left = lower(interval);
    const auto right = upper(interval);

    // Don't extend interval by more than max()
    if (size == IntervalTraits<Interval>::max_size() ||
        right > IntervalTraits<Interval>::max() - size)
    {
        return Interval{left, IntervalTraits<Interval>::max()};
    }

    return Interval{lower(interval), right + size};
}

template <typename Interval>
Interval left_extend(Interval interval,
                     typename IntervalTraits<Interval>::BaseDifferenceType size)
{
    using namespace boost::icl;
    const auto left = lower(interval);
    const auto right = upper(interval);

    // Don't extend interval by more than max()
    if (size == IntervalTraits<Interval>::max_size() ||
        left < IntervalTraits<Interval>::lowest() + size)
    {
        return Interval{IntervalTraits<Interval>::lowest(), right};
    }

    return Interval{lower(interval) - size, right};
}

} // namespace operators

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVAL_OPERATORS_H
