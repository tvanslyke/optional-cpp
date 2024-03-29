//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// constexpr T* Optional<T>::operator->();

#ifdef _LIBCPP_DEBUG
#define _LIBCPP_ASSERT(x, m) ((x) ? (void)0 : std::exit(0))
#endif

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

using tim::Optional;

struct X
{
    int test() noexcept {return 3;}
};

struct Y
{
    constexpr int test() {return 3;}
};

constexpr int
test()
{
    Optional<Y> opt{Y{}};
    return opt->test();
}

int main(int, char**)
{
    {
        tim::Optional<X> opt; ((void)opt);
        ASSERT_SAME_TYPE(decltype(opt.operator->()), X*);
        // ASSERT_NOT_NOEXCEPT(opt.operator->());
        // FIXME: This assertion fails with GCC because it can see that
        // (A) operator->() is constexpr, and
        // (B) there is no path through the function that throws.
        // It's arguable if this is the correct behavior for the noexcept
        // operator.
        // Regardless this function should still be noexcept(false) because
        // it has a narrow contract.
    }
    {
        Optional<X> opt(X{});
        assert(opt->test() == 3);
    }
    {
        static_assert(test() == 3, "");
    }
#ifdef _LIBCPP_DEBUG
    {
        Optional<X> opt;
        assert(opt->test() == 3);
        assert(false);
    }
#endif  // _LIBCPP_DEBUG

  return 0;
}
