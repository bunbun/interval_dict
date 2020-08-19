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
/// \file interval_traits.h
/// \brief Definitions of traits to obtain the minimum and maximum values for
/// the underlying types for intervals
///
/// The minimum and maximum values should be carefully chosen so that they can
/// be used in calculations and not just in sentinels.
/// Thus
/// 1) lowest() and max() must be normal values and not just sentinels like
/// INF or NAN
/// 2) lowest() and max() must be chosen that calculations do not underflow()

#ifndef INCLUDE_INTERVAL_DICT__INTERVAL_TRAITS_H
#define INCLUDE_INTERVAL_DICT__INTERVAL_TRAITS_H
#include <boost/icl/interval_traits.hpp>
#include <limits>
#include <type_traits>
namespace interval_dict
{

/// \brief Provide lowest(), max(), max_size(), and BaseType and
///  BaseDifferenceType for each type that can be used to build intervals
/// \tparam Interval
/// \tparam Enabled For controlling specialisation
template <typename Interval, typename Enabled = void> class IntervalTraits
{
};

/// \brief Provide lowest(), max(), max_size(), and BaseType and
/// BaseDifferenceType for value types such as int and float so they
/// can be used to build intervals
template <typename Interval>
class IntervalTraits<
    Interval,
    typename std::enable_if<
        std::numeric_limits<typename boost::icl::interval_traits<
            Interval>::domain_type>::is_specialized,
        void>::type>
{
public:
    /// Get underlying type for the interval. Eg. Date or Time
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;

    /// Get the type for interval differences.
    /// This is often different from BaseType. E.g. Data or Time durations
    using BaseDifferenceType =
        typename boost::icl::difference_type_of<BaseType>::type;

    /// The minimum value for an interval start
    static constexpr BaseType lowest() throw()
    {
        return std::numeric_limits<BaseType>::lowest();
    }

    /// The maximum value for an interval end
    static constexpr BaseType max() throw()
    {
        return std::numeric_limits<BaseType>::max();
    }

    /// The maximum size of an interval
    static constexpr BaseDifferenceType max_size() throw()
    {
        return std::numeric_limits<BaseDifferenceType>::max();
    }
};

/// \brief The largest possible interval for a given underlying type
template <typename IntervalType>
IntervalType interval_extent =
    IntervalType{IntervalTraits<IntervalType>::lowest(),
                 IntervalTraits<IntervalType>::max()};

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT__INTERVAL_TRAITS_H
