#include "ConfigReader.h"

#include "Common.h"
#include "ClockClient.h"
#include "HighResolutionClock.h"
#include "PhaseLockedClock.h"
#include "Exceptions.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <memory>

namespace clockkitx
{
#ifdef POCO_OS_FAMILY_WINDOWS
	const std::string DEFAULT_CONFIG_FILE_PATH = "C:\\clockkit.conf";
#else
	const std::string DEFAULT_CONFIG_FILE_PATH = "/etc/clockkit.conf";
#endif

	std::unique_ptr<PhaseLockedClock> PhaseLockedClockFromConfigFile(const std::string& filename, std::unique_ptr<ClockClient>& clockClient)
	{
		std::string server = "localhost";
		int port = 4444;
		int timeout = 1000;
		int phasePanic = 5000;
		int updatePanic = 5000000;

		std::ifstream file(filename.c_str());
		if (!file.is_open())
			throw Poco::Exception("failed to open config file " + DEFAULT_CONFIG_FILE_PATH);

		while (!file.eof())
		{
			std::string line;
			file >> line;
			const size_t pos = line.find(':');
			if (pos < 0)
				break;
			
			std::string name = line.substr(0, pos);
			std::string value = line.substr(pos + 1);

			if (name == "server")
			{
				server = value;
			}
			else if (name == "port")
			{
				port = atoi(value.c_str());
			}
			else if (name == "timeout")
			{
				timeout = atoi(value.c_str());
			}
			else if (name == "phasePanic")
			{
				phasePanic = atoi(value.c_str());
			}
			else if (name == "updatePanic")
			{
				updatePanic = atoi(value.c_str());
			}
		}

		file.close();

		std::cout << "config [server:" << server << "]" << std::endl;
		std::cout << "config [port:" << port << "]" << std::endl;
		std::cout << "config [timeout:" << timeout << "]" << std::endl;
		std::cout << "config [phasePanic:" << phasePanic << "]" << std::endl;
		std::cout << "config [updatePanic:" << updatePanic << "]" << std::endl;

		const Poco::Net::IPAddress address(server);
		clockClient = std::make_unique<ClockClient>(address, port);
		clockClient->setTimeout(timeout);
		clockClient->setAcknowledge(true);

		std::unique_ptr<PhaseLockedClock> plc = std::make_unique<PhaseLockedClock>(
			HighResolutionClock::instance(),
			*clockClient);
		
		plc->setPhasePanic(phasePanic);
		plc->setUpdatePanic(updatePanic);

		return plc;
	}
}
