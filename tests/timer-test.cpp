// Created by amironenko on 19/11/2020.
//
#include <chrono>
#include <ratio>
#include <memory>
#include <functional>

#include <gtest/gtest.h>

#include "timer.h"

using namespace std;
using namespace chrono;
using namespace posixcpp;

class TimerTest: public ::testing::Test {
  protected:

  public:
    int _tick = 0;

    TimerTest()
    {
      // initialization;
    }

    void SetUp( ) override
    {
      // initialization or some code to run before each test
    }

    void TearDown( ) override
    {
      // code to run after each test;
      // can be used instead of a destructor,
      // but exceptions can be handled in this function only
      _tick = 0;
    }

    void increment_tick(void* tick)
    {
      EXPECT_EQ((long)tick, (long)&_tick);
      //_tick ++;
      (*((int*)tick))++;
    }

    ~TimerTest( )  override {
      // resources cleanup, no exceptions allowed
    }
};

TEST_F(TimerTest, CreatingNew)
{
  EXPECT_TRUE(true);
  duration<long> period_s(5s);

  std::unique_ptr<timer> tm (
      new timer(
        period_s,
        std::bind(&TimerTest::increment_tick, this, std::placeholders::_1), // callback
       (void*) &_tick )                                                     // pointer to data
      );
  tm->start();

  int max_ticks = 5;
  for(int i = 0; i < max_ticks; i++)
  {
    sleep(period_s.count());
    EXPECT_EQ(_tick, i+1);
    cout << "tick: " << _tick << endl;
  }

  tm->stop();
  sleep(period_s.count());

  EXPECT_EQ(_tick, max_ticks);
}

TEST_F(TimerTest, SuspendResume)
{
  EXPECT_TRUE(true);
  duration<long> period_s(5s);

  std::unique_ptr<timer> tm (
      new timer(
        period_s,
        std::bind(&TimerTest::increment_tick, this, std::placeholders::_1), // callback
        (void*) &_tick,                                                     // pointer to data
        true
        )
      );

  tm->start();

  sleep(1);

  tm->suspend();

  sleep(1);

  tm->resume();

  sleep(4);

  EXPECT_EQ(_tick, 1);
}


