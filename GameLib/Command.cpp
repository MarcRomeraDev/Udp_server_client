#include "Command.h"

Command::Command() : data("")
{

}

Command::Command(std::string _data, unsigned short _senderPort, unsigned int _id ) : data(_data), senderPort(_senderPort), id(_id)
{

}