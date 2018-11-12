#include "MutexLock.hh"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define CHECK(exp) \
    if(!exp) \
{ \
    fprintf(stderr, "Error/(%s, %d):[" #exp "] check error %d :%s, abort.\n", __FILE__, __LINE__, errno, strerror(errno)); abort();\
}

MutexLock::MutexLock():m_isLocking(false){
	CHECK(!pthread_mutex_init(&m_mutex, NULL));
}

MutexLock::~MutexLock(){
	assert(!isLocking());	//布尔值
	CHECK(!pthread_mutex_destroy(&m_mutex)); //整形值
}

void MutexLock::lock(){
	CHECK(!pthread_mutex_lock(&m_mutex));
	m_isLocking = true;
}

void MutexLock::unlock(){
	m_isLocking = false;  //先改状态再解锁,保证其原子性.
	CHECK(!pthread_mutex_unlock(&m_mutex));
}
