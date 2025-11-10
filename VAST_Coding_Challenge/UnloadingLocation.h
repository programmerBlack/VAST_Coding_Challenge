#pragma once

#include "BaseEntity.h"
#include "MiningTruck.h"

#include <queue>
#include <unordered_set>

enum class EUnloadingLocationState : size_t
{
	Idle, 	   // The unloading location is idle.
	Unloading  // The unloading location is actively unloading Helium-3 from a mining truck.
};

class UnloadingLocation : public BaseEntity
{
public:	
	
	UnloadingLocation() = default;
	~UnloadingLocation() override = default;

	virtual void Tick(float deltaTime);
	float GetQueueTime() const;
	void UnloadHelium(float deltaUnloadingTime);
	void AddMiningTruckToQueue(BaseEntity* miningTruck);
	EUnloadingLocationState GetState() const;
	double GetTotalUnloadingTime() const;

	void SetState(EUnloadingLocationState newState);
	void MiningTruckUnloadingFinished(unsigned int truckUniqueId);

	Delegate<unsigned int> OnRequestMiningTruckStartUnloading;

private:
	void ProcessQueue(float deltaTime);

	EUnloadingLocationState State = EUnloadingLocationState::Idle;

	// Queue of Trucks ready / waiting to unload.
	std::queue<unsigned int> miningTruckQueue;

	// Set of IDs of trucks in the queue to disallow duplicate trucks being added to the queue.
	std::unordered_set<unsigned int> trackedMiningTruckIds;
	unsigned int miningTruckUnloadingId = 0;
	float totalQueueTime = 0.0f;

	// Will accumulate the amount of time spent unloading for each mining truck that visits the station.
	double totalTimeUnloading = 0.0;
};
