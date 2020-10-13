#pragma once

#include <Poco/Mutex.h>
#include "Common.h"
#include "Clock.h"

namespace clockkitx
{
	/**
	 * Provides a system clock as microseconds (usec) since UNIX epoch.
	 * 
	 * Note, in windows, the resolution of this clock may be very bad.
	 * Look into using PerformanceCounter to improve accuracy.
	 * 
	 * Use instance() to obtain an instance of this object.
	 *
	 * This class makes use of a mutex to ensure that many threads can
	 * safely call the clock. In some OS implementations, calls to 
	 * gettimeofday() may not be thread safe. 
	 */
	class SystemClock : public Clock
	{
		Poco::Mutex mutex_;
		
	public:

		SystemClock(SystemClock& c) = delete;

		SystemClock& operator=(SystemClock& rhs) = delete;
		
		/// @return A singleton instance of the system clock.
		/// 
		static SystemClock& instance();

		/// @return the system clock as a timestamp value
		///         in microseconds since UNIX epoch.
		/// 
		timestamp_t getValue() override;

	private:

		/// Constructor is private.  Use SystemClock::instance() to get
		/// an instance of the SystemClock singleton.
		/// 
		SystemClock() = default;

		// Static variable for the instance of the singleton.
		// 
		static SystemClock instance_;
	};
}
