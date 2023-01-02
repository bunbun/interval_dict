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
/// \file value_interval.h
/// \brief Definitions of structs to hold a single value valid over an interval
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_VALUE_INTERVAL_H
#define INCLUDE_INTERVAL_DICT_VALUE_INTERVAL_H

#include <boost/icl/concept/interval.hpp>
#include <boost/icl/interval_traits.hpp>

#include <algorithm>
#include <compare>
#include <tuple>
#include <vector>

namespace interval_dict
{
  ///
  /// ValueInterval
  ///
  /// Holds a single Value valid over a boost:icl::Interval
  /// Sorts by interval then value
  ///
  template<typename Value, typename Interval>
  struct ValueInterval
  {
    using BaseType =
      typename boost::icl::interval_traits<Interval>::domain_type;
    using IntervalType = Interval;
    using ValueType = Value;

    Value value;
    Interval interval;

    // Spaceship operator
    // Returns category that is common of strong_ordering and whatever the
    // spaceship operator for value returns
    // Sort by interval then value
    // boost::icl intervals don't support space travel yet
    auto operator<=> (const ValueInterval &rhs) const
      -> std::common_comparison_category_t<std::strong_ordering,
                                           decltype (value <=> rhs.value)>
    {
      if (interval == rhs.interval)
      {
        return value <=> rhs.value;
      }
      return interval < rhs.interval ? std::strong_ordering::less
                                     : std::strong_ordering::greater;
    }

    bool operator== (const ValueInterval &) const = default;
    template<typename Other>
    bool operator== (const Other &rhs) const
    {
      return std::tie (interval, value) == std::tie (rhs.interval, rhs.value);
    }

    ValueInterval () = default;
    ValueInterval (Value value, Interval interval)
      : value (value)
      , interval (interval)
    {
    }
  };

  //
  // ValueIntervalRef
  //
  // Lightweight reference object for comparisons with ValueInterval without the
  // construction costs.
  // Reference semantics: beware of lifetime issues. Do not store!
  //
  template<typename Value, typename Interval>
  struct ValueIntervalRef
  {
    using BaseType =
      typename boost::icl::interval_traits<Interval>::domain_type;
    using IntervalType = Interval;
    using ValueType = Value;

    ValueIntervalRef (Value &value, Interval &interval)
      : value (value)
      , interval (interval)
    {
    }

    template<typename Other>
    bool operator== (const Other &rhs) const
    {
      return std::tie (rhs.interval, rhs.value) == std::tie (interval, value);
    }

    Value &value;
    Interval &interval;
  };

  template<typename Value, typename Interval>
  std::ostream &operator<< (std::ostream &os,
                            const ValueInterval<Value, Interval> &node)
  {
    os << node.value << ", " << node.interval;
    return os;
  }
} // namespace interval_dict

namespace value_interval
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

  template<typename Value, typename Interval>
  size_t hash (const Value &value, const Interval &interval)
  {
    size_t hash_value = 0;
    value_interval::hash_combine (hash_value, boost::icl::lower (interval));
    value_interval::hash_combine (hash_value, boost::icl::upper (interval));
    value_interval::hash_combine (hash_value, value);
    return hash_value;
  }

} // namespace value_interval

namespace std
{
  // Partial specialisation of std::hash for ValueInterval and ValueIntervalRef

  /// partial specialise std::hash for ValueInterval
  /// assumes Interval::BaseType and Value are hashable types
  template<typename Value, typename Interval>
  struct hash<interval_dict::ValueInterval<Value, Interval>>
  {
    size_t
    operator() (interval_dict::ValueInterval<Value, Interval> const &in) const
    {
      return value_interval::hash (in.value, in.interval);
    }
  };

  /// partial specialise std::hash for ValueInterval
  /// assumes Interval::BaseType and Value are hashable types
  template<typename Value, typename Interval>
  struct hash<interval_dict::ValueIntervalRef<Value, Interval>>
  {
    size_t
    operator() (interval_dict::ValueInterval<Value, Interval> const &in) const
    {
      return value_interval::hash (in.value, in.interval);
    }
  };

} // namespace std

#endif // INCLUDE_INTERVAL_DICT_VALUE_INTERVAL_H
