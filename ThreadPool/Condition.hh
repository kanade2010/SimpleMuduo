#ifndef _CONDITION_HH
#define _CONDITION_HH

#include <mutex>
#include <condition_variable>

class Condition{
public:
  explicit Condition(std::mutex &mutex);
  ~Condition();
  void wait();//pthread_cond_wait
  void waitForSeconds(double seconds);//pthread_cond_timedwait
  void notify();//pthread_cond_signal
  void notifyAll();//pthread_cond_broadcast
private:
  Condition(const Condition&);
  const Condition& operator=(const Condition&);

  std::mutex &m_mutex;
  std::condition_variable m_cond;
};

#endif

