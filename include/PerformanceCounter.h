#pragma once

#include <Poco/Environment.h>

#include "Common.h"
#include "Clock.h"

#ifdef POCO_OS_FAMILY_WINDOWS

namespace clockkitx
{
	/// A class for WINDOWS ONLY.  It represents the system's high resolution
	/// performance counter.  However, the counter is adjusted so that it ticks
	/// at 1000000 Hz rather than the default frequency.
	/// 
	/// WARNING: The performance counter may loop around at some point.  It's 
	/// not necessarily monotonic.
	/// 
	class PerformanceCounter : public Clock
	{
	public:

		PerformanceCounter(PerformanceCounter& c) = delete;
		PerformanceCounter& operator=(PerformanceCounter& rhs) = delete;
		
		/// return an instance of the performance counter.
		/// This is a threadsafe singleton
		/// 
		static PerformanceCounter& instance();

		/// returns the system clock as a timestamp value
		/// in microseconds since UNIX epoch.
		/// 
		timestamp_t getValue() override;

	private:
		PerformanceCounter();

		static PerformanceCounter instance_;

		double freqConversion_;
	};
}

#endif