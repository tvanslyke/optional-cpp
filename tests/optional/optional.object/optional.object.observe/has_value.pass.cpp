//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// constexpr bool Optional<T>::has_value() const noexcept;

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

int main(int, char**)
{
    using tim::Optional;
    {
        const Optional<int> opt; ((void)opt);
        ASSERT_NOEXCEPT(opt.has_value());
        ASSERT_SAME_TYPE(decltype(opt.has_value()), bool);
    }
    {
        constexpr Optional<int> opt;
        static_assert(!opt.has_value(), "");
    }
    {
        constexpr Optional<int> opt(0);
        static_assert(opt.has_value(), "");
    }

  return 0;
}
