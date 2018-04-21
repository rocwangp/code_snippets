/*
 * 如果一个类重载了operator()函数
 * 那么它可以默认转换成std::function<>类型变量
 */

#include <functional>
#include <iostream>

template<typename... Args>
class FunctionWarpper
{
    public:
        void set_value(int n) {
            value_ = n;
        }
        void operator()(Args...) {
            std::cout << sizeof...(Args) << std::endl;
        }

    private:
        int value_;
};

int main()
{
    /* 此时fw的类型是FunctionWarpper<int, int> */
    auto fw = FunctionWarpper<int, int>{};
    fw.set_value(10);
    /* 自动转换 */
    std::function<void(int, int)> func = fw;
    func(1, 2);
    return 0;
}
