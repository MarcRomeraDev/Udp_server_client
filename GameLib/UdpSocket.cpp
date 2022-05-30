#include "UdpSocket.h"
#include "Types.h"
sf::Packet& operator<<(sf::Packet& pack, const Header& header)
{
	return pack << static_cast<int>(header);
}
UdpSocket::UdpSocket()
{
	udpSocket = new sf::UdpSocket();
}

UdpSocket::~UdpSocket()
{
	Unbind();
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
void UdpSocket::Unbind()
{
	return udpSocket->unbind();
}

bool UdpSocket::Send(const void* data, std::size_t size, const sf::IpAddress& remoteAddress, unsigned short port)
{
	return udpSocket->send(data, size + 1, remoteAddress, port) == sf::Socket::Done;
}

bool UdpSocket::Receive(void* data, std::size_t size, std::size_t& received, std::string& remoteAddress, unsigned short& port)
{
	sf::IpAddress aux;
	bool result = udpSocket->receive(data, size, received, aux, port) == sf::Socket::Done;
	remoteAddress = aux.toString();
	return result;
}