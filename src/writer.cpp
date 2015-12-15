#include "taslogger/writer.hpp"

using namespace TASLogger;

LogWriter::LogWriter()
	: writer(stringBuffer)
{
}

void LogWriter::Clear()
{
	consolePrintQueue.clear();
	damageQueue.clear();
	objectMoveQueue.clear();
	collisionQueue.clear();
	stringBuffer.Clear();
	writer.Reset(stringBuffer);
}

void LogWriter::StartLog(const char *toolVer, int32_t buildNumber, const char *mod)
{
	Clear();

	writer.StartObject();

	writer.Key(KEY_TOOL_VERSION);
	writer.String(toolVer);

	writer.Key(KEY_BUILD_NUMBER);
	writer.Int(buildNumber);

	writer.Key(KEY_MOD);
	writer.String(mod);

	writer.Key(KEY_PHYSICS_FRAMES);
	writer.StartArray();
}

void LogWriter::EndLog()
{
	writer.EndArray();
	writer.EndObject();
}

void LogWriter::StartPhysicsFrame(double frameTime, int32_t clstate, bool paused, const char *cbuf)
{
	writer.StartObject();

	writer.Key(KEY_FRAMETIME);
	writer.Double(frameTime);

	if (clstate != 5) {
		writer.Key(KEY_CLIENT_STATE);
		writer.Int(clstate);
	}

	writer.Key(KEY_COMMAND_BUFFER);
	writer.String(cbuf);

	if (paused) {
		writer.Key(KEY_PAUSED);
		writer.Bool(paused);
	}

	writer.Key(KEY_COMMAND_FRAMES);
	writer.StartArray();
}

void LogWriter::EndPhysicsFrame()
{
	writer.EndArray();

	if (!damageQueue.empty()) {
		writer.Key(KEY_DAMAGES);
		writer.StartArray();
		while (!damageQueue.empty()) {
			const Damage &damage = damageQueue.front();
			writer.StartObject();

			writer.Key(KEY_DAMAGE_AMOUNT);
			writer.Double(damage.damage);

			writer.Key(KEY_DAMAGE_BITS);
			writer.Int(damage.damageBits);

			if (damage.direction[0] != 0.0 || damage.direction[1] != 0.0 || damage.direction[2] != 0.0) {
				writer.Key(KEY_DAMAGE_DIRECTION);
				writer.StartArray();
				writer.Double(damage.direction[0]);
				writer.Double(damage.direction[1]);
				writer.Double(damage.direction[2]);
				writer.EndArray();
			}

			writer.EndObject();
			damageQueue.pop_front();
		}
		writer.EndArray();
	}

	if (!objectMoveQueue.empty()) {
		writer.Key(KEY_OBJECT_BOOSTS);
		writer.StartArray();
		while (!objectMoveQueue.empty()) {
			const ObjectMove &objectMove = objectMoveQueue.front();
			writer.StartObject();

			if (!objectMove.pull) {
				writer.Key(KEY_IS_PULL);
				writer.Bool(objectMove.pull);
			}

			writer.Key(KEY_OBJECT_VELOCITY);
			writer.StartArray();
			writer.Double(objectMove.velocity[0]);
			writer.Double(objectMove.velocity[1]);
			writer.Double(objectMove.velocity[2]);
			writer.EndArray();

			writer.Key(KEY_OBJECT_POSITION);
			writer.StartArray();
			writer.Double(objectMove.position[0]);
			writer.Double(objectMove.position[1]);
			writer.Double(objectMove.position[2]);
			writer.EndArray();

			writer.EndObject();
			objectMoveQueue.pop_front();
		}
		writer.EndArray();
	}

	writer.EndObject();
}

void LogWriter::PushDamage(const Damage damage)
{
	damageQueue.push_back(damage);
}

void LogWriter::PushObjectMove(const ObjectMove objectMove)
{
	objectMoveQueue.push_back(objectMove);
}

void LogWriter::StartCmdFrame(uint32_t framebulkId, uint32_t msec, double remainder)
{
	writer.StartObject();

	writer.Key(KEY_MILLISECONDS);
	writer.Uint(msec);

	writer.Key(KEY_FRAMETIME_REMAINDER);
	writer.Double(remainder);

	writer.Key(KEY_FRAMEBULK_ID);
	writer.Uint(framebulkId);
}

void LogWriter::SetSharedSeed(uint32_t seed)
{
	writer.Key(KEY_SHARED_SEED);
	writer.Uint(seed);
}

void LogWriter::SetViewangles(double yaw, double pitch, double roll)
{
	writer.Key(KEY_VIEWANGLES);
	writer.StartArray();
	writer.Double(yaw);
	writer.Double(pitch);
	writer.Double(roll);
	writer.EndArray();
}

void LogWriter::SetPunchangles(double yaw, double pitch, double roll)
{
	if (yaw == 0.0 && pitch == 0.0 && roll == 0.0)
		return;
	writer.Key(KEY_PUNCHANGLES);
	writer.StartArray();
	writer.Double(yaw);
	writer.Double(pitch);
	writer.Double(roll);
	writer.EndArray();
}

