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
/// \file set_print.h
/// \brief Support for outputting std::set to std::ostream
/// \author Leo Goodstadt
/// Contact intervaldict@llew.org.uk

#ifndef TESTS_PRINT_VECTOR_H
#define TESTS_PRINT_VECTOR_H

#include <iostream>
#include <set>

/*
 * To dump errors for debugging
 */
namespace std
{
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& values)
{
    if (!values.empty())
    {
        os << "[ ";
        auto ii = values.begin();
        os << *ii;
        while (++ii != values.end())
        {
            os << ", " << *ii;
        }
        os << " ]";
    }
    return os;
}
} // namespace std

#endif // TESTS_PRINT_VECTOR_H
