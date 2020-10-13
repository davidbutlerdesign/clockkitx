#include "SystemClock.h"
#include <Poco/Environment.h>

#ifdef POCO_OS_FAMILY_WINDOWS
#include <Windows.h>
#else
	#include <sys/time.h>
#endif

namespace clockkitx
{
	SystemClock SystemClock::instance_;

	SystemClock& SystemClock::instance()
	{
		return instance_;
	}

	timestamp_t SystemClock::getValue()
	{
		Poco::ScopedLock<Poco::Mutex> lock(mutex_);
		
		timestamp_t time;
	
#ifdef POCO_OS_FAMILY_WINDOWS

		FILETIME fileTime;
		GetSystemTimeAsFileTime(&fileTime);
		// convert to a 64bit int.
		time = 0;
		time = fileTime.dwHighDateTime;
		time <<= 32;
		time |= fileTime.dwLowDateTime;
		// convert from 100 n-sec units to usec:
		time /= 10;
		// convert from windows epoch to unix epoch.
		time -= 11644473600000000;

#else

		timeval now;
	    gettimeofday(&now, 0);
	    time = now.tv_sec;
	    time *= 1000000;
	    time += now.tv_usec;

#endif

		return time;
	}
}
