//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// Optional<T>& operator=(Optional<T>&& rhs)
//     noexcept(is_nothrow_move_assignable<T>::value &&
//              is_nothrow_move_constructible<T>::value); // constexpr in C++20

#include "tim/optional/Optional.hpp"
#include <cassert>
#include <type_traits>
#include <utility>

#include "test_macros.h"
#include "archetypes.h"

using tim::Optional;

struct X
{
    static bool throw_now;
    static int alive;

    X() { ++alive; }
    X(X&&)
    {
        if (throw_now)
            TEST_THROW(6);
        ++alive;
    }

    X& operator=(X&&)
    {
        if (throw_now)
            TEST_THROW(42);
        return *this;
    }

    ~X() { assert(alive > 0); --alive; }
};

struct Y {};

bool X::throw_now = false;
int X::alive = 0;


template <class Tp>
constexpr bool assign_empty(Optional<Tp>&& lhs) {
    Optional<Tp> rhs;
    lhs = std::move(rhs);
    return !lhs.has_value() && !rhs.has_value();
}

template <class Tp>
constexpr bool assign_value(Optional<Tp>&& lhs) {
    Optional<Tp> rhs(101);
    lhs = std::move(rhs);
    return lhs.has_value() && rhs.has_value() && *lhs == Tp{101};
}

int main(int, char**)
{
    {
        static_assert(std::is_nothrow_move_assignable<Optional<int>>::value, "");
        Optional<int> opt;
        constexpr Optional<int> opt2;
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == false, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        Optional<int> opt;
        constexpr Optional<int> opt2(2);
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == true, "");
        static_assert(*opt2 == 2, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }
    {
        Optional<int> opt(3);
        constexpr Optional<int> opt2;
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == false, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        Optional<T> opt(3);
        Optional<T> opt2;
        assert(T::alive == 1);
        opt = std::move(opt2);
        assert(T::alive == 0);
        assert(static_cast<bool>(opt2) == false);
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
    }
    {
        Optional<int> opt(3);
        constexpr Optional<int> opt2(2);
        opt = std::move(opt2);
        static_assert(static_cast<bool>(opt2) == true, "");
        static_assert(*opt2 == 2, "");
        assert(static_cast<bool>(opt) == static_cast<bool>(opt2));
        assert(*opt == *opt2);
    }
    {
        using O = Optional<int>;
#if TEST_STD_VER > 17
        LIBCPP_STATIC_ASSERT(assign_empty(O{42}), "");
        LIBCPP_STATIC_ASSERT(assign_value(O{42}), "");
#endif
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using O = Optional<TrivialTestTypes::TestType>;
#if TEST_STD_VER > 17
        LIBCPP_STATIC_ASSERT(assign_empty(O{42}), "");
        LIBCPP_STATIC_ASSERT(assign_value(O{42}), "");
#endif
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        static_assert(!std::is_nothrow_move_assignable<Optional<X>>::value, "");
        X::alive = 0;
        X::throw_now = false;
        Optional<X> opt;
        Optional<X> opt2(X{});
        assert(X::alive == 1);
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = std::move(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
        }
        assert(X::alive == 1);
    }
    assert(X::alive == 0);
    {
        static_assert(!std::is_nothrow_move_assignable<Optional<X>>::value, "");
        X::throw_now = false;
        Optional<X> opt(X{});
        Optional<X> opt2(X{});
        assert(X::alive == 2);
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = std::move(opt2);
            assert(false);
        }
        catch (int i)
        {
            assert(i == 42);
            assert(static_cast<bool>(opt) == true);
        }
        assert(X::alive == 2);
    }
    assert(X::alive == 0);
#endif // TEST_HAS_NO_EXCEPTIONS
    {
        static_assert(std::is_nothrow_move_assignable<Optional<Y>>::value, "");
    }
    {
        struct ThrowsMove {
            ThrowsMove() noexcept {}
            ThrowsMove(ThrowsMove const&) noexcept {}
            ThrowsMove(ThrowsMove &&) noexcept(false) {}
            ThrowsMove& operator=(ThrowsMove const&) noexcept { return *this; }
            ThrowsMove& operator=(ThrowsMove &&) noexcept { return *this; }
        };
        static_assert(!std::is_nothrow_move_assignable<Optional<ThrowsMove>>::value, "");
        struct ThrowsMoveAssign {
            ThrowsMoveAssign() noexcept {}
            ThrowsMoveAssign(ThrowsMoveAssign const&) noexcept {}
            ThrowsMoveAssign(ThrowsMoveAssign &&) noexcept {}
            ThrowsMoveAssign& operator=(ThrowsMoveAssign const&) noexcept { return *this; }
            ThrowsMoveAssign& operator=(ThrowsMoveAssign &&) noexcept(false) { return *this; }
        };
        static_assert(!std::is_nothrow_move_assignable<Optional<ThrowsMoveAssign>>::value, "");
        struct NoThrowMove {
            NoThrowMove() noexcept(false) {}
            NoThrowMove(NoThrowMove const&) noexcept(false) {}
            NoThrowMove(NoThrowMove &&) noexcept {}
            NoThrowMove& operator=(NoThrowMove const&) noexcept { return *this; }
            NoThrowMove& operator=(NoThrowMove&&) noexcept { return *this; }
        };
        static_assert(std::is_nothrow_move_assignable<Optional<NoThrowMove>>::value, "");
    }
    return 0;
}
