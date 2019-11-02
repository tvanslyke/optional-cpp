cmake_minimum_required(VERSION 3.8)

project(optional-cpp VERSION 1.0.0 LANGUAGES CXX)

option(OPTIONAL_ENABLE_TESTS "Enable tests." ON)

add_library(optional-cpp INTERFACE)

set(CXXSTD 17 CACHE STRING "C++ standard to use, default C++14")

enable_testing()

target_include_directories(optional-cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(optional-cpp INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include/tim/optional/Optional.hpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(OPTIONAL_ENABLE_TESTS)

	function(AddFailingTest NAME SOURCES)
		add_executable(test_${NAME} ${SOURCES})
		set_property(TARGET test_${NAME} PROPERTY CXX_STANDARD ${CXXSTD})
		target_link_libraries(test_${NAME} optional-cpp)
		target_include_directories(test_${NAME} PRIVATE ${PROJECT_SOURCE_DIR}/tests/support)
		if(MSVC)
			target_compile_options(test_${NAME} PRIVATE /W4 /WX)
		else()
			target_compile_options(test_${NAME} PRIVATE -Wall -Wextra -pedantic)
		endif()
		set_target_properties(test_${NAME} PROPERTIES
			EXCLUDE_FROM_ALL TRUE
			EXCLUDE_FROM_DEFAULT_BUILD TRUE
			LINKER_LANGUAGE CXX)
		add_test(NAME ${NAME}
			COMMAND ${CMAKE_COMMAND} --build . --target test_${NAME}
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
		set_tests_properties(${NAME} PROPERTIES WILL_FAIL TRUE)
	endfunction(AddFailingTest)

	function(AddPassingTest NAME SOURCES)
		add_executable(test_${NAME} ${SOURCES})
		set_property(TARGET test_${NAME} PROPERTY CXX_STANDARD ${CXXSTD})
		target_link_libraries(test_${NAME} optional-cpp)
		target_include_directories(test_${NAME} PRIVATE ${PROJECT_SOURCE_DIR}/tests/support)
		if(MSVC)
			target_compile_options(test_${NAME} PRIVATE /W4 /WX)
		else()
			target_compile_options(test_${NAME} PRIVATE -Wall -Wextra -pedantic)
		endif()
		set_target_properties(test_${NAME} PROPERTIES LINKER_LANGUAGE CXX)
		add_test(NAME ${NAME}
			COMMAND ${CMAKE_BINARY_DIR}/bin/test_${NAME}
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	endfunction(AddPassingTest)

	# Make test executable
	set(TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/constructors.cpp)


	# Tests that should fail to compile:
	AddFailingTest(optional_nullopt_nullopt_t_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullopt/nullopt_t.fail.cpp)
	AddFailingTest(optional_object_optional_object_ctor_ctor_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/ctor.fail.cpp)
	AddFailingTest(optional_object_optional_object_ctor_deduct_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/deduct.fail.cpp)
	AddFailingTest(optional_object_optional_object_ctor_move_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/move.fail.cpp)
	AddFailingTest(optional_object_optional_object_observe_value_const_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/value_const.fail.cpp)
	AddFailingTest(optional_object_optional_requires_destructible_object_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional_requires_destructible_object.fail.cpp)
	AddFailingTest(optional_syn_optional_in_place_t_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.syn/optional_in_place_t.fail.cpp)
	AddFailingTest(optional_syn_optional_nullopt_t_fail
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.syn/optional_nullopt_t.fail.cpp)

	# Tests that should pass normally:
	AddPassingTest(optional_bad_optional_access_default_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.bad_optional_access/default.pass.cpp)
	AddPassingTest(optional_bad_optional_access_derive_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.bad_optional_access/derive.pass.cpp)
	AddPassingTest(optional_comp_with_t_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.comp_with_t/equal.pass.cpp)
	AddPassingTest(optional_comp_with_t_greater_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.comp_with_t/greater.pass.cpp)
	AddPassingTest(optional_comp_with_t_greater_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.comp_with_t/greater_equal.pass.cpp)
	AddPassingTest(optional_comp_with_t_less_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.comp_with_t/less_equal.pass.cpp)
	AddPassingTest(optional_comp_with_t_less_than_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.comp_with_t/less_than.pass.cpp)
	AddPassingTest(optional_comp_with_t_not_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.comp_with_t/not_equal.pass.cpp)
	AddPassingTest(optional_hash_enabled_hash_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.hash/enabled_hash.pass.cpp)
	AddPassingTest(optional_hash_hash_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.hash/hash.pass.cpp)
	AddPassingTest(optional_nullops_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullops/equal.pass.cpp)
	AddPassingTest(optional_nullops_greater_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullops/greater.pass.cpp)
	AddPassingTest(optional_nullops_greater_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullops/greater_equal.pass.cpp)
	AddPassingTest(optional_nullops_less_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullops/less_equal.pass.cpp)
	AddPassingTest(optional_nullops_less_than_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullops/less_than.pass.cpp)
	AddPassingTest(optional_nullops_not_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullops/not_equal.pass.cpp)
	AddPassingTest(optional_nullopt_nullopt_t_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.nullopt/nullopt_t.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_assign_value_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/assign_value.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_const_optional_U_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/const_optional_U.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_copy_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/copy.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_emplace_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/emplace.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_emplace_initializer_list_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/emplace_initializer_list.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_move_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/move.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_nullopt_t_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/nullopt_t.pass.cpp)
	AddPassingTest(optional_object_optional_object_assign_optional_U_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.assign/optional_U.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_U_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/U.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_const_T_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/const_T.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_const_optional_U_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/const_optional_U.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_copy_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/copy.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_deduct_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/deduct.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_default_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/default.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_empty_in_place_t_does_not_clobber_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/empty_in_place_t_does_not_clobber.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_explicit_const_optional_U_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/explicit_const_optional_U.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_explicit_optional_U_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/explicit_optional_U.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_in_place_t_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/in_place_t.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_initializer_list_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/initializer_list.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_move_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/move.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_nullopt_t_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/nullopt_t.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_optional_U_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/optional_U.pass.cpp)
	AddPassingTest(optional_object_optional_object_ctor_rvalue_T_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.ctor/rvalue_T.pass.cpp)
	AddPassingTest(optional_object_optional_object_dtor_dtor_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.dtor/dtor.pass.cpp)
	AddPassingTest(optional_object_optional_object_mod_reset_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.mod/reset.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_bool_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/bool.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_dereference_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/dereference.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_dereference_const_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/dereference_const.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_dereference_const_rvalue_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/dereference_const_rvalue.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_dereference_rvalue_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/dereference_rvalue.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_has_value_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/has_value.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_op_arrow_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/op_arrow.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_op_arrow_const_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/op_arrow_const.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_value_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/value.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_value_const_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/value_const.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_value_const_rvalue_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/value_const_rvalue.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_value_or_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/value_or.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_value_or_const_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/value_or_const.pass.cpp)
	AddPassingTest(optional_object_optional_object_observe_value_rvalue_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.observe/value_rvalue.pass.cpp)
	AddPassingTest(optional_object_optional_object_swap_swap_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/optional.object.swap/swap.pass.cpp)
	AddPassingTest(optional_object_special_members_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/special_members.pass.cpp)
	AddPassingTest(optional_object_triviality_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/triviality.pass.cpp)
	AddPassingTest(optional_object_types_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.object/types.pass.cpp)
	AddPassingTest(optional_relops_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.relops/equal.pass.cpp)
	AddPassingTest(optional_relops_greater_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.relops/greater_equal.pass.cpp)
	AddPassingTest(optional_relops_greater_than_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.relops/greater_than.pass.cpp)
	AddPassingTest(optional_relops_less_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.relops/less_equal.pass.cpp)
	AddPassingTest(optional_relops_less_than_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.relops/less_than.pass.cpp)
	AddPassingTest(optional_relops_not_equal_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.relops/not_equal.pass.cpp)
	AddPassingTest(optional_specalg_make_optional_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.specalg/make_optional.pass.cpp)
	AddPassingTest(optional_specalg_make_optional_explicit_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.specalg/make_optional_explicit.pass.cpp)
	AddPassingTest(optional_specalg_make_optional_explicit_initializer_list_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.specalg/make_optional_explicit_initializer_list.pass.cpp)
	AddPassingTest(optional_specalg_swap_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.specalg/swap.pass.cpp)
	AddPassingTest(optional_syn_optional_includes_initializer_list_pass
		${CMAKE_CURRENT_SOURCE_DIR}/tests/optional/optional.syn/optional_includes_initializer_list.pass.cpp)

endif(OPTIONAL_ENABLE_TESTS)



