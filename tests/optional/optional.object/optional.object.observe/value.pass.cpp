//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14

// XFAIL: dylib-has-no-BadOptionalAccess && !libcpp-no-exceptions

// <Optional>

// constexpr T& Optional<T>::value() &;

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

using tim::Optional;
using tim::BadOptionalAccess;

struct X
{
    X() = default;
    X(const X&) = delete;
    constexpr int test() const & {return 3;}
    int test() & {return 4;}
    constexpr int test() const && {return 5;}
    int test() && {return 6;}
};

struct Y
{
    constexpr int test() & {return 7;}
};

constexpr int
test()
{
    Optional<Y> opt{Y{}};
    return opt.value().test();
}


int main(int, char**)
{
    {
        Optional<X> opt; ((void)opt);
        ASSERT_NOT_NOEXCEPT(opt.value());
        ASSERT_SAME_TYPE(decltype(opt.value()), X&);
    }
    {
        Optional<X> opt;
        opt.emplace();
        assert(opt.value().test() == 4);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        Optional<X> opt;
        try
        {
            (void)opt.value();
            assert(false);
        }
        catch (const BadOptionalAccess&)
        {
        }
    }
#endif
    static_assert(test() == 7, "");

  return 0;
}
