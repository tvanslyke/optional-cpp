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

int main(int, char**)
{
    tim::nullopt_t n = {};

  return 0;
}
