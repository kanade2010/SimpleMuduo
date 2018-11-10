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
	 m_mutex(threadSafe ? new MutexLock : NULL),
	 m_rollCnt(-1),
	 m_flushInterval(flushInterval),
	 m_file(new FileUtil::AppendFile(m_filePath)){
	//assert(filePath.find('/') == std::string::npos);
}

LogFile::~LogFile(){
}

void LogFile::append(const char* logline, int len){
	if(m_mutex.get()){
		MutexLockGuard lock(*m_mutex);
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
	if(m_mutex.get()){
		MutexLockGuard lock(*m_mutex);
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

/*void LogFile::checkLogNum(){
	char name[] = "./";
	char str[32] = {0};
	//把目录路径存放在字符数组内
	strcpy(str, name);
	//打开目录
	DIR *dir = opendir(str);
	assert(NULL != dir);

	int rcnt = 0;
	int pcnt = 0;
	int ccnt = 0;
	char rlog[64] = "~";
	char plog[64] = "~";
	char clog[64] = "~";
	struct dirent *ent = readdir(dir);
    while(NULL != ent)
    {
        if(8 == ent->d_type)
        {
 			if(strncmp("run", ent->d_name, 3) == 0){
 				if(strncmp(ent->d_name, rlog, 19) < 0){
 					strcpy(rlog, ent->d_name);
 				}

 				rcnt++;
 			}
 			if(strncmp("pla", ent->d_name, 3) == 0){
 				if(strncmp(ent->d_name, plog, 24) < 0){
 					strcpy(plog, ent->d_name);
 				}

 				pcnt++;
 			}
 			if(strncmp("com", ent->d_name, 3) == 0){
 				if(strncmp(ent->d_name, clog, 19) < 0){
 					strcpy(clog, ent->d_name);
 				}

 				ccnt++;
 			}
        }
        ent = readdir(dir);
    }
    closedir(dir);

	if(rcnt > 4){
		char filename[64] = "\0";
		strcat(filename, name);
		strcat(filename, rlog);
		unlink(filename);
	}

	if(ccnt > 4){
		char filename[64] = "\0";
		strcat(filename, name);
		strcat(filename, clog);
		unlink(filename);
	}

	if(pcnt > 4){
		char filename[64] = "\0";
		strcat(filename, name);
		strcat(filename, plog);
		unlink(filename);
	}
}*/







