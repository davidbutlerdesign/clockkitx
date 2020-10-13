#include <memory>
#include <Poco/Thread.h>

#include "PhaseLockedClock.h"
#include "ConfigReader.h"
#include "Exceptions.h"

using namespace clockkitx;

int main(int argc, char* argv[])
{
	if (argc > 2)
	{
		std::cout << "usage phaseLock [config file]" << std::endl;
		return 0;
	}

	std::string configFile;
	if (argc == 2)
		configFile = argv[1];
	else
	configFile = DEFAULT_CONFIG_FILE_PATH;

	std::unique_ptr<ClockClient> clockClient;
	
	auto plc = PhaseLockedClockFromConfigFile(configFile, clockClient);

	while (true)
	{
		try
		{
			const int offset = plc->getOffset();
			const timestamp_t now = plc->getValue();
			std::cout << "offset: " << offset << std::endl;
			std::cout << "time: " << Timestamp::timestampToString(now) << std::endl;
		}
		catch (const ClockException& ex)
		{
			std::cout << "offset: OUT OF SYNC" << std::endl;
		}

		Poco::Thread::sleep(1000);
	}

	std::cout << "DONE." << std::endl;
	return 0;
}
