#include <tuple>
#include <iostream>

namespace detail {
    template <typename...>
    struct is_tuple : public std::false_type {};

    template <typename ... Args>
    struct is_tuple<std::tuple<Args...>> : public std::true_type {};

    template <typename ... Args>
    constexpr static bool is_tuple_v = is_tuple<Args...>::value;

    template <typename Tuple, typename Func, size_t... Idx>
    constexpr auto foreach_tuple(Tuple&& t, Func&& f, std::index_sequence<Idx...>) {
        return (std::forward<Func>(f)(std::get<Idx>(std::forward<Tuple>(t)), Idx), ...);
    }

    template <size_t N, typename T, typename ... Args>
    constexpr auto one_line_impl(const T& value, const std::tuple<Args...>& t) {
        if constexpr (N + 1 == sizeof...(Args)) {
            return std::make_tuple(value * std::get<N>(t));
        }
        else {
            static_assert(std::is_integral_v<std::decay_t<T>> && std::is_integral_v<std::decay_t<std::tuple_element_t<N, std::tuple<Args...>>>>);
            return std::tuple_cat(std::make_tuple(value * std::get<N>(t)), one_line_impl<N + 1>(value, t));
        }
    }

    template <typename T, typename ... Args>
    constexpr auto one_line(const T& left, const std::tuple<Args...>& t) {
        static_assert(sizeof...(Args) > 0);
        return one_line_impl<0>(left, t);
    }

    template <size_t N, typename ... Args1, typename ... Args2>
    constexpr auto cartesian_impl(const std::tuple<Args1...>& t1, const std::tuple<Args2...>& t2) {
        if constexpr ((N >= sizeof...(Args1))) {
            return std::tuple<>{};
        }
        else {
            return std::tuple_cat(std::make_tuple(one_line(std::get<N>(t1), t2)), cartesian_impl<N + 1>(t1, t2));
        }
    }

    template <typename ... Args1, typename ... Args2>
    constexpr auto cartesian(const std::tuple<Args1...>& t1, const std::tuple<Args2...>& t2) {
        return detail::cartesian_impl<0>(t1, t2);
    }
}

template <typename Tuple, typename Func>
constexpr auto foreach(Tuple&& t, Func&& f) {
    using raw_tuple_type = std::decay_t<Tuple>;
    if constexpr (detail::is_tuple_v<raw_tuple_type>) {
        return detail::foreach_tuple(std::forward<Tuple>(t), std::forward<Func>(f), std::make_index_sequence<std::tuple_size_v<raw_tuple_type>>{});
    }
    else {
        throw std::invalid_argument("only tuple");
    }
}

template <typename ... Args1, typename ... Args2>
constexpr auto operator*(const std::tuple<Args1...>& t1, const std::tuple<Args2...>& t2) {
    return detail::cartesian(t1, t2);
}

int main() {
    const auto t1 = std::make_tuple(1, 2, 3);
    const auto t2 = std::make_tuple(1, 2, 3);
    const auto t = t1 * t2;
    foreach(t, [](const auto& element, size_t idx) {
        using tuple_type = std::decay_t<decltype(element)>;
        std::cout << idx << std::endl;
        foreach(element, [](const auto& sub, size_t index) { std::cout << sub << " "; });
        std::cout << std::endl;
    });
    // output:
    // 0
    // 1 2 3
    // 1
    // 2 4 6
    // 2
    // 3 6 9
    return 0;
}