// C++ STL headers
#include <csignal>
#include <chrono>
#include <ratio>
#include <memory>
#include <functional>
#include <map>
#include <system_error>

#pragma once
namespace posixcpp {

  class timer {
      
    class timer_;
    std::unique_ptr<timer_> _timer;

    public:
    using callback_t = std::function<void(void*)>;

    enum class error : int
    {
      // critical errors, decrease negative number to add a new error
      posix_timer_creation = -6,
      memcpy_failed = -5,
      posix_timer_gettime = -4, 
      posix_timer_settime = -3,
      signal_handler_registration = -2,
      unknown_error = -1,

      // warnings, increase number to add a new warning
      signal_handler_timer_null_pointer = 1,
      signal_handler_unexpected_signal = 2,
      start_already_started = 3,
      resume_already_running = 4,
      stop_while_not_running = 5,
      suspend_while_not_running = 6
    };

    struct error_category : std::error_category
    {
      const char* name() const noexcept override
      {
        return "posixcpp-timer";
      }

      std::string message(int err) const override
      {
        static std::map<int, std::string> err2str =
        {
          {static_cast<int>(error::posix_timer_creation), "POSIX timer_create has failed"},
          {static_cast<int>(error::memcpy_failed), "std::memcpy has failed"},
          {static_cast<int>(error::posix_timer_gettime), "POSIX timer_gettime has failed"},
          {static_cast<int>(error::posix_timer_settime), "POSIX timer_settime has failed"},
          {static_cast<int>(error::signal_handler_registration), "SYSTEM sigaction has failed"},
          {static_cast<int>(error::unknown_error), "unknown error"},
          {static_cast<int>(error::signal_handler_timer_null_pointer), "signal_handler timer pointer is null"},
          {static_cast<int>(error::signal_handler_unexpected_signal), "signal_handler unexpected signal"},
          {static_cast<int>(error::start_already_started), "attempt to start already running timer"},
          {static_cast<int>(error::resume_already_running), "attempt to resume already running timer"},
          {static_cast<int>(error::stop_while_not_running), "attempt to stop already stopped timer "},
          {static_cast<int>(error::suspend_while_not_running), "attempt to stop already stopped timer "}
        };

        if (!err2str.count(err))
        {
          return std::string("Unknown error");
        }

        return err2str[err];
      }

      static error_category& instance()
      {
        static error_category instance;
        return instance;
      }
    }; // struct error_category

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

    std::error_code try_start() noexcept;
    std::error_code try_reset() noexcept;
    std::error_code try_suspend() noexcept;
    std::error_code try_resume() noexcept;
    std::error_code try_stop() noexcept;
  }; // class timer

  inline std::error_code make_error_code(timer::error err) noexcept
  {
    return std::error_code(static_cast<int>(err), timer::error_category::instance());
  }

  inline bool is_warning(std::error_code ec)
  {
    return (ec && ec.value() > 0);
  }

}// namespace posixcpp

namespace std
{
  template <>
    struct is_error_code_enum<::posixcpp::timer::error> : true_type {};
} // namespace std
