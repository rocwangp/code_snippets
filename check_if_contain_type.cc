/* 给定一个类型列表，判断这些类型中是否包含某个类别
 * 通过std::conditional实现，使用方法
 *
 * std::conditional<bool, type1, type2>::type
 * 如果bool表达式为true，则返回type1，否则返回type2 */

#include <type_traits>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

template <typename T, typename... Args>
struct contains : std::false_type {};

template <typename T, typename U, typename... Args>
struct contains<T, U, Args...>
    : std::conditional_t<std::is_same_v<T, U>, std::true_type, contains<T, Args...>>
{ };


        template <typename T, typename ... Args>
        struct contain
        {
            static constexpr auto value = std::false_type{};
        };

        template <typename T, typename... Args>
        struct contain<T, T, Args...>
        {
            static constexpr auto value = std::true_type{};
        };

        template <typename T, typename U, typename... Args>
        struct contain<T, U, Args...>
        {
            static constexpr auto value = contain<T, Args...>::value;
        };

struct Type {};

template <typename T, typename... Args>
void check_if_contain_type(std::tuple<Args...>&) {
    if constexpr (contains<T, Args...>::value) {
        std::cout << "contains " << typeid(T).name() << std::endl;
    }
    else {
        std::cout << "no contains " << typeid(T).name() << std::endl;
    }
}

int main()
{
    std::tuple t { 1, 1.0, std::string{ "123" }, Type{} };
    check_if_contain_type<int>(t);
    check_if_contain_type<Type>(t);
    check_if_contain_type<std::vector<int>>(t);
    return 0;
}




