#ifndef _SYNCLOGGING_HH
#define _SYNCLOGGING_HH
#include "LogFile.hh"
#include "Logger.hh"
#include "LogStream.hh"
#include "MutexLock.hh"
#include <string>
#include <memory>

class SyncLogging
{
public:
	SyncLogging(const std::string& logPath, off_t rollSize, bool threadSafe = false);
	~SyncLogging();

	void append(const char *logline, int len);
	void flush();
private:
	SyncLogging(const SyncLogging&);
	SyncLogging& operator=(const SyncLogging&);

	typedef LogBuffer<kLargeBuffer> Buffer;
	typedef scoped_ptr<Buffer> BufferPtr;

	std::string m_logPath;
	off_t m_rollSize;
	MutexLock m_mutex;
	LogFile m_logFile;
	BufferPtr m_buffer;
};

#endif
