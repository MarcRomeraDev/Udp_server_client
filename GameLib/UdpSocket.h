#pragma once
#include <SFML\Network.hpp>

class UdpSocket
{
private:
	sf::UdpSocket* udpSocket;

public:
	UdpSocket();
	UdpSocket(sf::UdpSocket* _udpSocket);
	~UdpSocket();

	std::string GetLocalHost() const { return sf::IpAddress::LocalHost.toString(); }

	sf::UdpSocket* GetSocket() { return udpSocket; }
	void SetSocket(sf::UdpSocket* _udpSocket);
	//std::string GetRemoteIP();
	std::string GetLocalIP() const { return sf::IpAddress::getLocalAddress().toString(); }
	unsigned short GetLocalPort() const { return udpSocket->getLocalPort(); }
	unsigned short GetRemotePort();
	bool Bind(unsigned short);
	bool Bind();
	bool Send(const void*, std::size_t, const sf::IpAddress&, unsigned short);
	bool Receive(void*, std::size_t, std::size_t&, std::string&, unsigned short&);

	void Disconnect();
};