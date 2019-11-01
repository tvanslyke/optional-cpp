//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <Optional>
// UNSUPPORTED: c++98, c++03, c++11, c++14
// UNSUPPORTED: clang-5, apple-clang-9
// UNSUPPORTED: libcpp-no-deduction-guides
// Clang 5 will generate bad implicit deduction guides
//  Specifically, for the copy constructor.


// template<class T>
//   Optional(T) -> Optional<T>;


#include "tim/optional/Optional.hpp"
#include <cassert>

#include "test_macros.h"

struct A {};

int main(int, char**)
{
//  Test the explicit deduction guides
    {
//  Optional(T)
    tim::Optional opt(5);
    static_assert(std::is_same_v<decltype(opt), tim::Optional<int>>, "");
    assert(static_cast<bool>(opt));
    assert(*opt == 5);
    }

    {
//  Optional(T)
    tim::Optional opt(A{});
    static_assert(std::is_same_v<decltype(opt), tim::Optional<A>>, "");
    assert(static_cast<bool>(opt));
    }

//  Test the implicit deduction guides
    {
//  Optional(Optional);
    tim::Optional<char> source('A');
    tim::Optional opt(source);
    static_assert(std::is_same_v<decltype(opt), tim::Optional<char>>, "");
    assert(static_cast<bool>(opt) == static_cast<bool>(source));
    assert(*opt == *source);
    }

  return 0;
}
