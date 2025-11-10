#include "UnloadingLocation.h"

/*
* Mining Trucks are added to a Queue (std::queue) when they are done mining.
* If the current state of the Unloading location is idle (no truck is unloading there), pop a truck Id out of the top of the queue and store the Id.
* This store truck id will be used to request the Mining Controller to request the truck to start unloading at this unloading location.
*/
void UnloadingLocation::ProcessQueue(float deltaTime)
{
	// Ensure no truck is currently unloading.
	if (State == EUnloadingLocationState::Idle && miningTruckUnloadingId == 0)
	{
		if (miningTruckQueue.size() == 0)
		{
			return;
		}

		// Access the next truck ready to unload.
		miningTruckUnloadingId = miningTruckQueue.front();
		if (miningTruckUnloadingId)
		{
			miningTruckQueue.pop();
			OnRequestMiningTruckStartUnloading.ExecuteIfBound(miningTruckUnloadingId);
		}
	}
}

/*
* Update function to process the Unloading Truck Queue.
* The Unloading truck queue is a list of numbers that are Id's to the trucks waiting in queue (in-order).
*/
void UnloadingLocation::Tick(float DeltaTime)
{
	ProcessQueue(DeltaTime);
}

/*
* Return the total Queue Time. It's calculated as the time remaining to unload the currently active Mining truck, plus the calculated unload times of all the trucks in the queue.
*/
float UnloadingLocation::GetQueueTime() const
{
	return totalQueueTime;
}

/*
* Unloads some Helium. The amount is determined by deltaUnloadingTime. This simulation is assuming Time = Helium. Therefore 1 second = 1 Helium.
*/
void UnloadingLocation::UnloadHelium(float deltaUnloadingTime)
{
	// Update how much time has been spend unloading.
	totalTimeUnloading += deltaUnloadingTime;

	totalQueueTime -= deltaUnloadingTime;
	if (totalQueueTime <= 0.0f)
	{
		totalQueueTime = 0.0f;
	}
}

/*
* Adds a truck to the Unloading Queue.
* Trucks are added to a std::queue after being checked against a std::unordered_set to prevent re-adding the same truck to the queue more than once.
* This allows us to check errors and add logging since a truck can only be in a single queue, and only once at a time.
*/
void UnloadingLocation::AddMiningTruckToQueue(BaseEntity* miningTruckPtr)
{
	// Cannot add invalid truck.
	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckPtr);
	if (!miningTruck)
	{
		return;
	}

	unsigned int miningTruckID = miningTruck->GetUniqueId();

	// Check if Truck is already in queue.
	if (trackedMiningTruckIds.find(miningTruckID) != trackedMiningTruckIds.end())
	{
		return;
	}

	miningTruckQueue.emplace(miningTruckID);
	trackedMiningTruckIds.emplace(miningTruckID);
	totalQueueTime += miningTruck->GetRemainingUnloadingTime();
}

/*
* Return the current state of the Unloading station.
*/
EUnloadingLocationState UnloadingLocation::GetState() const
{
	return State;
}

/*
* Get the amount of time the station has spend unloading since the start of the simulation.
* This value is used in calculating the efficiency of the Unloading Station.
*/
double UnloadingLocation::GetTotalUnloadingTime() const
{
	return totalTimeUnloading;
}

/*
* Allows you to change the state of an Unloading station.
* Validates that the state being changed to is different before setting the value.
*/
void UnloadingLocation::SetState(EUnloadingLocationState newState)
{
	if (State != newState)
	{
		State = newState;
	}
}

/*
* As Mining trucks complete their unloading cylce, this function is called.
* A check is made to ensure that the truck completing it's unloading is the same as the expected truck Id.
* The Id is reset to 0 so a new truck can be processed from the queue, and the old Id is removed from the unordered_set to allow the finishing truck to re-visit this unloading station.
*/
void UnloadingLocation::MiningTruckUnloadingFinished(unsigned int truckUniqueId)
{
	if (truckUniqueId != miningTruckUnloadingId)
	{
		return;
	}
	
	// Unloading has finished. Clear the active unloading truck so the next one can start unloading.
	miningTruckUnloadingId = 0;
	trackedMiningTruckIds.erase(truckUniqueId);
	if (trackedMiningTruckIds.empty())
	{
		// If we are not waiting for any trucks to unload, then we can set the queue time back to 0.
		totalQueueTime = 0.0f;
	}
}