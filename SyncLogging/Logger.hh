#ifndef _LOGGER_HH
#define _LOGGER_HH

#include <string.h>
#include "LogStream.hh"
#include "TimeStamp.hh"

// CAUTION: do not write:
//
// if (good)
//   LOG_INFO << "Good news";
// else
//   LOG_WARN << "Bad news";
//
// this expends to
//
// if (good)
//   if (logging_INFO)
//     logInfoStream << "Good news";
//   else
//     logWarnStream << "Bad news";
//
#define LOG_TRACE if (Logger::logLevel() <= Logger::TRACE) \
	Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (Logger::logLevel() <= Logger::DEBUG) \
	Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
	Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
#define LOG_SYSERR Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL Logger(__FILE__, __LINE__, true).stream()

class Logger
{
public:
	enum LogLevel
	{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,
	};

//compile time calculation of basename of source file
	class SourceFile
	{
	public:
		template<int N>
		inline SourceFile(const char (&arr)[N])
			:m_data(arr),
			 m_size(N-1){
			const char* slash = strrchr(m_data, '/'); // builtin function
			if (slash){
				m_data = slash + 1;
				m_size -= static_cast<int>(m_data - arr);
			}
		}

		explicit SourceFile(const char* filename)
			: m_data(filename){
			const char* slash = strrchr(filename, '/');
			if (slash){
				m_data = slash + 1;
			}
			m_size = static_cast<int>(strlen(m_data));
		}

		const char* m_data;
		int m_size;
	};

	Logger(SourceFile file, int line);
	Logger(SourceFile file, int line, LogLevel level);
	Logger(SourceFile file, int line, LogLevel level, const char* func);
	Logger(SourceFile file, int line, bool toAbort);
	~Logger();

	static void setLogLevel(LogLevel level);
	static LogLevel logLevel();

	LogStream& stream() { return m_impl.m_stream; }

	typedef void (*outputFunc)(const char *msg, int len);
	typedef void (*flushFunc)();

	static void setOutput(outputFunc);
	static void setFlush(flushFunc);

private:
	Logger(const Logger &lg);			//no copyable
	Logger& operator=(const Logger &lg);

	class Impl
	{
	public:
		typedef Logger::LogLevel LogLevel;
		Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
		void formatTime();
		void finish();

		TimeStamp m_time;
		LogStream m_stream;
		LogLevel m_level;
		int m_line;
		SourceFile m_fileBaseName;
	};

	Impl m_impl;

};

const char* strerror_tl(int savedErrno);

#endif