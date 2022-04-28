#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>
#include <thread>

class Client
{
private:
	sf::UdpSocket socket; //--> Desvincular libreria de red
	sf::IpAddress recipient = "192.168.0.18";
	sf::IpAddress sender;
	unsigned short port;
	std::size_t received = 0;
	char data[1024] = "";

	void RecieveMessages(std::vector<std::string>*);

public:
	Client(std::vector<std::string>&);

	void SendMessage(std::string);
};