void LogWriter::SetButtons(uint32_t buttons)
{
	writer.Key(KEY_BUTTONS);
	writer.Uint(buttons);
}

void LogWriter::SetImpulse(uint32_t impulse)
{
	if (impulse == 0.0)
		return;
	writer.Key(KEY_IMPULSE);
	writer.Uint(impulse);
}

void LogWriter::SetFSU(double F, double S, double U)
{
	writer.Key(KEY_FSU);
	writer.StartArray();
	writer.Double(F);
	writer.Double(S);
	writer.Double(U);
	writer.EndArray();
}

void LogWriter::SetEntFriction(double friction)
{
	if (friction == 1.0)
		return;
	writer.Key(KEY_ENT_FRICTION);
	writer.Double(friction);
}

void LogWriter::SetEntGravity(double gravity)
{
	if (gravity == 1.0)
		return;
	writer.Key(KEY_ENT_GRAVITY);
	writer.Double(gravity);
}

void LogWriter::PushConsolePrint(const char *message)
{
	consolePrintQueue.push_back(message);
}

void LogWriter::PushCollision(const Collision &collision)
{
	collisionQueue.push_back(collision);
}

void LogWriter::SetCollisions(const std::deque<Collision> collisions)
{
	collisionQueue = collisions;
}

void LogWriter::StartPrePlayer()
{
	writer.Key(KEY_PRE_PLAYERMOVE);
	writer.StartObject();
}

void LogWriter::EndPrePlayer()
{
	writer.EndObject();
}

void LogWriter::StartPostPlayer()
{
	writer.Key(KEY_POST_PLAYERMOVE);
	writer.StartObject();
}

void LogWriter::EndPostPlayer()
{
	writer.EndObject();
}

void LogWriter::SetPosition(const float position[3])
{
	writer.Key(KEY_POSITION);
	writer.StartArray();
	writer.Double(position[0]);
	writer.Double(position[1]);
	writer.Double(position[2]);
	writer.EndArray();
}

void LogWriter::SetVelocity(const float velocity[3])
{
	writer.Key(KEY_VELOCITY);
	writer.StartArray();
	writer.Double(velocity[0]);
	writer.Double(velocity[1]);
	writer.Double(velocity[2]);
	writer.EndArray();
}

void LogWriter::SetBaseVelocity(const float baseVelocity[3])
{
	if (baseVelocity[0] == 0.0 && baseVelocity[1] == 0.0 && baseVelocity[2] == 0.0)
		return;
	writer.Key(KEY_BASEVELOCITY);
	writer.StartArray();
	writer.Double(baseVelocity[0]);
	writer.Double(baseVelocity[1]);
	writer.Double(baseVelocity[2]);
	writer.EndArray();
}

void LogWriter::SetOnGround(bool onGround)
{
	writer.Key(KEY_ONGROUND);
	writer.Bool(onGround);
}

void LogWriter::SetOnLadder(bool onLadder)
{
	if (!onLadder)
		return;
	writer.Key(KEY_ONLADDER);
	writer.Bool(onLadder);
}

void LogWriter::SetWaterLevel(uint32_t waterLevel)
{
	if (waterLevel == 0.0)
		return;
	writer.Key(KEY_WATERLEVEL);
	writer.Uint(waterLevel);
}

void LogWriter::SetDuckState(DuckState duckState)
{
	if (duckState == UNDUCKED)
		return;
	writer.Key(KEY_DUCK_STATE);
	writer.Uint(duckState);
}

void LogWriter::SetHealth(double health)
{
	writer.Key(KEY_HEALTH);
	writer.Double(health);
}

void LogWriter::SetArmor(double armor)
{
	writer.Key(KEY_ARMOR);
	writer.Double(armor);
}

void LogWriter::EndCmdFrame()
{
	if (!consolePrintQueue.empty()) {
		writer.Key(KEY_CONSOLE_MESSAGES);
		writer.StartArray();
		while (!consolePrintQueue.empty()) {
			writer.String(consolePrintQueue.front().c_str());
			consolePrintQueue.pop_front();
		}
		writer.EndArray();
	}

	if (!collisionQueue.empty()) {
		writer.Key(KEY_COLLISIONS);
		writer.StartArray();
		while (!collisionQueue.empty()) {
			const Collision &collision = collisionQueue.front();
			writer.StartObject();

			writer.Key(KEY_COLLISION_ENTITY);
			writer.Int(collision.entity);

			writer.Key(KEY_COLLISION_PLANE_NORMAL);
			writer.StartArray();
			writer.Double(collision.normal[0]);
			writer.Double(collision.normal[1]);
			writer.Double(collision.normal[2]);
			writer.EndArray();

			writer.Key(KEY_COLLISION_PLANE_DISTANCE);
			writer.Double(collision.distance);

			writer.Key(KEY_COLLISION_IMPACT_VELOCITY);
			writer.StartArray();
			writer.Double(collision.impactVelocity[0]);
			writer.Double(collision.impactVelocity[1]);
			writer.Double(collision.impactVelocity[2]);
			writer.EndArray();

			writer.EndObject();
			collisionQueue.pop_front();
		}
		writer.EndArray();
	}

	writer.EndObject();
}
