#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "TimeStamp.hh"
#include "Logger.hh"

__thread char t_time[64];
__thread time_t t_lastSecond;
__thread char t_errnobuf[512];

const char* strerror_tl(int savedErrno)
{
  return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}

Logger::LogLevel g_logLevel = Logger::TRACE;

void Logger::setLogLevel(LogLevel level){
	g_logLevel = level;
}

Logger::LogLevel Logger::logLevel(){
	return g_logLevel;
}

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
	"[TRACE]",
	"[DEBUG]",
	"[INFO ]",
	"[WARN ]",
	"[ERROR]",
	"[FATAL]",
};

// helper class for known string length at compile time
class T
{
 public:
  T(const char* str, unsigned len)
    :m_str(str),
     m_len(len)
  {
    assert(strlen(str) == m_len);
  }

  const char* m_str;
  const unsigned m_len;
};

void defaultOutput(const char *msg, int len){
	size_t n = fwrite(msg, 1, len, stdout);
	(void)n;
}

void defaultFlush(){
	fflush(stdout);
}

Logger::outputFunc g_output = defaultOutput;
Logger::flushFunc g_flush = defaultFlush;

void Logger::setOutput(outputFunc out){
	g_output = out;
}

void Logger::setFlush(flushFunc flush){
	g_flush = flush;
}

Logger::Logger(SourceFile file, int line)
	: m_impl(INFO, 0, file, line){
}

Logger::Logger(SourceFile file, int line, LogLevel level)
	: m_impl(level, 0, file, line){
}

Logger::Logger(SourceFile file, int line, bool toAbort)
	: m_impl(toAbort? FATAL:ERROR, errno, file, line){
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
	: m_impl(level, 0, file, line){
	m_impl.m_stream << '[' << func << "] ";
}

Logger::~Logger(){
	m_impl.finish();
	const LogStream::Buffer& buf(stream().buffer());
	g_output(buf.data(), buf.length());
	if (m_impl.m_level == FATAL)
	{
		g_flush();
		abort();
	}
}

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
	: m_time(TimeStamp::now()),
	  m_stream(),
	  m_level(level),
	  m_fileBaseName(file),
	  m_line(line)
{
	formatTime();
	m_stream << LogLevelName[level] << ' ';
	m_stream << '[' << m_fileBaseName.m_data << ':' << m_line << "] ";
	if (savedErrno != 0)
	{
		m_stream << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

void Logger::Impl::finish()
{
	m_stream<< '\n';
}

void Logger::Impl::formatTime()
{	
	int64_t microSecondsSinceEpoch = m_time.microSecondsSinceEpoch();
	time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / TimeStamp::kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(microSecondsSinceEpoch % TimeStamp::kMicroSecondsPerSecond);
	if (seconds != t_lastSecond){
		t_lastSecond = seconds;
		struct tm tm_time;

		::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime

		int len = snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d %02d:%02d:%02d",
		tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
		tm_time.tm_hour + 8, tm_time.tm_min, tm_time.tm_sec);
		assert(len == 19); (void)len;
	}

	Fmt us(".%06d ", microseconds);
	assert(us.length() == 8);
	m_stream << t_time << us.data();
}

