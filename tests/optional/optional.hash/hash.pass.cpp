//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// template <class T> struct hash<Optional<T>>;

#include "tim/optional/Optional.hpp"
#include <string>
#include <memory>
#include <cassert>

#include "poisoned_hash_helper.h"

#include "test_macros.h"

struct A {};
struct B {};

namespace std {

template <>
struct hash<B> {
  size_t operator()(B const&) TEST_NOEXCEPT_FALSE { return 0; }
};

}

int main(int, char**)
{
    using tim::Optional;
    const std::size_t nullopt_hash =
        std::hash<Optional<double>>{}(Optional<double>{});


    {
        Optional<B> opt;
        ASSERT_NOT_NOEXCEPT(std::hash<Optional<B>>()(opt));
        ASSERT_NOT_NOEXCEPT(std::hash<Optional<const B>>()(opt));
    }

    {
        typedef int T;
        Optional<T> opt;
        assert(std::hash<Optional<T>>{}(opt) == nullopt_hash);
        opt = 2;
        assert(std::hash<Optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
        typedef std::string T;
        Optional<T> opt;
        assert(std::hash<Optional<T>>{}(opt) == nullopt_hash);
        opt = std::string("123");
        assert(std::hash<Optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
        typedef std::unique_ptr<int> T;
        Optional<T> opt;
        assert(std::hash<Optional<T>>{}(opt) == nullopt_hash);
        opt = std::unique_ptr<int>(new int(3));
        assert(std::hash<Optional<T>>{}(opt) == std::hash<T>{}(*opt));
    }
    {
      test_hash_enabled_for_type<tim::Optional<int> >();
      test_hash_enabled_for_type<tim::Optional<int*> >();
      test_hash_enabled_for_type<tim::Optional<const int> >();
      test_hash_enabled_for_type<tim::Optional<int* const> >();

      test_hash_disabled_for_type<tim::Optional<A>>();
      test_hash_disabled_for_type<tim::Optional<const A>>();

      test_hash_enabled_for_type<tim::Optional<B>>();
      test_hash_enabled_for_type<tim::Optional<const B>>();
    }

  return 0;
}
