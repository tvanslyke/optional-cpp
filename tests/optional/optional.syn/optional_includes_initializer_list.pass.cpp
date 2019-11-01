//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// #include <initializer_list>

#include "tim/optional/Optional.hpp"

#include "test_macros.h"

int main(int, char**)
{
    using tim::Optional;

    std::initializer_list<int> list;
    (void)list;

  return 0;
}
