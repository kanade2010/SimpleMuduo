#include "FileUtil.hh"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static off_t fileSize(const std::string& path);

FileUtil::AppendFile::AppendFile(StringArg filePath)
	:m_fp(::fopen(filePath.c_str(), "ae")), // 'e' for O_CLOEXEC
	 m_writtenBytes(fileSize(filePath.c_str()))
{
	assert(m_fp);
	::setbuffer(m_fp, m_buffer, sizeof(m_buffer));
}

FileUtil::AppendFile::~AppendFile(){
	::fclose(m_fp);
}

void FileUtil::AppendFile::append(const char* logline, const size_t len){
	size_t nread = write(logline, len);
	size_t remain = len - nread;
	while(remain > 0){
		size_t n = write(logline + n, remain);
		if(0 == n){
			int err = ferror(m_fp);
			if(err){
				fprintf(stderr, "AppendFile::append failed : %s\n", strerror(err));
			}
			break;
		}
		nread += n;
		remain = len - nread;
	}

	m_writtenBytes += len;
}

size_t FileUtil::AppendFile::write(const char* logline, const size_t len){
	return ::fwrite_unlocked(logline, 1, len, m_fp);
}

void FileUtil::AppendFile::flush(){
	::fflush(m_fp);
}

static off_t fileSize(const std::string& path) // get file size
{
     struct stat fileInfo;
     if (stat(path.c_str(), &fileInfo) < 0)
     {
     	 switch(errno)
     	 {
     	 	case ENOENT:
     	 	  return 0;
     	 	default:
         	  fprintf(stderr, "stat fileInfo failed : %s\n", strerror(errno));
         	  abort();
         }
     }
     else
     {   
         return fileInfo.st_size;
     }
}