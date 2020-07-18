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
/// \file adaptor_traits.h
/// \brief Definitions of traits to manipulate types to implement IntervalDict

#ifndef INCLUDE_INTERVAL_DICT_ADAPTOR_TRAITS_H
#define INCLUDE_INTERVAL_DICT_ADAPTOR_TRAITS_H
#include <type_traits>
namespace interval_dict
{

/// Type manipulating function for obtaining the same implementation underlying
/// an IntervalDict that is uses the same Interval type but "rebased" with
/// a new Val type.
/// The return type is ::type as per C++ convention.
/// This is necessary for creating types to hold data in the "invert()"
/// orientation or to hold data after joining with a compatible IntervalDict
/// with possibly different Key / Value type. See "joined_to()".
template <typename OldVal_,
          typename NewVal_,
          typename Interval_,
          typename Impl_,
          typename enabled = void>
struct Rebased
{
    using Interval = Interval_;
    using OldVal = OldVal_;
    using Val = NewVal_;
    using Impl = Impl_;
    // using type = double;
};

} // namespace interval_dict

#endif // INCLUDE_INTERVAL_DICT_ADAPTOR_TRAITS_H
