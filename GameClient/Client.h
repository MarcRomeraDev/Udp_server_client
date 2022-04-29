#pragma once
#include <thread>
#include <UdpSocket.h>

class Client
{
private:
	UdpSocket* socket;
	std::string recipient = "192.168.0.18";
	std::string sender;
	unsigned short port;
	std::size_t received = 0;
	char data[1024] = "";

	void RecieveMessages(std::vector<std::string>*);

public:
	Client(std::vector<std::string>&);
	~Client();

	void SendMessage(std::string);
};