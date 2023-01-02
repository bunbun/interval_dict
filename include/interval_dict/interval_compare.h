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
/// \brief Definitions of functions to compare intervals specialised for
/// open/closed (asymmetric) or continuous (open-open) or discrete intervals
/// exclusive_less()
///
/// Provides the following functions:
///
///  - exclusive_less ()
///    left interval must be entirely less than and not touching right interval
///
///  - more_or_touches ()
///    left interval must overlap or touch right interval
///    N.B. assumes left interval.start < right interval.end
///
///  - exclusive_less ()
///    Single point must be less than and not touching right interval
///
///  - intersects ()
///    Intervals must not be exclusive_less of each other, either way around.
///
///  - lower_edge ()
///  - upper_edge ()
///    Get consistent bounds member of static and dynamic intervals whatever
///    that means
///
///
/// \author Leo Goodstadt
//
// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_INTERVAL_COMPARE_H
#define INCLUDE_INTERVAL_DICT_INTERVAL_COMPARE_H

#include <boost/icl/concept/interval.hpp>
#include <boost/icl/interval_traits.hpp>
#include <boost/icl/type_traits/is_asymmetric_interval.hpp>
#include <boost/icl/type_traits/is_continuous_interval.hpp>
#include <boost/icl/type_traits/is_discrete_interval.hpp>

#include <type_traits>

namespace interval_dict::comparisons
{
  namespace icl = boost::icl;

  template<typename Interval>
  concept AsymmetricOrContinuous
    = boost::icl::is_asymmetric_interval<Interval>::value != 0
      || boost::icl::is_continuous_interval<Interval>::value;

  template<typename Interval>
  concept SymmetricOrDiscrete
    = boost::icl::has_symmetric_bounds<Interval>::value
      || boost::icl::is_discrete_interval<Interval>::value;

  template<typename Interval>
  concept Asymmetric = boost::icl::is_asymmetric_interval<Interval>::value;

  template<typename Interval>
  concept Symmetric = boost::icl::has_symmetric_bounds<Interval>::value;

  template<typename Interval>
  concept Discrete = boost::icl::is_discrete_interval<Interval>::value;

  template<typename Interval>
  concept Continuous = boost::icl::is_continuous_interval<Interval>::value;

  //------------------------------------------------------------------------------
  // exclusive_less()
  // Like boost::icl::exclusive_less but does not check for interval emptiness
  // Specialised for the different sorts of boost::icl::Interval
  //
  template<AsymmetricOrContinuous IntervalType>
  bool exclusive_less (const IntervalType &left, const IntervalType &right)
  {
    return icl::upper (left) <= icl::lower (right);
  }

  template<SymmetricOrDiscrete IntervalType>
  bool exclusive_less (const IntervalType &left, const IntervalType &right)
  {
    return icl::last (left) < icl::first (right);
  }

  //------------------------------------------------------------------------------
  // exclusive_less() combined with touches
  template<Asymmetric IntervalType>
  bool more_or_touches (const IntervalType &left, const IntervalType &right)
  {
    return icl::upper (left) >= icl::lower (right);
  }

  template<Symmetric IntervalType>
  bool more_or_touches (const IntervalType &left, const IntervalType &right)
  {
    return icl::last_next (left) >= icl::first (right);
  }

  template<Discrete IntervalType>
  bool more_or_touches (const IntervalType &left, const IntervalType &right)
  {
    return icl::domain_next<IntervalType> (icl::last (left))
           >= icl::first (right);
  }

  template<Continuous IntervalType>
  bool more_or_touches (const IntervalType &left, const IntervalType &right)
  {
    return (icl::upper (left) > icl::lower (right)
            || (icl::is_complementary (icl::inner_bounds (left, right))
                && icl::upper (left) == icl::lower (right)));
  }

  //
  // exclusive_less()
  // Comparison operators between a single point and an interval on the right
  // Specialised for the different sorts of icl::Interval
  //
  template<typename BaseType, AsymmetricOrContinuous IntervalType>
  bool exclusive_less (BaseType left_upper, IntervalType right)
  {
    return icl::domain_less_equal<IntervalType> (left_upper, lower (right));
  }

  template<typename BaseType, SymmetricOrDiscrete IntervalType>
  bool exclusive_less (const BaseType &left_upper, const IntervalType &right)
  {
    return icl::domain_less<IntervalType> (left_upper, first (right));
  }

  //
  // exclusive_less()
  // Comparison operators between a single point and an interval on the right
  // Specialised for the different sorts of icl::Interval
  //
  template<AsymmetricOrContinuous IntervalType>
  bool exclusive_less (
    const typename icl::interval_traits<IntervalType>::domain_type &left_upper,
    const typename icl::interval_traits<IntervalType>::domain_type &lower_right)
  {
    return icl::domain_less_equal<IntervalType> (left_upper, lower_right);
  }

