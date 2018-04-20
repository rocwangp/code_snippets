/* 需求：获取函数的返回值，参数个数，每一个参数类型
 * 思路：通过模板参数推导，进行适当的特化
 */

#include <iostream>
#include <string>
#include <tuple>
#include <functional>

/* 仅仅为了特化而存在 */
template <typename R, typename... Args>
struct function_traits { };

/* 对于函数类型的特化，如int(int, std::string) */
template <typename R, typename... Args>
struct function_traits<R(Args...)> {
    static constexpr std::size_t arity = sizeof...(Args);
    using return_type = R;

    /* 通过tuple_element_t获取Args...中的某个类型 */
    template <std::size_t N>
    using arg = typename std::tuple_element_t<N, std::tuple<Args...>>;
};

/* 对于函数指针类型的特化，如int(*)(int, std::string) */
template <typename R, typename... Args>
struct function_traits<R (*)(Args...)> {
    static constexpr std::size_t arity = sizeof...(Args);
    using return_type = R;

    template <std::size_t N>
    using arg = typename std::tuple_element_t<N, std::tuple<Args...>>;
};


int func(int, std::string) { return 1; }

/* 作为模板参数传递时，函数类型转换为函数指针类型，所以需要单独特化 */
template <typename Function>
void get_function_type_and_arg(Function) {
    std::cout << "argument count is: " << function_traits<Function>::arity << std::endl;
    std::cout << "return type is same: "
              << std::is_same_v<int,
                                typename function_traits<Function>::return_type> << std::endl;
    std::cout << "argument type is same: "
              << std::is_same_v<std::string,
                                typename function_traits<Function>::template arg<1>> << std::endl;
}

int main()
{
    /* 调用函数类型的特化版本 */
    std::cout << "argument count is: " << function_traits<decltype(func)>::arity << std::endl;
    std::cout << "return type is same: "
              << std::is_same_v<int,
                                typename function_traits<decltype(func)>::return_type> << std::endl;
    std::cout << "argument type is same: "
              << std::is_same_v<std::string,
                                typename function_traits<decltype(func)>::template arg<1>> << std::endl;

    /* 调用函数指针类型的特化版本 */
    get_function_type_and_arg(func);
    return 0;
}



