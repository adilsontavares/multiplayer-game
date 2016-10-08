#pragma once
#include "CommandCodes.h"

struct CommandMoveRequest
{
	int command = CMD_MOVE_CHARACTER_REQ;
	bool up;
	bool down;
	bool right;
	bool left;
};

struct CommandMoveResponse
{
	int command = CMD_MOVE_CHARACTER_RES;
	float x;
	float y;
};