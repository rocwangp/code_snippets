/* 需求：获取函数的返回值，参数个数，每一个参数类型
 * 思路：通过模板参数推导，进行适当的特化
 */

#include <iostream>
#include <string>
#include <tuple>
#include <functional>

template <typename T>
struct function_traits;

template <typename ClassType, typename R, typename... Args>
struct function_traits<R(ClassType::*)(Args...)>
{
    using return_type = R;
    static constexpr std::size_t param_count = sizeof...(Args);
    static constexpr std::size_t traits_number = 0;

    template <std::size_t N>
    using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
};

template <typename ClassType, typename R, typename... Args>
struct function_traits<R(ClassType::*)(Args...) const>
{
    using return_type = R;
    static constexpr std::size_t param_count = sizeof...(Args);
    static constexpr std::size_t traits_number = 1;

    template <std::size_t N>
    using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
};

template <typename R, typename... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;
    static constexpr std::size_t param_count = sizeof...(Args);
    static constexpr std::size_t traits_number = 2;

    template <std::size_t N>
    using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
};

template <typename R, typename... Args>
struct function_traits<R(Args...) const>
{
    using return_type = R;
    static constexpr std::size_t param_count = sizeof...(Args);
    static constexpr std::size_t traits_number = 3;

    template <std::size_t N>
    using param_type = std::tuple_element_t<N, std::tuple<Args...>>;
};


template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{
};

int func(int, std::string) { return 1; }

/* 作为模板参数传递时，函数类型转换为函数指针类型，所以需要单独特化 */
template <typename Function>
void get_function_type_and_arg(Function) {
    std::cout << typeid(Function).name() << std::endl;
    std::cout << "traits_number is: " << function_traits<Function>::traits_number << std::endl;
    std::cout << "argument count is: " << function_traits<Function>::param_count << std::endl;
    std::cout << "return type is same: "
              << std::is_same_v<int,
                                typename function_traits<Function>::return_type> << std::endl;
    std::cout << "argument type is same: "
              << std::is_same_v<std::string,
                                typename function_traits<Function>::template param_type<1>> << std::endl;
}

int main()
{
    std::cout << typeid(decltype(func)).name() << std::endl;
    std::cout << "traits_number is: " << function_traits<decltype(func)>::traits_number << std::endl;
    std::cout << "argument count is: " << function_traits<decltype(func)>::param_count << std::endl;
    std::cout << "return type is same: "
              << std::is_same_v<int,
                                typename function_traits<decltype(func)>::return_type> << std::endl;
    std::cout << "argument type is same: "
              << std::is_same_v<std::string,
                                typename function_traits<decltype(func)>::template param_type<1>> << std::endl;
    /* 调用函数指针类型的特化版本 */
    std::cout << "\n\n";
    get_function_type_and_arg([](int, std::string) { return "hello world"; });
    return 0;
}



