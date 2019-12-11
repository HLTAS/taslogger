#include <unordered_map>
#include <functional>
#include <stdexcept>
#include "rapidjson/filereadstream.h"
#include "taslogger/reader.hpp"

using namespace TASLogger;

enum ParseState
{
	StateLog,
	StateToolVersion,
	StateBuildNumber,
	StateGameMod,

	StatePhysicsFrameList,
	StatePhysicsFrame,
	StateFrameTime,
	StateCommandBuffer,
	StatePaused,
	StateClientState,

	StateConsoleMessageList,
	StateConsoleMessage,

	StateDamageList,
	StateDamage,
	StateDamageAmount,
	StateDamageBits,
	StateDamageDirection,

	StateObjectMoveList,
	StateObjectMove,
	StateObjectPull,
	StateObjectVelocity,
	StateObjectPosition,

	StateCommandFrameList,
	StateCommandFrame,
	StateMilliseconds,
	StateFrameTimeRemainder,
	StateFramebulkId,
	StateSharedSeed,
	StateViewangles,
	StatePunchangles,
	StateButtons,
	StateImpulse,
	StateFSU,
	StateEntFriction,
	StateEntGravity,
	StateHealth,
	StateArmor,
	StatePrePlayerMove,
	StatePostPlayerMove,

	StateCollisionList,
	StateCollision,
	StateCollisionEntity,
	StateCollisionPlaneNormal,
	StateCollisionPlaneDistance,
	StateImpactVelocity,

	StateVelocity,
	StatePosition,
	StateBaseVelocity,
	StateOnGround,
	StateOnLadder,
	StateWaterLevel,
	StateDuckState
};

struct CharStringEqualTo
{
	bool operator()(const char *const &lhs, const char *const &rhs) const
	{
		return std::strcmp(lhs, rhs) == 0;
	}
};

struct CharStringHash
{
	// Source: djb2 from http://www.cse.yorku.ca/~oz/hash.html
	size_t operator()(const char *str) const
	{
		const unsigned char *s = reinterpret_cast<const unsigned char *>(str);
		size_t hash = 5381;
		while (size_t c = *s++)
			hash = ((hash << 5) + hash) + c;
		return hash;
	}
};

typedef std::unordered_map<
	const char *,
	ParseState,
	CharStringHash,
	CharStringEqualTo
> StateTableType;

class InternalHandler
{
public:
	InternalHandler();

	bool Null();
	bool Bool(bool b);
	bool Int(int i);
	bool Uint(unsigned i);
	bool Int64(int64_t i);
	bool Uint64(uint64_t i);
	bool Double(double d);
	bool RawNumber(const char *str, rapidjson::SizeType length, bool copy);
	bool String(const char *str, rapidjson::SizeType length, bool copy);
	bool StartObject();
	bool Key(const char *str, rapidjson::SizeType length, bool copy);
	bool EndObject(rapidjson::SizeType memberCount);
	bool StartArray();
	bool EndArray(rapidjson::SizeType elementCount);

	inline TASLog GetTASLog() const { return tasLog; }

private:
	TASLog tasLog;
	ParseState state;
	bool prePlayerMove;

	int arrayIndex;

	const StateTableType STATE_TABLE_LOG;
	const StateTableType STATE_TABLE_PHYSICS_FRAME;
	const StateTableType STATE_TABLE_DAMAGE;
	const StateTableType STATE_TABLE_OBJECT_MOVE;
	const StateTableType STATE_TABLE_COMMAND_FRAME;
	const StateTableType STATE_TABLE_COLLISION;
	const StateTableType STATE_TABLE_PLAYER;
};

