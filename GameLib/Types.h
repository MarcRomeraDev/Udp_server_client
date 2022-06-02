#pragma once

enum class Header {
	CONNECT = 0, // Client -> Server (Cuando el cliente se conecta por primera vez)
	CHALLENGE, // Server -> Client (Creates a challenge for the client)
	RSP_CHALLENGE, // Client -> Server ( Response of the challenge)
	ACK_CHALLENGE, // Server -> Client (Notifies the connection has been aproved)
	CLIENT_DISCONNECT, // Client -> Server (Notifies the client has disconnected)
	CLIENT_DISCONNECT_ACK, // Client -> Server (Notifies the client has disconnected)
    MESSAGE,
	SERVER_DISCONNECT, // Server -> Client ( Notifies the server has disconnected
	ACTION,  // Client -> Serve ( Move or Shoot)
	OK_ACTION,
	UPDT_WORLD, // Server -> Client ( Replication of the world to all users )
	READY, // Server -> Client ( Start countdown when match is complete)
	FINISH, // Server -> Client (End match after countdown or if all clients have left)
	COUNT
};

static uint32_t CreateSALT()
{
    uint32_t x = rand() & 0xff;
    x |= (rand() & 0xff) << 8;
    x |= (rand() & 0xff) << 16;
    x |= (rand() & 0xff) << 24;

    return x;
}

static int Len(std::string str)
{
    int length = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        length++;

    }
    return length;
}

static std::vector<std::string> Split(std::string str, char seperator)
{
    int currIndex = 0, i = 0;
    int startIndex = 0, endIndex = 0;
    std::vector<std::string> strings;
    while (i <= Len(str))
    {
        if (str[i] == seperator || i == Len(str))
        {
            endIndex = i;
            std::string subStr = "";
            subStr.append(str, startIndex, endIndex - startIndex);
            strings.push_back(subStr);
            currIndex += 1;
            startIndex = endIndex + 1;
        }
        i++;
    }
    return strings;
}