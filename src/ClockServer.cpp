#include "ClockServer.h"
#include "ClockPacket.h"
#include "Clock.h"
#include <iostream>
#include <cmath>
#include <Poco/Net/DatagramSocket.h>

namespace clockkitx
{
	const timestamp_t ClockServer::SYSTEM_STATE_PURGE_TIME = 5000000; // 5 seconds

	ClockServer::ClockServer(const Poco::Net::IPAddress address, int port, Clock& clock)
		: address_(address), port_(port), clock_(clock), log_(false)
	{
		lastUpdate_ = clock_.getValue();
	}

	void ClockServer::run()
	{
		Poco::Net::DatagramSocket socket(Poco::Net::SocketAddress(address_, port_));

		if (log_)
		{
			cout << "time\thost\toffset\trtt" << endl;
		}

		const int length = ClockPacket::PACKET_LENGTH;
		char buffer[length];
		ClockPacket packet;

		while (socket.available())
		{
			const timestamp_t serverReplyTime = clock_.getValue();
			Poco::Net::SocketAddress peerAddress;
			const int bytesReceived = socket.receiveFrom(buffer, length, peerAddress);
			if (bytesReceived != length)
			{
				cerr << "ERR: Received packet of wrong length." << endl;
			}
			else
			{
				packet.read(buffer);
				if (packet.getType() == ClockPacket::PacketType::REQUEST)
				{
					packet.setServerReplyTime(serverReplyTime);
					packet.setType(ClockPacket::PacketType::REPLY);
					packet.write(buffer);
					const int bytesSent = socket.sendBytes(buffer, length);
					if (bytesSent != length)
					{
						cerr << "ERR: Could not send entire packet." << endl;
					}
				}
				else if (packet.getType() == ClockPacket::PacketType::ACKNOWLEDGE)
				{
					updateEntry(
						peerAddress.toString(),
						packet.getClockOffset(),
						packet.getRTT());
				}
				else
				{
					cerr << "ERR: received packet of invalid type" << endl;
				}
			}
		}
	}

	void ClockServer::setLogging(bool log)
	{
		log_ = log;
	}

	void ClockServer::updateEntry(const string& address, int offset, int rtt)
	{
		const timestamp_t now = clock_.getValue();
		const string nowStr = Timestamp::timestampToString(now);

		ackData_[address].time = now;
		ackData_[address].offset = offset;
		ackData_[address].rtt = rtt;

		if (log_)
		{
			cout << nowStr << '\t'
				<< address << '\t'
				<< offset << '\t'
				<< rtt << endl;

			if ((now - lastUpdate_) > 1000000)
			{
				lastUpdate_ = now;

				map<string, Entry>::iterator it;

				// purge old entries
				for (it = ackData_.begin(); it != ackData_.end(); ++it)
				{
					timestamp_t entryTime = (it->second).time;
					if (now - entryTime > SYSTEM_STATE_PURGE_TIME)
						ackData_.erase(it);
				}

				// calculate maximum offset
				int maxOffset = 0;
				for (it = ackData_.begin(); it != ackData_.end(); ++it)
				{
					int offset = abs((it->second).offset) + ((it->second).rtt / 2);
					if (offset > maxOffset)
						maxOffset = offset;
				}

				cout << nowStr << '\t'
					<< "MAX_OFFSET" << '\t'
					<< maxOffset << '\t'
					<< endl;
			}
		}
	}
}