InternalHandler::InternalHandler()
	: state(StateLog),

	STATE_TABLE_LOG({
		{KEY_TOOL_VERSION, StateToolVersion},
		{KEY_BUILD_NUMBER, StateBuildNumber},
		{KEY_MOD, StateGameMod},
		{KEY_PHYSICS_FRAMES, StatePhysicsFrameList}
	}),

	STATE_TABLE_PHYSICS_FRAME({
		{KEY_FRAMETIME, StateFrameTime},
		{KEY_COMMAND_BUFFER, StateCommandBuffer},
		{KEY_PAUSED, StatePaused},
		{KEY_CLIENT_STATE, StateClientState},
		{KEY_DAMAGES, StateDamageList},
		{KEY_OBJECT_BOOSTS, StateObjectMoveList},
		{KEY_COMMAND_FRAMES, StateCommandFrameList},
		{KEY_CONSOLE_MESSAGES, StateConsoleMessageList}
	}),

	STATE_TABLE_DAMAGE({
		{KEY_DAMAGE_AMOUNT, StateDamageAmount},
		{KEY_DAMAGE_BITS, StateDamageBits},
		{KEY_DAMAGE_DIRECTION, StateDamageDirection}
	}),

	STATE_TABLE_OBJECT_MOVE({
		{KEY_IS_PULL, StateObjectPull},
		{KEY_OBJECT_VELOCITY, StateObjectVelocity},
		{KEY_OBJECT_POSITION, StateObjectPosition}
	}),

	STATE_TABLE_COMMAND_FRAME({
		{KEY_MILLISECONDS, StateMilliseconds},
		{KEY_FRAMETIME_REMAINDER, StateFrameTimeRemainder},
		{KEY_FRAMEBULK_ID, StateFramebulkId},
		{KEY_SHARED_SEED, StateSharedSeed},
		{KEY_VIEWANGLES, StateViewangles},
		{KEY_PUNCHANGLES, StatePunchangles},
		{KEY_BUTTONS, StateButtons},
		{KEY_IMPULSE, StateImpulse},
		{KEY_FSU, StateFSU},
		{KEY_ENT_FRICTION, StateEntFriction},
		{KEY_ENT_GRAVITY, StateEntGravity},
		{KEY_HEALTH, StateHealth},
		{KEY_ARMOR, StateArmor},
		{KEY_PRE_PLAYERMOVE, StatePrePlayerMove},
		{KEY_POST_PLAYERMOVE, StatePostPlayerMove},
		{KEY_COLLISIONS, StateCollisionList}
	}),

	STATE_TABLE_COLLISION({
		{KEY_COLLISION_ENTITY, StateCollisionEntity},
		{KEY_COLLISION_PLANE_NORMAL, StateCollisionPlaneNormal},
		{KEY_COLLISION_PLANE_DISTANCE, StateCollisionPlaneDistance},
		{KEY_COLLISION_IMPACT_VELOCITY, StateImpactVelocity}
	}),
		
	STATE_TABLE_PLAYER({
		{KEY_VELOCITY, StateVelocity},
		{KEY_POSITION, StatePosition},
		{KEY_BASEVELOCITY, StateBaseVelocity},
		{KEY_ONGROUND, StateOnGround},
		{KEY_ONLADDER, StateOnLadder},
		{KEY_WATERLEVEL, StateWaterLevel},
		{KEY_DUCK_STATE, StateDuckState}
	})
{
}

bool InternalHandler::Null()
{
	return false;
}

bool InternalHandler::Bool(bool b)
{
	switch (state) {
	case StatePaused:
		tasLog.physicsFrameList.back().paused = b;
		state = StatePhysicsFrame;
		break;
	case StateObjectPull:
		tasLog.physicsFrameList.back().objectMoveList.back().pull = b;
		state = StateObjectMove;
		break;
	case StateOnGround: {
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		(prePlayerMove ? frame.prePMState : frame.postPMState).onGround = b;
		state = prePlayerMove ? StatePrePlayerMove : StatePostPlayerMove;
		break;
	}
	case StateOnLadder: {
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		(prePlayerMove ? frame.prePMState : frame.postPMState).onLadder = b;
		state = prePlayerMove ? StatePrePlayerMove : StatePostPlayerMove;
		break;
	}
	default:
		return false;
	}

	return true;
}

bool InternalHandler::Int(int)
{
	return false;
}

