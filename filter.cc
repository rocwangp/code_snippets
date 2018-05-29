#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <list>

template <typename T, typename... Args>
struct contains : public std::false_type {};

template <typename T, typename U, typename... Args>
struct contains<T, U, Args...> :
    public std::conditional_t<std::is_same_v<T, U>, std::true_type, contains<T, Args...>> {};

template <typename... FilterArgs>
struct type_filter
{
    static constexpr auto filter() { return std::tuple{}; }

    template <typename T, typename... Args>
    static constexpr auto filter(T&& t, Args&&... args) {
        using type = std::remove_reference_t<T>;
        if constexpr (contains<type, FilterArgs...>::value) {
            return filter(std::forward<Args>(args)...);
        }
        else {
            return std::tuple_cat(std::make_tuple(std::forward<T>(t)), filter(std::forward<Args>(args)...));
        }
    }
};

template <typename Tuple, typename Func, std::size_t... Idx>
void for_each(Tuple&& t, Func&& f, std::index_sequence<Idx...>) {
    (f(std::get<Idx>(t)), ...);
}

template <typename... Args>
void filter_test(Args&&... args) {
    auto t = type_filter<int, std::list<std::string>, std::vector<int>>::filter(std::forward<Args>(args)...);
    for_each(t, [](auto& item) {
        std::cout << item << std::endl;
    }, std::make_index_sequence<std::tuple_size_v<decltype(t)>>{});
}

int main()
{
    // l被推导为左值引用std::list<std::string>&
	// n被推导为左值引用int&
	// 字面值常量"abc", 20, 2.5被推导为右值引用std::string&&, int&&, double&&
	// vector被推导为右值引用std::vector<int>&&
    std::list<std::string> l{ "abc", "def", "123" };
    int n = 10;
    filter_test(l, "abc", n, 20, 2.5, std::vector<int>{1, 2, 3});
    return 0;
}
