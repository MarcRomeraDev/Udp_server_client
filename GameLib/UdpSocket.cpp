#include "UdpSocket.h"

UdpSocket::UdpSocket()
{
	udpSocket = new sf::UdpSocket();
}

UdpSocket::~UdpSocket()
{
	delete udpSocket;
}

unsigned short UdpSocket::GetRemotePort()
{
	return 0;
}

bool UdpSocket::Bind(unsigned short port)
{
	return udpSocket->bind(port) == sf::Socket::Done;
}

bool UdpSocket::Bind()
{
	return udpSocket->bind(sf::Socket::AnyPort) == sf::Socket::Done;
}

bool UdpSocket::Send(const void* data, std::size_t size, const sf::IpAddress& remoteAddress, unsigned short port)
{
	return udpSocket->send(data, size + 1, remoteAddress, port) == sf::Socket::Done;
}

bool UdpSocket::Receive(void* data, std::size_t size, std::size_t& received, std::string& remoteAddress, unsigned short& port)
{	
	return udpSocket->receive(data, size, received, sf::IpAddress(remoteAddress), port) == sf::Socket::Done;
}