#include "AccCMD.h"
#include <string>

std::string AccCMD::GetCommandData()
{
	std::string message = std::to_string((int)Header::COMMAND);	

	for (const auto& command : accCommands)
	{
		message += "<" + command.data;
	}

	accCommands.clear();

	return message;
}