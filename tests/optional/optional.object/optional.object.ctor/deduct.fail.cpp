//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <Optional>
// UNSUPPORTED: c++98, c++03, c++11, c++14
// UNSUPPORTED: clang-5
// UNSUPPORTED: libcpp-no-deduction-guides
// Clang 5 will generate bad implicit deduction guides
//  Specifically, for the copy constructor.


// template<class T>
//   Optional(T) -> Optional<T>;


#include "tim/optional/Optional.hpp"
#include <cassert>

struct A {};

int main(int, char**)
{
//  Test the explicit deduction guides

//  Test the implicit deduction guides
    {
//  Optional()
    tim::Optional opt;   // expected-error-re {{{{declaration of variable 'opt' with deduced type 'tim::Optional' requires an initializer|no viable constructor or deduction guide for deduction of template arguments of 'Optional'}}}}
//  clang-6 gives a bogus error here:
//      declaration of variable 'opt' with deduced type 'tim::Optional' requires an initializer
//  clang-7 (and later) give a better message:
//      no viable constructor or deduction guide for deduction of template arguments of 'Optional'
//  So we check for one or the other.
    }

    {
//  Optional(nullopt_t)
    tim::Optional opt(tim::nullopt);   // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with nullopt_t is ill-formed"}}
    }

  return 0;
}
