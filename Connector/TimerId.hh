#ifndef _TIMER_ID_HH
#define _TIMER_ID_HH
#include <cstdio>

class Timer;

///
/// An opaque identifier, for canceling Timer.
///
class TimerId
{
 public:
  TimerId()
    : m_timer(NULL),
      m_sequence(0)
  {
  }

  TimerId(Timer* timer, int64_t seq)
    : m_timer(timer),
      m_sequence(seq)
  {
  }

  // default copy-ctor, dtor and assignment are okay

  friend class TimerQueue;

 private:
  //TimerId& operator=(const TimerId&);
  //TimerId(const TimerId&);

  Timer* m_timer;
  int64_t m_sequence;
};

#endif