#pragma once

namespace TASLogger
{
	struct Damage
	{
		double damage;
		double direction[3];
		int32_t damageBits;
	};

	struct ObjectMove
	{
		double velocity[3];
		double position[3];
		bool pull;
	};

	struct Collision
	{
		double normal[3];
		double distance;
		double impactVelocity[3];
		int32_t entity;
	};

	enum DuckState : uint32_t
	{
		UNDUCKED = 0,
		INDUCK,
		DUCKED
	};
}