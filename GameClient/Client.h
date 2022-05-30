#pragma once
#include <thread>
#include <UdpSocket.h>
#include <Types.h>

class Client
{
private:
	UdpSocket* socket;
	std::string recipient;
	std::string sender;
	bool end = false;
	bool connected = false;
	unsigned short port;
	std::size_t received = 0;
	char data[1024] = "";
	void ReceiveMessages(std::vector<std::string>* messages, bool*);
	uint32_t CreateSALT();
	void Connect();
	void SendCriticalMessage(Header header, std::string data);
public:
	Client();
	~Client();

	void SendMessage(std::string);
};