#pragma once
#ifndef _MY_TYPE_TRAITS_
#define _MY_TPYE_TRAITS_
//UNDER C++ 20
#include <cstddef>
namespace mystl {

template<typename T,T v>
struct _integral_constant {
	using value_type = T;
	using type = _integral_constant<T, v>;
	//Static is essential, because we will not instantiate this class actually.
	static constexpr T value = v;

	constexpr operator value_type() const noexcept {
		return value;
	}

	constexpr value_type operator()() const noexcept {
		return value;
	}
};

template<bool B>
using _bool_constant = _integral_constant<bool, B>;

//Notice that _true_type and _false_type is specialization of _integral_constant, 
//they are different classes, so the static member "value" will not be shared. 
using _true_type = _bool_constant<true>;
using _false_type = _bool_constant<false>;

/*********basic _type_identify*************/

template<typename T>
struct _type_identify {
	using type = T;
};

/******************_remove_cv*****************/

template<typename T>
using _type = typename _type_identify<T>::type;

template<typename T>
struct _remove_volatile : _type_identify<T> {};

template<typename T>
struct _remove_volatile<T volatile> : _type_identify<T> {};

template<typename T>
using _remove_volatile_t = typename _remove_volatile<T>::type;

template<typename T>
struct _remove_const : _type_identify<T> {};

template<typename T>
struct _remove_const<T const> : _type_identify<T> {};

template<typename T>
using _remove_const_t = typename _remove_const<T>::type;

template<typename T>
struct _remove_cv : _remove_const<_remove_volatile_t<T>>{};

template<typename T>
using _remove_cv_t = typename _remove_cv<T>::type;

/***************add cv*******************/
template<typename T>
struct _add_const : _type_identify<const T>{};

template<typename T>
using _add_const_t = typename _add_const<T>::type;

template<typename T>
struct _add_volatile : _type_identify<volatile T> {};

template<typename T>
using _add_volatile_t = typename _add_volatile<T>::type;

template<typename T>
struct _add_cv : _type_identify<const volatile T> {};

template<typename T>
using _add_cv_t = typename _add_cv<T>::type;

/******switch-logic******/

template<bool condition, typename T, typename F>
struct _type_switch: _type_identify<T>{};

template<typename T, typename F>
struct _type_switch<false, T, F>: _type_identify<F>{};

/*********union\class\... *************/

template<typename T>
using _is_union = _bool_constant<__is_union(T)>;

template<typename T>
constexpr bool _is_union_v = _is_union<T>::value;

namespace detail {

	//Ignore the warning because we can use decltype to get the return type
	//even though the compiler do not know how the function running.
	template<typename T>
	_bool_constant<! _is_union_v<T>> is_class_or_union(int T::*);

	template<typename T>
	_false_type is_class_or_union(...);
}

//This feature can also be implemented using compiler function __is_class,
//actually that is how STL did it. 
//The function is_class_or_union judges if the type have a pointer to its member,
//so it's equal to have_pointer_to_member.
template<typename T>
using _is_class = decltype(detail::is_class_or_union<T>(nullptr));

template<typename T>
constexpr bool _is_class_v = _is_class<T>::value;

/*********************_is_base_of***************************/

//Base class and its derived class have proprities as follows:
//1.When a pointer point to base class convert to the derived class,
//	the process of conversion must be declared, e.g. static_cast<Derived*> Base*
//2.When a pointer point to derived class convert to its base class,
//	the process of conversion can be implicit.
namespace detail {

	template<typename B>
	_true_type test_ptr_convert(const volatile B*);

	template<typename>
	_false_type test_ptr_convert(const volatile void*);

	template<typename B, typename D>
	auto test_is_base_of(int) -> decltype(test_ptr_convert<B>(static_cast<D*>(nullptr)));

	//Other cases will catched by this meta function.
	template <typename, typename>
	auto test_is_base_of(...) -> _true_type;
}

template<typename Base, typename Derived>
using _is_base_of_type = decltype(detail::test_is_base_of<Base, Derived>(0));

template<typename Base, typename Derived>
struct _is_base_of : _bool_constant<
	_is_class_v<Base>
	&& _is_class_v<Derived>
	&& _is_base_of_type<Base, Derived>::value> {
};

template<typename Base, typename Derived>
constexpr bool _is_base_of_v = _is_base_of<Base, Derived>::value;

/*********************_is_in_pack****************************/

//Use _is_base_of to implement this feature.
namespace detail {

