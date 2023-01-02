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
/// \file default_init_allocator.h
/// \brief Allocator for e.g. Standard Containers that does not value
/// initialise elements. Uses inheritance for simplicity
///
/// \author Leo Goodstadt
///
/// Contact intervaldict@llew.org.uk

/// @cond Suppress_Doxygen_Warning

#ifndef INCLUDE_INTERVAL_DICT_DEFAULT_INIT_ALLOCATOR_H
#define INCLUDE_INTERVAL_DICT_DEFAULT_INIT_ALLOCATOR_H

#include <memory>

namespace interval_dict
{
  /// Allocator for standards container (e.g. std::vector) that
  /// does not value initialise elements.
  /// Can provide substantial performance benefits with std::vector<int>
  template<typename T,
           typename A = std::allocator<T>,
           typename = std::enable_if_t<std::is_trivial<T>::value>>
  class DefaultInitAllocator : public A
  {
    public:
    using allocator_traits = std::allocator_traits<A>;
    using value_type = typename allocator_traits::value_type;
    using pointer = typename allocator_traits::pointer;
    using const_pointer = typename allocator_traits::const_pointer;
    using size_type = typename allocator_traits::size_type;
    using difference_type = typename allocator_traits::difference_type;

    // http://en.cppreference.com/w/cpp/language/using_declaration
    using A::A; // Inherit constructors from A

    template<typename U>
    struct rebind
    {
      using other = DefaultInitAllocator<
        U,
        typename allocator_traits::template rebind_alloc<U>>;
    };

    // Do not initialise
    template<typename U>
    void construct (U *ptr) noexcept (
      std::is_nothrow_default_constructible<U>::value)
    {
      ::new (static_cast<void *> (ptr)) U;
    }

    template<typename U, typename... Args>
    void construct (U *ptr, Args &&...args)
    {
      allocator_traits::construct (
        static_cast<A &> (*this), ptr, std::forward<Args> (args)...);
    }

    using propagate_on_container_copy_assignment =
      typename allocator_traits::propagate_on_container_copy_assignment;
    using propagate_on_container_move_assignment =
      typename allocator_traits::propagate_on_container_move_assignment;
    using propagate_on_container_swap =
      typename allocator_traits::propagate_on_container_swap;
  };

} // namespace interval_dict

#endif