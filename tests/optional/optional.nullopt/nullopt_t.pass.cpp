//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// struct nullopt_t{see below};
// inline constexpr nullopt_t nullopt(unspecified);

// [Optional.nullopt]/2:
//   Type nullopt_t shall not have a default constructor or an initializer-list
//   constructor, and shall not be an aggregate.

#include "tim/optional/Optional.hpp"
#include <type_traits>

#include "test_macros.h"

using tim::nullopt_t;
using tim::nullopt;

constexpr bool test()
{
    nullopt_t foo{nullopt};
    (void)foo;
    return true;
}

int main(int, char**)
{
    static_assert(std::is_empty_v<nullopt_t>);
    static_assert(!std::is_default_constructible_v<nullopt_t>);

    static_assert(std::is_same_v<const nullopt_t, decltype(nullopt)>);
    static_assert(test());

  return 0;
}
