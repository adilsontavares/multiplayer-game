#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <Common.h>

#include "Player.h"

Player mainPlayer;
SOCKET serverSocket;
sockaddr_in serverAddr;

void setupSockets();
void terminateSockets();

int main()
{
	setupSockets();

	char buffer[50];
	int result;
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);

	while (true)
	{
		result = recvfrom(serverSocket, buffer, PACKET_SIZE, 0, (SOCKADDR*)&addr, &addrLen);

		if (result != SOCKET_ERROR)
		{
			CommandMoveRequest req = *((CommandMoveRequest*)buffer);

			std::cout << "Message received: " << std::endl;
			std::cout << "LEFT: \t" << req.left << std::endl;
			std::cout << "RIGHT: \t" << req.right << std::endl;
			std::cout << "UP: \t" << req.up << std::endl;
			std::cout << "DOWN: \t" << req.down << std::endl;
			std::cout << "-" << std::endl;

			if (req.left)
				mainPlayer.x -= PLAYER_SPEED;
			if (req.right)
				mainPlayer.x += PLAYER_SPEED;
			if (req.down)
				mainPlayer.y += PLAYER_SPEED;
			if (req.up)
				mainPlayer.y -= PLAYER_SPEED;

			CommandMoveResponse res;
			res.x = mainPlayer.x;
			res.y = mainPlayer.y;

			result = sendto(serverSocket, (char*)&res, PACKET_SIZE, 0, (sockaddr*)&addr, addrLen);
			if (result == SOCKET_ERROR)
				std::cout << "Could not send response: " << WSAGetLastError() << std::endl;
		}
	}

	terminateSockets();

	return 0;
}

void setupSockets()
{
	WSAData data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		fatalError("Could not start windows sockets.");

	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(12346);

	if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		fatalError("Could not bind socket.");
}

void terminateSockets()
{
	WSACleanup();
}