  template<SymmetricOrDiscrete IntervalType>
  bool exclusive_less (
    const typename icl::interval_traits<IntervalType>::domain_type &left_upper,
    const typename icl::interval_traits<IntervalType>::domain_type &first_right)
  {
    return icl::domain_less<IntervalType> (left_upper, first_right);
  }

  /// intersects()
  /// Like icl::intersects but without empty() tests
  template<typename T>
  bool intersects (const T &a, const T &b)
  {
    return !(comparisons::exclusive_less (a, b)
             || comparisons::exclusive_less (b, a));
  }

  //
  // upper_edge()
  // Get the edges of an interval, consistent across dynamic and static
  // interval types
  // Specialised for the different sorts of icl::Interval
  //
  template<AsymmetricOrContinuous IntervalType>
  inline IntervalTraits<IntervalType>::BaseType
  upper_edge (const IntervalType &interval)
  {
    return icl::upper (interval);
  }

  template<SymmetricOrDiscrete IntervalType>
  inline IntervalTraits<IntervalType>::BaseType
  upper_edge (const IntervalType &interval)
  {
    return icl::last (interval);
  }

  /**
   * lower_edge()
   *
   * \brief  Get the edges of an interval, consistent across dynamic and
   * static interval types
   *
   * Specialised for the different sorts of icl::Interval
   */
  template<AsymmetricOrContinuous IntervalType>
  inline IntervalTraits<IntervalType>::BaseType
  lower_edge (const IntervalType &interval)
  {
    return icl::lower (interval);
  }

  template<SymmetricOrDiscrete IntervalType>
  IntervalTraits<IntervalType>::BaseType
  lower_edge (const IntervalType &interval)
  {
    return icl::first (interval);
  }

  /**
   * \brief Compare by value then by interval
   */
  struct CompareValInterval
  {
    using is_transparent = void;
    template<typename S, typename T>
    bool operator() (const S &s, const T &t) const
    {
      return std::tie (s.value, s.interval) < std::tie (t.value, t.interval);
    }
  };

  /// \brief Sorts by Value then intervals.
  /// Intervals compare normally unless overlap when it always returns false
  ///
  /// Heterogeneous comparator
  struct CompareValIntervalOverlap
  {
    // accepts arguments of arbitrary types with std find / lower_bound
    using is_transparent = void;
    template<typename S, typename T>
    constexpr bool operator() (const S &s, const T &t) const
    {
      if (s.value != t.value)
      {
        return s.value < t.value;
      }
      if (icl::intersects (s.interval, t.interval))
      {
        return false;
      }
      return s.interval < t.interval;
    }
  };

  /// \brief Sorts by Value then intervals.
  /// Used in insert() to gather (& combine) touching intervals with the same
  /// value Intervals compare normally unless overlap or *touch* when it
  /// returns false Heterogeneous comparator
  struct CompareValIntervalTouches
  {
    // accepts arguments of arbitrary types with std find / lower_bound
    using is_transparent = void;
    template<typename S, typename T>
    bool operator() (const S &s, const T &t) const
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
        if (icl::intersects (s.interval, t.interval)
            || icl::touches (s.interval, t.interval))
        {
          return false;
        }
        return true;
      }
      return false;
    }
  };

  /**
   * \brief Trait for extracting interval from either an interval itself
   * or something that contains an interval member
   */
  template<typename T, typename Enabled = void>
  struct GetIntervalOp
  {
    static const T &apply (const T &interval)
    {
      return interval;
    }
  };

  template<typename HasIntervalType>
    requires std::is_class_v<typename HasIntervalType::IntervalType>
  struct GetIntervalOp<HasIntervalType>
  {
    static const typename HasIntervalType::IntervalType &
    apply (const HasIntervalType &value)
    {
      return value.interval;
    }
  };

  // Compare Interval then value
  class CompareInterval
  {
    public:
    template<typename T1, typename T2>
    bool operator() (const T1 &lhs, const T2 &rhs) const
    {
      using lhsGetInterval = GetIntervalOp<T1>;
      using rhsGetInterval = GetIntervalOp<T2>;
      //    const auto lhs_interval =
      //          std::tuple(comparisons::lower_edge(lhsGetInterval::apply(lhs)),
      //                     comparisons::upper_edge(lhsGetInterval::apply(lhs)));
      //    const auto rhs_interval =
      //          std::tuple(comparisons::lower_edge(rhsGetInterval::apply(rhs)),
      //                     comparisons::upper_edge(rhsGetInterval::apply(rhs)));
      /*
       * Use forward as tuple so comparisons work whether lower and upper edge
       *   are r or lvalues.
       * TODO: Check
       */
      return std::forward_as_tuple (
               comparisons::lower_edge (lhsGetInterval::apply (lhs)),
               comparisons::upper_edge (lhsGetInterval::apply (lhs)))
             < std::forward_as_tuple (
               comparisons::lower_edge (rhsGetInterval::apply (rhs)),
               comparisons::upper_edge (rhsGetInterval::apply (rhs)));
    }
  };
} // namespace interval_dict::comparisons

#endif // INCLUDE_INTERVAL_DICT_INTERVAL_COMPARE_H
