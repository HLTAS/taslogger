#pragma once

#include <cinttypes>

namespace TASLogger
{
	const char KEY_TOOL_VERSION[] = "tool_ver";
	const char KEY_BUILD_NUMBER[] = "build";
	const char KEY_MOD[] = "mod";
	const char KEY_PHYSICS_FRAMES[] = "pf";
	const char KEY_FRAMETIME[] = "ft";
	const char KEY_CLIENT_STATE[] = "cls";
	const char KEY_RNG[] = "rng";
	const char KEY_COMMAND_BUFFER[] = "cbuf";
	const char KEY_PAUSED[] = "p";
	const char KEY_COMMAND_FRAMES[] = "cf";
	const char KEY_DAMAGES[] = "dmg";
	const char KEY_DAMAGE_AMOUNT[] = "dmg";
	const char KEY_DAMAGE_BITS[] = "bits";
	const char KEY_DAMAGE_DIRECTION[] = "dir";
	const char KEY_OBJECT_BOOSTS[] = "obj";
	const char KEY_IS_PULL[] = "pull";
	const char KEY_OBJECT_VELOCITY[] = "vel";
	const char KEY_OBJECT_POSITION[] = "pos";
	const char KEY_MILLISECONDS[] = "ms";
	const char KEY_FRAMETIME_REMAINDER[] = "rem";
	const char KEY_FRAMEBULK_ID[] = "bid";
	const char KEY_SHARED_SEED[] = "ss";
	const char KEY_VIEWANGLES[] = "view";
	const char KEY_PUNCHANGLES[] = "pview";
	const char KEY_BUTTONS[] = "btns";
	const char KEY_IMPULSE[] = "impls";
	const char KEY_FSU[] = "fsu";
	const char KEY_ENT_FRICTION[] = "efric";
	const char KEY_ENT_GRAVITY[] = "egrav";
	const char KEY_PRE_PLAYERMOVE[] = "prepm";
	const char KEY_POST_PLAYERMOVE[] = "postpm";
	const char KEY_POSITION[] = "pos";
	const char KEY_VELOCITY[] = "vel";
	const char KEY_BASEVELOCITY[] = "bvel";
	const char KEY_ONGROUND[] = "og";
	const char KEY_ONLADDER[] = "ol";
	const char KEY_WATERLEVEL[] = "wlvl";
	const char KEY_DUCK_STATE[] = "dst";
	const char KEY_HEALTH[] = "hp";
	const char KEY_ARMOR[] = "ap";
	const char KEY_CONSOLE_MESSAGES[] = "cmsg";
	const char KEY_COLLISIONS[] = "col";
	const char KEY_COLLISION_ENTITY[] = "ent";
	const char KEY_COLLISION_PLANE_NORMAL[] = "n";
	const char KEY_COLLISION_PLANE_DISTANCE[] = "d";
	const char KEY_COLLISION_IMPACT_VELOCITY[] = "ivel";
	const char KEY_IDUM[] = "idum";
	const char KEY_IY[] = "iy";
	const char KEY_IV[] = "iv";

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
