#include "AsyncLogging.hh"
#include "LogFile.hh"
#include <assert.h>
#include <stdio.h>

AsyncLogging::AsyncLogging(const std::string filePath, off_t rollSize, double flushInterval)
	:m_filePath(filePath),
	 m_rollSize(rollSize),
	 m_flushInterval(flushInterval),
	 m_isRunning(false),
	 m_thread(std::bind(&AsyncLogging::threadRoutine, this)),
	 m_mutex(),
	 m_cond(),
	 m_currentBuffer(new Buffer),
	 m_nextBuffer(new Buffer),
	 m_buffers()
{
}

AsyncLogging::~AsyncLogging(){
	if(m_isRunning) stop();
}

void AsyncLogging::append(const char* logline, int len){
	std::lock_guard<std::mutex> lock(m_mutex);
	if(m_currentBuffer->avail() > len){
		m_currentBuffer->append(logline, len);
	}
	else{
		m_buffers.push_back(m_currentBuffer.release());

		if(m_nextBuffer){
			m_currentBuffer = std::move(m_nextBuffer);
		}
		else{
			m_currentBuffer.reset(new Buffer);
		}

		m_currentBuffer->append(logline, len);
		m_cond.notify();
	}
}

void AsyncLogging::threadRoutine(){
	assert(m_isRunning == true);
	LogFile output(m_filePath, m_rollSize, false);
	BufferPtr backupBuffer1(new Buffer);
	BufferPtr backupBuffer2(new Buffer);
	BufferVector buffersToWrite;
	buffersToWrite.reserve(8);

	while(m_isRunning){
		assert(buffersToWrite.empty());
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if(m_buffers.empty()){
				m_cond.waitForSeconds(lock, m_flushInterval);
			}
			m_buffers.push_back(m_currentBuffer.release());
			m_currentBuffer = std::move(backupBuffer1);
			m_buffers.swap(buffersToWrite);
			if(!m_nextBuffer)
				m_nextBuffer = std::move(backupBuffer2);
		}

		assert(!buffersToWrite.empty());

		for(size_t i = 0; i < buffersToWrite.size(); ++i){
			output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
		}

		if(buffersToWrite.size() > 2)
		{
		    // drop non-bzero-ed buffers, avoid trashing
			buffersToWrite.resize(2);
		}

		if(!backupBuffer1)
		{
			assert(!buffersToWrite.empty());
			backupBuffer1 = std::move(buffersToWrite.pop_back());
			backupBuffer1->reset();
		}

		if(!backupBuffer2)
		{
			assert(!buffersToWrite.empty());
			backupBuffer2 = std::move(buffersToWrite.pop_back());
			backupBuffer2->reset();
		}

		buffersToWrite.clear();
		output.flush();
	}

	output.flush();
}
