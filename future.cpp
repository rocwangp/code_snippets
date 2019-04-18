#include <memory>
#include <iostream>
#include <functional>
#include <vector>
#include <iterator>

template <typename T>
class Try {
public:
    Try() {}
    explicit Try(T&& t) : result_(std::forward<T>(t)) {}

    Try(Try&& t) noexcept : result_(std::move(t.result_)) {}

    Try& operator=(Try&& t) noexcept { result_ = std::move(t.result_); return *this; }

    T&& result() noexcept  { return std::move(result_); }
private:
    T result_;
};

template <typename T>
Try<T> make_try(T&& t) {
    return Try<T>(std::forward<T>(t));
}

template <typename T>
class Core {
public:
    using Result = Try<T>;
    using Callback = std::function<void(Result&&)>;

    enum class State {
        START,
        READY,
        RESULT,
        CALLBACK,
        DONE
    };

    Core() : state_(State::START), callback_(nullptr) {}

    explicit Core(Try<T>&& t) : state_(State::RESULT), result_(std::forward<Try<T>>(t)), callback_(nullptr) {}

    Core(Core&& core) noexcept : state_(core.state_), result_(std::move(core.result_)), callback_(std::move(core.callback_)) {}

    Result&& result() noexcept { return std::move(result_); }

    void set_callback(Callback callback) noexcept {
        callback_ = std::move(callback);

        if (state_ == State::RESULT || state_ == State::CALLBACK) {
            state_ = State::DONE;
            callback_(std::move(result_));
        }
    }

    void set_try(Try<T>&& t) {
        result_ = std::move(t);
        // std::cout << "in set try: " << t.result() <<std::endl;
        if (callback_ != nullptr) {
            state_ = State::DONE;
            callback_(std::move(result_));
        }
        else {
            state_ = State::CALLBACK;
        }
    }
private:
    State state_;
    Result result_;
    Callback callback_;
};

class __FutureUnit {

};

template <typename T>
class Future {
public:
    Future() : core_(new Core<T>()) {}
    explicit Future(Core<T>&& core) : core_(new Core<T>(std::move(core))) {}

    Future(Future&& f) noexcept : core_(std::move(f.core_)) {}

    template <typename Func>
    auto then(Func&& func) {
        if constexpr (!std::is_same_v<T, __FutureUnit>)  {
            using result_type = std::invoke_result_t<Func, T>;
            Future<result_type> future;
            get_core()->set_callback([core = future.get_core(), f = std::forward<Func>(func)](Try<T>&& t) mutable {
                core->set_try(make_try(f(std::move(t).result())));
            });
            return future;
        }
        else {
            using result_type = std::invoke_result_t<Func>;
            Future<result_type> future;
            get_core()->set_callback([core = future.get_core(), f = std::forward<Func>(func)](Try<T>&& t) mutable {
                core->set_try(make_try(f()));
            });
            return future;
        }
    }

    T&& get() noexcept {
        return (core_->result().result());
    }

    std::shared_ptr<Core<T>> get_core() const { return core_; }
private:
    std::shared_ptr<Core<T>> core_;
};

template <typename T>
static Future<T> make_future(T&& t) {
    return Future<T>(Core<T>(make_try(std::forward<T>(t))));
}

static Future<__FutureUnit> make_future() {
    return make_future(__FutureUnit{});
}

int main() {
    auto future1 = make_future();
    auto future2 = future1.then([]() {  
        std::cout << "in callback1\n"; 
        return 10;  
    }).then([](int&& n) { 
        std::cout << "in callback2 with n: " << n << "\n"; 
        return std::string("hello world");
    }).then([](std::string&& s) {
        std::cout << s << std::endl;
        return std::vector<int> { 1, 2, 3, 4, 5 };
    }).then([](std::vector<int>&& nums) {
        std::copy(nums.begin(), nums.end(), std::ostream_iterator<int>(std::cout, "\n"));
        return 100;
    });
    auto result = future2.get();
    std::cout << "result: " << result << "\n";
    return 0;
}

