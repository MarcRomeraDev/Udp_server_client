#pragma once
#include <thread>
#include <UdpSocket.h>

class Client
{
private:
	UdpSocket* socket;
	std::string recipient;
	std::string sender;
	unsigned short port;
	std::size_t received = 0;
	char data[1024] = "";

	void ReceiveMessages(std::vector<std::string>* messages);

public:
	Client(std::vector<std::string>&);
	~Client();

	void SendMessage(std::string);
};