#pragma once

#include "Common.h"
#include "Clock.h"

namespace clockkitx
{
	/**
	 * A class for WINDOWS ONLY.  It represents the system's high resolution
	 * performance counter.  However, the counter is adjusted so that it ticks
	 * at 1000000 Hz rather than the default frequency.
	 * 
	 * WARNING: The performance counter may loop around at some point.  It's 
	 * not necessarly monotonic.
	 */
	class PerformanceCounter : public Clock
	{
	public:

		/**
		 * return an instance of the performance counter.
		 * This is a threadsafe singelton
		 */
		static PerformanceCounter& instance();

		/**
		 * returns the system clock as a timestamp value
		 * in microseconds since UNIX epoch.
		 */
		timestamp_t getValue() override;

	private:
		PerformanceCounter();
		PerformanceCounter(PerformanceCounter& c);
		PerformanceCounter& operator=(PerformanceCounter& rhs);

		static PerformanceCounter instance_;

		double freqConversion_;
	};
}
