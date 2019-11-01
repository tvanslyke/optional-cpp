//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// constexpr Optional(const Optional<T>& rhs);

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "archetypes.h"

using tim::Optional;

template <class T, class ...InitArgs>
void test(InitArgs&&... args)
{
    const Optional<T> rhs(std::forward<InitArgs>(args)...);
    bool rhs_engaged = static_cast<bool>(rhs);
    Optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(*lhs == *rhs);
}

template <class T, class ...InitArgs>
constexpr bool constexpr_test(InitArgs&&... args)
{
    static_assert( std::is_trivially_copy_constructible_v<T>, ""); // requirement
    const Optional<T> rhs(std::forward<InitArgs>(args)...);
    Optional<T> lhs = rhs;
    return (lhs.has_value() == rhs.has_value()) &&
           (lhs.has_value() ? *lhs == *rhs : true);
}

void test_throwing_ctor() {
#ifndef TEST_HAS_NO_EXCEPTIONS
    struct Z {
        Z() : count(0) {}
        Z(Z const& o) : count(o.count + 1)
        { if (count == 2) throw 6; }
        int count;
    };
    const Z z;
    const Optional<Z> rhs(z);
    try
    {
        Optional<Z> lhs(rhs);
        assert(false);
    }
    catch (int i)
    {
        assert(i == 6);
    }
#endif
}

template <class T, class ...InitArgs>
void test_ref(InitArgs&&... args)
{
    const Optional<T> rhs(std::forward<InitArgs>(args)...);
    bool rhs_engaged = static_cast<bool>(rhs);
    Optional<T> lhs = rhs;
    assert(static_cast<bool>(lhs) == rhs_engaged);
    if (rhs_engaged)
        assert(&(*lhs) == &(*rhs));
}


void test_reference_extension()
{
#if defined(_LIBCPP_VERSION) && 0 // FIXME these extensions are currently disabled.
    using T = TestTypes::TestType;
    T::reset();
    {
        T t;
        T::reset_constructors();
        test_ref<T&>();
        test_ref<T&>(t);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::destroyed == 1);
    assert(T::alive == 0);
    {
        T t;
        const T& ct = t;
        T::reset_constructors();
        test_ref<T const&>();
        test_ref<T const&>(t);
        test_ref<T const&>(ct);
        assert(T::alive == 1);
        assert(T::constructed == 0);
        assert(T::assigned == 0);
        assert(T::destroyed == 0);
    }
    assert(T::alive == 0);
    assert(T::destroyed == 1);
    {
        static_assert(!std::is_copy_constructible<tim::Optional<T&&>>::value, "");
        static_assert(!std::is_copy_constructible<tim::Optional<T const&&>>::value, "");
    }
#endif
}

int main(int, char**)
{
    test<int>();
    test<int>(3);
    static_assert(constexpr_test<int>(), "" );
    static_assert(constexpr_test<int>(3), "" );

    {
        const Optional<const int> o(42);
        Optional<const int> o2(o);
        assert(*o2 == 42);
    }
    {
        using T = TestTypes::TestType;
        T::reset();
        const Optional<T> rhs;
        assert(T::alive == 0);
        const Optional<T> lhs(rhs);
        assert(lhs.has_value() == false);
        assert(T::alive == 0);
    }
    TestTypes::TestType::reset();
    {
        using T = TestTypes::TestType;
        T::reset();
        const Optional<T> rhs(42);
        assert(T::alive == 1);
        assert(T::value_constructed == 1);
        assert(T::copy_constructed == 0);
        const Optional<T> lhs(rhs);
        assert(lhs.has_value());
        assert(T::copy_constructed == 1);
        assert(T::alive == 2);
    }
    TestTypes::TestType::reset();
    {
        using namespace ConstexprTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    {
        using namespace TrivialTestTypes;
        test<TestType>();
        test<TestType>(42);
    }
    {
        test_throwing_ctor();
    }
    {
        test_reference_extension();
    }
    {
        constexpr tim::Optional<int> o1{4};
        constexpr tim::Optional<int> o2 = o1;
        static_assert( *o2 == 4, "" );
    }

  return 0;
}
