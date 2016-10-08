#pragma once

#include <iostream>

void fatalError(const char *message)
{
	std::cout << "Fatal error: " << message << std::endl;
	getchar();
	getchar();
	exit(-1);
}