#include "timer.h"
#include "timer_.h"

namespace posixcpp 
{
  timer::timer(std::chrono::duration<long, std::nano> period_nsec,
      callback_t callback, void* data, bool is_single_shot, int sig) :
    _timer(new timer_(period_nsec, callback, data, is_single_shot, sig))
  {
  }

  timer::~timer()
  {
    syslog(LOG_INFO, "timer::~timer()");
  }

  void timer::start()
  {
    _timer->start();
  }

  void timer::reset()
  {
    _timer->reset();
  }

  void timer::suspend()
  {
    _timer->suspend();
  }

  void timer::resume()
  {
    _timer->resume();
  }

  void timer::stop()
  {
    _timer->stop();
  }
} //namespace posixcpp
