//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// constexpr Optional(const Optional<T>&& rhs);
//  C++17 said:
//   If is_trivially_move_constructible_v<T> is true,
//    this constructor shall be a constexpr constructor.
//
//  P0602 changed this to:
//     If is_trivially_move_constructible_v<T> is true, this constructor is trivial.
//
//  which means that it can't be constexpr if T is not trivially move-constructible,
//    because you have to do a placement new to get the value into place.
//    Except in the case where it is moving from an empty Optional - that could be
//    made to be constexpr (and libstdc++ does so).

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

struct S {
    constexpr S()   : v_(0) {}
    S(int v)        : v_(v) {}
    constexpr S(const S  &rhs) : v_(rhs.v_) {} // not trivially moveable
    constexpr S(      S &&rhs) : v_(rhs.v_) {} // not trivially moveable
    int v_;
    };


constexpr bool test() // expected-error {{constexpr function never produces a constant expression}}
{
    tim::Optional<S> o1{3};
    tim::Optional<S> o2 = std::move(o1);
    return o2.has_value();  // return -something-
}


int main(int, char**)
{
    static_assert (!std::is_trivially_move_constructible_v<S>, "" );
    static_assert (test(), "");  // expected-error {{static_assert expression is not an integral constant expression}}
    return 0;
}
