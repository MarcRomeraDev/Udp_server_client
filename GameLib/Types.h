#pragma once

enum class Header {
	CONNECT = 0, // Client -> Server (Cuando el cliente se conecta por primera vez)
	CHALLENGE, // Server -> Client (Creates a challenge for the client)
	RSP_CHALLENGE, // Client -> Server ( Response of the challenge)
	ACK_CHALLENGE, // Server -> Client (Notifies the connection has been aproved)
	CLIENT_DISCONNECT, // Client -> Server (Notifies the client has disconnected)
	SERVER_DISCONNECT, // Server -> Client ( Notifies the server has disconnected
	ACTION,  // Client -> Serve ( Move or Shoot)
	UPDT_WORLD, // Server -> Client ( Replication of the world to all users )
	READY, // Server -> Client ( Start countdown when match is complete)
	FINISH // Server -> Client (End match after countdown or if all clients have left)
};

