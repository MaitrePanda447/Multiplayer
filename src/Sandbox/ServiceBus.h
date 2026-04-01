#pragma once
#include <comElem.h>
#include <cstdint>
#include <mutex>
#include <GCE/Core/Maths/Vector3.h>

struct ennemies
{
	uint32_t id;
	gce::Vector3f32 position;
	gce::Vector3f32 rotation;
	bool alive;
};

struct receivedData
{
	uint32_t id;
	gce::Vector3f32 position;
	gce::Vector3f32 rotation;
	bool alive;
};

struct WorldState
{
	std::vector<ennemies> ennemiesList;

	std::unordered_map<uint32_t, ennemies> ennemiesById;

	bool PlayerAlive;
	gce::Vector3f32 spawnPoint;
	bool hasSpawn = false;

	uint32_t playerID;
	bool hasYOU;
};

class ServiceBus
{
public:

	/*
		Render towards Networking
		We are sending player position/rotation to the networking service to broadcast it to other players
	*/

	commsElement playerState{};
	std::mutex playerStateMutex;
	std::condition_variable player_condition_variable;
	bool outDirty = false;
	std::mutex shootMutex;
	bool shootRequested = false;


	/* 
		We are receiving data from the networking service about other players and if we are dead
	 */

	std::mutex WorldStateMutex;
	WorldState worldState;
	std::condition_variable world_condition_variable;

};