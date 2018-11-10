#include "EventLoop.hh"
#include "Poller.hh"
#include "Logger.hh"
#include <assert.h>
#include <poll.h>

__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

EventLoop::EventLoop()
	:m_looping(false),
  m_threadId(CurrentThread::tid()),
  m_poller(new Poller(this))
{
  LOG_TRACE << "EventLoop Create " << this << " in thread " << m_threadId;
  if(t_loopInThisThread)
  {  //每个线程只有一个EventLoop对象 , 如果当前线程创建了其他 EventLoop对象,则终止程序.
    LOG_FATAL << "Anthor EventLoop " << t_loopInThisThread
              << " exists in this thread " << m_threadId;
  }
  else
  {
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop()
{
  assert(!m_looping);
  t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
  assert(!m_looping);
  assertInLoopThread();
  m_looping = true;
  m_quit = false;

  LOG_TRACE << "EventLoop " << this << " start loopig";

  while(!m_quit)
  {
    m_activeChannels.clear();
    m_poller->poll(1000, &m_activeChannels);
    for(ChannelList::iterator it = m_activeChannels.begin();
      it != m_activeChannels.end(); ++it)
    {
      (*it)->handleEvent();
    }

  }

  LOG_TRACE << "EventLoop " << this << " stop loopig";
  m_looping = false;

}

void EventLoop::abortNotInLoopThread()
{
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << m_threadId
            << ", current thread id = " <<  CurrentThread::tid();
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

void EventLoop::quit()
{
  m_quit = true;
  //wakeup();
}

void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  m_poller->updateChannel(channel);
}

