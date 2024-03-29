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
//   Optional(const Optional<U>& rhs);

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

using tim::Optional;

template <class T, class U>
void
test(const Optional<U>& rhs, bool is_going_to_throw = false)
{
    bool rhs_engaged = static_cast<bool>(rhs);
#ifndef TEST_HAS_NO_EXCEPTIONS
    try
    {
        Optional<T> lhs = rhs;
        assert(is_going_to_throw == false);
        assert(static_cast<bool>(lhs) == rhs_engaged);
        if (rhs_engaged)
            assert(*lhs == *rhs);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#else
    if (is_going_to_throw) return;
    Optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == *rhs);
#endif
}

class X
{
    int i_;
public:
    X(int i) : i_(i) {}
    X(const X& x) : i_(x.i_) {}
    ~X() {i_ = 0;}
    friend bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

class Y
{
    int i_;
public:
    Y(int i) : i_(i) {}

    friend constexpr bool operator==(const Y& x, const Y& y) {return x.i_ == y.i_;}
};

int count = 0;

class Z
{
    int i_;
public:
    Z(int i) : i_(i) {TEST_THROW(6);}

    friend constexpr bool operator==(const Z& x, const Z& y) {return x.i_ == y.i_;}
};


int main(int, char**)
{
    {
        typedef short U;
        typedef int T;
        Optional<U> rhs;
        test<T>(rhs);
    }
    {
        typedef short U;
        typedef int T;
        Optional<U> rhs(U{3});
        test<T>(rhs);
    }
    {
        typedef X T;
        typedef int U;
        Optional<U> rhs;
        test<T>(rhs);
    }
    {
        typedef X T;
        typedef int U;
        Optional<U> rhs(U{3});
        test<T>(rhs);
    }
    {
        typedef Y T;
        typedef int U;
        Optional<U> rhs;
        test<T>(rhs);
    }
    {
        typedef Y T;
        typedef int U;
        Optional<U> rhs(U{3});
        test<T>(rhs);
    }
    {
        typedef Z T;
        typedef int U;
        Optional<U> rhs;
        test<T>(rhs);
    }
    {
        typedef Z T;
        typedef int U;
        Optional<U> rhs(U{3});
        test<T>(rhs, true);
    }

    static_assert(!(std::is_constructible<Optional<X>, const Optional<Y>&>::value), "");

  return 0;
}