	template<typename ...Ts>
	struct is_in_pack_impl : _type_identify<Ts> ...{};
}
template<typename TargetT, typename ...Ts>
constexpr bool _is_in_pack_v = _is_base_of_v<_type_identify<TargetT>, detail::is_in_pack_impl<Ts...>>;

template<typename TargetT, typename ...Ts>
struct _is_in_pack : _bool_constant<_is_in_pack_v<TargetT, Ts...>> {};

//Other implemention such as recursion can find in cppcon20 slide.

/******************pointer identify**********************/

//There are not only const T* but T* const, whose const identifer can 
//not be removed by remove_cv, so we must use specialization to solve
//this problem.
template<typename T>
constexpr bool _is_pointer_v = false;

template<typename T>
constexpr bool _is_pointer_v<T*> = true;

template<typename T>
constexpr bool _is_pointer_v<T* const> = true;

template<typename T>
constexpr bool _is_pointer_v<T* volatile> = true;

template<typename T>
constexpr bool _is_pointer_v<T* const volatile> = true;

template<typename T>
using _is_pointer = _bool_constant<_is_pointer_v<T>>;
/*******************reference identify*******************/

template<typename T>
struct _is_lvalue_reference : _false_type{};

template<typename T>
struct _is_lvalue_reference<T &> : _true_type{};

template<typename T>
constexpr bool _is_lvalue_reference_v = _is_lvalue_reference<T>::value;

template<typename T>
struct _is_rvalue_reference : _false_type {};

template<typename T>
struct _is_rvalue_reference<T &&> : _true_type {};

template<typename T>
constexpr bool _is_rvalue_reference_v = _is_rvalue_reference<T>::value;

template<typename T>
constexpr bool _is_reference_v = _is_lvalue_reference_v<T> || _is_rvalue_reference_v<T>;

template<typename T>
using _is_reference = _bool_constant<_is_reference_v<T>>;

/*********basic type(e.g. int,float) identify***********/

template<typename T1, typename T2>
struct _is_same : _false_type {};

template<typename T>
struct _is_same<T, T> : _true_type {};

template<typename T1, typename T2>
constexpr bool _is_same_v = _is_same<T1, T2>::value;

template<typename T1, typename T2>
using _is_same_raw = _is_same<_remove_cv_t<T1>, _remove_cv_t<T2>>;

template<typename T1, typename T2>
constexpr bool _is_same_raw_v = _is_same_raw<T1, T2>::value;

//nullptr
//Type nullptr_t is not equal to void*, _is_pointer_v<nullptr_t> = false;
template<typename T>
constexpr bool _is_nullptr_v = _is_same_raw_v<T, std::nullptr_t>;

template<typename T>
using _is_nullptr = _bool_constant<_is_nullptr_v<T>>;

//void
template<typename T>
constexpr bool _is_void_v = _is_same_raw_v<T, void>;

template<typename T>
using _is_void = _bool_constant<_is_void_v<T>>;

//integral
//char8_t since C++20
template<typename T>
constexpr bool _is_integral_v = _is_in_pack_v<_remove_cv_t<T>
	, bool, char, signed char, unsigned char
	, char8_t, char16_t, char32_t, wchar_t
	, short, int, long, long long, unsigned short
	, unsigned int, unsigned long, unsigned long long
>;


template<typename T>
using _is_integral = _bool_constant<_is_integral_v<T>>;

//float
template<typename T>
constexpr bool _is_floating_point_v = _is_in_pack_v<_remove_cv_t<T>, float, double, long double>;

template<typename T>
using _is_floating_point = _bool_constant<_is_floating_point_v<T>>;

/*****************************big type******************************/

//arithmetic
template<typename T>
constexpr bool _is_arithmetic_type_v = (_is_floating_point_v<T> || _is_integral_v<T>);

template<typename T>
using _is_arithmetic_type = _bool_constant<_is_arithmetic_type_v<T>>;

//fundmental
template<typename T>
constexpr bool _is_fundamental_type_v = (_is_arithmetic_type_v<T> || _is_nullptr_v<T> || _is_void_v<T>);

template<typename T>
using _is_fundmental_type = _bool_constant<_is_fundamental_type_v<T>>;

//composed
template<typename T>
constexpr bool _is_compound_type_v = !_is_fundamental_type_v<T>;

template<typename T>
using _is_compound_type = _bool_constant<_is_compound_type_v<T>>;


//Features not implemented yet:
//		is_function
//		is_enum
//		is_array
//And other feature based on these features above, such as
//		is_scalar
}
#endif