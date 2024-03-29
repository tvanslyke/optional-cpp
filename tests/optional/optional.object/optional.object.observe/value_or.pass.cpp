//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class U> constexpr T Optional<T>::value_or(U&& v) &&;

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

using tim::Optional;
using tim::in_place_t;
using tim::in_place;

struct Y
{
    int i_;

    constexpr Y(int i) : i_(i) {}
};

struct X
{
    int i_;

    constexpr X(int i) : i_(i) {}
    constexpr X(X&& x) : i_(x.i_) {x.i_ = 0;}
    constexpr X(const Y& y) : i_(y.i_) {}
    constexpr X(Y&& y) : i_(y.i_+1) {}
    friend constexpr bool operator==(const X& x, const X& y)
        {return x.i_ == y.i_;}
};

constexpr int test()
{
    {
        Optional<X> opt(in_place, 2);
        Y y(3);
        assert(std::move(opt).value_or(y) == 2);
        assert(*opt == 0);
    }
    {
        Optional<X> opt(in_place, 2);
        assert(std::move(opt).value_or(Y(3)) == 2);
        assert(*opt == 0);
    }
    {
        Optional<X> opt;
        Y y(3);
        assert(std::move(opt).value_or(y) == 3);
        assert(!opt);
    }
    {
        Optional<X> opt;
        assert(std::move(opt).value_or(Y(3)) == 4);
        assert(!opt);
    }
    return 0;
}

int main(int, char**)
{
    static_assert(test() == 0);

  return 0;
}
