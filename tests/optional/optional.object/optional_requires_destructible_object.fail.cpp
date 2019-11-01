//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// T shall be an object type and shall satisfy the requirements of Destructible

#include "tim/optional/Optional.hpp"

using tim::Optional;

struct X
{
private:
    ~X() {}
};

int main(int, char**)
{
    using tim::Optional;
    {
        // expected-error-re@Optional:* 2 {{static_assert failed{{.*}} "instantiation of Optional with a reference type is ill-formed}}
        Optional<int&> opt1;
        Optional<int&&> opt2;
    }
    {
        // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with a non-destructible type is ill-formed"}}
        Optional<X> opt3;
    }
    {
        // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with a non-object type is undefined behavior"}}
        // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with a non-destructible type is ill-formed}}
        Optional<void()> opt4;
    }
    {
        // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with a non-object type is undefined behavior"}}
        // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with a non-destructible type is ill-formed}}
        // expected-error@Optional:* 1+ {{cannot form a reference to 'void'}}
        Optional<const void> opt4;
    }
    // FIXME these are garbage diagnostics that Clang should not produce
    // expected-error@Optional:* 0+ {{is not a base class}}

  return 0;
}
