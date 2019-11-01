//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class T> constexpr bool operator>=(const Optional<T>& x, nullopt_t) noexcept;
// template <class T> constexpr bool operator>=(nullopt_t, const Optional<T>& x) noexcept;

#include "tim/optional/Optional.hpp"

#include "test_macros.h"

int main(int, char**)
{
    using tim::Optional;
    using tim::nullopt_t;
    using tim::nullopt;

    {
    typedef int T;
    typedef Optional<T> O;

    constexpr O o1;     // disengaged
    constexpr O o2{1};  // engaged

    static_assert (  (nullopt >= o1), "" );
    static_assert ( !(nullopt >= o2), "" );
    static_assert (  (o1 >= nullopt), "" );
    static_assert (  (o2 >= nullopt), "" );

    static_assert (noexcept(nullopt >= o1), "");
    static_assert (noexcept(o1 >= nullopt), "");
    }

  return 0;
}
