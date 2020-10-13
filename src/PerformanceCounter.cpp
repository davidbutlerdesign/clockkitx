#include "PerformanceCounter.h"
#include "Exceptions.h"
#include <windows.h>

namespace clockkitx
{
	using namespace std;

	PerformanceCounter PerformanceCounter::instance_;

	PerformanceCounter::PerformanceCounter()
	{
		LARGE_INTEGER rate;
		if (!QueryPerformanceFrequency(&rate))
			throw ClockException("Error getting performance counter frequency");
		freqConversion_ = 1000000.0 / static_cast<double>(rate.QuadPart);
	}

	PerformanceCounter& PerformanceCounter::instance()
	{
		return instance_;
	}

	timestamp_t PerformanceCounter::getValue()
	{
		LARGE_INTEGER qpc;
		if (!QueryPerformanceCounter(&qpc))
			throw ClockException("Error getting performance counter");
		return static_cast<timestamp_t>(static_cast<double>(qpc.QuadPart) * freqConversion_);
	}
}
