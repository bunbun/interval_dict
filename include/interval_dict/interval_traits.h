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
#include <boost/icl/interval.hpp>
#include <boost/icl/interval_set.hpp>
#include <limits>
#include <type_traits>
namespace interval_dict
{

template <typename Interval, typename Enabled = void> class IntervalTraits
{
public:
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;
    using BaseSizeType =
        typename boost::icl::difference_type_of<Interval>::type;
};

template <typename Interval>
class IntervalTraits<
    Interval,
    typename std::enable_if<
        std::numeric_limits<typename boost::icl::interval_traits<
            Interval>::domain_type>::is_specialized,
        void>::type>
{
public:
    using BaseType =
        typename boost::icl::interval_traits<Interval>::domain_type;
    using BaseDifferenceType =
        typename boost::icl::difference_type_of<BaseType>::type;
    static constexpr BaseType lowest() throw()
    {
        return std::numeric_limits<BaseType>::lowest();
    }
    static constexpr BaseType max() throw()
    {
        return std::numeric_limits<BaseType>::max();
    }
    static constexpr BaseDifferenceType max_size() throw()
    {
        return std::numeric_limits<BaseDifferenceType>::max();
    }
    static constexpr Interval max_range() throw()
    {
        Interval{lowest(), max()};
    }
};

/// \brief The largest possible interval for a given underlying type
template <typename IntervalType>
IntervalType interval_extent =
    IntervalType{IntervalTraits<IntervalType>::lowest(),
                 IntervalTraits<IntervalType>::max()};

/// \brief Set of disjoint intervals
template <typename Interval>
using Intervals = boost::icl::interval_set<
    typename boost::icl::interval_traits<Interval>::domain_type,
    std::less,
    Interval>;


/*
 * Insertion and Erasure types
 */
template <typename Key, typename Val, typename Interval>
using Insertions = std::vector<std::tuple<Key, Val, Interval>>;
template <typename Key, typename Val, typename Interval>
using Erasures = std::vector<std::tuple<Key, Val, Interval>>;

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT__INTERVAL_TRAITS_H
