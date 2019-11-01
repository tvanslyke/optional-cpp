//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03, c++11, c++14
// <Optional>

// T shall be an object type other than cv in_place_t or cv nullopt_t 
//   and shall satisfy the Cpp17Destructible requirements.
// Note: array types do not satisfy the Cpp17Destructible requirements.

#include "tim/optional/Optional.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

struct NonDestructible { ~NonDestructible() = delete; };

int main(int, char**)
{
	{
	tim::Optional<char &> o1;	        // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with a reference type is ill-formed"}}
	tim::Optional<NonDestructible> o2;  // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with a non-destructible type is ill-formed"}}	
	tim::Optional<char[20]> o3;	        // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with an array type is ill-formed"}}	
	}

	{
	tim::Optional<               tim::in_place_t> o1;  // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with in_place_t is ill-formed"}}
	tim::Optional<const          tim::in_place_t> o2;  // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with in_place_t is ill-formed"}}
	tim::Optional<      volatile tim::in_place_t> o3;  // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with in_place_t is ill-formed"}}
	tim::Optional<const volatile tim::in_place_t> o4;  // expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with in_place_t is ill-formed"}}
	}

	{
	tim::Optional<               tim::nullopt_t> o1;	// expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with nullopt_t is ill-formed"}}
	tim::Optional<const          tim::nullopt_t> o2;	// expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with nullopt_t is ill-formed"}}
	tim::Optional<      volatile tim::nullopt_t> o3;	// expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with nullopt_t is ill-formed"}}
	tim::Optional<const volatile tim::nullopt_t> o4;	// expected-error-re@Optional:* {{static_assert failed{{.*}} "instantiation of Optional with nullopt_t is ill-formed"}}
	}

	return 0;
}
