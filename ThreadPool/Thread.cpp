#include "Thread.hh"

Thread::Thread(const ThreadFunc& threadRoutine)
  :m_isStarted(false),
  m_isJoined(false),
  m_threadRoutine(threadRoutine)
{

}

Thread::~Thread()
{
  if(m_isStarted && !m_isJoined)
  {
    p_thread->detach();
  }
}

void Thread::join()
{
  assert(m_isStarted);
  assert(!m_isJoined);
  m_isJoined = true;
  p_thread->join();
}

void Thread::detach()
{
  assert(m_isStarted);
  assert(!m_isJoined);
  p_thread->detach();
}

void Thread::start()
{
  assert(!m_isStarted);
  assert(!p_thread);
  p_thread.reset(new std::thread(m_threadRoutine));
  m_isStarted = true;
}
