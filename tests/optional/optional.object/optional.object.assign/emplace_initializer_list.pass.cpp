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
//   T& Optional<T>::emplace(initializer_list<U> il, Args&&... args);

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>
#include <vector>

#include "test_macros.h"

using tim::Optional;

class X
{
    int i_;
    int j_ = 0;
public:
    static bool dtor_called;
    constexpr X() : i_(0) {}
    constexpr X(int i) : i_(i) {}
    constexpr X(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) {}
    ~X() {dtor_called = true;}

    friend constexpr bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

bool X::dtor_called = false;

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
    static bool dtor_called;
    Z() : i_(0) {}
    Z(int i) : i_(i) {}
    Z(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1])
        { TEST_THROW(6);}
    ~Z() {dtor_called = true;}

    friend bool operator==(const Z& x, const Z& y)
        {return x.i_ == y.i_ && x.j_ == y.j_;}
};

bool Z::dtor_called = false;

int main(int, char**)
{
    {
        X x;
        Optional<X> opt(x);
        assert(X::dtor_called == false);
        auto &v = opt.emplace({1, 2});
        static_assert( std::is_same_v<X&, decltype(v)>, "" );
        assert(X::dtor_called == true);
        assert(*opt == X({1, 2}));
        assert(&v == &*opt);
    }
    {
        Optional<std::vector<int>> opt;
        auto &v = opt.emplace({1, 2, 3}, std::allocator<int>());
        static_assert( std::is_same_v<std::vector<int>&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(*opt == std::vector<int>({1, 2, 3}));
        assert(&v == &*opt);
    }
    {
        Optional<Y> opt;
        auto &v = opt.emplace({1, 2});
        static_assert( std::is_same_v<Y&, decltype(v)>, "" );
        assert(static_cast<bool>(opt) == true);
        assert(*opt == Y({1, 2}));
        assert(&v == &*opt);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        Z z;
        Optional<Z> opt(z);
        try
        {
            assert(static_cast<bool>(opt) == true);
            assert(Z::dtor_called == false);
            auto &v = opt.emplace({1, 2});
            static_assert( std::is_same_v<Z&, decltype(v)>, "" );
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
            assert(Z::dtor_called == true);
        }
    }
#endif

  return 0;
}
