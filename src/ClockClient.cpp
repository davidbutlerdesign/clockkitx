#include "ClockClient.h"
#include "Exceptions.h"
#include "HighResolutionClock.h"

namespace clockkitx
{
	ClockClient::ClockClient(const Poco::Net::IPAddress& address, int port)
		: timeout_(1000), sequence_(0), lastRTT_(0), acknowledge_(false)
	{
		// opens a udp socket on a local port, starting at 5000.
		const Poco::Net::IPAddress localhost("0.0.0.0"); // any
		int localPort = 5000;
		bool bound = false;
		while (!bound)
		{
			try
			{
				socket_ = std::make_unique<Poco::Net::DatagramSocket>(Poco::Net::SocketAddress(localhost, localPort));
				bound = true;
			}
			catch (const Poco::Exception& ex)
			{
				socket_.reset();
				localPort++;
			}
		}

		// set the destination address
		socket_->connect(Poco::Net::SocketAddress(address, port));
	}

	timestamp_t ClockClient::getValue()
	{
		Clock& baseClock = HighResolutionClock::instance();

		// remember that: secondaryClock + phase = primaryClock
		timestamp_t phase = getPhase(baseClock, false);
		timestamp_t now = baseClock.getValue();
		return (now + phase);
	}

	void ClockClient::sendPacket(ClockPacket& packet) const
	{
		const int length = ClockPacket::PACKET_LENGTH;
		char buffer[length];
		packet.write(buffer);

		const int bytesSent = socket_->sendBytes(buffer, length);
		if (bytesSent != length)
			throw ClockException("could not send packet");
	}


	ClockPacket ClockClient::receivePacket(Clock& clock)
	{
		const int length = ClockPacket::PACKET_LENGTH;
		char buffer[length];
		int timeoutMsec = timeout_ / 1000;
		if (timeoutMsec < 1) timeoutMsec = 1;

		while (true)
		{
			const bool packetArrived = socket_->poll(timeoutMsec, Poco::Net::Socket::SELECT_READ);

			if (!packetArrived)
				throw ClockException("timeout");

			const int bytesReceived = socket_->receiveBytes(buffer, length);

			if (bytesReceived != length)
				throw ClockException("received packet of wrong length");

			ClockPacket packet(buffer);
			packet.setClientReceiveTime(clock.getValue());

			if (packet.getSequenceNumber() != sequence_)
			{
				std::cout << "wrong sequence number, waiting for another packet" << std::endl;
			}
			else if (packet.getType() != ClockPacket::PacketType::REPLY)
			{
				std::cout << "packet of wrong type, waiting for another packet" << std::endl;
			}
			else if (packet.getRTT() > timeout_)
			{
				throw ClockException("response took too long");
			}
			else
			{
				lastRTT_ = packet.getRTT();
				return packet;
			}
		}
	}


	timestamp_t ClockClient::getPhase(Clock& clock)
	{
		return getPhase(clock, acknowledge_);
	}

	timestamp_t ClockClient::getPhase(Clock& clock, bool acknowledge)
	{
		sequence_ = (sequence_ % 250) + 1;

		ClockPacket packet;
		packet.setType(ClockPacket::PacketType::REQUEST);
		packet.setSequenceNumber(sequence_);
		packet.setClientRequestTime(clock.getValue());

		sendPacket(packet);

		packet = receivePacket(clock);

		if (acknowledge)
		{
			packet.setType(ClockPacket::PacketType::ACKNOWLEDGE);
			sendPacket(packet);
		}

		return packet.getClockOffset();
	}

	int ClockClient::getTimeout() const
	{
		return timeout_;
	}

	void ClockClient::setTimeout(int timeout)
	{
		timeout_ = timeout;
	}

	int ClockClient::getLastRTT() const
	{
		return lastRTT_;
	}

	void ClockClient::setAcknowledge(bool acknowledge)
	{
		acknowledge_ = acknowledge;
	}
}
