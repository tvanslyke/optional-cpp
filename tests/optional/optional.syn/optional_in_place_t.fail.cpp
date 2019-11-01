//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// A program that necessitates the instantiation of template Optional for
// (possibly cv-qualified) in_place_t is ill-formed.

#include "tim/optional/Optional.hpp"

int main(int, char**)
{
    using tim::Optional;
    using tim::in_place_t;
    using tim::in_place;

    Optional<in_place_t> opt; // expected-note {{requested here}}
    // expected-error@Optional:* {{"instantiation of Optional with in_place_t is ill-formed"}}

  return 0;
}
