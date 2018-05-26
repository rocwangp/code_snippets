/* 需求：获取函数的返回值，参数个数，每一个参数类型
 * 思路：通过模板参数推导，进行适当的特化
 */

#include <iostream>
#include <string>
#include <tuple>
#include <functional>

template <typename R, typename... Args>
struct function_traits_helper
{
    static constexpr auto param_count = sizeof...(Args);
    using return_type = R;

    template <std::size_t N>
    using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
};

template <typename T>
struct function_traits;

// int(*)(int, int)
template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> : public function_traits_helper<R, Args...>
{
};

// int(&)(int, int)
template <typename R, typename... Args>
struct function_traits<R(&)(Args...)> : public function_traits_helper<R, Args...>
{
};

// int(int, int)
template <typename R, typename... Args>
struct function_traits<R(Args...)> : public function_traits_helper<R, Args...>
{
};

// int(int, int) const
template <typename R, typename... Args>
struct function_traits<R(Args...) const> : public function_traits_helper<R, Args...>
{
};

// int(main()::<lambda(int, int)>*)(int, int)
// int(std::funciton<int(int, int)>)(int, int)
template <typename ClassType, typename R, typename... Args>
struct function_traits<R(ClassType::*)(Args...)> : public function_traits_helper<R, Args...>
{
    using class_type = ClassType;
};

// int(main()::<lambda(int, int)>*)(int, int) const
// int(std::funciton<int(int, int)>)(int, int) const
template <typename ClassType, typename R, typename... Args>
struct function_traits<R(ClassType::*)(Args...) const> : public function_traits_helper<R, Args...>
{
    using class_type = ClassType;
};

// 对函数对象进行operator()展开
// main()::<lambda(int, int)>
// std::function<int(int, int)>
template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

template <typename Func>
void traits_test(Func&&) {
    using function_t = function_traits<Func>;
    std::cout << function_t::param_count << std::endl;
}

int pointer_func(int a, int b) {
    return a + b;
}


int main()
{
    // Func =  main()::<lambda(int, int)>
    // Func::operator() = int (main()::<lambda(int, int)*)(int, int)
    auto func = [](int a, int b) { return a + b; };
    traits_test(std::move(func));

    traits_test([](int a, int b) { return a + b; });

    // Func = int(int, int)
    using function_t = function_traits<decltype(pointer_func)>;
    std::cout << function_t::param_count << std::endl;

    // Func = int(&)(int, int) or Func = int(*)(int, int)
    traits_test(pointer_func);

    // Func = std::function<int(int, int)> const;
    // Func::opeartor() = int (std::function<int(int, int)>*)(int, int) const
    std::function<int(int, int)> f = std::bind(pointer_func, std::placeholders::_1, std::placeholders::_2);
    traits_test(f);

    return 0;
}


