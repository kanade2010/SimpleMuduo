#ifndef _CONDITION_HH
#define _CONDITION_HH
#include "MutexLock.hh"
#include <pthread.h>

class Condition{
public:
	explicit Condition(MutexLock &mutex);
	~Condition();
	void wait();//pthread_cond_wait
	bool waitForSeconds(double seconds);//pthread_cond_timedwait
	void notify();//pthread_cond_signal
	void notifyAll();//pthread_cond_broadcast
private:
	Condition(const Condition&);
	Condition& operator=(const Condition&);

	MutexLock &m_mutex;
	pthread_cond_t m_cond;
};

#endif

