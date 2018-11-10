#ifndef _THREAD_HH
#define _THREAD_HH

#include <pthread.h>
#include <functional>

class Thread{
public:
//typedef void (*ThreadFunc)(void);
	typedef std::function<void ()> ThreadFunc;

	explicit Thread(const ThreadFunc& threadRoutine);
	~Thread();
	void start();
	void join();
	static void *threadGuide(void *arg);
	pthread_t getThreadId() const{
		return m_threadId;
	}
private:
	Thread(const Thread&);
	Thread& operator=(const Thread&);

	pthread_t m_threadId;
	bool m_isRuning;
	ThreadFunc m_threadRoutine;
};

#endif
