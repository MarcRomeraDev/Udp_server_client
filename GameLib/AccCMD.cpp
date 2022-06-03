#include "AccCMD.h"
#include <string>

std::string AccCMD::GetCommandData()
{
	std::string message = std::to_string((int)Header::COMMAND);	
	message +="<" +std::to_string(id) + "<" + std::to_string(senderPort);
	

	for (const auto& command : accCommands)
	{
		message += "<" + command->data;
	}

	accCommands.clear();

	return message;
}