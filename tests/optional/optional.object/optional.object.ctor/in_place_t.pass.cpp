//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: c++98, c++03, c++11, c++14

// <Optional>

// template <class... Args>
//   constexpr explicit Optional(in_place_t, Args&&... args);

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

using tim::Optional;
using tim::in_place_t;
using tim::in_place;

class X
{
    int i_;
    int j_ = 0;
public:
    X() : i_(0) {}
    X(int i) : i_(i) {}
    X(int i, int j) : i_(i), j_(j) {}

    ~X() {}

    friend bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Y
{
    int i_;
    int j_ = 0;
public:
    constexpr Y() : i_(0) {}
    constexpr Y(int i) : i_(i) {}
    constexpr Y(int i, int j) : i_(i), j_(j) {}

    friend constexpr bool operator==(const Y& x, const Y& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Z
{
public:
    Z(int) {TEST_THROW(6);}
};


int main(int, char**)
{
    {
        constexpr Optional<int> opt(in_place, 5);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor
            : public Optional<int>
        {
            constexpr test_constexpr_ctor(in_place_t, int i)
                : Optional<int>(in_place, i) {}
        };

    }
    {
        Optional<const int> opt(in_place, 5);
        assert(*opt == 5);
    }
    {
        const Optional<X> opt(in_place);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X());
    }
    {
        const Optional<X> opt(in_place, 5);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X(5));
    }
    {
        const Optional<X> opt(in_place, 5, 4);
        assert(static_cast<bool>(opt) == true);
        assert(*opt == X(5, 4));
    }
    {
        constexpr Optional<Y> opt(in_place);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(), "");

        struct test_constexpr_ctor
            : public Optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t)
                : Optional<Y>(in_place) {}
        };

    }
    {
        constexpr Optional<Y> opt(in_place, 5);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(5), "");

        struct test_constexpr_ctor
            : public Optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t, int i)
                : Optional<Y>(in_place, i) {}
        };

    }
    {
        constexpr Optional<Y> opt(in_place, 5, 4);
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y(5, 4), "");

        struct test_constexpr_ctor
            : public Optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t, int i, int j)
                : Optional<Y>(in_place, i, j) {}
        };

    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        try
        {
            const Optional<Z> opt(in_place, 1);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
    }
#endif

  return 0;
}
