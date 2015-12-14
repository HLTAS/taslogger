#ifndef TASLOGGER_HPP
#define TASLOGGER_HPP

#include <cinttypes>
#include <deque>
#include <string>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

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

	class LogWriter
	{
	public:
		LogWriter();

		void StartLog(const char *toolVer, int buildNumber, const char *mod);
		void EndLog();

		void StartPhysicsFrame(double frameTime, bool paused, const char *cbuf);
		void EndPhysicsFrame();

		void PushDamage(const Damage damage);
		void PushObjectMove(const ObjectMove objectMove);

		void StartCmdFrame(uint32_t framebulkId, uint32_t msec, double remainder);
		void EndCmdFrame();

		void SetNonsharedSeed(uint32_t seed);
		void SetViewangles(double yaw, double pitch, double roll);
		void SetPunchangles(double yaw, double pitch, double roll);
		void SetButtons(uint32_t buttons);
		void SetImpulse(uint32_t impulse);
		void SetFSU(double F, double S, double U);
		void SetEntFriction(double friction);
		void SetEntGravity(double gravity);
		void SetHealth(double health);
		void SetArmor(double armor);

		void PushConsolePrint(const char *message);
		void PushCollision(const Collision &collision);
		void SetCollisions(const std::deque<Collision> collisions);

		void StartPrePlayer();
		void EndPrePlayer();
		void StartPostPlayer();
		void EndPostPlayer();
		void SetPosition(const float position[3]);
		void SetVelocity(const float velocity[3]);
		void SetBaseVelocity(const float baseVelocity[3]);
		void SetOnGround(bool onGround);
		void SetOnLadder(bool onLadder);
		void SetWaterLevel(uint32_t waterLevel);
		void SetDuckState(DuckState duckState);

		void Clear();

		inline const char *GetString() const
		{
			return stringBuffer.GetString();
		}

	private:
		rapidjson::StringBuffer stringBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer;

		std::deque<std::string> consolePrintQueue;
		std::deque<Damage> damageQueue;
		std::deque<Collision> collisionQueue;
		std::deque<ObjectMove> objectMoveQueue;
	};
}

#endif
