#pragma once

#include <string>
#include <map>
#include <Poco/Runnable.h>
#include <Poco/Net/IPAddress.h>
#include "Clock.h"


using namespace std;

namespace clockkitx
{
	/**
	 * A network server for a local clock.
	 * The server accpts and answers requests on a UDP port.
	 * It also stores responses from the clients on the state of their
	 * synchronization.
	 *
	 * The server will not start untill it's thread is started.
	 * To start up the server, call start().
	 */
	class ClockServer : public Poco::Runnable
	{
	private:
		Poco::Net::IPAddress address_;
		int port_;
		Clock& clock_;

		struct Entry
		{
			timestamp_t time;
			int offset;
			int rtt;
		};

		map<string, Entry> ackData_;
		bool log_;

		static const timestamp_t SYSTEM_STATE_PURGE_TIME;
		timestamp_t lastUpdate_;

	public:

		/// Create a new ClockServer.  The server will bind to the provided
		/// IP address and port.  To bind to ANY local address, then set the
		/// IP address to 0.0.0.0 .  The server will then use the provided clock
		/// to answer requests for time.
		/// 
		ClockServer(Poco::Net::IPAddress address, int port, Clock& clock);


		void setLogging(bool log);

	protected:

		/// The thread calls this run method, which is responsible for
		/// all network operations.
		/// 
		void run() override;

		void updateEntry(const string& address, int offset, int rtt);
	};
}
