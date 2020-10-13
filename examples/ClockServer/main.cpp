
#include <cstdlib>
#include <iostream>
#include <Poco/Thread.h>
#include <Poco/Net/IPAddress.h>

#include "ClockServer.h"
#include "HighResolutionClock.h"

using namespace clockkitx;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "usage: clockServer <port>" << endl;
        return 0;
    }

    const int port = atoi(argv[1]);
    const Poco::Net::IPAddress address( "0.0.0.0" );
    
    // used to create a off-frequency clock for testing
    //VariableFrequencyClock vfc(HighResolutionClock::instance());
    //vfc.setFrequency(1000000 - 2000);
    //ClockServer server(addr, port, vfc);


    ClockServer server(address, port, HighResolutionClock::instance());
    
    server.setLogging(true);

    Poco::Thread thread;

	thread.start(server);

	while(cin.peek() == EOF) { }
	
    thread.join();

    return 0;
}
