//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class T, class U> constexpr bool operator>=(const Optional<T>& x, const U& v);
// template <class T, class U> constexpr bool operator>=(const U& v, const Optional<T>& x);

#include "tim/optional/Optional.hpp"

#include "test_macros.h"

using tim::Optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator>=(const X& lhs, const X& rhs) {
  return lhs.i_ >= rhs.i_;
}

int main(int, char**) {
  {
    typedef X T;
    typedef Optional<T> O;

    constexpr T val(2);
    constexpr O o1;      // disengaged
    constexpr O o2{1};   // engaged
    constexpr O o3{val}; // engaged

    static_assert(!(o1 >= T(1)), "");
    static_assert((o2 >= T(1)), ""); // equal
    static_assert((o3 >= T(1)), "");
    static_assert(!(o2 >= val), "");
    static_assert((o3 >= val), ""); // equal
    static_assert(!(o3 >= T(3)), "");

    static_assert((T(1) >= o1), "");
    static_assert((T(1) >= o2), ""); // equal
    static_assert(!(T(1) >= o3), "");
    static_assert((val >= o2), "");
    static_assert((val >= o3), ""); // equal
    static_assert((T(3) >= o3), "");
  }
  {
    using O = Optional<int>;
    constexpr O o1(42);
    static_assert(o1 >= 42l, "");
    static_assert(!(11l >= o1), "");
  }
  {
    using O = Optional<const int>;
    constexpr O o1(42);
    static_assert(o1 >= 42, "");
    static_assert(!(11 >= o1), "");
  }

  return 0;
}
