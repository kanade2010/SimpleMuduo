#include <assert.h>

#include "Logger.hh"
#include "EventLoop.hh"
#include "EventLoopThread.hh"

EventLoopThread::EventLoopThread()
  :p_loop(NULL),
  m_exiting(false),
  m_thread(std::bind(&EventLoopThread::threadFunc, this)),
  m_mutex(),
  m_cond()
{

}

EventLoopThread::~EventLoopThread()
{
  LOG_TRACE << "EventLoopThread::~EventLoopThread()";

  m_exiting = true;
  if(p_loop != NULL)
  {  
    p_loop->quit();
    m_thread.join();
  }
}


EventLoop* EventLoopThread::startLoop()
{
  assert(!m_thread.isStarted());
  m_thread.start();

  {
    std::unique_lock<std::mutex> lock(m_mutex);
    while(p_loop == NULL)
    {
      LOG_TRACE << "EventLoopThread::startLoop() wait()";
      m_cond.wait(lock);
    }
  }

  LOG_TRACE << "EventLoopThread::startLoop() wakeup";

  return p_loop;
}


void EventLoopThread::threadFunc()
{
  EventLoop loop;

  if(m_threadInitCallBack)
  {
    m_threadInitCallBack(&loop);
  }

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    p_loop = &loop;
    m_cond.notify();
    LOG_TRACE << "EventLoopThread::threadFunc() notify()";
  }

  loop.loop();

  p_loop = NULL;

}