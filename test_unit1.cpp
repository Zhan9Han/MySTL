#include "TypeTraits.h"
#include <iostream>
int main(int argc, char argv[]) {

	class Foo {
	public:
		double x;
	};
	union foo {
	public:
		double x;
		double y;
	};
	std::cout << mystl::_is_class_v<Foo> << std::endl<< mystl::_is_union_v<foo><< std::endl;
	//std::cout << mystl::_is_pod_type_v<int> << std::endl << mystl::_is_pod_type_v<foo> << std::endl;
	//std::cout << mystl::_is_pod_type_v<int*> << std::endl << mystl::_is_pod_type_v<nullptr_t> << std::endl;
	std::cout << mystl::_is_pointer_v<nullptr_t> << std::endl<<mystl::_is_pointer_v<void*> << std::endl;

	mystl::_add_const_t<const int> t = 42;
	std::cout << typeid(t).name() << std::endl;
	return 0;
}