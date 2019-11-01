//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class U, class... Args>
//     constexpr
//     explicit Optional(in_place_t, initializer_list<U> il, Args&&... args);

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <vector>
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
    constexpr Y(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) {}

    friend constexpr bool operator==(const Y& x, const Y& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

class Z
{
    int i_;
    int j_ = 0;
public:
    Z() : i_(0) {}
    Z(int i) : i_(i) {}
    Z(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1])
        {TEST_THROW(6);}

    friend bool operator==(const Z& x, const Z& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

int main(int, char**)
{
    {
        static_assert(!std::is_constructible<X, std::initializer_list<int>&>::value, "");
        static_assert(!std::is_constructible<Optional<X>, std::initializer_list<int>&>::value, "");
    }
    {
        Optional<std::vector<int>> opt(in_place, {3, 1});
        assert(static_cast<bool>(opt) == true);
        assert((*opt == std::vector<int>{3, 1}));
        assert(opt->size() == 2);
    }
    {
        Optional<std::vector<int>> opt(in_place, {3, 1}, std::allocator<int>());
        assert(static_cast<bool>(opt) == true);
        assert((*opt == std::vector<int>{3, 1}));
        assert(opt->size() == 2);
    }
    {
        static_assert(std::is_constructible<Optional<Y>, std::initializer_list<int>&>::value, "");
        constexpr Optional<Y> opt(in_place, {3, 1});
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == Y{3, 1}, "");

        struct test_constexpr_ctor
            : public Optional<Y>
        {
            constexpr test_constexpr_ctor(in_place_t, std::initializer_list<int> i)
                : Optional<Y>(in_place, i) {}
        };

    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        static_assert(std::is_constructible<Optional<Z>, std::initializer_list<int>&>::value, "");
        try
        {
            Optional<Z> opt(in_place, {3, 1});
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