bool InternalHandler::Uint(unsigned i)
{
	switch (state) {
	case StateBuildNumber:
		tasLog.buildNumber = static_cast<int32_t>(i);
		state = StateLog;
		break;
	case StateClientState:
		tasLog.physicsFrameList.back().clientState = static_cast<int8_t>(i);
		state = StatePhysicsFrame;
		break;
	case StateDamageBits:
		tasLog.physicsFrameList.back().damageList.back().damageBits = static_cast<int32_t>(i);
		state = StateDamage;
		break;
	case StateMilliseconds:
		tasLog.physicsFrameList.back().commandFrameList.back().msec = static_cast<uint8_t>(i);
		state = StateCommandFrame;
		break;
	case StateFramebulkId:
		tasLog.physicsFrameList.back().commandFrameList.back().framebulkId = i;
		state = StateCommandFrame;
		break;
	case StateSharedSeed:
		tasLog.physicsFrameList.back().commandFrameList.back().sharedSeed = i;
		state = StateCommandFrame;
		break;
	case StateImpulse:
		tasLog.physicsFrameList.back().commandFrameList.back().impulse = static_cast<uint8_t>(i);
		state = StateCommandFrame;
		break;
	case StateButtons:
		tasLog.physicsFrameList.back().commandFrameList.back().buttons = static_cast<uint8_t>(i);
		state = StateCommandFrame;
		break;
	case StateWaterLevel: {
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		(prePlayerMove ? frame.prePMState : frame.postPMState).waterLevel = static_cast<uint8_t>(i);
		state = prePlayerMove ? StatePrePlayerMove : StatePostPlayerMove;
		break;
	}
	case StateDuckState: {
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		(prePlayerMove ? frame.prePMState : frame.postPMState).duckState = static_cast<uint8_t>(i);
		state = prePlayerMove ? StatePrePlayerMove : StatePostPlayerMove;
		break;
	}
	case StateCollisionEntity:
		tasLog.physicsFrameList.back().commandFrameList.back().collisionList.back()
			.entity = static_cast<int8_t>(i);
		state = StateCollision;
		break;
	default:
		return false;
	}

	return true;
}

bool InternalHandler::Int64(int64_t)
{
	return false;
}

bool InternalHandler::Uint64(uint64_t)
{
	return false;
}

bool InternalHandler::Double(double d)
{
	switch (state) {
	case StateFrameTime:
		tasLog.physicsFrameList.back().frameTime = static_cast<float>(d);
		state = StatePhysicsFrame;
		break;
	case StateDamageAmount:
		tasLog.physicsFrameList.back().damageList.back().damage = static_cast<float>(d);
		state = StateDamage;
		break;
	case StateDamageDirection:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().damageList.back()
			.direction[arrayIndex++] = static_cast<float>(d);
		break;
	case StateObjectVelocity:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().objectMoveList.back()
			.velocity[arrayIndex++] = static_cast<float>(d);
		break;
	case StateObjectPosition:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().objectMoveList.back()
			.position[arrayIndex++] = static_cast<float>(d);
		break;
	case StateFrameTimeRemainder:
		tasLog.physicsFrameList.back().commandFrameList.back()
			.frameTimeRemainder = static_cast<float>(d);
		state = StateCommandFrame;
		break;
	case StateViewangles:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().commandFrameList.back()
			.viewangles[arrayIndex++] = static_cast<float>(d);
		break;
	case StatePunchangles:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().commandFrameList.back()
			.punchangles[arrayIndex++] = static_cast<float>(d);
		break;
	case StateFSU:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().commandFrameList.back()
			.FSU[arrayIndex++] = static_cast<float>(d);
		break;
	case StateEntFriction:
		tasLog.physicsFrameList.back().commandFrameList.back().entFriction = static_cast<float>(d);
		state = StateCommandFrame;
		break;
	case StateEntGravity:
		tasLog.physicsFrameList.back().commandFrameList.back().entGravity = static_cast<float>(d);
		state = StateCommandFrame;
		break;
	case StateHealth:
		tasLog.physicsFrameList.back().commandFrameList.back().health = static_cast<float>(d);
		state = StateCommandFrame;
		break;
	case StateArmor:
		tasLog.physicsFrameList.back().commandFrameList.back().armor = static_cast<float>(d);
		state = StateCommandFrame;
		break;
	case StateVelocity: {
		if (arrayIndex >= 3)
			return false;
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		(prePlayerMove ? frame.prePMState : frame.postPMState)
			.velocity[arrayIndex++] = static_cast<float>(d);
		break;
	}
	case StatePosition: {
		if (arrayIndex >= 3)
			return false;
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		(prePlayerMove ? frame.prePMState : frame.postPMState)
			.position[arrayIndex++] = static_cast<float>(d);
		break;
	}
	case StateBaseVelocity: {
		if (arrayIndex >= 3)
			return false;
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		(prePlayerMove ? frame.prePMState : frame.postPMState)
			.baseVelocity[arrayIndex++] = static_cast<float>(d);
		break;
	}
	case StateCollisionPlaneNormal:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().commandFrameList.back().collisionList.back()
			.normal[arrayIndex++] = static_cast<float>(d);
		break;
	case StateCollisionPlaneDistance:
		tasLog.physicsFrameList.back().commandFrameList.back().collisionList.back()
			.distance = static_cast<float>(d);
		state = StateCollision;
		break;
	case StateImpactVelocity:
		if (arrayIndex >= 3)
			return false;
		tasLog.physicsFrameList.back().commandFrameList.back().collisionList.back()
			.impactVelocity[arrayIndex++] = static_cast<float>(d);
		break;
	default:
		return false;
	}

	return true;
}

