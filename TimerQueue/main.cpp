#include <errno.h>
#include <thread>
#include <strings.h>
#include <poll.h>
#include <functional>
#include "EventLoop.hh"
#include "Channel.hh"
#include "Poller.hh"
#include "Logger.hh"
#include "Timer.hh"
#include "TimeStamp.hh"
#include "TimerQueue.hh"

/*
void test()
{
  LOG_DEBUG << "----------test1-----------";
}

void created_Timer()
{
  int i = 1000000;
  while(i--) Timer timerTest(test, times::addTime(TimeStamp::now(), 1.0), 1.0);
}

int main()
{
  Timer timerTest(test, times::addTime(TimeStamp::now(), 1.0), 1.0);
  timerTest.run();
  timerTest.restart(TimeStamp::now());
  LOG_DEBUG << "numCreated() : " << timerTest.numCreated();

  std::thread t(created_Timer);
  created_Timer();

  t.join();
  LOG_DEBUG << "numCreated() : " << timerTest.numCreated();
}
*/



EventLoop* g_loop;

void print() { LOG_DEBUG << "test print()";  }

void test()
{

  LOG_DEBUG << "[test] : test timerQue";

  //g_loop->runAfter(1.0, print);
  //g_loop->quit();

}


int main()
{
  EventLoop loop;
  g_loop = &loop;

  TimerQueue timerQue(&loop);
  timerQue.addTimer(test, times::addTime(TimeStamp::now(), 3.0));
  timerQue.addTimer(test, times::addTime(TimeStamp::now(), 3.0));
  timerQue.addTimer(test, times::addTime(TimeStamp::now(), 5.0));

  /*loop.runAt(times::addTime(TimeStamp::now(), 7.0), test);
  loop.runAt(times::addTime(TimeStamp::now(), 2.0), test);
  loop.runAt(times::addTime(TimeStamp::now(), 5.0), test);*/

  loop.loop();

  return 0;
}

