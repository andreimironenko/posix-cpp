// C++ STL headers
#include <csignal>
#include <chrono>
#include <ratio>
#include <memory>
#include <functional>

#pragma once
namespace posixcpp {
  class timer {
    class timer_;
    std::unique_ptr<timer_> _timer;

    public:
    using callback_t = std::function<void(void*)>;

    explicit timer(std::chrono::seconds period_sec,
        std::chrono::nanoseconds period_nsec = static_cast<std::chrono::seconds>(0),
        callback_t callback = nullptr, void* data = nullptr,
        bool is_single_shot = false, int sig = SIGRTMAX
        );

    ~timer();

    timer(const timer&) = delete;
    timer(timer&&) = delete;
    timer& operator=(const timer&) = delete;
    timer& operator=(timer&&) = delete;

    void start();
    void reset();
    void suspend();
    void resume();
    void stop();
  };
} // namespace posixcpp
