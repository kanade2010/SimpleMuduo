#ifndef _THREAD_HH
#define _THREAD_HH

#include <thread>
#include <functional>
#include <memory>
#include <assert.h>

class Thread{
public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(const ThreadFunc& threadRoutine);
  ~Thread();

  void start();
  void join();
  void detach();

  bool isStarted() { return m_isStarted; }
  bool isJoined() { return m_isJoined; }

  std::thread::id getThreadId() const{ assert(m_isStarted); return p_thread->get_id(); }

private:
  Thread(const Thread&);
  const Thread& operator=(const Thread&);

  bool m_isStarted;
  bool m_isJoined;
  ThreadFunc m_threadRoutine;
  std::unique_ptr<std::thread> p_thread;
};


#endif