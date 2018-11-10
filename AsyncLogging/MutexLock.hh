#ifndef _MUTEXLOCK_HH
#define _MUTEXLOCK_HH

#include <pthread.h>

class MutexLock{

public:
	MutexLock();
	~MutexLock();
	void lock();
	void unlock();
	bool isLocking() const{
		return m_isLocking;
	}
	pthread_mutex_t *getMutexPtr(){
		return &m_mutex;
	}

private:
	MutexLock(MutexLock&);	//no copyable
	MutexLock& operator=(const MutexLock&);

	friend class Condition;//使condition类可以调用restoreMutexStatus();

	void restoreMutexStatus(){
		m_isLocking = true;
	}
	
	pthread_mutex_t m_mutex;
	bool m_isLocking;
};


class MutexLockGuard
{
public:
	MutexLockGuard(MutexLock &mutex):m_mutex(mutex){
		m_mutex.lock();//构造时加锁.
	}
	~MutexLockGuard(){//析构时解锁.
		m_mutex.unlock();
	}

private:
	MutexLock &m_mutex;
};

#endif
