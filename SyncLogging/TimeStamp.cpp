#include <sys/time.h>
#include "TimeStamp.hh"

TimeStamp TimeStamp::now(){
	struct timeval tv;
	gettimeofday(&tv, 0);
	int64_t seconds = tv.tv_sec;
	return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
