//Condition.cpp

#include "Condition.hh"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define CHECK(exp) \
	if(!exp) \
{ \
	fprintf(stderr, "Error/(%s, %d):[" #exp "] check error, abort.\n", __FILE__, __LINE__); abort();\
}

Condition::Condition(MutexLock &mutex):m_mutex(mutex){
	CHECK(!pthread_cond_init(&m_cond, NULL));
}

Condition::~Condition(){
	CHECK(!pthread_cond_destroy(&m_cond));
}

void Condition::wait(){
	//VERIFY(m_mutex.isLocking());
	CHECK(!pthread_cond_wait(&m_cond, m_mutex.getMutexPtr()));
	//m_mutex.restoreMutexStatus();
}

// returns true if time out, false otherwise.
bool Condition::waitForSeconds(double seconds){
	struct timespec abstime;
	// FIXME: use CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW to prevent time rewind.
	clock_gettime(CLOCK_REALTIME, &abstime);

	const int64_t kNanoSecondsPerSecond = 1000000000;
	int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

	abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
	abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

	return ETIMEDOUT == pthread_cond_timedwait(&m_cond, m_mutex.getMutexPtr(), &abstime);
}

void Condition::notify(){
	CHECK(!pthread_cond_signal(&m_cond));
}

void Condition::notifyAll(){
	CHECK(!pthread_cond_broadcast(&m_cond));
}











