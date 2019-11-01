//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// Optional<T>& operator=(const Optional<T>& rhs); // constexpr in C++20

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

using tim::Optional;

struct X
{
    static bool throw_now;

    X() = default;
    X(const X&)
    {
        if (throw_now)
            TEST_THROW(6);
    }
};

bool X::throw_now = false;

template <class Tp>
constexpr bool assign_empty(Optional<Tp>&& lhs) {
    const Optional<Tp> rhs;
    lhs = rhs;
    return !lhs.has_value() && !rhs.has_value();
}

template <class Tp>
constexpr bool assign_value(Optional<Tp>&& lhs) {
    const Optional<Tp> rhs(101);
    lhs = rhs;
    return lhs.has_value() && rhs.has_value() && *lhs == *rhs;
}

int main(int, char**)
{
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
    {
        using O = Optional<TestTypes::TestType>;
        assert(assign_empty(O{42}));
        assert(assign_value(O{42}));
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        Optional<T> opt(3);
        const Optional<T> opt2;
        assert(T::alive == 1);
        opt = opt2;
        assert(T::alive == 0);
        assert(!opt2.has_value());
        assert(!opt.has_value());
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        Optional<X> opt;
        Optional<X> opt2(X{});
        assert(static_cast<bool>(opt2) == true);
        try
        {
            X::throw_now = true;
            opt = opt2;
            assert(false);
        }
        catch (int i)
        {
            assert(i == 6);
            assert(static_cast<bool>(opt) == false);
        }
    }
#endif

  return 0;
}
