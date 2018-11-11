#include <sys/time.h>
#include "TimeStamp.hh"
#include <inttypes.h>

TimeStamp TimeStamp::now(){
	struct timeval tv;
	gettimeofday(&tv, 0);
	int64_t seconds = tv.tv_sec;
	return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}


std::string TimeStamp::toString() const
{
  char buf[32] = {0};
  int64_t seconds = m_microSecondsSinceEpoch / kMicroSecondsPerSecond;
  int64_t microseconds = m_microSecondsSinceEpoch % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}
