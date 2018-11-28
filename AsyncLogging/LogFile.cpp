#include "LogFile.hh"
#include "FileUtil.hh"
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <assert.h>

LogFile::LogFile(const std::string& filePath, off_t rollSize, bool threadSafe, int flushInterval)
	:m_filePath(filePath),
	 m_roolSize(rollSize),
	 m_mutex(threadSafe ? new std::mutex : NULL),
	 m_rollCnt(-1),
	 m_flushInterval(flushInterval),
	 m_file(new FileUtil::AppendFile(m_filePath)){
	//assert(filePath.find('/') == std::string::npos);
}

LogFile::~LogFile(){
}

void LogFile::append(const char* logline, int len){
	if(m_mutex){
		std::lock_guard<std::mutex> lock(*m_mutex);
		append_unlocked(logline, len);
	}
	else{
		append_unlocked(logline, len);
	}
}

void LogFile::append_unlocked(const char* logline, int len){
	m_file->append(logline, len);

	if(m_file->writtenBytes() > m_roolSize){
		rollFile();
	}
}

void LogFile::flush(){
	if(m_mutex){
		std::lock_guard<std::mutex> lock(*m_mutex);
		m_file->flush();
	}
	else{
		m_file->flush();
	}
}

bool LogFile::rollFile(){
	//std::string fileNameNew = m_filePath;
	//fileNameNew = getlogFileName(m_filePath);

	if(m_file->writtenBytes() < m_roolSize)
		m_file.reset(new FileUtil::AppendFile(m_filePath));
	else
	{
		assert(remove(m_filePath.c_str()) == 0);
		m_file.reset(new FileUtil::AppendFile(m_filePath));
	}
	//checkLogNum();

	return true;
}

std::string LogFile::getlogFileName(const std::string& baseName){
	std::string fileName;
	fileName.reserve(baseName.size() + 32);
	fileName = baseName.substr(0, baseName.rfind("."));

	char timebuf[24];
	struct tm tm;
	time_t now = time(NULL);
	gmtime_r(&now, &tm);
	strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S", &tm);
	fileName += timebuf;
	fileName += ".log";

	return fileName;
}







