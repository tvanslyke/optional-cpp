#ifndef TIM_OPTIONAL_OPTIONAL_HPP
#define TIM_OPTIONAL_OPTIONAL_HPP

#include <type_traits>
#include <memory>

namespace tim {

inline namespace optional {

template <class T>
struct Optional;

class BadOptionalAccess;


namespace detail {

struct empty_tag_t {};
inline constexpr empty_tag_t empty_tag = empty_tag_t{};

} /* namespace detail */

struct in_place_t {};
inline constexpr in_place_t in_place = in_place_t{};

struct nullopt_t {};
inline constexpr nullopt_t nullopy = nullopt_t{};

class BadOptionalAccess: std::exception {
public:
	BadOptionalAccess() noexcept = default;

	virtual const char* what() const noexcept override {
		return "bad optional access";
	}
};

namespace detail {

template <class T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <class T>
using is_cv_void = std::is_same<std::remove_cv_t<T>, void>;

template <class T>
inline constexpr bool is_cv_void_v = is_cv_void<T>::value;

template <class T>
struct ManualScopeGuard {

	~ManualScopeGuard() {
		if(active) {
			action();
		}
	}

	T action;
	bool active;
};

template <class T>
ManualScopeGuard<std::decay_t<T>> make_manual_scope_guard(T&& action) {
	return ManualScopeGuard<std::decay_t<T>>{std::forward<T>(action), true};
}

struct EmptyAlternative {};

enum class MemberStatus {
	Defaulted,
	Deleted,
	Defined
};

template <MemberStatus S, class T>
struct OptionalUnionImpl;

template <
	class T,
	bool = (
		std::is_same_v<std::remove_cv_t<T>, T>
		&& std::is_empty_v<T>
		&& std::is_standard_layout_v<T>
		&& std::is_class_v<T>
		&& !std::is_final_v<T>
       )
>
struct ValueWrapper;

template <class T>
struct ValueWrapper<T, false> {
	using value_type = T;

	ValueWrapper() = default;

	ValueWrapper(const ValueWrapper&) = default;
	ValueWrapper(ValueWrapper&&) = default;

	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, Args&& ...>
			&& std::conditional_t<
				(sizeof...(Args) == 1),
				std::negation<
					std::disjunction<
						std::is_same<ValueWrapper, std::decay_t<Args>>...
					>
				>,
				std::true_type
			>::value,
			bool
		> = false
	>
	constexpr ValueWrapper(Args&& ... args):
		value_(std::forward<Args>(args)...)
	{

	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, std::initializer_list<U>&, Args&& ...>,
			bool
		> = false
	>
	constexpr ValueWrapper(std::initializer_list<U> ilist, Args&& ... args):
		value_(ilist, std::forward<Args>(args)...)
	{

	}

	ValueWrapper& operator=(const ValueWrapper&) = default;
	ValueWrapper& operator=(ValueWrapper&&) = default;

	constexpr const value_type&  value() const&  { return this->value_; }
	constexpr       value_type&  value()      &  { return this->value_; }
	constexpr const value_type&& value() const&& { return std::move(this->value_); }
	constexpr       value_type&& value()      && { return std::move(this->value_); }

private:
	T value_;
};

template <class T>
struct ValueWrapper<T, true>: private T {
	using value_type = T;

	ValueWrapper() = default;

	ValueWrapper(const ValueWrapper&) = default;
	ValueWrapper(ValueWrapper&&) = default;

	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, Args&& ...>
			&& std::conditional_t<
				(sizeof...(Args) == 1),
				std::negation<
					std::disjunction<
						std::is_same<ValueWrapper, std::decay_t<Args>>...
					>
				>,
				std::true_type
			>::value,
			bool
		> = false
	>
	constexpr ValueWrapper(Args&& ... args):
		value_type(std::forward<Args>(args)...)
	{

	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, std::initializer_list<U>&, Args&& ...>,
			bool
		> = false
	>
	constexpr ValueWrapper(std::initializer_list<U> ilist, Args&& ... args):
		value_type(ilist, std::forward<Args>(args)...)
	{

	}

	ValueWrapper& operator=(const ValueWrapper&) = default;
	ValueWrapper& operator=(ValueWrapper&&) = default;

	constexpr const value_type&  value() const&  { return *static_cast<const T*>(this); }
	constexpr       value_type&  value()      &  { return *static_cast<T*>(this); }
	constexpr const value_type&& value() const&& { return std::move(*static_cast<const T*>(this)); }
	constexpr       value_type&& value()      && { return std::move(*static_cast<T*>(this)); }

};

template <class T>
struct OptionalUnionImpl<MemberStatus::Defaulted, T> {

	constexpr OptionalUnionImpl() = default;
	constexpr OptionalUnionImpl(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl(OptionalUnionImpl&&) = default;
	constexpr OptionalUnionImpl& operator=(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl& operator=(OptionalUnionImpl&&) = default;

	constexpr OptionalUnionImpl(detail::empty_tag_t):
		hidden_{}
	{
	
	}

	template <class ... Args>
	constexpr OptionalUnionImpl(in_place_t, Args&& ... args):
		value(std::forward<Args>(args)...)
	{
		
	}

	union {
		EmptyAlternative hidden_;
		ValueWrapper<std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>> value;
	};
};

template <class T>
struct OptionalUnionImpl<MemberStatus::Deleted, T> {

	constexpr OptionalUnionImpl() = default;
                  
	constexpr OptionalUnionImpl(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl(OptionalUnionImpl&&) = default;

	constexpr OptionalUnionImpl& operator=(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl& operator=(OptionalUnionImpl&&) = default;

	~OptionalUnionImpl() = delete;	

	constexpr OptionalUnionImpl(detail::empty_tag_t):
		hidden_{}
	{
	
	}

	template <class ... Args>
	constexpr OptionalUnionImpl(in_place_t, Args&& ... args):
		value(std::forward<Args>(args)...)
	{
		
	}

	union {
		EmptyAlternative hidden_;
		ValueWrapper<std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>> value;
	};
};

template <class T>
struct OptionalUnionImpl<MemberStatus::Defined, T> {

	constexpr OptionalUnionImpl() = default;
                     
	constexpr OptionalUnionImpl(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl(OptionalUnionImpl&&) = default;

	constexpr OptionalUnionImpl& operator=(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl& operator=(OptionalUnionImpl&&) = default;

	~OptionalUnionImpl() {}

	constexpr OptionalUnionImpl(detail::empty_tag_t):
		hidden_{}
	{
	
	}

	template <class ... Args>
	constexpr OptionalUnionImpl(in_place_t, Args&& ... args):
		value(std::forward<Args>(args)...)
	{
		
	}

	union {
		EmptyAlternative hidden_;
		ValueWrapper<std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>> value;
	};
};

struct OptionalUnionImpl<MemberStatus::Defaulted, void> {

	constexpr OptionalUnionImpl() = default;
	constexpr OptionalUnionImpl(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl(OptionalUnionImpl&&) = default;
	constexpr OptionalUnionImpl& operator=(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl& operator=(OptionalUnionImpl&&) = default;

	constexpr OptionalUnionImpl(in_place_t) {}

	constexpr OptionalUnionImpl(detail::empty_tag_t) {}

};

struct OptionalUnionImpl<MemberStatus::Defaulted, const void> {

	constexpr OptionalUnionImpl() = default;
	constexpr OptionalUnionImpl(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl(OptionalUnionImpl&&) = default;
	constexpr OptionalUnionImpl& operator=(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl& operator=(OptionalUnionImpl&&) = default;

	constexpr OptionalUnionImpl(in_place_t) {}

	constexpr OptionalUnionImpl(detail::empty_tag_t) {}

};

struct OptionalUnionImpl<MemberStatus::Defaulted, volatile void> {

	constexpr OptionalUnionImpl() = default;
	constexpr OptionalUnionImpl(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl(OptionalUnionImpl&&) = default;
	constexpr OptionalUnionImpl& operator=(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl& operator=(OptionalUnionImpl&&) = default;

	constexpr OptionalUnionImpl(in_place_t) {}

	constexpr OptionalUnionImpl(detail::empty_tag_t) {}

};

struct OptionalUnionImpl<MemberStatus::Defaulted, const volatile void> {

	constexpr OptionalUnionImpl() = default;
	constexpr OptionalUnionImpl(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl(OptionalUnionImpl&&) = default;
	constexpr OptionalUnionImpl& operator=(const OptionalUnionImpl&) = default;
	constexpr OptionalUnionImpl& operator=(OptionalUnionImpl&&) = default;

	constexpr OptionalUnionImpl(in_place_t) {}

	constexpr OptionalUnionImpl(detail::empty_tag_t) {}

};

template <class T>
using OptionalUnion = std::conditional_t<
	std::disjunction<
		detail::is_cv_void<T>,
		std::is_destructible<T>
	>
	std::conditional_t<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_trivially_destructible<T>
		>,
		OptionalUnionImpl<MemberStatus::Defaulted, T>,
		OptionalUnionImpl<MemberStatus::Defined, T>
	>,
	OptionalUnionImpl<MemberStatus::Deleted, T>
>;

template <class T, bool = detail::is_cv_void_v<T>>
struct OptionalBaseMethods;

template <class T>
struct OptionalBaseMethods<T, false> {
	using value_type = T;

	constexpr OptionalBaseMethods() = default;

	template <class ... Args>
	constexpr OptionalBaseMethods(in_place_t, Args&& ... args):
		has_value_(true),
		data_(in_place, std::forward<Args>(args)...)
	{
		
	}

	template <class ... Args>
	constexpr OptionalBaseMethods(detail::empty_tag_t):
		has_value_(false),
		data_(detail::empty_tag)
	{

	}

	constexpr const value_type& value() const { return std::launder(std::addressof(data_.value))->value(); }
	constexpr       value_type& value()       { return std::launder(std::addressof(data_.value))->value(); }

	constexpr const bool& has_value() const noexcept { return has_value_; }
	constexpr bool&       has_value()       noexcept { return has_value_; }

	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<value_type, Args&&...>,
			bool
		> = false
	>
	constexpr void emplace(Args&& ... args)
		noexcept(std::is_nothrow_constructible_v<value_type, Args&&...>)
	{
		new (std::addressof(data_.value)) ValueWrapper<T>(std::forward<Args>(args)...);
	}

	constexpr void destruct() noexcept {
		if constexpr(
			std::conjunction_v<
				std::negation<is_cv_void<T>>,
				std::negation<std::is_trivially_destructible<T>>
			>
		) {
			std::destroy_at(std::addressof(data_.value));
		}
	}

	[[noreturn]]
	void throw_bad_optional_access() const noexcept(false) {
		throw BadOptionalAccess();
	}
	
private:
	bool has_value_ = false;
	OptionalUnion<T> data_;
};

template <class T>
struct OptionalBaseMethods<T, true> {

	constexpr OptionalBaseMethods() = default;

	constexpr OptionalBaseMethods(in_place_t):
		has_value_(true)
	{
		
	}

	template <class ... Args>
	constexpr OptionalBaseMethods(detail::empty_tag_t):
		has_value_(false)
	{

	}

	constexpr const value_type& value() const { return std::launder(std::addressof(data_.value))->value(); }
	constexpr       value_type& value()       { return std::launder(std::addressof(data_.value))->value(); }

	constexpr const bool& has_value() const noexcept { return has_value_; }
	constexpr bool&       has_value()       noexcept { return has_value_; }

	constexpr void emplace() noexcept {}

	constexpr void destruct() noexcept {}

	[[noreturn]]
	void throw_bad_optional_access() const noexcept(false) {
		throw BadOptionalAccess();
	}
	
private:
	bool has_value_ = false;
};

template <MemberStatus S, class T>
struct OptionalDestructor;

template <MemberStatus S, class T>
struct OptionalDefaultConstructor;

template <MemberStatus S, class T>
struct OptionalCopyConstructor;

template <MemberStatus S, class T>
struct OptionalMoveConstructor;

template <MemberStatus S, class T>
struct OptionalCopyAssign;

template <MemberStatus S, class T>
struct OptionalMoveAssign;

template <class T>
using optional_destructor_type = std::conditional_t<
	std::disjunction<
		detail::is_cv_void<T>,
		std::is_trivially_destructible<T>
	>,
	OptionalDestructor<MemberStatus::Defaulted, T>,
	OptionalDestructor<MemberStatus::Defined, T>
>;

template <class T>
using optional_default_constructor_type = std::conditional_t<
	std::disjunction_v<
		detail::is_cv_void<T>,
		std::is_default_constructible<T>
	>,
	OptionalDefaultConstructor<MemberStatus::Defined, T>,
	OptionalDefaultConstructor<MemberStatus::Deleted, T>
>;


template <class T>
using optional_copy_constructor_type = std::conditional_t<
	std::disjunction<
		detail::is_cv_void<T>,
		std::is_copy_constructible<T>
	>,
	std::conditional_t<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_trivially_copy_constructible<T>
		>,
		OptionalCopyConstructor<MemberStatus::Defaulted, T>,
		OptionalCopyConstructor<MemberStatus::Defined, T>
	>,
	OptionalCopyConstructor<MemberStatus::Deleted, T>
>;

template <class T>
using optional_move_constructor_type = std::conditional_t<
	std::disjunction<
		detail::is_cv_void<T>,
		std::is_move_constructible<T>
	>,
	std::conditional_t<
		std::disjunction<
			detail::is_cv_void<T>,
			std::is_trivially_move_constructible<T>
		>,
		OptionalMoveConstructor<MemberStatus::Defaulted, T>,
		OptionalMoveConstructor<MemberStatus::Defined, T>
	>,
	OptionalMoveConstructor<MemberStatus::Deleted, T>
>;

template <class T>
using optional_copy_assign_type = std::conditional_t<
	std::disjunction_v<
		detail::is_cv_void<T>,
		std::conjunction<
			std::negation<detail::is_cv_void<T>>,
			std::is_copy_assignable<T>,
			std::is_copy_constructible<T>
		>
	>,
	std::conditional_t<
		std::disjunction<
			detail::is_cv_void<T>,
			std::conjunction<
				std::negation<detail::is_cv_void<T>>,
				std::is_trivially_copy_assignable<T>,
				std::is_trivially_copy_constructible<T>,
				std::is_trivially_destructible<T>
			>
		>,
		OptionalCopyAssign<MemberStatus::Defaulted, T>,
		OptionalCopyAssign<MemberStatus::Defined, T>
	>,
	OptionalCopyAssign<MemberStatus::Deleted, T>
>;

template <class T>
using optional_move_assign_type = std::conditional_t<
	std::disjunction_v<
		detail::is_cv_void<T>,
		std::conjunction<
			std::negation<detail::is_cv_void<T>>,
			std::is_move_assignable<T>,
			std::is_move_constructible<T>,
			std::is_trivially_destructible<T>
		>
	>,
	std::conditional_t<
		std::disjunction<
			detail::is_cv_void<T>,
			std::conjunction<
				std::is_trivially_move_assignable<T>,
				std::is_trivially_move_constructible<T>,
				std::is_trivially_destructible<T>
			>
		>,
		OptionalMoveAssign<MemberStatus::Defaulted, T>,
		OptionalMoveAssign<MemberStatus::Defined, T>
	>,
	OptionalMoveAssign<MemberStatus::Deleted, T>
>;

template <class T>
using optional_data_type = optional_move_assign_type<T>;

template <class T>
struct OptionalDestructor<MemberStatus::Defaulted, T>:
	OptionalBaseMethods<T>
{
	using base_type = OptionalBaseMethods<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalDestructor() = default;

	constexpr OptionalDestructor(const OptionalDestructor&) = default;
	constexpr OptionalDestructor(OptionalDestructor&&) = default;

	constexpr OptionalDestructor& operator=(const OptionalDestructor&) = default;
	constexpr OptionalDestructor& operator=(OptionalDestructor&&) = default;
};

template <class T>
struct OptionalDestructor<MemberStatus::Defined, T>:
	OptionalBaseMethods<T>
{
	using base_type = OptionalBaseMethods<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;
	
	constexpr OptionalDestructor() = default;
	constexpr OptionalDestructor(const OptionalDestructor&) = default;
	constexpr OptionalDestructor(OptionalDestructor&&) = default;
	constexpr OptionalDestructor& operator=(const OptionalDestructor&) = default;
	constexpr OptionalDestructor& operator=(OptionalDestructor&&) = default;

	~OptionalDestructor() {
		this->destruct();
	}
};

template <class T>
struct OptionalDefaultConstructor<MemberStatus::Defaulted, T>:
	optional_destructor_type<T>
{
	using base_type = optional_destructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalDefaultConstructor() = default;
	constexpr OptionalDefaultConstructor(const OptionalDefaultConstructor&) = default;
	constexpr OptionalDefaultConstructor(OptionalDefaultConstructor&&) = default;
	constexpr OptionalDefaultConstructor& operator=(const OptionalDefaultConstructor&) = default;
	constexpr OptionalDefaultConstructor& operator=(OptionalDefaultConstructor&&) = default;
};

template <class T>
struct OptionalDefaultConstructor<MemberStatus::Deleted, T>:
	optional_destructor_type<T>
{
	using base_type = optional_destructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalDefaultConstructor() = delete;
	constexpr OptionalDefaultConstructor(const OptionalDefaultConstructor&) = default;
	constexpr OptionalDefaultConstructor(OptionalDefaultConstructor&&) = default;
	constexpr OptionalDefaultConstructor& operator=(const OptionalDefaultConstructor&) = default;
	constexpr OptionalDefaultConstructor& operator=(OptionalDefaultConstructor&&) = default;
};

template <class T>
struct OptionalDefaultConstructor<MemberStatus::Defined, T>:
	optional_destructor_type<T>
{
	using base_type = optional_destructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;
	
	constexpr OptionalDefaultConstructor() noexcept(std::is_nothrow_default_constructible_v<T>):
		base_type(detail::empty_tag)
	{
		
	}

	constexpr OptionalDefaultConstructor(const OptionalDefaultConstructor& other) = default;
	constexpr OptionalDefaultConstructor(OptionalDefaultConstructor&&) = default;
	constexpr OptionalDefaultConstructor& operator=(const OptionalDefaultConstructor&) = default;
	constexpr OptionalDefaultConstructor& operator=(OptionalDefaultConstructor&&) = default;
};

template <class T>
struct OptionalCopyConstructor<MemberStatus::Defaulted, T>:
	optional_default_constructor_type<T>
{
	using base_type = optional_default_constructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalCopyConstructor() = default;
	constexpr OptionalCopyConstructor(const OptionalCopyConstructor&) = default;
	constexpr OptionalCopyConstructor(OptionalCopyConstructor&&) = default;
	constexpr OptionalCopyConstructor& operator=(const OptionalCopyConstructor&) = default;
	constexpr OptionalCopyConstructor& operator=(OptionalCopyConstructor&&) = default;
};

template <class T>
struct OptionalCopyConstructor<MemberStatus::Deleted, T>:
	optional_default_constructor_type<T>
{
	using base_type = optional_default_constructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalCopyConstructor() = default;
	constexpr OptionalCopyConstructor(const OptionalCopyConstructor&) = delete;
	constexpr OptionalCopyConstructor(OptionalCopyConstructor&&) = default;
	constexpr OptionalCopyConstructor& operator=(const OptionalCopyConstructor&) = default;
	constexpr OptionalCopyConstructor& operator=(OptionalCopyConstructor&&) = default;
};

template <class T>
struct OptionalCopyConstructor<MemberStatus::Defined, T>
{
	using base_type = optional_default_constructor_type<T>;
	using value_type = std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>;
	
	constexpr OptionalCopyConstructor() = default;
	constexpr OptionalCopyConstructor(const OptionalCopyConstructor& other):
		base_([&other]{
			if(other.has_value()) {
				return base_type(in_place, other.value());
			} else {
				return base_type(empty_tag);
			}
		}())
	{
		
	}

	constexpr OptionalCopyConstructor(OptionalCopyConstructor&&) = default;

	constexpr OptionalCopyConstructor& operator=(const OptionalCopyConstructor&) = default;
	constexpr OptionalCopyConstructor& operator=(OptionalCopyConstructor&&) = default;

	template <class ... Args>
	constexpr OptionalCopyConstructor(in_place_t, Args&& ... args):
		base_(in_place, std::forward<Args>(args)...)
	{
		
	}

	constexpr OptionalCopyConstructor(empty_tag_t):
		base_(empty_tag)
	{

	}

	constexpr const value_type& value() const { return base_.value(); }
	constexpr       value_type& value()       { return base_.value(); }

	constexpr const bool& has_value() const noexcept { return base_.has_value(); }
	constexpr bool&       has_value()       noexcept { return base_.has_value(); }

	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<value_type, Args&&...>,
			bool
		> = false
	>
	constexpr void emplace(Args&& ... args)
		noexcept(std::is_nothrow_constructible_v<value_type, Args&&...>)
	{
		base_.emplace(std::forward<Args>(args)...);
	}

	[[noreturn]]
	void throw_bad_optional_access() const noexcept(false) { base_.throw_bad_optional_access(); }

	constexpr void destruct() noexcept {
		return base_.destruct();
	}

private:
	base_type base_;
};

template <class T>
struct OptionalMoveConstructor<MemberStatus::Defaulted, T>:
	optional_copy_constructor_type<T>
{
	static_assert(std::disjunction_v<detail::is_cv_void<T>, std::is_trivially_move_constructible<T>>);
	using base_type = optional_copy_constructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalMoveConstructor() = default;
	constexpr OptionalMoveConstructor(const OptionalMoveConstructor&) = default;
	constexpr OptionalMoveConstructor(OptionalMoveConstructor&&) = default;
	constexpr OptionalMoveConstructor& operator=(const OptionalMoveConstructor&) = default;
	constexpr OptionalMoveConstructor& operator=(OptionalMoveConstructor&&) = default;
};

template <class T>
struct OptionalMoveConstructor<MemberStatus::Deleted, T>:
	optional_copy_constructor_type<T>
{
	static_assert(
		(!std::disjunction_v<detail::is_cv_void<T>, std::is_trivially_move_constructible<T>>)
	);
	using base_type = optional_copy_constructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalMoveConstructor() = default;
	constexpr OptionalMoveConstructor(const OptionalMoveConstructor&) = default;
	constexpr OptionalMoveConstructor(OptionalMoveConstructor&&) = delete;
	constexpr OptionalMoveConstructor& operator=(const OptionalMoveConstructor&) = default;
	constexpr OptionalMoveConstructor& operator=(OptionalMoveConstructor&&) = default;
};

template <class T>
struct OptionalMoveConstructor<MemberStatus::Defined, T> {
	using base_type = optional_copy_constructor_type<T>;
	using value_type = std::conditional_t<is_cv_void_v<T>, EmptyAlternative, T>;
	
	constexpr OptionalMoveConstructor() = default;
	constexpr OptionalMoveConstructor(const OptionalMoveConstructor&) = default;
	constexpr OptionalMoveConstructor(OptionalMoveConstructor&& other):
		base_([&]{
			if(other.has_value()) {
				return base_type(in_place, std::move(other.value()));
			} else {
				return base_type(empty_tag);
			}
		}())
	{
		
	}

	constexpr OptionalMoveConstructor& operator=(const OptionalMoveConstructor&) = default;
	constexpr OptionalMoveConstructor& operator=(OptionalMoveConstructor&&) = default;

	template <class ... Args>
	constexpr OptionalMoveConstructor(in_place_t, Args&& ... args):
		base_(in_place, std::forward<Args>(args)...)
	{
		
	}

	constexpr OptionalMoveConstructor(empty_tag_t):
		base_(empty_tag)
	{
		
	}

	constexpr const value_type& value() const { return base_.value(); }
	constexpr       value_type& value()       { return base_.value(); }

	constexpr const bool& has_value() const noexcept { return base_.has_value(); }
	constexpr bool&       has_value()       noexcept { return base_.has_value(); }

	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<value_type, Args&&...>,
			bool
		> = false
	>
	constexpr void emplace(Args&& ... args)
		noexcept(std::is_nothrow_constructible_v<value_type, Args&&...>)
	{
		base_.emplace(std::forward<Args>(args)...);
	}

	[[noreturn]]
	void throw_bad_optional_access() const& noexcept(false) { base_.throw_bad_optional_access(); }

	constexpr void destruct() noexcept {
		return base_.destruct();
	}

private:
	base_type base_;
};

template <class T>
struct OptionalCopyAssign<MemberStatus::Defaulted, T>:
	optional_move_constructor_type<T>
{
	using base_type = optional_move_constructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalCopyAssign() = default;
	constexpr OptionalCopyAssign(const OptionalCopyAssign&) = default;
	constexpr OptionalCopyAssign(OptionalCopyAssign&&) = default;
	constexpr OptionalCopyAssign& operator=(const OptionalCopyAssign&) = default;
	constexpr OptionalCopyAssign& operator=(OptionalCopyAssign&&) = default;
};

template <class T>
struct OptionalCopyAssign<MemberStatus::Deleted, T>:
	optional_move_constructor_type<T>
{
	using base_type = optional_move_constructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalCopyAssign() = default;
	constexpr OptionalCopyAssign(const OptionalCopyAssign&) = default;
	constexpr OptionalCopyAssign(OptionalCopyAssign&&) = default;
	constexpr OptionalCopyAssign& operator=(const OptionalCopyAssign&) = delete;
	constexpr OptionalCopyAssign& operator=(OptionalCopyAssign&&) = default;
};

template <class T>
struct OptionalCopyAssign<MemberStatus::Defined, T>:
	optional_move_constructor_type<T>
{
	using base_type = optional_move_constructor_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;
	
	constexpr OptionalCopyAssign() = default;
	constexpr OptionalCopyAssign(const OptionalCopyAssign& other) = default;
	constexpr OptionalCopyAssign(OptionalCopyAssign&&) = default;
	constexpr OptionalCopyAssign& operator=(const OptionalCopyAssign& other) noexcept(
		std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_copy_constructible<T>>
		&& std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_copy_assignable<T>>
		&& std::is_nothrow_copy_constructible_v<E>
		&& std::is_nothrow_copy_assignable_v<E>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				this->value() = other.value();
			} else {
				this->destruct();
				this->has_value() = false;
			}
		} else {
			if(other.has_value()) {
				this->emplace(other.value());
				this->has_value() = true;
			}
		}
		return *this;
	}
	constexpr OptionalCopyAssign& operator=(OptionalCopyAssign&&) = default;

};

template <class T>
struct OptionalMoveAssign<MemberStatus::Defaulted, T>:
	optional_copy_assign_type<T>
{
	using base_type = optional_copy_assign_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalMoveAssign() = default;
	constexpr OptionalMoveAssign(const OptionalMoveAssign&) = default;
	constexpr OptionalMoveAssign(OptionalMoveAssign&&) = default;
	constexpr OptionalMoveAssign& operator=(const OptionalMoveAssign&) = default;
	constexpr OptionalMoveAssign& operator=(OptionalMoveAssign&&) = default;
};

template <class T>
struct OptionalMoveAssign<MemberStatus::Deleted, T>:
	optional_copy_assign_type<T>
{
	using base_type = optional_copy_assign_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;

	constexpr OptionalMoveAssign() = default;
	constexpr OptionalMoveAssign(const OptionalMoveAssign&) = default;
	constexpr OptionalMoveAssign(OptionalMoveAssign&&) = default;
	constexpr OptionalMoveAssign& operator=(const OptionalMoveAssign&) = default;
	constexpr OptionalMoveAssign& operator=(OptionalMoveAssign&&) = delete;
};

template <class T>
struct OptionalMoveAssign<MemberStatus::Defined, T>:
	optional_copy_assign_type<T>
{
	using base_type = optional_copy_assign_type<T>;
	using base_type::base_type;
	using base_type::has_value;
	using base_type::value;
	using base_type::destruct;
	using base_type::emplace;
	using base_type::throw_bad_optional_access;
	
	constexpr OptionalMoveAssign() = default;
	constexpr OptionalMoveAssign(const OptionalMoveAssign& other) = default;
	constexpr OptionalMoveAssign(OptionalMoveAssign&&) = default;
	constexpr OptionalMoveAssign& operator=(const OptionalMoveAssign&) = default;
	constexpr OptionalMoveAssign& operator=(OptionalMoveAssign&& other) noexcept(
		std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_move_constructible<T>>
		&& std::disjunction_v<detail::is_cv_void<T>, std::is_nothrow_move_assignable<T>>
		&& std::is_nothrow_move_constructible_v<E>
		&& std::is_nothrow_move_assignable_v<E>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				this->value() = std::move(other.value());
			} else {
				this->destruct();
				this->has_value() = false;
			}
		} else {
			if(other.has_value()) {
				this->emplace(std::move(other.value()));
				this->has_value() = true;
			}
		}
		return *this;
	}
};

} /* namespace detail */


template <class T>
struct Optional;

namespace traits {

namespace detail {

template <class T, decltype((std::declval<T>() ? true : false)) = false>
constexpr std::true_type is_contextually_convertible_to_bool_helper(std::in_place_type_t<T>, int) {
	return std::true_type{};
}

template <class T>
constexpr std::false_type is_contextually_convertible_to_bool_helper(std::in_place_type_t<T>, ...) {
	return std::false_type{};
}

template <class T>
struct is_contextually_convertible_to_bool:
	decltype(tim::traits::detail::is_contextually_convertible_to_bool_helper(std::in_place_type<T>, 0))
{
};

template <class T>
inline constexpr bool is_contextually_convertible_to_bool_v
	= is_contextually_convertible_to_bool<T>::value;


static_assert(is_contextually_convertible_to_bool_v<bool>);
static_assert(is_contextually_convertible_to_bool_v<int>);
static_assert(is_contextually_convertible_to_bool_v<void*>);
struct NotConvertibleToBool {};
static_assert(!is_contextually_convertible_to_bool_v<NotConvertibleToBool>);

template <class T>
struct is_optional: std::false_type {};

template <class T>
struct is_optional<Optional<T>>: std::true_type {};

} /* namespace detail */

} /* namespace traits */

template <class T>
struct Optional {
	static_assert(!std::is_reference_v<T>,
		"Instantiating Optional<T> for reference type 'T' is not permitted.");
	static_assert(!std::is_function_v<T>,
		"Instantiating Optional<T> for function type 'T' is not permitted.");
	static_assert(!std::is_same_v<tim::optional::in_place_t, std::remove_cv_t<T>>,
		"Instantiating Optional<T> where 'T' is const- or volatile-qualified 'in_place_t' is not permitted.");

	template <class U>
	friend struct Optional;

private:
	using data_type = detail::optional_data_type<T>;
	static constexpr detail::in_place_t in_place = detail::in_place;
	static constexpr detail::error_tag_t error_tag = detail::error_tag;
public:

	using value_type = T;

	Optional() = default;
	Optional(const Optional&) = default;
	Optional(Optional&&) = default;

	template <
		class U,
		std::enable_if_t<
			std::is_convertible<const U&, T>
			&& !std::is_same_v<T, U>
			&& std::conjunction_v<
				std::is_constructible<T, const U&>,
				std::negation<std::is_constructible<T, Optional<U>&>>,
				std::negation<std::is_constructible<T, Optional<U>&&>>,
				std::negation<std::is_constructible<T, const Optional<U>&>>,
				std::negation<std::is_constructible<T, const Optional<U>&&>>,
				std::negation<std::is_convertible<Optional<U>&, T>>,
				std::negation<std::is_convertible<Optional<U>&&, T>>,
				std::negation<std::is_convertible<const Optional<U>&, T>>,
				std::negation<std::is_convertible<const Optional<U>&&, T>>
			>,
			bool
		> = false
	>
	explicit constexpr Optional(const Optional<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(in_place, other.val());
			} else {
				return data_type(detail::empty_tag);
			}
		}())
	{
		
	}

	template <
		class U,
		std::enable_if_t<
			std::is_convertible<const U&, T>
			&& !std::is_same_v<T, U>
			&& std::conjunction_v<
				std::is_constructible<T, const U&>,
				std::negation<std::is_constructible<T, Optional<U>&>>,
				std::negation<std::is_constructible<T, Optional<U>&&>>,
				std::negation<std::is_constructible<T, const Optional<U>&>>,
				std::negation<std::is_constructible<T, const Optional<U>&&>>,
				std::negation<std::is_convertible<Optional<U>&, T>>,
				std::negation<std::is_convertible<Optional<U>&&, T>>,
				std::negation<std::is_convertible<const Optional<U>&, T>>,
				std::negation<std::is_convertible<const Optional<U>&&, T>>
			>,
			bool
		> = false
	>
	constexpr Optional(const Optional<U>& other) noexcept(std::is_nothrow_constructible_v<T, const U&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(in_place, other.val());
			} else {
				return data_type(detail::empty_tag);
			}
		}())
	{
		
	}

	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible<U&&, T>
			&& !std::is_same_v<T, U>
			&& std::conjunction_v<
				std::is_constructible<T, U&&>,
				std::negation<std::is_constructible<T, Optional<U>&>>,
				std::negation<std::is_constructible<T, Optional<U>&&>>,
				std::negation<std::is_constructible<T, const Optional<U>&>>,
				std::negation<std::is_constructible<T, const Optional<U>&&>>,
				std::negation<std::is_convertible<Optional<U>&, T>>,
				std::negation<std::is_convertible<Optional<U>&&, T>>,
				std::negation<std::is_convertible<const Optional<U>&, T>>,
				std::negation<std::is_convertible<const Optional<U>&&, T>>
			>,
			bool
		> = false
	>
	constexpr explicit Optional(Optional<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(in_place, std::move(other.val()));
			} else {
				return data_type(detail::empty_tag);
			}
		}())
	{
		
	}
	
	template <
		class U,
		class G,
		std::enable_if_t<
			std::is_convertible<U&&, T>,
			&& !std::is_same_v<T, U>
			&& std::conjunction_v<
				std::is_constructible<T, U&&>,
				std::negation<std::is_constructible<T, Optional<U>&>>,
				std::negation<std::is_constructible<T, Optional<U>&&>>,
				std::negation<std::is_constructible<T, const Optional<U>&>>,
				std::negation<std::is_constructible<T, const Optional<U>&&>>,
				std::negation<std::is_convertible<Optional<U>&, T>>,
				std::negation<std::is_convertible<Optional<U>&&, T>>,
				std::negation<std::is_convertible<const Optional<U>&, T>>,
				std::negation<std::is_convertible<const Optional<U>&&, T>>
			>,
			bool
		> = false
	>
	constexpr Optional(Optional<U>&& other) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_([&]() -> data_type {
			if(other.has_value()) {
				return data_type(in_place, std::move(other.val()));
			} else {
				return data_type(detail::empty_tag);
			}
		}())
	{
		
	}
	
	template <
		class U = T,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_convertible<U&&, T>>,
				std::is_constructible<T, U&&>,
				std::negation<std::is_same<std::decay_t<U>, in_place_t>>,
				std::negation<std::is_same<std::decay_t<U>, nullopt_t>>,
				std::negation<std::is_same<std::decay_t<U>, Optional>>
			>,
			bool
		> = false
	>
	constexpr explicit Optional(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_(in_place, std::forward<U>(v))
	{
			
	}

	template <
		class U = T,
		std::enable_if_t<
			std::conjunction_v<
				std::is_convertible<U&&, T>,
				std::is_constructible<T, U&&>,
				std::negation<std::is_same<std::decay_t<U>, in_place_t>>,
				std::negation<std::is_same<std::decay_t<U>, nullopt_t>>,
				std::negation<std::is_same<std::decay_t<U>, Optional>>
			>,
			bool
		> = false
	>
	constexpr Optional(U&& v) noexcept(std::is_nothrow_constructible_v<T, U&&>):
		data_(in_place, std::forward<U>(v))
	{
			
	}


	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, Args...>,
			bool
		> = false
	>
	constexpr explicit Optional(in_place_t, Args&& ... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>):
		data_(in_place, std::forward<Args>(args)...)
	{
			
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr explicit Optional(in_place_t, std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args&&...>
	):
		data_(in_place, ilist, std::forward<Args>(args)...)
	{
			
	}

	constexpr Optional(nullopt_t) noexcept:
		data_(detail::empty_tag)
	{

	}

	constexpr Optional& operator=(const Optional&) = default;
	constexpr Optional& operator=(Optional&&) = default;

	constexpr Optional& operator=(nullopt_t) noexcept {
		if(!this->has_value()) {
			return *this;
		}
		data_.destruct();
		data_.has_value() = false;
		return *this;
	}

	template <
		class U,
		std::enable_if_t<
			std::conjunction_v<
				std::negation<std::is_same<Optional, detail::remove_cvref_t<U>>>,
    				std::is_constructible<T, U&&>,
    				std::is_assignable<T&, U&&>,
    				std::disjunction<
					std::negation<std::is_scalar<T>>,
					std::negation<std::is_same<T, std::decay_t<U>>>
				>
			>,
			bool
		> = false
	>
	constexpr Optional& operator=(U&& v) noexcept(
		std::is_nothrow_assignable_v<T&, U&&>
		&& std::is_nothrow_constructible_v<T, U&&>
	)
	{
		if(this->has_value()) {
			this->val() = std::forward<U>(v);
			return *this;
		}
		data_.emplace(std::forward<U>(v));
		data_.has_value() = true;
		return *this;
	}

	template <
		class Opt,
		std::enable_if_t<
			std::conjunction_v<
				detail::is_optional<std::decay_t<Opt>>,
				std::negation<std::is_same_v<T, typename std::decay_t<Opt>>>,
				std::negation<std::is_constructible<T, std::decay_t<Opt>&&>>>,
				std::negation<std::is_constructible<T, const std::decay_t<Opt>&&>>>,
				std::negation<std::is_constructible<T, std::decay_t<Opt>&>>>,
				std::negation<std::is_constructible<T, const std::decay_t<Opt>&>>>,
				std::negation<std::is_assignable<T&, std::decay_t<Opt>&&>>>,
				std::negation<std::is_assignable<T&, const std::decay_t<Opt>&&>>>,
				std::negation<std::is_assignable<T&, std::decay_t<Opt>&>>>,
				std::negation<std::is_assignable<T&, const std::decay_t<Opt>&>>>,
				std::negation<std::is_convertible<std::decay_t<Opt>&&, T>>>,
				std::negation<std::is_convertible<const std::decay_t<Opt>&&, T>>>,
				std::negation<std::is_convertible<std::decay_t<Opt>&, T>>>,
				std::negation<std::is_convertible<const std::decay_t<Opt>&, T>>>,
				std::conditional_t<
					std::is_rvalue_reference_v<Opt&&>,
					std::is_constructible<T, typename std::decay_t<Opt>::value_type>,
					std::is_constructible<T, const typename std::decay_t<Opt>::value_type&>
				>,
				std::conditional_t<
					std::is_rvalue_reference_v<Opt&&>,
					std::is_assignable<T&, typename std::decay_t<Opt>::value_type>,
					std::is_assignable<T&, const typename std::decay_t<Opt>::value_type&>
				>
			>,
			bool
		> = false

	>
	constexpr Optional& operator=(Opt&& v) noexcept(
		std::is_nothrow_assignable_v<T&, U&&>
		&& std::is_nothrow_constructible_v<T, U&&>
	)
	{
		if(this->has_value()) {
			if(v.has_value()) {
				data_.value() = *std::forward<Opt>(v);
			} else {
				data_.destruct();
				data_.has_value() = false;
			}
		} else {
			if(v.has_value()) {
				data_.emplace(*std::forward<Opt>(v));
				data_.has_value() = true;
			} else {
				(void)0;
			}
		}
		return *this;
	}


	template <
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, Args&&...>,
			bool
		> = false
	>
	constexpr T& emplace(Args&& ... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>) {
		if constexpr(std::is_nothrow_constructible_v<T, Args&&...>) {
			if(data_.has_value()) {
				data_.destruct();
			}
			data_.emplace(std::forward<Args>(args)...);
			data_.has_value() = true;
		} else if constexpr(std::is_nothrow_move_constructible_v<T>) {
			if(!data_.has_value()) {
				data_.emplace(std::forward<Args>(args)...);
			} else {
				T tmp(std::move(data_.value());
				data_.destruct();
				auto guard = make_manual_scope_guard([&]() {
					data_.emplace(std::move(tmp));
				});
				data_.emplace(std::forward<Args>(args)...);
				guard.active = false;
				data_.has_value() = true;
			}
		} else {
			if(data_.has_value()) {
				data_.destruct();
				data_.has_value() = false;
			}
			data_.emplace(std::forward<Args>(args)...);
			data_.has_value() = true;
		}
		return this->val();
	}

	template <
		class U,
		class ... Args,
		std::enable_if_t<
			std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>,
			bool
		> = false
	>
	constexpr T& emplace(std::initializer_list<U> ilist, Args&& ... args) noexcept(
		std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args&&...>
	) {
		return this->emplace<std::initializer_list<U>&, Args...>(ilist, std::forward<Args>(args)...);
	}

	template <
		class Other = Optional,
		std::enable_if_t<
			std::conjunction_v<
				std::is_same<std::decay_t<Other>, Optional>,
				std::is_move_constructible<T>,
				std::is_swappable<T>
			>,
			bool
		> = false
	>
	constexpr void swap(Other& other) noexcept(
		std::conjunction_v<
			std::is_nothrow_move_constructible<T>,
			std::is_nothrow_swappable<T>,
		>
	) {
		if(this->has_value()) {
			if(other.has_value()) {
				using std::swap;
				swap(this->val(), other.val());
			} else {
				other.data_.emplace(std::move(this->val()));
				this->data_.destruct();
				this->data_.has_value() = false;
			}
		} else {
			if(other.has_value()) {
				data_.emplace(std::move(other.val()));
				other.data_.destruct();
				other.data_.has_value() = false;
			} else {
				(void)0;
			}
		}
	}

	constexpr void reset() {
		if(this->has_value()) {
			data_.destruct();
			data_.has_value() = false;
		}
	}

	constexpr T gut() {
		assert_has_value();
		auto guard = detail::make_manual_scope_guard([&data_](){
			data_.destruct();
			data_.has_value() = false;
		});
		return static_cast<T>(std::move(this->val()));
	}
	
	
	constexpr bool has_value() const {
		return this->data_.has_value();
	}

	explicit constexpr operator bool() const {
		return this->has_value();
	}

	constexpr const T* operator->() const {
		assert_has_value();
		return std::addressof(this->val());
	}

	constexpr T* operator->() {
		assert_has_value();
		return std::addressof(this->val());
	}

	constexpr const T&& operator*() const&& {
		assert_has_value();
		return std::move(this->val());
	}

	constexpr const T& operator*() const& {
		assert_has_value();
		return this->val();
	}

	constexpr T&& operator*() && {
		assert_has_value();
		return std::move(this->val());
	}

	constexpr T& operator*() & {
		assert_has_value();
		return this->val();
	}

	constexpr const T& value() const& {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return this->val();
	}

	constexpr const T&& value() const&& {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return std::move(this->val());
	}

	constexpr T& value() & {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return this->val();
	}

	constexpr T&& value() && {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return std::move(this->val());
	}

	template <class U>
	constexpr T value_or(U&& alt) const& {
		if(this->has_value()) {
			return this->val();
		}
		return std::forward<U>(alt);
	}

	template <class U>
	constexpr T value_or(U&& alt) && {
		if(this->has_value()) {
			return std::move(this->val());
		}
		return std::forward<U>(alt);
	}

private:

	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	constexpr void destruct() noexcept {
		return data_.destruct();
	}

	constexpr const T& val() const {
		return this->data_.value();
	}

	constexpr T& val() {
		return this->data_.value();
	}

	optional_data_type data_;
};

template <>
struct Optional<void> {
	template <class U>
	friend struct Optional;

	using value_type = void;

	Optional() = default;
	Optional(const Optional&) = default;
	Optional(Optional&&) = default;

	template <class U, std::enable_if_t<detail::is_cv_void_v<U> && !std::is_same_v<value_type, U>>, bool> = false>
	constexpr Optional(const Optional<U>& other) noexcept
		has_value_(other.has_value())
	{
		
	}

	constexpr explicit Optional(in_place_t) noexcept
		has_value_(true)
	{
			
	}

	constexpr Optional(nullopt_t) noexcept:
		has_value_(false)
	{

	}

	constexpr Optional& operator=(const Optional&) = default;
	constexpr Optional& operator=(Optional&&) = default;

	constexpr Optional& operator=(nullopt_t) noexcept {
		has_value_ = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		has_value_ = true;
	}

	constexpr void swap(Other& other) noexcept {
		std::swap(has_value_, other.has_value_);
	}

	constexpr void reset() noexcept {
		has_value_ = false;
	}

	constexpr void gut() {
		assert_has_value();
		has_value_ = false;
	}
	
	
	constexpr bool has_value() const {
		return has_value_;
	}

	explicit constexpr operator bool() const {
		return has_value_;
	}

	constexpr void value() const {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return this->val();
	}

private:

	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	bool has_value_ = false;
};

template <>
struct Optional<const void> {
	template <class U>
	friend struct Optional;

	using value_type = const void;

	Optional() = default;
	Optional(const Optional&) = default;
	Optional(Optional&&) = default;

	template <class U, std::enable_if_t<detail::is_cv_void_v<U> && !std::is_same_v<value_type, U>>, bool> = false>
	constexpr Optional(const Optional<U>& other) noexcept
		has_value_(other.has_value())
	{
		
	}

	constexpr explicit Optional(in_place_t) noexcept
		has_value_(true)
	{
			
	}

	constexpr Optional(nullopt_t) noexcept:
		has_value_(false)
	{

	}

	constexpr Optional& operator=(const Optional&) = default;
	constexpr Optional& operator=(Optional&&) = default;

	constexpr Optional& operator=(nullopt_t) noexcept {
		has_value_ = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		has_value_ = true;
	}

	constexpr void swap(Other& other) noexcept {
		std::swap(has_value_, other.has_value_);
	}

	constexpr void reset() noexcept {
		has_value_ = false;
	}

	constexpr void gut() {
		assert_has_value();
		has_value_ = false;
	}
	
	
	constexpr bool has_value() const {
		return has_value_;
	}

	explicit constexpr operator bool() const {
		return has_value_;
	}

	constexpr void value() const {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return this->val();
	}

private:

	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	bool has_value_ = false;
};

template <>
struct Optional<volatile void> {
	template <class U>
	friend struct Optional;

	using value_type = volatile void;

	Optional() = default;
	Optional(const Optional&) = default;
	Optional(Optional&&) = default;

	template <class U, std::enable_if_t<detail::is_cv_void_v<U> && !std::is_same_v<value_type, U>>, bool> = false>
	constexpr Optional(const Optional<U>& other) noexcept
		has_value_(other.has_value())
	{
		
	}

	constexpr explicit Optional(in_place_t) noexcept
		has_value_(true)
	{
			
	}

	constexpr Optional(nullopt_t) noexcept:
		has_value_(false)
	{

	}

	constexpr Optional& operator=(const Optional&) = default;
	constexpr Optional& operator=(Optional&&) = default;

	constexpr Optional& operator=(nullopt_t) noexcept {
		has_value_ = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		has_value_ = true;
	}

	constexpr void swap(Other& other) noexcept {
		std::swap(has_value_, other.has_value_);
	}

	constexpr void reset() noexcept {
		has_value_ = false;
	}

	constexpr void gut() {
		assert_has_value();
		has_value_ = false;
	}
	
	
	constexpr bool has_value() const {
		return has_value_;
	}

	explicit constexpr operator bool() const {
		return has_value_;
	}

	constexpr void value() const {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return this->val();
	}

private:

	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	bool has_value_ = false;
};

template <>
struct Optional<const volatile void> {
	template <class U>
	friend struct Optional;

	using value_type = const volatile void;

	Optional() = default;
	Optional(const Optional&) = default;
	Optional(Optional&&) = default;

	template <class U, std::enable_if_t<detail::is_cv_void_v<U> && !std::is_same_v<value_type, U>>, bool> = false>
	constexpr Optional(const Optional<U>& other) noexcept
		has_value_(other.has_value())
	{
		
	}

	constexpr explicit Optional(in_place_t) noexcept
		has_value_(true)
	{
			
	}

	constexpr Optional(nullopt_t) noexcept:
		has_value_(false)
	{

	}

	constexpr Optional& operator=(const Optional&) = default;
	constexpr Optional& operator=(Optional&&) = default;

	constexpr Optional& operator=(nullopt_t) noexcept {
		has_value_ = false;
		return *this;
	}

	constexpr void emplace() noexcept {
		has_value_ = true;
	}

	constexpr void swap(Other& other) noexcept {
		std::swap(has_value_, other.has_value_);
	}

	constexpr void reset() noexcept {
		has_value_ = false;
	}

	constexpr void gut() {
		assert_has_value();
		has_value_ = false;
	}
	
	
	constexpr bool has_value() const {
		return has_value_;
	}

	explicit constexpr operator bool() const {
		return has_value_;
	}

	constexpr void value() const {
		if(!this->has_value()) {
			throw_bad_optional_access();
		}
		return this->val();
	}

private:

	constexpr void assert_has_value() const {
#if defined(assert) && !defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(this->has_value());
#endif
	}

	constexpr void assert_not_has_value() const {
#if defined(assert) && defined(TIM_OPTIONAL_OPTIONAL_DISABLE_ASSERTIONS)
		assert(not this->has_value());
#endif
	}

	bool has_value_ = false;
};

namespace traits::detail {

template <class R1, class R2>
struct results_are_equality_comparable{};

template <class T1, class E1, class T2, class E2>
struct results_are_equality_comparable<Optional<T1, E1>, Optional<T2, E2>> {
	template <class T>
	struct Tag {};

	template <class T, class U, class R1 = decltype(std::declval<const T&>() == std::declval<const U&>())>
	static constexpr auto is_eq_comp(Tag<T>, Tag<U>, int)
		-> std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>
	{
		return std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>{};
	}
	
	template <class T, class U>
	static constexpr std::false_type is_eq_comp(Tag<T>, Tag<U>, ...) { return std::false_type{}; }
	
	static constexpr bool value = std::conjunction_v<
		std::disjunction<
			std::conjunction<
				::tim::result::detail::is_cv_void<T1>,
				::tim::result::detail::is_cv_void<T2>
			>,
			decltype(is_eq_comp(Tag<T1>{}, Tag<T2>{}, 0))
		>,
		decltype(is_eq_comp(Tag<E1>{}, Tag<E2>{}, 0))
	>;
};

template <class R1, class R2>
inline constexpr bool results_are_equality_comparable_v
	= results_are_equality_comparable<R1, R2>::value;

template <class R1, class R2>
struct results_are_inequality_comparable{};
template <class T1, class E1, class T2, class E2>
struct results_are_inequality_comparable<Optional<T1, E1>, Optional<T2, E2>> {
	template <class T>
	struct Tag {};

	template <class T, class U, class R1 = decltype(std::declval<const T&>() != std::declval<const U&>())>
	static constexpr auto is_eq_comp(Tag<T>, Tag<U>, int)
		-> std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>
	{
		return std::conditional_t<
			is_contextually_convertible_to_bool_v<R1>,
			std::true_type,
			std::false_type
		>{};
	}
	
	template <class T, class U>
	static constexpr std::false_type is_eq_comp(Tag<T>, Tag<U>, ...) { return std::false_type{}; }
	
	static constexpr bool value = std::conjunction_v<
		std::disjunction<
			std::conjunction<
				::tim::result::detail::is_cv_void<T1>,
				::tim::result::detail::is_cv_void<T2>
			>,
			decltype(is_eq_comp(Tag<T1>{}, Tag<T2>{}, 0))
		>,
		decltype(is_eq_comp(Tag<E1>{}, Tag<E2>{}, 0))
	>;
};

template <class R1, class R2>
inline constexpr bool results_are_inequality_comparable_v
	= results_are_inequality_comparable<R1, R2>::value;


} /* namespace traits::detail */

/* --- Equality Operators --- */
template <
	class T1, class E1,
	class T2, class E2,
	std::enable_if_t<
		traits::detail::results_are_equality_comparable_v<Optional<T1, E1>, Optional<T2, E2>>,
		bool
	> = false
>
constexpr bool operator==(const Optional<T1, E1>& lhs, const Optional<T2, E2>& rhs) {
	if(lhs.has_value()) {
		if(rhs.has_value()) {
			if constexpr(!detail::is_cv_void_v<T1>) {
				return *lhs == *rhs;
			} else {
				return true;
			}
		}
		return false;
	} else {
		if(!rhs.has_value()) {
			return lhs.error() == rhs.error();
		}
		return false;
	}
}

template <
	class T1,
	class E1,
	class T2,
	std::enable_if_t<
		!detail::is_cv_void_v<T1>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const T1&>() == std::declval<T2&&>())>
		&& !traits::is_result_v<std::decay_t<T2>>
		&& !traits::is_error_v<std::decay_t<T2>>,
		bool
	> = false
>
constexpr bool operator==(const Optional<T1, E1>& lhs, T2&& rhs) {
	if(lhs.has_value()) {
		return *lhs == std::forward<T2>(rhs);
	} else {
		return false;
	}
}

template <
	class T1,
	class T2,
	class E2,
	std::enable_if_t<
		!detail::is_cv_void_v<T2>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<T1&&>() == std::declval<const T2&>())>
		&& !traits::is_result_v<T1>
		&& !traits::is_error_v<T1>,
		bool
	> = false
>
constexpr bool operator==(T1&& lhs, const Optional<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return std::forward<T1>(lhs) == *rhs;
	} else {
		return false;
	}
}

template <
	class T1,
	class E1,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() == std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator==(const Optional<T1, E1>& lhs, const Error<E2>& rhs) {
	if(lhs.has_value()) {
		return false;
	}
	return lhs.error() == rhs.value();
}

template <
	class E1,
	class T2,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() == std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator==(const Error<E1>& lhs, const Optional<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return false;
	}
	return lhs.value() == rhs.error();
}

/* --- Inequality Operators --- */
template <
	class T1, class E1,
	class T2, class E2,
	std::enable_if_t<
		traits::detail::results_are_inequality_comparable_v<Optional<T1, E1>, Optional<T2, E2>>,
		bool
	> = false
>
constexpr bool operator!=(const Optional<T1, E1>& lhs, const Optional<T2, E2>& rhs) {
	static_assert(detail::is_cv_void_v<T1> == detail::is_cv_void_v<T2>);
	if(lhs.has_value()) {
		if(rhs.has_value()) {
			if constexpr(!detail::is_cv_void_v<T1>) {
				return *lhs != *rhs;
			} else {
				return false;
			}
		}
		return true;
	} else {
		if(!rhs.has_value()) {
			return lhs.error() != rhs.error();
		}
		return true;
	}
}

template <
	class T1,
	class E1,
	class T2,
	std::enable_if_t<
		!detail::is_cv_void_v<T1>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const T1&>() != std::declval<T2&&>())>
		&& !traits::is_result_v<std::decay_t<T2>>
		&& !traits::is_error_v<std::decay_t<T2>>,
		bool
	> = false
>
constexpr bool operator!=(const Optional<T1, E1>& lhs, T2&& rhs) {
	if(lhs.has_value()) {
		return *lhs != std::forward<T2>(rhs);
	} else {
		return true;
	}
}

template <
	class T1,
	class T2,
	class E2,
	std::enable_if_t<
		!detail::is_cv_void_v<T2>
		&& traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<T1&&>() != std::declval<const T2&>())>
		&& !traits::is_result_v<T1>
		&& !traits::is_error_v<T1>,
		bool
	> = false
>
constexpr bool operator!=(T1&& lhs, const Optional<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return std::forward<T1>(lhs) != *rhs;
	} else {
		return true;
	}
}

template <
	class T1,
	class E1,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() != std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator!=(const Optional<T1, E1>& lhs, const Error<E2>& rhs) {
	if(lhs.has_value()) {
		return true;
	}
	return lhs.error() != rhs.value();
}

template <
	class E1,
	class T2,
	class E2,
	std::enable_if_t<
		traits::detail::is_contextually_convertible_to_bool_v<decltype(std::declval<const E1&>() != std::declval<const E2&>())>,
		bool
	> = false
>
constexpr bool operator!=(const Error<E1>& lhs, const Optional<T2, E2>& rhs) {
	if(rhs.has_value()) {
		return true;
	}
	return lhs.value() != rhs.error();
}

template <
	class T,
	class E,
	std::enable_if_t<
		!detail::is_cv_void_v<T>
		&& std::is_move_constructible_v<T>
		&& std::is_swappable_v<T>
		&& std::is_move_constructible_v<E>
		&& std::is_swappable_v<E>
		&& (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>),
		bool
	> = false
>
constexpr void swap(Optional<T>& lhs, Optional<T>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	return lhs.swap(rhs);
}

template <
	class T,
	class E,
	std::enable_if_t<
		std::is_move_constructible_v<E>
		&& std::is_swappable_v<E>,
		bool
	> = false
>
constexpr void swap(Optional<void, E>& lhs, Optional<void, E>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	return lhs.swap(rhs);
}

template <
	class T,
	class E,
	std::enable_if_t<
		std::is_move_constructible_v<E>
		&& std::is_swappable_v<E>,
		bool
	> = false
>
constexpr void swap(Optional<const void, E>& lhs, Optional<const void, E>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	return lhs.swap(rhs);
}

template <
	class T,
	class E,
	std::enable_if_t<
		std::is_move_constructible_v<E>
		&& std::is_swappable_v<E>,
		bool
	> = false
>
constexpr void swap(Optional<volatile void, E>& lhs, Optional<volatile void, E>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	return lhs.swap(rhs);
}

template <
	class T,
	class E,
	std::enable_if_t<
		std::is_move_constructible_v<E>
		&& std::is_swappable_v<E>,
		bool
	> = false
>
constexpr void swap(Optional<const volatile void, E>& lhs, Optional<const volatile void, E>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
	return lhs.swap(rhs);
}

} /* inline namespace optional */

} /* namespace tim */

#endif /* TIM_OPTIONAL_OPTIONAL_HPP */
