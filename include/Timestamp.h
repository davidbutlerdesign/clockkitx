#pragma once

#include <string>
#include <cstdint>

namespace clockkitx
{
	typedef int64_t timestamp_t;

	class Timestamp
	{
	public:

		/// Converts a timestamp value into a string.
		/// 
		static std::string timestampToString(timestamp_t t);

		/// Converts a string to a timestamp value. 
		/// 
		static timestamp_t stringToTimestamp(const std::string& t);

		/// Writes a 64 bit timestamp out to buffer location
		/// with proper byte ordering.
		/// - Writes 8 bytes into the buffer.
		/// 
		static void timestampToBytes(timestamp_t time, char* buffer);

		/// Reads a 64 bit timestamp from a buffer location
		/// with proper byte ordering.
		/// - Reads 8 bytes out of the buffer.
		/// 
		static timestamp_t bytesToTimestamp(const char* buffer);

	private:

		Timestamp() = default;
	};
}
