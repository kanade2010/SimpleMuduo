#ifndef _NET_EVENTLOOPTHREAD_HH
#define _NET_EVENTLOOPTHREAD_HH

#include <functional>
#include <string>

#include "MutexLock.hh"
#include "Condition.hh"
#include "Thread.hh"

class EventLoop;

class EventLoopThread
{
public:
typedef std::function<void(EventLoop* )> ThreadInitCallBack;
  EventLoopThread(const ThreadInitCallBack& cb = ThreadInitCallBack(), const std:string& name = std::string());
  ~EventLoopThread();
  EventLoop* startLoop();

private:
  threadFunc();

  EventLoop* p_loop;
  bool m_exiting;
  Thread m_thread;
  MutexLock m_mutex;
  Condition m_cond;
  ThreadInitCallBack m_callBack;

};


#endif