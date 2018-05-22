/*
 * 用户可以传入带有以下类型的任意数量的参数
 *      int
 *      double
 *      string
 *      ...有限个且已知都有哪些类型
 * 如何在底层用相同类型的std::function对象保存这些函数对象
 *
 * 方法：对用户传入的函数对象再进行一层包装
 */

#include <vector>
#include <tuple>
#include <memory>
#include <functional>

template <typename R, typename... Args>
struct function_traits {};


template <typename R, typename... Args>
struct function_traits<R(Args...)> {
    static constexpr std::size_t artiy = sizeof...(Args);
    using return_type = R;

    template <std::size_t N>
    using arg = std::tuple_element_t<N, std::tuple<Args...>>;
};

template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
    static constexpr std::size_t artiy = sizeof...(Args);
    using return_type = R;

    template <std::size_t N>
    using arg = std::tuple_element_t<N, std::tuple<Args...>>;
};



/* 假设用户只能传int64_t, uint64_t, double, string四种类型的参数，且类型的顺序无要求，个数无要求 */
struct routing_params
{
    std::vector<int64_t> int_params;
    std::vector<uint64_t> uint_params;
    std::vector<double> double_params;
    std::vector<std::string> string_params;

    template <typename T>
    T get(unsigned) const;
};

template <>
inline int64_t routing_params::get<int64_t>(unsigned index) const {
    return int_params[index];
}
template <>
inline uint64_t routing_params::get<uint64_t>(unsigned index) const {
    return uint_params[index];
}
template <>
inline double routing_params::get<double>(unsigned index) const {
    return double_params[index];
}
template <>
inline std::string routing_params::get<std::string>(unsigned index) const {
    return string_params[index];
}

template <typename H>
struct call_params
{
    H& handler;
    const routing_params& params;
};

template <typename... T>
struct S
{
    template <typename U>
    using push_front = S<U, T...>;
    template <typename U>
    using push_back = S<T..., U>;
};

template <typename Type, int Pos>
struct call_pair
{
    using type = Type;
    static constexpr int pos = Pos;
};

template <typename F, int Nint, int Nuint, int Ndouble, int Nstring, typename S1, typename S2>
struct call { };

template <typename F, int Nint, int Nuint, int Ndouble, int Nstring, typename... Args1, typename... Args2>
struct call<F, Nint, Nuint, Ndouble, Nstring, S<int64_t, Args1...>, S<Args2...>>
{
    void operator()(F cparams) {
        using pushed = typename S<Args2...>::template push_back<call_pair<int64_t, Nint>>;
        call<F, Nint + 1, Nuint, Ndouble, Nstring, S<Args1...>, pushed>()(cparams);
    }
};

template <typename F, int Nint, int Nuint, int Ndouble, int Nstring, typename... Args1, typename... Args2>
struct call<F, Nint, Nuint, Ndouble, Nstring, S<uint64_t, Args1...>, S<Args2...>>
{
    void operator()(F cparams) {
        using pushed = typename S<Args2...>::template push_back<call_pair<uint64_t, Nuint>>;
        call<F, Nint, Nuint + 1, Ndouble, Nstring, S<Args1...>, pushed>()(cparams);
    }
};

template <typename F, int Nint, int Nuint, int Ndouble, int Nstring, typename... Args1, typename... Args2>
struct call<F, Nint, Nuint, Ndouble, Nstring, S<double, Args1...>, S<Args2...>>
{
    void operator()(F cparams) {
        using pushed = typename S<Args2...>::template push_back<call_pair<double, Ndouble>>;
        call<F, Nint, Nuint, Ndouble + 1, Nstring, S<Args1...>, pushed>()(cparams);
    }
};

template <typename F, int Nint, int Nuint, int Ndouble, int Nstring, typename... Args1, typename... Args2>
struct call<F, Nint, Nuint, Ndouble, Nstring, S<std::string, Args1...>, S<Args2...>>
{
    void operator()(F cparams) {
        using pushed = typename S<Args2...>::template push_back<call_pair<std::string, Nstring>>;
        call<F, Nint, Nuint, Ndouble, Nstring + 1, S<Args1...>, pushed>()(cparams);
    }
};

template <typename F, int Nint, int Nuint, int Ndouble, int Nstring, typename... Args1>
struct call<F, Nint, Nuint, Ndouble, Nstring, S<>, S<Args1...>>
{
    void operator()(F cparams) {
        cparams.handler(cparams.params.template get<typename Args1::type>(Args1::pos)...);
    }
};

template <typename Func, typename... ArgsWarpper>
struct function_warpper
{
    template <typename... Args>
    void set_(Func f) {
        handler_ = [f = std::move(f)](Args... args){
            std::forward<Func>(f)(args...);
        };
    }

    void operator()(const routing_params& params) {
        call<call_params<decltype(handler_)>, 0, 0, 0, 0, S<ArgsWarpper...>, S<>>()(call_params<decltype(handler_)>{handler_, params});
    }
    std::function<void(ArgsWarpper...)> handler_;
};

class DynamicRule
{
    public:
        DynamicRule(std::string&& rule)
            : rule_(std::forward<std::string>(rule))
        { }
        template<typename Func>
        void operator()(Func f) {
            /* 首先需要知道用户传入的函数对象的类型是什么，也就是需要解析出
             * 返回值类型，每个参数类型，参数个数 */
            using function_t = function_traits<Func>;
            erased_handler_ = warp(std::move(f), std::make_index_sequence<function_t::artiy>{});
        }

        template <typename Func, std::size_t... Indices>
        std::function<void(routing_params&)> warp(Func f, std::index_sequence<Indices...>) {
            using function_t = function_traits<Func>;
            auto ret = function_warpper<Func, typename function_t::template arg<Indices>...>{};
            ret.template set_<function_t::template arg<Indices>...>(std::move(f));
            return ret;
        }
    private:
        std::string rule_;
        std::function<void(routing_params&)> erased_handler_;
};


class Router
{
    public:
        DynamicRule& new_rule_dynamic(std::string&& rule) {
            auto dynamic_rule = new DynamicRule(std::forward<std::string>(rule));
            all_rules_.emplace_back(dynamic_rule);
            return *dynamic_rule;
        }
    private:
        std::vector<std::unique_ptr<DynamicRule>> all_rules_;
};

class Server
{
    public:
        void routing_dynamic(std::string&& rule) {
            router_.new_rule_dynamic(std::forward<std::string>(rule));
        }
    private:
        Router router_;
};

