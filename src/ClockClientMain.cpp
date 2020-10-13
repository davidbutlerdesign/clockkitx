
#include <iostream>
#include <Poco/Thread.h>

#include "ClockClient.h"
#include "HighResolutionClock.h"
#include "Exceptions.h"

using namespace clockkitx;

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
	    std::cout << "usage clockClient <server address> <port>" << std::endl;
        return 0;
    }

    const Poco::Net::IPAddress address(argv[1]);
    const int port = atoi(argv[2]);

    std::cout << "Starting Client:" << std::endl;
    std::cout << "  bind: " << address.toString() << std::endl;
    std::cout << "  port: " << port << std::endl;
        

    Clock& hires = HighResolutionClock::instance();
    ClockClient client(address, port);
    client.setTimeout(15000);
    client.setAcknowledge(true);

    while (true)
    {
        try
        {
	        const timestamp_t now = client.getPhase(hires);
            std::cout << "offset: " << now
                 << "\trtt: " << client.getLastRTT() << std::endl;
        }
        catch (const ClockException&) { }
    	
        Poco::Thread::sleep(1000);
    }

    return 0;
}

