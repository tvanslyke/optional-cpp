//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class T, class U> constexpr bool operator> (const Optional<T>& x, const Optional<U>& y);

#include "tim/optional/Optional.hpp"

#include "test_macros.h"

using tim::Optional;

struct X {
  int i_;

  constexpr X(int i) : i_(i) {}
};

constexpr bool operator>(const X& lhs, const X& rhs) { return lhs.i_ > rhs.i_; }

int main(int, char**) {
  {
    typedef Optional<X> O;

    constexpr O o1;    // disengaged
    constexpr O o2;    // disengaged
    constexpr O o3{1}; // engaged
    constexpr O o4{2}; // engaged
    constexpr O o5{1}; // engaged

    static_assert(!(o1 > o1), "");
    static_assert(!(o1 > o2), "");
    static_assert(!(o1 > o3), "");
    static_assert(!(o1 > o4), "");
    static_assert(!(o1 > o5), "");

    static_assert(!(o2 > o1), "");
    static_assert(!(o2 > o2), "");
    static_assert(!(o2 > o3), "");
    static_assert(!(o2 > o4), "");
    static_assert(!(o2 > o5), "");

    static_assert((o3 > o1), "");
    static_assert((o3 > o2), "");
    static_assert(!(o3 > o3), "");
    static_assert(!(o3 > o4), "");
    static_assert(!(o3 > o5), "");

    static_assert((o4 > o1), "");
    static_assert((o4 > o2), "");
    static_assert((o4 > o3), "");
    static_assert(!(o4 > o4), "");
    static_assert((o4 > o5), "");

    static_assert((o5 > o1), "");
    static_assert((o5 > o2), "");
    static_assert(!(o5 > o3), "");
    static_assert(!(o5 > o4), "");
    static_assert(!(o5 > o5), "");
  }
  {
    using O1 = Optional<int>;
    using O2 = Optional<long>;
    constexpr O1 o1(42);
    static_assert(o1 > O2(1), "");
    static_assert(!(O2(42) > o1), "");
  }
  {
    using O1 = Optional<int>;
    using O2 = Optional<const int>;
    constexpr O1 o1(42);
    static_assert(o1 > O2(1), "");
    static_assert(!(O2(42) > o1), "");
  }

  return 0;
}
