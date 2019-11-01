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
// (possibly cv-qualified) nullopt_t is ill-formed.

#include "tim/optional/Optional.hpp"

int main(int, char**)
{
    using tim::Optional;
    using tim::nullopt_t;
    using tim::nullopt;

    Optional<nullopt_t> opt; // expected-note 1 {{requested here}}
    Optional<const nullopt_t> opt1; // expected-note 1 {{requested here}}
    Optional<nullopt_t &> opt2; // expected-note 1 {{requested here}}
    Optional<nullopt_t &&> opt3; // expected-note 1 {{requested here}}
    // expected-error@Optional:* 4 {{instantiation of Optional with nullopt_t is ill-formed}}

  return 0;
}
