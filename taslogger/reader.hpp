#pragma once

#include <string>
#include <vector>
#include "common.hpp"
#include "rapidjson/reader.h"

namespace TASLogger
{
	struct ReaderCollision
	{
		float normal[3];
		float distance;
		float impactVelocity[3];
		int32_t entity;
	};

	struct ReaderObjectMove
	{
		float velocity[3];
		float position[3];
		bool pull;
	};

	struct ReaderDamage
	{
		float damage;
		float direction[3];
		int32_t damageBits;
	};

	struct ReaderPlayerState
	{
		float position[3];
		float velocity[3];
		float baseVelocity[3];
		bool onGround;
		bool onLadder;
		uint8_t waterLevel;
		uint8_t duckState;
	};

	struct ReaderCommandFrame
	{
		ReaderPlayerState prePMState;
		ReaderPlayerState postPMState;
		std::vector<ReaderCollision> collisionList;
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

	struct ReaderRng
	{
		int32_t idum;
		int32_t iy;
		int32_t iv[32];
	};

	struct ReaderPhysicsFrame
	{
		std::string commandBuffer;
		std::vector<std::string> consolePrintList;
		std::vector<ReaderCommandFrame> commandFrameList;
		std::vector<ReaderDamage> damageList;
		std::vector<ReaderObjectMove> objectMoveList;
		float frameTime;
		bool paused;
		int8_t clientState;
		ReaderRng rng;
	};

	struct TASLog
	{
		std::string toolVersion;
		std::string gameMod;
		std::vector<ReaderPhysicsFrame> physicsFrameList;
		int32_t buildNumber;
	};

	rapidjson::ParseResult ParseFile(FILE *file, TASLog &tasLog);
}
