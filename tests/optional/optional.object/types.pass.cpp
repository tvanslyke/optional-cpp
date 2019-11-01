//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class T>
// class Optional
// {
// public:
//     typedef T value_type;
//     ...

#include "tim/optional/Optional.hpp"
#include <type_traits>

#include "test_macros.h"

using tim::Optional;

template <class Opt, class T>
void
test()
{
    static_assert(std::is_same<typename Opt::value_type, T>::value, "");
}

int main(int, char**)
{
    test<Optional<int>, int>();
    test<Optional<const int>, const int>();
    test<Optional<double>, double>();
    test<Optional<const double>, const double>();

  return 0;
}
