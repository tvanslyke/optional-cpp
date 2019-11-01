//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class U>
//   Optional(Optional<U>&& rhs);

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <memory>
#include <cassert>

#include "test_macros.h"

using tim::Optional;

template <class T, class U>
void
test(Optional<U>&& rhs, bool is_going_to_throw = false)
{
    bool rhs_engaged = static_cast<bool>(rhs);
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        Optional<T> lhs = std::move(rhs);
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#else
    if (is_going_to_throw) return;
    Optional<T> lhs = std::move(rhs);
    assert(static_cast<bool>(lhs) == rhs_engaged);
#endif
}

class X
{
    int i_;
public:
    X(int i) : i_(i) {}
    X(X&& x) : i_(std::exchange(x.i_, 0)) {}
    ~X() {i_ = 0;}
    friend bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

int count = 0;

struct Z
{
    Z(int) { TEST_THROW(6); }
};

int main(int, char**)
{
    {
        Optional<short> rhs;
        test<int>(std::move(rhs));
    }
    {
        Optional<short> rhs(short{3});
        test<int>(std::move(rhs));
    }
    {
        Optional<int> rhs;
        test<X>(std::move(rhs));
    }
    {
        Optional<int> rhs(3);
        test<X>(std::move(rhs));
    }
    {
        Optional<int> rhs;
        test<Z>(std::move(rhs));
    }
    {
        Optional<int> rhs(3);
        test<Z>(std::move(rhs), true);
    }

    static_assert(!(std::is_constructible<Optional<X>, Optional<Z>>::value), "");

  return 0;
}
