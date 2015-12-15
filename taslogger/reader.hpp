#pragma once

#include <string>
#include <vector>
#include "common.hpp"
#include "rapidjson/reader.h"

namespace TASLogger
{
	struct PlayerState
	{
		float position[3];
		float velocity[3];
		float baseVelocity[3];
		bool onGround;
		bool onLadder;
		uint8_t waterLevel;
		uint8_t duckState;
	};

	struct CommandFrame
	{
		PlayerState prePMState;
		PlayerState postPMState;
		std::vector<std::string> consolePrintList;
		std::vector<Collision> collisionList;
		float viewangles[3];
		float punchangles[3];
		float FSU[3];
		float frameTimeRemainder;
		float entFriction;
		float entGravity;
		float health;
		float armor;
		uint32_t framebulkId;
		uint32_t sharedSeed;
		uint8_t msec;
		uint8_t buttons;
		uint8_t impulse;
	};

	struct PhysicsFrame
	{
		std::string commandBuffer;
		std::vector<CommandFrame> commandFrameList;
		std::vector<Damage> damageList;
		std::vector<ObjectMove> objectMoveList;
		float frameTime;
		bool paused;
		int8_t clientState;
	};

	struct TASLog
	{
		std::string toolVersion;
		std::string gameMod;
		std::vector<PhysicsFrame> physicsFrameList;
		int32_t buildNumber;
	};

	rapidjson::ParseResult ParseString(const char *json, TASLog &tasLog);
}
