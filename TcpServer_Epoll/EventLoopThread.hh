#ifndef _NET_EVENTLOOPTHREAD_HH
#define _NET_EVENTLOOPTHREAD_HH

#include <functional>
#include <mutex>

#include "Condition.hh"
#include "Thread.hh"

class EventLoop;

class EventLoopThread{
public:
  typedef std::function<void(EventLoop* )> ThreadInitCallBack;

  EventLoopThread();
  ~EventLoopThread();

  EventLoop* startLoop();

private:
  const EventLoopThread& operator=(const EventLoopThread&);
  EventLoopThread(const EventLoopThread&);

  void threadFunc();

  EventLoop* p_loop;
  bool m_exiting;
  Thread m_thread;
  std::mutex m_mutex;
  Condition m_cond;
  ThreadInitCallBack m_threadInitCallBack;

};

#endif