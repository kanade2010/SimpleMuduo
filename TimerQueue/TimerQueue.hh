#ifndef _NET_TIMERQUEUE_HH
#define _NET_TIMERQUEUE_HH
#include "TimerId.hh"
#include "CallBacks.hh"
#include "TimeStamp.hh"
#include "Channel.hh"
#include <set>
#include <vector>

class EventLoop;

class TimerQueue
{
public:
  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  // Schedules the callback to be run at given time,

  TimerId addTimer(const NetCallBacks::TimerCallBack& cb, TimeStamp when, double interval = 0.0);

  void cancel(TimerId timerId);

private:
  typedef std::pair<TimeStamp, Timer*> Entry;
  typedef std::set<Entry> TimerList;
  typedef std::pair<Timer*, int64_t> ActiveTimer;
  typedef std::set<ActiveTimer> ActiveTimerSet;

  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  //called when timerfd alarms
  void handleRead();
  //move out all expired timers and return they.
  std::vector<Entry> getExpired(TimeStamp now);
  bool insert(Timer* timer);
  void reset(const std::vector<Entry>& expired, TimeStamp now);

  EventLoop* p_loop;
  const int m_timerfd;
  Channel m_timerfdChannel;

  //Timer List sorted by expiration
  TimerList m_timers;
  ActiveTimerSet m_activeTimers;

  bool m_callingExpiredTimers; /*atomic*/
  ActiveTimerSet m_cancelingTimers;

};

#endif
