#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <functional>
#include "Thread.hh"

#define CHECK(exp) \
	if(!exp) \
{ \
	fprintf(stderr, "Error/(%s, %d):[" #exp "] check error, abort.\n", __FILE__, __LINE__); abort();\
}

Thread::Thread(const ThreadFunc& threadRoutine)
	:m_isRuning(false),
	 m_threadId(0),
	 m_threadRoutine(threadRoutine){
}

Thread::~Thread(){
}

void *Thread::threadGuide(void *arg){
	Thread *p = static_cast<Thread *>(arg);
	p->m_threadRoutine();
	return NULL;
}

void Thread::join(){
	assert(m_isRuning);
	CHECK(!pthread_join(m_threadId, NULL));
	m_isRuning = false;
}

void Thread::start(){

	pthread_attr_t attr;

	CHECK(!pthread_attr_init(&attr));

	/*
	CHECK(!pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));    //set thread separation state property

	CHECK(!pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED));    //Set thread inheritance

	CHECK(!pthread_attr_setschedpolicy(&attr, SCHED_OTHER));                //set thread scheduling policy

	CHECK(!pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM));             //Set thread scope
    */
	CHECK(!pthread_create(&m_threadId, &attr, threadGuide, this));

	m_isRuning = true;

}




