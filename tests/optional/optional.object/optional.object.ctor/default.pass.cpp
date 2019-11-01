//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// constexpr Optional() noexcept;

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

using tim::Optional;

template <class Opt>
void
test_constexpr()
{
    static_assert(std::is_nothrow_default_constructible<Opt>::value, "");
    static_assert(std::is_trivially_destructible<Opt>::value, "");
    static_assert(std::is_trivially_destructible<typename Opt::value_type>::value, "");

    constexpr Opt opt;
    static_assert(static_cast<bool>(opt) == false, "");

    struct test_constexpr_ctor
        : public Opt
    {
        constexpr test_constexpr_ctor() {}
    };
}

template <class Opt>
void
test()
{
    static_assert(std::is_nothrow_default_constructible<Opt>::value, "");
    static_assert(!std::is_trivially_destructible<Opt>::value, "");
    static_assert(!std::is_trivially_destructible<typename Opt::value_type>::value, "");
    {
        Opt opt;
        assert(static_cast<bool>(opt) == false);
    }
    {
        const Opt opt;
        assert(static_cast<bool>(opt) == false);
    }

    struct test_constexpr_ctor
        : public Opt
    {
        constexpr test_constexpr_ctor() {}
    };
}

int main(int, char**)
{
    test_constexpr<Optional<int>>();
    test_constexpr<Optional<int*>>();
    test_constexpr<Optional<ImplicitTypes::NoCtors>>();
    test_constexpr<Optional<NonTrivialTypes::NoCtors>>();
    test_constexpr<Optional<NonConstexprTypes::NoCtors>>();
    test<Optional<NonLiteralTypes::NoCtors>>();
    // EXTENSIONS
#if defined(_LIBCPP_VERSION) && 0 // FIXME these extensions are currently disabled.
    test_constexpr<Optional<int&>>();
    test_constexpr<Optional<const int&>>();
    test_constexpr<Optional<int&>>();
    test_constexpr<Optional<NonLiteralTypes::NoCtors&>>();
    test_constexpr<Optional<NonLiteralTypes::NoCtors&&>>();
#endif

  return 0;
}
