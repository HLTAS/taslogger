#include "taslogger.hpp"

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

void LogWriter::StartLog(const char *toolVer, int buildNumber, const char *mod)
{
	Clear();

	writer.StartObject();

	writer.Key("tool_ver");
	writer.String(toolVer);

	writer.Key("build");
	writer.Int(buildNumber);

	writer.Key("mod");
	writer.String(mod);

	writer.Key("pf");
	writer.StartArray();
}

void LogWriter::EndLog()
{
	writer.EndArray();
	writer.EndObject();
}

void LogWriter::StartPhysicsFrame(double frameTime, bool paused, const char *cbuf)
{
	writer.StartObject();

	writer.Key("ft");
	writer.Double(frameTime);

	writer.Key("cbuf");
	writer.String(cbuf);

	if (paused) {
		writer.Key("p");
		writer.Bool(paused);
	}

	writer.Key("cf");
	writer.StartArray();
}

void LogWriter::EndPhysicsFrame()
{
	if (!damageQueue.empty()) {
		writer.Key("dmg");
		writer.StartArray();
		while (!damageQueue.empty()) {
			const Damage &damage = damageQueue.front();
			writer.StartObject();

			writer.Key("dmg");
			writer.Double(damage.damage);

			writer.Key("bits");
			writer.Int(damage.damageBits);

			if (damage.direction[0] != 0.0 || damage.direction[1] != 0.0 || damage.direction[2] != 0.0) {
				writer.Key("dir");
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
		writer.Key("obj");
		writer.StartArray();
		while (!objectMoveQueue.empty()) {
			const ObjectMove &objectMove = objectMoveQueue.front();
			writer.StartObject();

			if (!objectMove.pull) {
				writer.Key("pull");
				writer.Bool(objectMove.pull);
			}

			writer.Key("vel");
			writer.StartArray();
			writer.Double(objectMove.velocity[0]);
			writer.Double(objectMove.velocity[1]);
			writer.Double(objectMove.velocity[2]);
			writer.EndArray();

			writer.Key("pos");
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

	writer.EndArray();
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

	writer.Key("ms");
	writer.Uint(msec);

	writer.Key("rem");
	writer.Double(remainder);

	writer.Key("bid");
	writer.Uint(framebulkId);
}

void LogWriter::SetNonsharedSeed(uint32_t seed)
{
	writer.Key("nss");
	writer.Uint(seed);
}

void LogWriter::SetViewangles(double yaw, double pitch, double roll)
{
	writer.Key("view");
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
	writer.Key("pview");
	writer.StartArray();
	writer.Double(yaw);
	writer.Double(pitch);
	writer.Double(roll);
	writer.EndArray();
}

void LogWriter::SetButtons(uint32_t buttons)
{
	writer.Key("btns");
	writer.Uint(buttons);
}

void LogWriter::SetImpulse(uint32_t impulse)
{
	if (impulse == 0.0)
		return;
	writer.Key("impls");
	writer.Uint(impulse);
}

void LogWriter::SetFSU(double F, double S, double U)
{
	writer.Key("fsu");
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
	writer.Key("efric");
	writer.Double(friction);
}

void LogWriter::SetEntGravity(double gravity)
{
	if (gravity == 1.0)
		return;
	writer.Key("egrav");
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
	writer.Key("prepm");
	writer.StartObject();
}

void LogWriter::EndPrePlayer()
{
	writer.EndObject();
}

void LogWriter::StartPostPlayer()
{
	writer.Key("postpm");
	writer.StartObject();
}

void LogWriter::EndPostPlayer()
{
	writer.EndObject();
}

void LogWriter::SetPosition(const float position[3])
{
	writer.Key("pos");
	writer.StartArray();
	writer.Double(position[0]);
	writer.Double(position[1]);
	writer.Double(position[2]);
	writer.EndArray();
}

void LogWriter::SetVelocity(const float velocity[3])
{
	writer.Key("vel");
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
	writer.Key("bvel");
	writer.StartArray();
	writer.Double(baseVelocity[0]);
	writer.Double(baseVelocity[1]);
	writer.Double(baseVelocity[2]);
	writer.EndArray();
}

void LogWriter::SetOnGround(bool onGround)
{
	writer.Key("og");
	writer.Bool(onGround);
}

void LogWriter::SetOnLadder(bool onLadder)
{
	if (!onLadder)
		return;
	writer.Key("ol");
	writer.Bool(onLadder);
}

void LogWriter::SetWaterLevel(uint32_t waterLevel)
{
	if (waterLevel == 0.0)
		return;
	writer.Key("wlvl");
	writer.Uint(waterLevel);
}

void LogWriter::SetDuckState(DuckState duckState)
{
	if (duckState == UNDUCKED)
		return;
	writer.Key("dst");
	writer.Uint(duckState);
}

void LogWriter::SetHealth(double health)
{
	writer.Key("hp");
	writer.Double(health);
}

void LogWriter::SetArmor(double armor)
{
	writer.Key("ap");
	writer.Double(armor);
}

void LogWriter::EndCmdFrame()
{
	if (!consolePrintQueue.empty()) {
		writer.Key("cmsg");
		writer.StartArray();
		while (!consolePrintQueue.empty()) {
			writer.String(consolePrintQueue.front().c_str());
			consolePrintQueue.pop_front();
		}
		writer.EndArray();
	}

	if (!collisionQueue.empty()) {
		writer.Key("col");
		writer.StartArray();
		while (!collisionQueue.empty()) {
			const Collision &collision = collisionQueue.front();
			writer.StartObject();

			writer.Key("ent");
			writer.Int(collision.entity);

			writer.Key("n");
			writer.StartArray();
			writer.Double(collision.normal[0]);
			writer.Double(collision.normal[1]);
			writer.Double(collision.normal[2]);
			writer.EndArray();

			writer.Key("d");
			writer.Double(collision.distance);

			writer.Key("ivel");
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
