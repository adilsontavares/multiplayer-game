#pragma once
#include "CommandCodes.h"

struct CommandMoveRequest
{
	int command = CMD_MOVE_CHARACTER_REQ;
	int movX;
	int movY;
};

struct CommandMoveResponse
{
	int command = CMD_MOVE_CHARACTER_RES;
	float x;
	float y;
	int movX;
	int movY;
};