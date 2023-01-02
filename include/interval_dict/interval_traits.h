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
/// 1) minimum() and maximum() must be normal values and not just sentinels
/// like INF or NAN
/// 2) minimum() and maximum() must be chosen that calculations do not
/// underflow(). I.e. it must be possible to have `maximum () - minimum ()`

#ifndef INCLUDE_INTERVAL_DICT_INTERVAL_TRAITS_H
#define INCLUDE_INTERVAL_DICT_INTERVAL_TRAITS_H
#include <boost/icl/interval_traits.hpp>
#include <limits>
#include <type_traits>
namespace interval_dict
{
  template<typename Interval>
  struct ICLTraits
  {
    using BaseType = boost::icl::interval_traits<Interval>::domain_type;
  };

  template<typename Interval>
  class IntervalTraits
  {
    //      static constexpr T minimum () noexcept
    //      { return ???;  }
    //      /// The maximum value for an interval end
    //      static constexpr T maximum () noexcept
    //      { return ???;  }
    //      /// The maximum size of an interval
    //      static constexpr U max_size () noexcept
    //      { return ???;  }
  };

  /// \brief Provide minimum(), maximum(), max_size(), and BaseType and
  /// BaseDifferenceType for value types such as int and float so they
  /// can be used to build intervals
  template<typename Interval>
  concept NumericInterval = std::numeric_limits<
    typename ICLTraits<Interval>::BaseType>::is_specialized;

  template<NumericInterval Interval>
  class IntervalTraits<Interval>
  {
    public:
    /// Get underlying type for the interval. Eg. Date or Time
    using BaseType = ICLTraits<Interval>::BaseType;

    /// Get the type for interval differences.
    /// This is often different from BaseType. E.g. Data or Time durations
    using BaseDifferenceType =
      typename boost::icl::difference_type_of<BaseType>::type;

    /// The minimum value for an interval start
    /// Note that lowest is the most -ve values for floating point.
    /// min() is the smallest +ve value.
    static constexpr BaseType minimum () noexcept
    {
      return std::numeric_limits<BaseType>::lowest ();
    }

    /// The maximum value for an interval end
    static constexpr BaseType maximum () noexcept
    {
      return std::numeric_limits<BaseType>::max ();
    }

    /// The maximum size of an interval
    static constexpr BaseDifferenceType max_size () noexcept
    {
      return std::numeric_limits<BaseDifferenceType>::max ();
    }
  };

  /// \brief The largest possible interval for a given underlying type
  template<typename IntervalType>
  IntervalType interval_extent
    = IntervalType {IntervalTraits<IntervalType>::minimum (),
                    IntervalTraits<IntervalType>::maximum ()};

  /// \brief Gap between two intervals and their values
  template<typename Value, typename Interval>
  using SandwichedGap
    = std::tuple<std::vector<Value>, Interval, std::vector<Value>>;

  /// \brief All gaps between intervals and their values for any key
  template<typename Value, typename Interval>
  using SandwichedGaps = std::vector<SandwichedGap<Value, Interval>>;

  /// \brief A disjoint interval and all values therein
  template<typename Value, typename Interval>
  using ValuesDisjointInterval = std::tuple<std::vector<Value>, Interval>;

  /// \brief A disjoint interval pertaining to a key, and all values therein
  template<typename Key, typename Value, typename Interval>
  using KeyValuesDisjointInterval
    = std::tuple<Key, std::vector<Value>, Interval>;

  /// \brief Corresponding Key-value-interval
  template<typename Key, typename Value, typename Interval>
  using KeyValueInterval = std::tuple<Key, Value, Interval>;

} // namespace interval_dict
#endif // INCLUDE_INTERVAL_DICT__INTERVAL_TRAITS_H
