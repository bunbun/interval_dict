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
/// \brief Definitions of functions to compare intervals
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_INTERVAL_COMPARE_H
#define INCLUDE_INTERVAL_DICT_INTERVAL_COMPARE_H

#include <boost/icl/concept/interval.hpp>
#include <boost/icl/interval_traits.hpp>
#include <boost/icl/type_traits/is_asymmetric_interval.hpp>
#include <boost/icl/type_traits/is_continuous_interval.hpp>
#include <boost/icl/type_traits/is_discrete_interval.hpp>

#include <type_traits>

namespace interval_dict
{

namespace comparisons
{

//------------------------------------------------------------------------------
// exclusive_less()
// Like boost::icl::exclusive_less but does not check for interval emptiness
// Specialised for the different sorts of boost::icl::Interval
//
template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    bool>
exclusive_less(const IntervalType& left, const IntervalType& right)
{
    return boost::icl::upper(left) <= boost::icl::lower(right);
}

template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::has_symmetric_bounds<IntervalType>::value ||
        boost::icl::is_discrete_interval<IntervalType>::value,
    bool>
exclusive_less(const IntervalType& left, const IntervalType& right)
{
    return boost::icl::last(left) < boost::icl::first(right);
}

//------------------------------------------------------------------------------
// exclusive_less() combined with touches
template <typename IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_asymmetric_interval<IntervalType>::value, bool>
    more_or_touches(const IntervalType& left, const IntervalType& right)
{
    return boost::icl::upper(left) >= boost::icl::lower(right);
}

template <typename IntervalType>
inline typename std::
    enable_if_t<boost::icl::has_symmetric_bounds<IntervalType>::value, bool>
    more_or_touches(const IntervalType& left, const IntervalType& right)
{
    return boost::icl::last_next(left) >= boost::icl::first(right);
}

template <typename IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_discrete_interval<IntervalType>::value, bool>
    more_or_touches(const IntervalType& left, const IntervalType& right)
{
    return boost::icl::domain_next<IntervalType>(boost::icl::last(left)) >=
           boost::icl::first(right);
}

template <typename IntervalType>
inline typename std::
    enable_if_t<boost::icl::is_continuous_interval<IntervalType>::value, bool>
    more_or_touches(const IntervalType& left, const IntervalType& right)
{
    return (
        boost::icl::upper(left) > boost::icl::lower(right) ||
        (boost::icl::is_complementary(boost::icl::inner_bounds(left, right)) &&
         boost::icl::upper(left) == boost::icl::lower(right)));
}

//
// exclusive_less()
// Comparison operators between a single point and an interval on the right
// Specialised for the different sorts of boost::icl::Interval
//
template <typename BaseType, typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    bool>
exclusive_less(BaseType left_upper, IntervalType right)
{
    return boost::icl::domain_less_equal<IntervalType>(left_upper,
                                                       lower(right));
}

template <typename BaseType, typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::has_symmetric_bounds<IntervalType>::value ||
        boost::icl::is_discrete_interval<IntervalType>::value,
    bool>
exclusive_less(const BaseType& left_upper, const IntervalType& right)
{
    return boost::icl::domain_less<IntervalType>(left_upper, first(right));
}

//
// exclusive_less()
// Comparison operators between a single point and an interval on the right
// Specialised for the different sorts of boost::icl::Interval
//
template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    bool>
exclusive_less(
    const typename boost::icl::interval_traits<IntervalType>::domain_type&
        left_upper,
    const typename boost::icl::interval_traits<IntervalType>::domain_type&
        lower_right)
{
    return boost::icl::domain_less_equal<IntervalType>(left_upper, lower_right);
}

template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::has_symmetric_bounds<IntervalType>::value ||
        boost::icl::is_discrete_interval<IntervalType>::value,
    bool>
exclusive_less(
    const typename boost::icl::interval_traits<IntervalType>::domain_type&
        left_upper,
    const typename boost::icl::interval_traits<IntervalType>::domain_type&
        first_right)
{
    return boost::icl::domain_less<IntervalType>(left_upper, first_right);
}

// intersects()
/// Like boost::icl::intersects but without empty() tests
template <typename T>
bool intersects(const T& a, const T& b)
{
    return !(comparisons::exclusive_less(a, b) || comparisons::exclusive_less(b, a));
}

//
// upper_edge()
// Get the edges of an interval, consistent across dynamic and static
// interval types
// Specialised for the different sorts of boost::icl::Interval
//
template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
upper_edge(const IntervalType& interval)
{
    return boost::icl::upper(interval);
}

template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_discrete_interval<IntervalType>::value ||
        boost::icl::has_symmetric_bounds<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
upper_edge(const IntervalType& interval)
{
    return boost::icl::last(interval);
}

//
// lower_edge()
// Get the edges of an interval, consistent across dynamic and static
// interval types
// Specialised for the different sorts of boost::icl::Interval
//
template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_asymmetric_interval<IntervalType>::value ||
        boost::icl::is_continuous_interval<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
lower_edge(const IntervalType& interval)
{
    return boost::icl::lower(interval);
}

template <typename IntervalType>
inline typename std::enable_if_t<
    boost::icl::is_discrete_interval<IntervalType>::value ||
        boost::icl::has_symmetric_bounds<IntervalType>::value,
    typename boost::icl::interval_traits<IntervalType>::domain_type>
lower_edge(const IntervalType& interval)
{
    return boost::icl::first(interval);
}

// \brief Compare by member value then by interval
struct CompareValInterval
{
    using is_transparent = void;
    template <typename S, typename T>
    bool operator()(const S& s, const T& t) const
    {
        return std::tie(s.value, s.interval) < std::tie(t.value, t.interval);
    }
};

// \brief Sorts by Value then intervals.
// Intervals compare normally unless overlap when it always returns false
// Heterogeneous comparator
struct CompareValIntervalOverlap
{
    using is_transparent = void;
    template <typename S, typename T>
    bool operator()(const S& s, const T& t) const
    {
        if (s.value != t.value)
        {
            return s.value < t.value;
        }
        if (boost::icl::intersects(s.interval, t.interval))
        {
            return false;
        }
        return s.interval < t.interval;
    }
};

// \brief Sorts by Value then intervals.
// Used in insert() to gather (& combine) touching intervals with the same value
// Intervals compare normally unless overlap or *touch* when it returns false
// Heterogeneous comparator
struct CompareValIntervalTouches
{
    using is_transparent = void;
    template <typename S, typename T>
    bool operator()(const S& s, const T& t) const
    {
        if (s.value != t.value)
        {
            return s.value < t.value;
        }
        if (s.interval == t.interval)
        {
            return false;
        }
        if (s.interval < t.interval)
        {

            if (boost::icl::intersects(s.interval, t.interval) ||
                boost::icl::touches(s.interval, t.interval))
            {
                return false;
            }
            return true;
        }
        return false;
    }
};

// Trait for getting intervals from different types
template <typename T, typename Enabled = void> struct GetIntervalOp
{
    static const T& apply(const T& interval)
    {
        return interval;
    }
};

template <typename HasIntervalType>
struct GetIntervalOp<
    HasIntervalType,
    std::enable_if_t<std::is_class_v<typename HasIntervalType::IntervalType>>>
{
    static const typename HasIntervalType::IntervalType&
    apply(const HasIntervalType& value)
    {
        return value.interval;
    }
};

// Compare Interval then value
class CompareInterval
{
public:
    template <typename T1, typename T2>
    bool operator()(const T1& lhs, const T2& rhs) const
    {
        using lhsGetInterval = GetIntervalOp<T1>;
        using rhsGetInterval = GetIntervalOp<T2>;
        const auto lhs_interval =
            std::tuple(comparisons::lower_edge(lhsGetInterval::apply(lhs)),
                       comparisons::upper_edge(lhsGetInterval::apply(lhs)));
        const auto rhs_interval =
            std::tuple(comparisons::lower_edge(rhsGetInterval::apply(rhs)),
                       comparisons::upper_edge(rhsGetInterval::apply(rhs)));

        return lhs_interval < rhs_interval;
    }
};

} // namespace comparisons

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_INTERVAL_COMPARE_H
