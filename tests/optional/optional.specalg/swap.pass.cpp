//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class T> void swap(Optional<T>& x, Optional<T>& y)
//     noexcept(noexcept(x.swap(y)));

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
    Z(Z&&) { TEST_THROW(7);}

    friend constexpr bool operator==(const Z& x, const Z& y) {return x.i_ == y.i_;}
    friend void swap(Z&, Z&) { TEST_THROW(6);}
};


struct NonSwappable {
    NonSwappable(NonSwappable const&) = delete;
};
void swap(NonSwappable&, NonSwappable&) = delete;

void test_swap_sfinae() {
    using tim::Optional;
    {
        using T = TestTypes::TestType;
        static_assert(std::is_swappable_v<Optional<T>>, "");
    }
    {
        using T = TestTypes::MoveOnly;
        static_assert(std::is_swappable_v<Optional<T>>, "");
    }
    {
        using T = TestTypes::Copyable;
        static_assert(std::is_swappable_v<Optional<T>>, "");
    }
    {
        using T = TestTypes::NoCtors;
        static_assert(!std::is_swappable_v<Optional<T>>, "");
    }
    {
        using T = NonSwappable;
        static_assert(!std::is_swappable_v<Optional<T>>, "");
    }
    {
        // Even thought CopyOnly has deleted move operations, those operations
        // cause Optional<CopyOnly> to have implicitly deleted move operations
        // that decay into copies.
        using T = TestTypes::CopyOnly;
        using Opt = Optional<T>;
        T::reset();
        Opt L(101), R(42);
        T::reset_constructors();
        std::swap(L, R);
        assert(L->value == 42);
        assert(R->value == 101);
        assert(T::copy_constructed == 1);
        assert(T::constructed == T::copy_constructed);
        assert(T::assigned == 2);
        assert(T::assigned == T::copy_assigned);
    }
}

int main(int, char**)
{
    test_swap_sfinae();
    {
        Optional<int> opt1;
        Optional<int> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<int> opt1(1);
        Optional<int> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<int> opt1;
        Optional<int> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<int> opt1(1);
        Optional<int> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<X> opt1;
        Optional<X> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(X::dtor_called == 0);
    }
    {
        Optional<X> opt1(1);
        Optional<X> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        X::dtor_called = 0;
        swap(opt1, opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<X> opt1;
        Optional<X> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        swap(opt1, opt2);
        assert(X::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<X> opt1(1);
        Optional<X> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == true, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        X::dtor_called = 0;
        swap(opt1, opt2);
        assert(X::dtor_called == 1);  // from inside std::swap
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<Y> opt1;
        Optional<Y> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        assert(Y::dtor_called == 0);
    }
    {
        Optional<Y> opt1(1);
        Optional<Y> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<Y> opt1;
        Optional<Y> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        assert(Y::dtor_called == 1);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == false);
    }
    {
        Optional<Y> opt1(1);
        Optional<Y> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        assert(Y::dtor_called == 0);
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 2);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 1);
    }
    {
        Optional<Z> opt1;
        Optional<Z> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
        swap(opt1, opt2);
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == false);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        Optional<Z> opt1;
        opt1.emplace(1);
        Optional<Z> opt2;
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == false);
        try
        {
            swap(opt1, opt2);
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
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == false);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            swap(opt1, opt2);
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
        static_assert(noexcept(swap(opt1, opt2)) == false, "");
        assert(static_cast<bool>(opt1) == true);
        assert(*opt1 == 1);
        assert(static_cast<bool>(opt2) == true);
        assert(*opt2 == 2);
        try
        {
            swap(opt1, opt2);
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
#endif // TEST_HAS_NO_EXCEPTIONS

  return 0;
}
