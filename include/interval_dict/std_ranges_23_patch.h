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
/// \file std_ranges_23_path.h
/// \brief Functions to make up deficiencies in std::ranges before c++23
/// \author Leo Goodstadt

/// Contact intervaldict@llew.org.uk
#ifndef INTERVAL_DICT_STD_RANGES_23_PATCH_H
#define INTERVAL_DICT_STD_RANGES_23_PATCH_H

#include <algorithm>
#include <ranges>
#include <set>
#include <unordered_set>
#include <vector>

/// Get rid of after c++23
/// From stack overflow 58808030 Rupert Nash
namespace legacy_ranges_conversion
{
  namespace detail
  {
    // Type acts as a tag to find the correct operator| overload
    struct ToVectorTag
    {
    };
    struct ToSetTag
    {
    };
    struct ToUnorderedSetTag
    {
    };

    template<std::ranges::range Range>
      requires std::ranges::input_range<Range>
    auto operator| (Range &&range, ToVectorTag)
    {
      std::vector<std::ranges::range_value_t<Range>> results;
      if constexpr (std::ranges::sized_range<Range>)
      {
        results.reserve (std::ranges::size (range));
      }
      std::ranges::copy (range, std::back_inserter (results));
      return results;
    }

    template<std::ranges::range Range>
      requires std::ranges::input_range<Range>
    auto operator| (Range &&range, ToSetTag)
    {
      std::set<std::ranges::range_value_t<Range>> results;
      std::ranges::copy (range, std::inserter (results, results.end ()));
      return results;
    }

    template<std::ranges::range Range>
      requires std::ranges::input_range<Range>
    auto operator| (Range &&range, ToUnorderedSetTag)
    {
      std::unordered_set<std::ranges::range_value_t<Range>> results;
      std::ranges::copy (range, std::inserter (results, results.end ()));
      return results;
    }
  } // namespace detail

  // Resolve with ADL
  inline auto to_vector ()
  {
    return detail::ToVectorTag {};
  }
  inline auto to_set ()
  {
    return detail::ToSetTag {};
  }
  inline auto to_unordered_set ()
  {
    return detail::ToUnorderedSetTag {};
  }
} // namespace legacy_ranges_conversion
#endif // INTERVAL_DICT_STD_RANGES_23_PATCH_H
