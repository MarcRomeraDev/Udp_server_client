#pragma once
#include <iostream>
#include "Types.h"

class Command
{
private:
	unsigned short senderPort;
	Header confirmationHeader;
	unsigned int id;
public:
	std::string data;
	Command();
	Command(std::string _data, Header _confirmationHeader, unsigned short port, unsigned int _id);

};