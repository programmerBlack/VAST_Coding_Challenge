#pragma once
#include "BaseEntity.h"
#include "Delegate.h"

enum class EMiningTruckState : size_t
{
	Idle,						// The truck is idle.
	MovingToMiningLocation,		// The truck is moving to a mining location.
	Mining,						// The truck is actively mining.
	MovingToUnloadingLocation,  // The truck is moving to unloading location.
	InUnloadingQueue,			// The truck is waiting in the Unloading Queue.
	TransitioningToUnload,		// The truck is moving to actually unload.
	Unloading                   // The truck is unloading Helium-3.
};

enum class EMiningTruckMovementTarget : size_t
{
	MiningLocation,				// The truck is moving to a Mining Location.
	UnloadingQueue,				// The truck is moving to the Unloading Queue.
	UnloadingLocation			// The truck is moving into the unloading station.
};

class MiningTruck : public BaseEntity
{
public:	
	MiningTruck() = default;
	~MiningTruck() override = default;

	virtual void Tick(float deltaTime);

	EMiningTruckState GetState() const;
	float GetRemainingMiningTime() const;
	float GetRemainingUnloadingTime() const;
	float GetTotalHeliumUnloaded() const;
	void SetState(EMiningTruckState newState);
	void SetMiningTruckSpeed(float miningTruckSpeed);
	float CalculateDistanceToFinalLocation() const;
	void CalculateMiningTruckSpeed(EMiningTruckMovementTarget movementTarget);
	bool MoveToLocation(BaseEntity* location, EMiningTruckMovementTarget movementTarget);
	void OnArrivedAtMiningLocation();
	void OnArrivedAtUnloadingQueue();
	void OnArrivedAtUnloadingLocation();
	void SetMiningAndUnloadingTimes(const MiningAndUnloadingTimes& miningAndUnloadinTimes);

	// Calculates a time between 1 and 5 hours, in seconds. Time will be used in combination with delta time (from tick) to countdown remaining mining time.
	void CalculateMiningTimer();

	// Calculates a time that is exactly 5 minuntes (in seconds).
	void CalculateUnloadTimer();

	// Callback Delegate instances.
	Delegate<> MoveCompleted;									// No args required for this callback delegate.
	Delegate<unsigned int> OnMoveToMiningLocationComplete;
	Delegate<unsigned int> OnMiningCompleted;
	Delegate<unsigned int> OnMoveToUnloadingQueueComplete;
	Delegate<unsigned int> OnMoveToUnloadingLocationComplete;
	Delegate<unsigned int> OnUnloadingCompleted;
	Delegate<unsigned int, float> OnUnloadHelium;

private:

	// Perform mining truck action of actually mining a location.
	void MineLocation(float deltaTime);

	// Perform mining truck action of unloading Helium-3.
	void UnloadHelium(float deltaTime);

	// Mining Truck State.
	EMiningTruckState State = EMiningTruckState::Idle;

	// Time values are measured in seconds.
	float MiningTimeLeft = 0.0f;
	float UnloadingTimeLeft = 0.0f;
	float MiningTruckSpeedMultiplier = 1.0f;
	float TotalHeliumUnloaded = 0.0f;

	// Calculated speed on the Mining Truck given the distance to the target location.
	float MiningTruckTravelSpeed = 1.0f;

	// Times to spend Mining.
	float MinMiningTimeHours = 1.0f;
	float MaxMiningTimeHours = 5.0f;

	float MinUnloadingTimeMinutes = 5.0f;
	float MaxUnloadingTimeMinutes = 5.0f;
};
