#include <SFML/Graphics.hpp>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <Common.h>

SOCKET clientSocket;
sockaddr_in serverAddr;
sockaddr_in addr;

sf::CircleShape playerShape;

int curMovX;
int curMovY;
int oldMovX;
int oldMovY;

void setupSockets();
void terminateSockets();
void receivePackets();

void handleMovement(sf::CircleShape& shape, int movX, int movY);
void handleMovement(sf::CircleShape& shape, sf::RenderWindow& window);

int main()
{
	setupSockets();

	new std::thread(receivePackets);

	sf::RenderWindow window(sf::VideoMode(800, 600), "Ola mundo");

	playerShape.setRadius(40);
	playerShape.setPosition(0, 0);
	playerShape.setFillColor(sf::Color::Cyan);

	while (window.isOpen())
	{
		Sleep(1000.0 / 60.0);

		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				window.close();
		}

		handleMovement(playerShape, window);
		handleMovement(playerShape, curMovX, curMovY);

		window.clear();
		window.draw(playerShape);
		window.display();
	}

	terminateSockets();

	return 0;
}

void setupSockets()
{
	WSAData data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		fatalError("Could not start windows sockets.");

	clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, "0.0.0.0", &addr.sin_addr);
	serverAddr.sin_port = htons(12345);

	bind(clientSocket, (sockaddr*)&addr, 0);

	serverAddr.sin_family = AF_INET;
	InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = htons(12346);
}

void terminateSockets()
{
	WSACleanup();
}

void receivePackets()
{
	char buffer[PACKET_SIZE];
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	int result;

	while (true)
	{
		result = recvfrom(clientSocket, buffer, PACKET_SIZE, 0, (sockaddr*)&addr, &addrLen);

		if (result != SOCKET_ERROR)
		{
			CommandMoveResponse res = *((CommandMoveResponse*)buffer);

			oldMovX = curMovX = res.movX;
			oldMovY = curMovY = res.movY;

			std::cout << "Received: " << curMovX << ", " << curMovY << std::endl;

			playerShape.setPosition(res.x, res.y);
		}
	}
}

void handleMovement(sf::CircleShape& shape, int movX, int movY)
{
	const float speed = PLAYER_SPEED;

	auto pos = shape.getPosition();

	pos.x += movX * speed;
	pos.y += movY * speed;

	shape.setPosition(pos);
}

void handleMovement(sf::CircleShape& shape, sf::RenderWindow& window)
{
	if (!window.hasFocus())
		return;

	curMovX = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) - sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
	curMovY = sf::Keyboard::isKeyPressed(sf::Keyboard::Down) - sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

	if (curMovX != oldMovX || curMovY != oldMovY)
	{
		CommandMoveRequest req;

		req.movX = curMovX;
		req.movY = curMovY;

		sendto(clientSocket, (char*)&req, PACKET_SIZE, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

		oldMovX = curMovX;
		oldMovY = curMovY;
	}
}