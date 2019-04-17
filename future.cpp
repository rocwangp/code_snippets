#include <memory>
#include <iostream>
#include <functional>

template <typename T>
class Try {
public:
    Try() {}
    explicit Try(T&& t) : result_(std::forward<T>(t)) {}

    Try(Try&& t) noexcept : result_(std::move(t.result_)) {}

    Try& operator=(Try&& t) noexcept { result_ = std::move(t.result_); }

    T&& result() && noexcept  { return std::move(result_); }
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

    const Result& result() const { return result_; }

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

class FutureUnit {

};

template <typename T>
class Future {
public:
    Future() : core_(new Core<T>()) {}
    explicit Future(Core<T>&& core) : core_(new Core<T>(std::move(core))) {}

    Future(Future&& f) noexcept : core_(std::move(f.core_)) {}

    template <typename Func>
    Future<std::invoke_result_t<Func, T>> then(Func&& func) {
        // std::cout << typeid(decltype(func)).name() << std::endl;
        using result_type = std::invoke_result_t<Func, T>;
        // using result_type  = int;
        Future<result_type> future;
        get_core()->set_callback([core = future.get_core(), f = std::forward<Func>(func)](Try<T>&& t) mutable {
            core->set_try(make_try(f(std::move(t).result())));
        });
        return future;
    }

    const T& get() & const {
        return (core_->result().result());
    }

    T&& get() && {
        return std::move(core_->result().result());
    }

    std::shared_ptr<Core<T>> get_core() const { return core_; }
private:
    std::shared_ptr<Core<T>> core_;
};

template <typename T>
static Future<T> make_future(T&& t) {
    return Future<T>(Core<T>(make_try(std::forward<T>(t))));
}

static Future<FutureUnit> make_future() {
    return make_future(FutureUnit{});
}

int main() {
    auto future1 = make_future();
    auto future2 = future1.then([](FutureUnit unit) -> int {  
            std::cout << "in callback1\n"; 
            return 10;  
    }).then([](int n) -> std::string { 
        std::cout << "in callback2 with n: " << n << "\n"; 
        return "hello world";
    }).then([](std::string s) {
        std::cout << s << std::endl;
        return 100;
    });
    auto result = future2.get();
    std::cout << "result: " << result << "\n";
    return 0;
}

