#include <stdexcept>
#include <cstring>

#include <syslog.h>
#include <time.h>

#include "timer_.h"

void timer::timer_::signal_handler(int sig, siginfo_t *si, void *uc)
{
  auto tm = static_cast<timer_*>(si->si_value.sival_ptr);
  if (tm && sig == tm->_signal)
  {
    syslog(LOG_INFO, "timer_::signal_handler with period_nsec = %ld has expired", tm->_period.count());
    syslog(LOG_INFO, "timer_::signal_handler timer->data = 0x%0x", (unsigned long) tm->_data);
#if 0
    if (tm->_is_single_shot)
    {
      tm->stop();
    }
#endif
    tm->_callback(tm->_data);
  }
}

timer::timer_::timer_(std::chrono::duration<long, std::nano> period_nsec,
    callback_t callback, void* data,
    bool is_single_shot, int sig
    ):
  _period(period_nsec),
  _callback(callback),
  _data(data),
  _is_single_shot(is_single_shot),
  _signal(sig),
  _timer(nullptr)
{
  syslog(LOG_INFO, "timer with period_nsec = %ld", period_nsec.count());
  _sa.sa_flags = SA_SIGINFO;
  _sa.sa_sigaction = signal_handler;
  sigemptyset(&_sa.sa_mask);

  if (sigaction(SIGRTMAX, &_sa, NULL) != 0)
  {
    syslog(LOG_ERR, "call of sigaction(SIGRTMAX) is failed");
    return;
  }

  /* Create and start one timer for each command-line argument */
  _sev.sigev_notify = SIGEV_SIGNAL;     /* Notify via signal */
  _sev.sigev_signo = sig;               /* Notify using this signal*/
  _sev.sigev_value.sival_ptr = this;

  _ts.it_value.tv_sec = _period.count() / 1'000'000'000;
  _ts.it_value.tv_nsec = _period.count() % 1'000'000'000;

  syslog(LOG_INFO, "it_value.tv_sec = %ld", _ts.it_value.tv_sec);
  syslog(LOG_INFO, "it_value.tv_nsec = %ld", _ts.it_value.tv_nsec);

  if (!_is_single_shot) {
    _ts.it_interval.tv_sec = _ts.it_value.tv_sec;
    _ts.it_interval.tv_nsec = _ts.it_value.tv_nsec;
    syslog(LOG_INFO, "it_interval.tv_sec = %ld", _ts.it_interval.tv_sec);
    syslog(LOG_INFO, "it_interval.tv_nsec = %ld", _ts.it_interval.tv_nsec);
  }

  /* Allows handler to get ID of this timer */
  if (timer_create(CLOCK_REALTIME, &_sev, &_timer) != 0)
  {
    throw std::runtime_error("Failed to create a timer");
  }

  syslog(LOG_INFO, "timer with period_nsec = %ld has created", period_nsec.count());
}

timer::timer_::~timer_()
{
  syslog(LOG_INFO, "timer_::~timer_()");
  stop();
  timer_delete(_timer);
}

void timer::timer_::start()
{
  struct itimerspec ts;

  syslog(LOG_INFO, "starting timer with period_nsec = %ld", _period.count());

  //get current time from timer
  if (timer_gettime(_timer, &ts) != 0)
  {
    throw std::runtime_error("Failed to gettime timer");
  }

  if (ts.it_value.tv_sec != 0 || ts.it_value.tv_nsec != 0)
  {
    // timer is already started, doing nothing
    return;
  }

  // oethrwise set to the defined value
  if (timer_settime(_timer, 0, &_ts, NULL) != 0)
  {
    throw std::runtime_error("Failed to start timer");
  }

  syslog(LOG_INFO, "timer with period_nsec started = %ld", _period.count());
}

void timer::timer_::reset()
{
  _ts.it_value.tv_sec = _period.count() / 1'000'000'000;
  _ts.it_value.tv_nsec = _period.count() % 1'000'000'000;

  if (!_is_single_shot) {
    _ts.it_interval.tv_sec = _ts.it_value.tv_sec;
    _ts.it_interval.tv_nsec = _ts.it_value.tv_nsec;
  }

  // oethrwise set to the defined value
  if (timer_settime(_timer, 0, &_ts, NULL) != 0)
  {
    throw std::runtime_error("Failed to start timer");
  }
}

void timer::timer_::suspend()
{
  struct itimerspec ts;

  syslog(LOG_INFO, "trying to suspend timer 0x%x0X", (unsigned long)(_timer));

  //get current time from timer
  if (timer_gettime(_timer, &ts) != 0)
  {
    throw std::runtime_error("Failed to gettime timer");
  }

  if (ts.it_value.tv_sec == 0 && ts.it_value.tv_nsec == 0)
  {
    // timer is not started, doing nothing
    return;
  }

  // copy current value
  std::memcpy(&_ts, &ts, sizeof(ts));

  ts.it_value.tv_sec = 0;
  ts.it_value.tv_nsec = 0;

  if (!_is_single_shot) {
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
  }

  //disarm timer
  if (timer_settime(_timer, 0, &ts, NULL) != 0)
  {
    throw std::runtime_error("Failed to settime timer");
  }

  syslog(LOG_INFO, "timer 0x%x0lX is suspended", (unsigned long)(_timer));
}

void timer::timer_::resume()
{
  struct itimerspec ts;

  syslog(LOG_INFO, "trying to resume timer 0x%x0lX", (unsigned long)(_timer));

  //get current time from timer
  if (timer_gettime(_timer, &ts) != 0)
  {
    throw std::runtime_error("Failed to gettime timer");
  }

  syslog(LOG_INFO, "the timer 0x%x0lX ts: %ld, %ld, %ld, %ld", (unsigned long)(_timer),
      ts.it_value.tv_sec, ts.it_value.tv_nsec,
      ts.it_interval.tv_sec, ts.it_interval.tv_nsec
      );

  if (ts.it_value.tv_sec != 0 || ts.it_value.tv_nsec != 0)
  {
    syslog(LOG_INFO, "the timer 0x%x0lX is already running", (unsigned long)(_timer));
    return;
  }

  syslog(LOG_INFO, "the timer 0x%x0lX _ts: %ld, %ld, %ld, %ld", (unsigned long)(_timer),
      _ts.it_value.tv_sec, _ts.it_value.tv_nsec,
      _ts.it_interval.tv_sec, _ts.it_interval.tv_nsec
      );

  if (timer_settime(_timer, 0, &_ts, NULL) != 0)
  {
    throw std::runtime_error("Failed to start timer");
  }

  syslog(LOG_INFO, "timer 0x%x0lX is resumed", (unsigned long)(_timer));
}

void timer::timer_::stop()
{
  syslog(LOG_INFO, "timer::timer_ trying to stop timer 0x%x0lX", (unsigned long)(_timer));
  _ts.it_value.tv_sec = 0;
  _ts.it_value.tv_nsec = 0;

  if (!_is_single_shot) {
    _ts.it_interval.tv_sec = 0;
    _ts.it_interval.tv_nsec = 0;
  }

  if (timer_settime(_timer, 0, &_ts, NULL) != 0)
  {
    throw std::runtime_error("Failed to settime timer");
  }

  syslog(LOG_INFO, "timer::timer_ stopped timer 0x%x0lX", (unsigned long)(_timer));
}
