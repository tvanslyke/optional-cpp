//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// void swap(Optional&)
//     noexcept(is_nothrow_move_constructible<T>::value &&
//              is_nothrow_swappable<T>::value)

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

using tim::Optional;

class X
{
    int i_;
public:
    static unsigned dtor_called;
    X(int i) : i_(i) {}
    X(X&& x) = default;
    X& operator=(X&&) = default;
    ~X() {++dtor_called;}

    friend bool operator==(const X& x, const X& y) {return x.i_ == y.i_;}
};

unsigned X::dtor_called = 0;

class Y
{
    int i_;
public:
    static unsigned dtor_called;
    Y(int i) : i_(i) {}
    Y(Y&&) = default;
    ~Y() {++dtor_called;}

    friend constexpr bool operator==(const Y& x, const Y& y) {return x.i_ == y.i_;}
    friend void swap(Y& x, Y& y) {std::swap(x.i_, y.i_);}
};

unsigned Y::dtor_called = 0;

class Z
{
    int i_;
public:
    Z(int i) : i_(i) {}
    Z(Z&&) {TEST_THROW(7);}

    friend constexpr bool operator==(const Z& x, const Z& y) {return x.i_ == y.i_;}
    friend void swap(Z&, Z&) {TEST_THROW(6);}
};


int main(int, char**)
{
    {
        Optional<int> opt1;
        Optional<int> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<int> opt1(1);
        Optional<int> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<int> opt1;
        Optional<int> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<int> opt1(1);
        Optional<int> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<X> opt1;
        Optional<X> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(X::dtor_called == 0);
    }
    {
        Optional<X> opt1(1);
        Optional<X> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<X> opt1;
        Optional<X> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<X> opt1(1);
        Optional<X> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        opt1.swap(opt2);
        assert(X::dtor_called == 1);  // from inside std::swap
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<Y> opt1;
        Optional<Y> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(Y::dtor_called == 0);
    }
    {
        Optional<Y> opt1(1);
        Optional<Y> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<Y> opt1;
        Optional<Y> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<Y> opt1(1);
        Optional<Y> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        opt1.swap(opt2);
        assert(Y::dtor_called == 0);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<Z> opt1;
        Optional<Z> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        opt1.swap(opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        Optional<Z> opt1;
        opt1.emplace(1);
        Optional<Z> opt2;
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        try
        {
            opt1.swap(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<Z> opt1;
        Optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            opt1.swap(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 7);
        }
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
    {
        Optional<Z> opt1;
        opt1.emplace(1);
        Optional<Z> opt2;
        opt2.emplace(2);
        static_assert(noexcept(opt1.swap(opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            opt1.swap(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
        }
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
    }
#endif

  return 0;
}
