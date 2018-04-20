/* std::get(tuple)只支持编译期获取特定位置的元素
 *
 * 需求：实现运行期的元素获取以及遍历tuple
 * 思路：通过std::index_sequence和fold expression */

#include <utility>
#include <iostream>
#include <tuple>


template <std::size_t Idx, typename Tuple, typename Func>
void invoke_nth_element_of_tuple_by_func_impl(Tuple& t, std::size_t n, Func&& f) {
    if(Idx == n) {
        std::forward<Func>(f)(std::get<Idx>(t));
    }
}
template <typename Tuple, typename Func, std::size_t... Idx>
void invoke_nth_element_of_tuple_by_func(std::size_t i, Tuple& t, Func&& f, std::index_sequence<Idx...>) {
    (invoke_nth_element_of_tuple_by_func_impl<Idx>(t, i, std::forward<Func>(f)), ...);
}

int main()
{
    std::tuple t{ 1, 2.0f, std::string{ "hello" } };
    invoke_nth_element_of_tuple_by_func(2, t, [](auto& item) {
        std::cout << item << std::endl;
    }, std::make_index_sequence<std::tuple_size_v<decltype(t)>>{});
    return 0;
}



