#include <stdint.h>
#include <assert.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "Logger.hh"
#include "EventLoop.hh"
#include "Timer.hh"
#include "TimerQueue.hh"

namespace TimerFd
{

int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  LOG_TRACE << "createTimerfd() fd : " << timerfd;
  if (timerfd < 0)
  {
    LOG_SYSFATAL << "Failed in timerfd_create";
  }
  return timerfd;
}

struct timespec howMuchTimeFromNow(TimeStamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
                         - TimeStamp::now().microSecondsSinceEpoch();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / TimeStamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % TimeStamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

void readTimerfd(int timerfd, TimeStamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
  if (n != sizeof howmany)
  {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

void resetTimerfd(int timerfd, TimeStamp expiration)
{
  // wake up loop by timerfd_settime()
  LOG_TRACE << "resetTimerfd()";
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret)
  {
    LOG_SYSERR << "timerfd_settime()";
  }
}

};

using namespace TimerFd;

TimerQueue::TimerQueue(EventLoop* loop)
  :p_loop(loop),
   m_timerfd(createTimerfd()),
   m_timerfdChannel(p_loop, m_timerfd),
   m_timers(),
   m_callingExpiredTimers(false)
{
  m_timerfdChannel.setReadCallBack(std::bind(&TimerQueue::handleRead, this));
  m_timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
  m_timerfdChannel.disableAll();
  m_timerfdChannel.remove();
  ::close(m_timerfd);
  for (TimerList::iterator it = m_timers.begin();
      it != m_timers.end(); ++it)
  {
    delete it->second;
  }
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(TimeStamp now)
{
  std::vector<Entry> expired;
  Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>UINTPTR_MAX);
  TimerList::iterator it = m_timers.lower_bound(sentry);
  assert(it == m_timers.end() || now < it->first);
  std::copy(m_timers.begin(), it, back_inserter(expired));
  m_timers.erase(m_timers.begin(), it);

  for(std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    size_t n = m_activeTimers.erase(timer);
    assert(n == 1); (void)n;
  }

  assert(m_timers.size() == m_activeTimers.size());

  return expired;
}


TimerId TimerQueue::addTimer(const NetCallBacks::TimerCallBack& cb, TimeStamp when, double interval)
{
  Timer* timer = new Timer(cb, when, interval);
  p_loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer, timer->sequence());
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  p_loop->assertInLoopThread();
  bool earliestChanged = insert(timer);

  if (earliestChanged)
  {
    resetTimerfd(m_timerfd, timer->expiration());
  }
}

void TimerQueue::cancel(TimerId timerId)
{
  p_loop->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
  p_loop->assertInLoopThread();
  assert(m_timers.size() ==  m_activeTimers.size());
  ActiveTimer timer(timerId.m_timer, timerId.m_sequence);
  ActiveTimerSet::iterator it = m_activeTimers.find(timer);
  if(it != m_activeTimers.end())
  {
    size_t n = m_timers.erase(Entry(it->first->expiration(), it->first));
    assert(n == 1);
    delete it->first;
  }
  else if (m_callingExpiredTimers)
  {
    m_cancelingTimers.insert(timer);
  }
  assert(m_timers.size() == m_activeTimers.size());
}

bool TimerQueue::insert(Timer* timer)
{
  p_loop->assertInLoopThread();
  assert(m_timers.size() == m_activeTimers.size());
  bool earliestChanged = false;
  TimeStamp when = timer->expiration();
  TimerList::iterator it = m_timers.begin();
  if (it == m_timers.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
    std::pair<TimerList::iterator, bool> result
      = m_timers.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
    std::pair<ActiveTimerSet::iterator, bool> result
      = m_activeTimers.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  LOG_TRACE << "TimerQueue::insert() " << "m_timers.size() : "
  << m_timers.size() << " m_activeTimers.size() : " << m_activeTimers.size();

  assert(m_timers.size() == m_activeTimers.size());
  return earliestChanged;
}


void TimerQueue::handleRead()
{
  p_loop->assertInLoopThread();
  TimeStamp now(TimeStamp::now());
  readTimerfd(m_timerfd, now);

  std::vector<Entry> expired = getExpired(now);

  LOG_TRACE << "Expired Timer size " << expired.size() << "  ";

  m_callingExpiredTimers = true;
  m_cancelingTimers.clear();

  for(std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it )
  {
    it->second->run();
  }

  m_callingExpiredTimers = false;

  reset(expired, now);
}


void TimerQueue::reset(const std::vector<Entry>& expired, TimeStamp now)
{
  TimeStamp nextExpire;

  for(std::vector<Entry>::const_iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    if(it->second->repeat()
      && m_cancelingTimers.find(timer) == m_cancelingTimers.end())
    {//如果是周期定时器则重新设定时间插入. 否则delete.
      it->second->restart(now);
      insert(it->second);
    }
    else
    {// FIXME move to a free list no delete please
      delete it->second;
    }
  }

  if (!m_timers.empty())
  {
    nextExpire = m_timers.begin()->second->expiration();
  }

  if (nextExpire.valid())
  {
    resetTimerfd(m_timerfd, nextExpire);
  }
}