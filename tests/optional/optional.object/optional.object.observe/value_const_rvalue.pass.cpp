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

// constexpr const T& Optional<T>::value() const &&;

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

using tim::Optional;
using tim::in_place_t;
using tim::in_place;
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

int main(int, char**)
{
    {
        const Optional<X> opt; ((void)opt);
        ASSERT_NOT_NOEXCEPT(std::move(opt).value());
        ASSERT_SAME_TYPE(decltype(std::move(opt).value()), X const&&);
    }
    {
        constexpr Optional<X> opt(in_place);
        static_assert(std::move(opt).value().test() == 5, "");
    }
    {
        const Optional<X> opt(in_place);
        assert(std::move(opt).value().test() == 5);
    }
#ifndef TEST_HAS_NO_EXCEPTIONS
    {
        const Optional<X> opt;
        try
        {
            (void)std::move(opt).value();
            assert(false);
        }
        catch (const BadOptionalAccess&)
        {
        }
    }
#endif

  return 0;
}
