#pragma once

#include <string>

#include "ClockClient.h"
#include "PhaseLockedClock.h"

namespace clockkitx
{
	extern const std::string DEFAULT_CONFIG_FILE_PATH;

	std::unique_ptr<PhaseLockedClock> PhaseLockedClockFromConfigFile(const std::string& filename, std::unique_ptr<ClockClient>& clockClient);
}
