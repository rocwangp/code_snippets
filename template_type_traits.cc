#include <iostream>
#include <vector>
#include <tuple>
#include <type_traits>

/* 萃取模板类型的参数，如Test<int, double>，萃取出arg_type<0> == int, arg_type<1> == double */

template <typename T>
class Test
{
    T value;
};

template <typename... Args>
struct template_type_traits {};

template <template <typename...> class ClassType, typename... Args>
struct template_type_traits<ClassType<Args...>>
{
    template <std::size_t N>
    using arg_type = std::tuple_element_t<N, std::tuple<Args...>>;
};


int main()
{
    std::cout << typeid(typename template_type_traits<Test<int>>::template arg_type<0>).name() << std::endl;
    return 0;
}
