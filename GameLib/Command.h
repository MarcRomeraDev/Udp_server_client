#pragma once
#include <iostream>
#include "Types.h"

class Command
{
private:
	
public:
	Command();
	Command(std::string _data, Header _confirmationHeader);

	std::string data;
};