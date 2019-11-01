//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// ~Optional();

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

using tim::Optional;

struct PODType {
  int value;
  int value2;
};

class X
{
public:
    static bool dtor_called;
    X() = default;
    ~X() {dtor_called = true;}
};

bool X::dtor_called = false;

int main(int, char**)
{
    {
        typedef int T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<Optional<T>>::value, "");
        static_assert(std::is_literal_type<Optional<T>>::value, "");
    }
    {
        typedef double T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<Optional<T>>::value, "");
        static_assert(std::is_literal_type<Optional<T>>::value, "");
    }
    {
        typedef PODType T;
        static_assert(std::is_trivially_destructible<T>::value, "");
        static_assert(std::is_trivially_destructible<Optional<T>>::value, "");
        static_assert(std::is_literal_type<Optional<T>>::value, "");
    }
    {
        typedef X T;
        static_assert(!std::is_trivially_destructible<T>::value, "");
        static_assert(!std::is_trivially_destructible<Optional<T>>::value, "");
        static_assert(!std::is_literal_type<Optional<T>>::value, "");
        {
            X x;
            Optional<X> opt{x};
            assert(X::dtor_called == false);
        }
        assert(X::dtor_called == true);
    }

  return 0;
}
