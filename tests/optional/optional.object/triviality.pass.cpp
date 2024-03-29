//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14, c++17

// <Optional>

// The following special member functions should propagate the triviality of
// the element held in the Optional (see P0602R4):
//
// constexpr Optional(const Optional& rhs);
// constexpr Optional(Optional&& rhs) noexcept(see below);
// constexpr Optional<T>& operator=(const Optional& rhs);
// constexpr Optional<T>& operator=(Optional&& rhs) noexcept(see below);


#include "tim/optional/Optional.hpp"
#include <type_traits>

#include "archetypes.h"

#include "test_macros.h"


constexpr bool implies(bool p, bool q) {
    return !p || q;
}

template <class T>
struct SpecialMemberTest {
    using O = tim::Optional<T>;

    static_assert(implies(std::is_trivially_copy_constructible_v<T>,
                          std::is_trivially_copy_constructible_v<O>),
        "Optional<T> is trivially copy constructible if T is trivially copy constructible.");

    static_assert(implies(std::is_trivially_move_constructible_v<T>,
                          std::is_trivially_move_constructible_v<O>),
        "Optional<T> is trivially move constructible if T is trivially move constructible");

    static_assert(implies(std::is_trivially_copy_constructible_v<T> &&
                          std::is_trivially_copy_assignable_v<T> &&
                          std::is_trivially_destructible_v<T>,

                          std::is_trivially_copy_assignable_v<O>),
        "Optional<T> is trivially copy assignable if T is "
        "trivially copy constructible, "
        "trivially copy assignable, and "
        "trivially destructible");

    static_assert(implies(std::is_trivially_move_constructible_v<T> &&
                          std::is_trivially_move_assignable_v<T> &&
                          std::is_trivially_destructible_v<T>,

                          std::is_trivially_move_assignable_v<O>),
        "Optional<T> is trivially move assignable if T is "
        "trivially move constructible, "
        "trivially move assignable, and"
        "trivially destructible.");
};

template <class ...Args> static void sink(Args&&...) {}

template <class ...TestTypes>
struct DoTestsMetafunction {
    DoTestsMetafunction() { sink(SpecialMemberTest<TestTypes>{}...); }
};

struct TrivialMoveNonTrivialCopy {
    TrivialMoveNonTrivialCopy() = default;
    TrivialMoveNonTrivialCopy(const TrivialMoveNonTrivialCopy&) {}
    TrivialMoveNonTrivialCopy(TrivialMoveNonTrivialCopy&&) = default;
    TrivialMoveNonTrivialCopy& operator=(const TrivialMoveNonTrivialCopy&) { return *this; }
    TrivialMoveNonTrivialCopy& operator=(TrivialMoveNonTrivialCopy&&) = default;
};

struct TrivialCopyNonTrivialMove {
    TrivialCopyNonTrivialMove() = default;
    TrivialCopyNonTrivialMove(const TrivialCopyNonTrivialMove&) = default;
    TrivialCopyNonTrivialMove(TrivialCopyNonTrivialMove&&) {}
    TrivialCopyNonTrivialMove& operator=(const TrivialCopyNonTrivialMove&) = default;
    TrivialCopyNonTrivialMove& operator=(TrivialCopyNonTrivialMove&&) { return *this; }
};

int main(int, char**) {
    sink(
        ImplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        ExplicitTypes::ApplyTypes<DoTestsMetafunction>{},
        NonLiteralTypes::ApplyTypes<DoTestsMetafunction>{},
        NonTrivialTypes::ApplyTypes<DoTestsMetafunction>{},
        DoTestsMetafunction<TrivialMoveNonTrivialCopy, TrivialCopyNonTrivialMove>{}
    );
    return 0;
}
