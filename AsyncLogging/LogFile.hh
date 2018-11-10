#ifndef _LOG_FILE_HH
#define _LOG_FILE_HH
#include <string>
#include "scoped_ptr.hh"
#include "MutexLock.hh"

namespace FileUtil{
	class AppendFile;
}

class LogFile
{
public:
	LogFile(const std::string& filePath, off_t rollSize = 2048*1000, bool threadSafe = true, int flushInterval = 0);
	~LogFile();

	void append(const char* logline, int len);
	void flush();

	std::string getlogFileName(const std::string& baseName);

private:
	void append_unlocked(const char* logline, int len);

	//static std::string getlogFileName(const std::string& filePath);
	bool rollFile();

	const std::string m_filePath;
	const int m_flushInterval;
	
	int m_rollCnt;
	off_t m_roolSize;
	scoped_ptr<MutexLock> m_mutex;
	scoped_ptr<FileUtil::AppendFile> m_file;
};

#endif