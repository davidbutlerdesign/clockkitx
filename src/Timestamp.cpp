#include "Timestamp.h"
#include "Common.h"
#include <cstdio>
#include <Poco/Platform.h>

namespace clockkitx
{
	std::string Timestamp::timestampToString(timestamp_t t)
	{
		const timestamp_t second = 1000000;
		int secs = static_cast<int>(t / second);
		int usecs = static_cast<int>(t % second);

		char buff[256];
		sprintf(buff, "<time %i %i>", secs, usecs);
		return std::string(buff);
	}

	timestamp_t Timestamp::stringToTimestamp(const std::string& t)
	{
		int secs, usecs;
		if (sscanf(t.c_str(), "<time %i %i>", &secs, &usecs) != 2)
			return 0;
		const timestamp_t second = 1000000;
		return static_cast<timestamp_t>(secs * second) + usecs;
	}


	void Timestamp::timestampToBytes(timestamp_t time, char* buffer)
	{
		char* t = (char*)&time;
		
#ifdef POCO_ARCH_BIG_ENDIAN
			for (int i = 0; i < 8; i++) buffer[i] = t[i];
#else
			for (int i = 0; i < 8; i++) buffer[i] = t[7 - i];
#endif
	}

	timestamp_t Timestamp::bytesToTimestamp(const char* buffer)
	{
		timestamp_t time;
		char* t = (char*)&time;
		
#ifdef POCO_ARCH_BIG_ENDIAN
			for (int i = 0; i < 8; i++) t[i] = buffer[i];
#else
			for (int i = 0; i < 8; i++) t[i] = buffer[7 - i];
#endif
		
		return time;
	}
}
