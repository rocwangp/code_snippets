/* 给定一个元组std::tuple，其中元组的参数可以未知，也就是形如
 * template <typename... Args> std::tuple<Args...> t;
 *
 * 需求：找到第一个类型为T的元素的值
 *
 * 思路：通过std::get<N>(t)调用取得元组第N个元素的值
 *       需要保证第N个元素的类型是T，所以需要计算N的值
 *       通过模板参数推导以及模板特化，解决相等与不相等的两种情况
 */

#include <tuple>
#include <string>
#include <iostream>


/* 定义普通版本，仅仅是为了可以将其进行特化 */
template <typename T, std::size_t N, typename... Args>
struct get_index_of_element_from_tuple_by_type_impl {
    static constexpr auto value = N;
};

/* Args...展开后的类型和目标类型T相等的情况，会调用该特化版本 */
template <typename T, std::size_t N, typename... Args>
struct get_index_of_element_from_tuple_by_type_impl<T, N, T, Args...> {
    static constexpr auto value = N;
};

/* Args...展开后的类型和目标类型T不相等的情况，会调用该特化版本 */
template <typename T, std::size_t N, typename U, typename... Args>
struct get_index_of_element_from_tuple_by_type_impl<T, N, U, Args...> {
    /* 这里主要依赖于Args...的展开操作
     * 由于第N个类型和T不同，所以下次需要判断第N + 1个类型
     * 所以继续将Args...展开
     * 如果展开的类型仍然和T不同，那么会再次进入这个特化版本，否则会进入上面那个特化版本 */
    static constexpr auto value = get_index_of_element_from_tuple_by_type_impl<T, N + 1, Args...>::value;
};


/* 调用情况 */
template <typename T, typename... Args>
T& get_element_by_type(std::tuple<Args...>& t) {
    /* 这里依赖于可变模板参数Args...的展开情况，会选择上面不同的几个
     * get_index_of_element_from_tuple_by_type_impl */
    return std::get<get_index_of_element_from_tuple_by_type_impl<T, 0, Args...>::value>(t);
}

class Type
{
    public:
        int a = 1;
};


template <typename... Args>
void make_tuple_and_get_element(std::tuple<Args...>& t) {
    auto& string_value = get_element_by_type<std::string, Args...>(t);
    std::cout << string_value << std::endl;
}
int main()
{
    std::tuple t{ 1, 2.0, std::string{ "123" }, Type{} };
    make_tuple_and_get_element(t);
    return 0;
}
