#include "SyncLogging.hh"
#include "LogFile.hh"
#include <assert.h>
#include <stdio.h>

SyncLogging::SyncLogging(const std::string& logPath, off_t rollSize, bool threadSafe)
	: m_logPath(logPath),
	  m_rollSize(rollSize),
	  m_mutex(),
	  m_logFile(m_logPath, m_rollSize, false),
	  m_buffer(new Buffer())
{
}

SyncLogging::~SyncLogging(){
	flush();
}

void SyncLogging::flush(){
	//m_logFile.append(m_buffer->data(), m_buffer->length());
	//m_logFile.flush();
}

void SyncLogging::append(const char* logline, int len){
	MutexLockGuard lock(m_mutex);
	/*if(m_buffer->avail() > len){
		m_buffer->append(logline, len);
	}
	else{
		m_logFile.append(m_buffer->data(), m_buffer->length());
		
		m_buffer->reset();

		if(m_buffer->avail() > len){
			m_buffer->append(logline, len);
		}
	}*/
	m_logFile.append(logline, len);	
	m_logFile.flush();
}
