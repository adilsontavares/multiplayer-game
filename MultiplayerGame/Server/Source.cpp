#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <Common.h>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

#include "Player.h"

Player mainPlayer;
SOCKET serverSocket;
sockaddr_in serverAddr;

void setupSockets();
void terminateSockets();

int movX;
int movY;

std::mutex mutex;

void handleMovement();

int main()
{
	setupSockets();

	char buffer[50];
	int result;
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);

	new std::thread(handleMovement);

	std::vector<sockaddr_in> sockets;

	while (true)
	{
		result = recvfrom(serverSocket, buffer, PACKET_SIZE, 0, (SOCKADDR*)&addr, &addrLen);

		if (std::find_if(sockets.begin(), sockets.end(), [&](sockaddr_in a) { return a.sin_addr.s_addr == addr.sin_addr.s_addr && a.sin_port == addr.sin_port; }) == sockets.end())
			sockets.push_back(addr);

		if (result != SOCKET_ERROR)
		{
			CommandMoveRequest req = *((CommandMoveRequest*)buffer);

			std::cout << "Message received!" << std::endl;

			mutex.lock();
			movX = req.movX;
			movY = req.movY;

			CommandMoveResponse res;
			res.x = mainPlayer.x;
			res.y = mainPlayer.y;
			res.movX = movX;
			res.movY = movY;
			mutex.unlock();

			std::cout << "Enviando para " << sockets.size() << " sockets..." << std::endl;
			for (sockaddr_in addr : sockets)
			{
				result = sendto(serverSocket, (char*)&res, PACKET_SIZE, 0, (sockaddr*)&addr, sizeof(addr));
				if (result == SOCKET_ERROR)
					std::cout << "Could not send response: " << WSAGetLastError() << std::endl;
			}
		}
	}

	terminateSockets();

	return 0;
}

void handleMovement()
{
	while (true)
	{
		Sleep(1000.0 / 60.0);

		mutex.lock();

		mainPlayer.x += movX * PLAYER_SPEED;
		mainPlayer.y += movY * PLAYER_SPEED;

		mutex.unlock();
	}
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