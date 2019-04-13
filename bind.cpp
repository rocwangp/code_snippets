// ConsoleApplication5.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <pch.h>
#include <iostream>

#include <tuple>
#include <iostream>
#include <functional>

template <typename Callable, typename ... Args>
struct Binder {
public:
	Binder(Callable f, Args... args) : f_(f), args_(std::make_tuple(std::forward<Args>(args)...)) {}

	template <size_t BoundIdx, size_t UnBoundIdx, size_t BoundSize, size_t UnboundSize, typename ... Bounds, typename ... Unbounds>
	constexpr auto select_args(const std::tuple<Bounds...>& bounds, const std::tuple<Unbounds...>& unbounds) {
		if constexpr (BoundIdx >= BoundSize && UnBoundIdx >= UnboundSize) {
			return std::tuple<>{};
		}
		else if constexpr (std::is_placeholder<std::decay_t<std::tuple_element_t<BoundIdx, std::tuple<Bounds...>>>>::value) {
			return std::tuple_cat(std::make_tuple(std::get<UnBoundIdx>(unbounds)), select_args<BoundIdx + 1, UnBoundIdx + 1, BoundSize, UnboundSize>(bounds, unbounds));
		}
		else {
			return std::tuple_cat(std::make_tuple(std::get<BoundIdx>(bounds)), select_args<BoundIdx + 1, UnBoundIdx, BoundSize, UnboundSize>(bounds, unbounds));
		}
	}

	template <size_t... Idx, typename ... Bounds>
	constexpr auto internal_invoke_impl(std::index_sequence<Idx...>, const std::tuple<Bounds...>& bounds) {
		return std::invoke(f_, std::get<Idx>(bounds)...);
	}
	template <typename ... Bounds>
	constexpr auto internal_invoke(const std::tuple<Bounds...>& bounds) {
		return internal_invoke_impl(std::make_index_sequence<sizeof...(Bounds)>{}, bounds);
	}
	template <typename ... Unbounds>
	constexpr auto operator()(Unbounds&& ... unbounds) {
		return internal_invoke(select_args<0, 0, sizeof...(Args), sizeof...(Unbounds)>(args_, std::make_tuple(std::forward<Unbounds>(unbounds)...)));
	}
private:
	Callable f_;
	std::tuple<Args...> args_;
};


template <typename Callable, typename ... Args>
constexpr auto tsl_bind(Callable callback, Args ... args) {
	return Binder<Callable, Args...>(callback, args...);
}

int func(int a, int b) {
	return a + b;
}

class MemberFunc {
public:
	int func(int a, int b) {
		return a + b;
	}
};


int main() {
	auto binder1 = tsl_bind(func, 10, std::placeholders::_1);
	int res = binder1(11);
	std::cout << res << std::endl;

	MemberFunc mf;
	auto binder2 = tsl_bind(&MemberFunc::func, &mf, 10, std::placeholders::_1);
	res = binder2(20);
	std::cout << res << std::endl;
	return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

