#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <tuple>

template <typename T, typename... Args>
struct contains : public std::false_type {};

template <typename T, typename U, typename... Args>
struct contains<T, U, Args...> : public std::conditional_t<std::is_same_v<T, U>, std::true_type, contains<T, Args...>> {};



template <typename... FilterArgs>
struct type_filter
{
    constexpr auto operator()() {
        return std::tuple{};
    }

    template <typename T, typename... Args>
    constexpr auto operator()(T&& t, Args&&... args) {
        if constexpr (contains<T, FilterArgs...>::value) {
            return operator()(std::forward<Args>(args)...);
        }
        else {
            return std::tuple_cat(std::tuple(std::forward<T>(t)), operator()(std::forward<Args>(args)...));
        }
    }
};

using rename_map = std::unordered_map<std::string, std::unordered_set<std::string>>;
using query_field_set = std::unordered_set<std::string_view>;

template <typename... Args>
void filter(Args&&... args) {
    auto t = type_filter<rename_map, query_field_set>{}(std::forward<Args>(args)...);
    std::cout << std::get<0>(t) << std::endl;
    std::cout << std::get<1>(t) << std::endl;
    std::cout << std::get<2>(t) << std::endl;
}
int main()
{
    filter("hello", "world", 10, rename_map{}, query_field_set{});
    return 0;
}
