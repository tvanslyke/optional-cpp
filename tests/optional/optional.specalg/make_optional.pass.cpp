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
//
// template <class T>
//   constexpr Optional<decay_t<T>> make_optional(T&& v);

#include "tim/optional/Optional.hpp"
#include <string>
#include <memory>
#include <cassert>

#include "test_macros.h"

int main(int, char**)
{
    using tim::Optional;
    using tim::make_optional;
    {
        int arr[10]; ((void)arr);
        ASSERT_SAME_TYPE(decltype(make_optional(arr)), Optional<int*>);
    }
    {
        constexpr auto opt = make_optional(2);
        ASSERT_SAME_TYPE(decltype(opt), const Optional<int>);
        static_assert(opt.value() == 2);
    }
    {
        Optional<int> opt = make_optional(2);
        assert(*opt == 2);
    }
    {
        std::string s("123");
        Optional<std::string> opt = make_optional(s);
        assert(*opt == s);
    }
    {
        std::unique_ptr<int> s(new int(3));
        Optional<std::unique_ptr<int>> opt = make_optional(std::move(s));
        assert(**opt == 3);
        assert(s == nullptr);
    }

  return 0;
}
