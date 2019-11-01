//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14

// XFAIL: dylib-has-no-BadOptionalAccess

// <Optional>

// class BadOptionalAccess is default constructible

#include "tim/optional/Optional.hpp"
#include <type_traits>

#include "test_macros.h"

int main(int, char**)
{
    using tim::BadOptionalAccess;
    BadOptionalAccess ex;

  return 0;
}
