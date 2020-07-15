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
/// \file tuple_print.h
/// \brief Support for outputting tuples to std::ostream
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef TESTS_TUPLE_PRINT_H
#define TESTS_TUPLE_PRINT_H

#include <tuple>
#include <iostream>

/*
 * To dump errors for debugging
 */
namespace aux
{
template <class TupType, size_t... I>
void print_tuple(const TupType& _tup, std::index_sequence<I...>, std::ostream& os)
{
    os << "(";
    (..., (os << (I == 0 ? "" : ", ") << std::get<I>(_tup)));
    os << ")";
}
}
namespace std
{
template <typename... Types>
std::ostream& operator<<(std::ostream& os, const std::tuple<Types...>& value)
{
    aux::print_tuple(value, std::make_index_sequence<sizeof...(Types)>(), os);
    return os;
}
}

#endif //TESTS_TUPLE_PRINT_H