bool InternalHandler::RawNumber(const char *str, rapidjson::SizeType length, bool copy)
{
	return false;
}

bool InternalHandler::String(const char *str, rapidjson::SizeType length, bool)
{
	switch (state) {
	case StateToolVersion:
		tasLog.toolVersion = std::string(str, length);
		state = StateLog;
		break;
	case StateGameMod:
		tasLog.gameMod = std::string(str, length);
		state = StateLog;
		break;
	case StateCommandBuffer:
		tasLog.physicsFrameList.back().commandBuffer = std::string(str, length);
		state = StatePhysicsFrame;
		break;
	case StateConsoleMessageList:
		tasLog.physicsFrameList.back().consolePrintList.push_back(std::string(str, length));
		break;
	default:
		return false;
	}

	return true;
}

bool InternalHandler::StartObject()
{
	switch (state) {
	case StateLog:
		break;
	case StatePhysicsFrameList:
		state = StatePhysicsFrame;
		tasLog.physicsFrameList.push_back(ReaderPhysicsFrame());
		tasLog.physicsFrameList.back().paused = false;
		tasLog.physicsFrameList.back().clientState = 5;
		break;
	case StateDamageList: {
		state = StateDamage;
		std::vector<ReaderDamage> &damageList = tasLog.physicsFrameList.back().damageList;
		damageList.push_back(ReaderDamage());
		damageList.back().direction[0] = 0;
		damageList.back().direction[1] = 0;
		damageList.back().direction[2] = 0;
		break;
	}
	case StateObjectMoveList: {
		state = StateObjectMove;
		std::vector<ReaderObjectMove> &objectMoveList = tasLog.physicsFrameList.back()
			.objectMoveList;
		objectMoveList.push_back(ReaderObjectMove());
		objectMoveList.back().pull = true;
		break;
	}
	case StateCommandFrameList: {
		state = StateCommandFrame;
		std::vector<ReaderCommandFrame> &commandFrameList = tasLog.physicsFrameList.back()
			.commandFrameList;
		commandFrameList.push_back(ReaderCommandFrame());
		ReaderCommandFrame &frame = commandFrameList.back();
		frame.punchangles[0] = 0;
		frame.punchangles[1] = 0;
		frame.punchangles[2] = 0;
		frame.impulse = 0;
		frame.entFriction = 1;
		frame.entGravity = 1;
		break;
	}
	case StatePrePlayerMove: {
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		frame.prePMState.baseVelocity[0] = 0;
		frame.prePMState.baseVelocity[1] = 0;
		frame.prePMState.baseVelocity[2] = 0;
		frame.prePMState.onLadder = false;
		frame.prePMState.waterLevel = 0;
		frame.prePMState.duckState = static_cast<uint8_t>(UNDUCKED);
		break;
	}
	case StatePostPlayerMove: {
		ReaderCommandFrame &frame = tasLog.physicsFrameList.back().commandFrameList.back();
		frame.postPMState.baseVelocity[0] = 0;
		frame.postPMState.baseVelocity[1] = 0;
		frame.postPMState.baseVelocity[2] = 0;
		frame.postPMState.onLadder = false;
		frame.postPMState.waterLevel = 0;
		frame.postPMState.duckState = static_cast<uint8_t>(UNDUCKED);
		break;
	}
	case StateCollisionList: {
		state = StateCollision;
		tasLog.physicsFrameList.back().commandFrameList.back()
			.collisionList.push_back(ReaderCollision());
		break;
	}
	default:
		return false;
	}

	return true;
}

