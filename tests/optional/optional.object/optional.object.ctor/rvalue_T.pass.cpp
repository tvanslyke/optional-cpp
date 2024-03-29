//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: c++98, c++03, c++11, c++14

// XFAIL: dylib-has-no-BadOptionalAccess && !libcpp-no-exceptions

// <Optional>

// constexpr Optional(T&& v);

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"


using tim::Optional;


class Z
{
public:
    Z(int) {}
    Z(Z&&) {TEST_THROW(6);}
};


int main(int, char**)
{
    {
        typedef int T;
        constexpr Optional<T> opt(T(5));
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 5, "");

        struct test_constexpr_ctor
            : public Optional<T>
        {
            constexpr test_constexpr_ctor(T&&) {}
        };
    }
    {
        typedef double T;
        constexpr Optional<T> opt(T(3));
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(*opt == 3, "");

        struct test_constexpr_ctor
            : public Optional<T>
        {
            constexpr test_constexpr_ctor(T&&) {}
        };
    }
    {
        const int x = 42;
        Optional<const int> o(std::move(x));
        assert(*o == 42);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        Optional<T> opt = T{3};
        assert(T::alive == 1);
        assert(T::move_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ExplicitTestTypes::TestType T;
        static_assert(!std::is_convertible<T&&, Optional<T>>::value, "");
        T::reset();
        Optional<T> opt(T{3});
        assert(T::alive == 1);
        assert(T::move_constructed == 1);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef TestTypes::TestType T;
        T::reset();
        Optional<T> opt = {3};
        assert(T::alive == 1);
        assert(T::value_constructed == 1);
        assert(T::copy_constructed == 0);
        assert(T::move_constructed == 0);
        assert(static_cast<bool>(opt) == true);
        assert(opt.value().value == 3);
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr Optional<T> opt = {T(3)};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public Optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };
    }
    {
        typedef ConstexprTestTypes::TestType T;
        constexpr Optional<T> opt = {3};
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public Optional<T>
        {
            constexpr test_constexpr_ctor(const T&) {}
        };
    }
    {
        typedef ExplicitConstexprTestTypes::TestType T;
        static_assert(!std::is_convertible<T&&, Optional<T>>::value, "");
        constexpr Optional<T> opt(T{3});
        static_assert(static_cast<bool>(opt) == true, "");
        static_assert(opt.value().value == 3, "");

        struct test_constexpr_ctor
            : public Optional<T>
        {
            constexpr test_constexpr_ctor(T&&) {}
        };

    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        struct Z {
            Z(int) {}
            Z(Z&&) {throw 6;}
        };
        typedef Z T;
        try
        {
            T t(3);
            Optional<T> opt(std::move(t));
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
