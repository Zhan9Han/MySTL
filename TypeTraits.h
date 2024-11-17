#pragma once
#ifndef _MY_TYPE_TRAITS_
#define _MY_TPYE_TRAITS_

namespace mystl {
template<typename T,T v>
struct _integral_constant {
	using value_type = T;
	using type = _integral_constant<T, v>;
	//Òþº¬ÁËinline
	constexpr T value = v;

	static constexpr 
};
}

#endif