bool InternalHandler::Key(const char *str, rapidjson::SizeType, bool)
{
	try {
		switch (state) {
		case StateLog:
			state = STATE_TABLE_LOG.at(str);
			break;
		case StatePhysicsFrame:
			state = STATE_TABLE_PHYSICS_FRAME.at(str);
			break;
		case StateCommandFrame:
			state = STATE_TABLE_COMMAND_FRAME.at(str);
			break;
		case StatePrePlayerMove:
			state = STATE_TABLE_PLAYER.at(str);
			prePlayerMove = true;
			break;
		case StatePostPlayerMove:
			state = STATE_TABLE_PLAYER.at(str);
			prePlayerMove = false;
			break;
		case StateDamage:
			state = STATE_TABLE_DAMAGE.at(str);
			break;
		case StateObjectMove:
			state = STATE_TABLE_OBJECT_MOVE.at(str);
			break;
		case StateCollision:
			state = STATE_TABLE_COLLISION.at(str);
			break;
		default:
			return false;
		}
	} catch (std::out_of_range) {
		return false;
	}

	return true;
}

bool InternalHandler::EndObject(rapidjson::SizeType)
{
	switch (state) {
	case StateLog:
		break;
	case StatePhysicsFrame:
		state = StatePhysicsFrameList;
		break;
	case StateDamage:
		state = StateDamageList;
		break;
	case StateObjectMove:
		state = StateObjectMoveList;
		break;
	case StateCommandFrame:
		state = StateCommandFrameList;
		break;
	case StatePrePlayerMove:
		state = StateCommandFrame;
		break;
	case StatePostPlayerMove:
		state = StateCommandFrame;
		break;
	case StateCollision:
		state = StateCollisionList;
		break;
	default:
		return false;
	}

	return true;
}

bool InternalHandler::StartArray()
{
	switch (state) {
	case StatePhysicsFrameList:
		tasLog.physicsFrameList.reserve(10000);
		break;
	case StateDamageList:
		break;
	case StateDamageDirection:
		arrayIndex = 0;
		break;
	case StateObjectMoveList:
		break;
	case StateObjectVelocity:
		arrayIndex = 0;
		break;
	case StateObjectPosition:
		arrayIndex = 0;
		break;
	case StateCommandFrameList:
		break;
	case StateViewangles:
		arrayIndex = 0;
		break;
	case StatePunchangles:
		arrayIndex = 0;
		break;
	case StateFSU:
		arrayIndex = 0;
		break;
	case StateVelocity:
		arrayIndex = 0;
		break;
	case StatePosition:
		arrayIndex = 0;
		break;
	case StateBaseVelocity:
		arrayIndex = 0;
		break;
	case StateConsoleMessageList:
		break;
	case StateCollisionList:
		break;
	case StateCollisionPlaneNormal:
		arrayIndex = 0;
		break;
	case StateImpactVelocity:
		arrayIndex = 0;
		break;
	default:
		return false;
	}

	return true;
}

bool InternalHandler::EndArray(rapidjson::SizeType)
{
	switch (state) {
	case StatePhysicsFrameList:
		state = StateLog;
		break;
	case StateDamageList:
		state = StatePhysicsFrame;
		break;
	case StateDamageDirection:
		state = StateDamage;
		break;
	case StateObjectMoveList:
		state = StatePhysicsFrame;
		break;
	case StateObjectVelocity:
		state = StateObjectMove;
		break;
	case StateObjectPosition:
		state = StateObjectMove;
		break;
	case StateCommandFrameList:
		state = StatePhysicsFrame;
		break;
	case StateViewangles:
		state = StateCommandFrame;
		break;
	case StatePunchangles:
		state = StateCommandFrame;
		break;
	case StateFSU:
		state = StateCommandFrame;
		break;
	case StateVelocity:
		state = prePlayerMove ? StatePrePlayerMove : StatePostPlayerMove;
		break;
	case StatePosition:
		state = prePlayerMove ? StatePrePlayerMove : StatePostPlayerMove;
		break;
	case StateBaseVelocity:
		state = prePlayerMove ? StatePrePlayerMove : StatePostPlayerMove;
		break;
	case StateConsoleMessageList:
		state = StatePhysicsFrame;
		break;
	case StateCollisionList:
		state = StateCommandFrame;
		break;
	case StateCollisionPlaneNormal:
		state = StateCollision;
		break;
	case StateImpactVelocity:
		state = StateCollision;
		break;
	default:
		return false;
	}

	return true;
}

rapidjson::ParseResult TASLogger::ParseFile(FILE *file, TASLog &tasLog)
{
	char buf[65536];
	rapidjson::FileReadStream fs(file, buf, sizeof(buf));
	InternalHandler internalHandler;
	rapidjson::Reader reader;
	rapidjson::ParseResult res = reader.Parse(fs, internalHandler);
	tasLog = internalHandler.GetTASLog();
	return res;
}
