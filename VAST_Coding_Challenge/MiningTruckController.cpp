#include "MiningTruckController.h"

#include "MiningTruck.h"
#include "MiningLocation.h"
#include "UnloadingLocation.h"

#include <cmath>

constexpr double PI = 3.141592653589793;

/*
* Primary Tick (update) function.
* Ticks the Simulation Timer, and all Entities performing actions in the Simulation.
*/
bool MiningTruckController::Tick(float deltaTime)
{
	// Scale the change in time by the Global Time Dilation value.
	deltaTime *= SimulationTimer.GetGlobalTimeDilation();

	// Tick the Simulation Timer.
	bool exit = SimulationTimer.Tick(deltaTime);
	if (exit)
	{
		// Simulation complete.
		return true;
	}

	// Tick (update) every truck.
	for (auto iterator : MiningTruckRegistry)
	{
		MiningTruck* miningTruckPtr = reinterpret_cast<MiningTruck*>(iterator.second);
		if (miningTruckPtr)
		{
			miningTruckPtr->Tick(deltaTime);
		}
	}

	// Mining Locations don't need to tick. They only have state changes.

	// Tick (update) every unloading location.
	for (auto iterator : UnloadingLocationRegistry)
	{
		UnloadingLocation* unloadingLocationPtr = reinterpret_cast<UnloadingLocation*>(iterator.second);
		if (unloadingLocationPtr)
		{
			unloadingLocationPtr->Tick(deltaTime);
		}
	}

	// Return false. As returning true will exit the simulation.
	return false;
}

/*
* Clean up function called when the Simulation is complete.
* Destroys all Entities that were peforming Actions and were allocated dynamically, then clears any Maps that their pointers were housed in.
*/
OperationEfficiency MiningTruckController::Teardown()
{
	OperationEfficiency efficiency = GetOperationEfficiency();
	DestroyAllEntities();
	
	// Since all entities have been destroyed at this point, clear all state related maps.
	MoveToMiningLocationPending.clear();
	ActiveMiningTrucks.clear();
	MovingToUnloadingLocationPending.clear();
	MiningTrucksPendingUnload.clear();
	MiningTrucksTransitioningToUnload.clear();
	ActiveUnloadingTrucks.clear();

	return efficiency;
}

/*
* Calculates the Efficiency of Each Mining Truck, Unloading Location, and Globally then returns the Report to the caller.
*/
OperationEfficiency MiningTruckController::GetOperationEfficiency() const
{
	float elapsedSimulationTime = SimulationTimer.GetElapsedSimulationTime();
	OperationEfficiency efficiency;
	efficiency.GlobalEfficiency = GetMiningEfficiency();

	for (auto iterator : MiningTruckRegistry)
	{
		MiningTruck* miningTruckPtr = reinterpret_cast<MiningTruck*>(iterator.second);
		if (miningTruckPtr)
		{
			efficiency.PerTruckEfficiency.push_back(miningTruckPtr->GetTotalHeliumUnloaded() / elapsedSimulationTime);
		}
	}

	for (auto iterator : UnloadingLocationRegistry)
	{
		UnloadingLocation* unloadingLocationPtr = reinterpret_cast<UnloadingLocation*>(iterator.second);
		if (unloadingLocationPtr)
		{
			efficiency.PerUnloadingLocationEfficiency.push_back(unloadingLocationPtr->GetTotalUnloadingTime() / elapsedSimulationTime);
		}
	}

	return efficiency;
}

/*
* Returns the speed adjustment applied to the Mining Trucks.
*/
float MiningTruckController::GetMiningTruckSpeed()
{
	return MiningTruckSpeed;
}

/*
* Increases the Simulation speed by Dilating it's accumulation of time.
*/
void MiningTruckController::IncreasePlaybackSpeed()
{
	SimulationPlaybackSpeed += 1.0f;
	if (SimulationPlaybackSpeed > 5.0f) // 5x is the max simulation speed.
	{
		SimulationPlaybackSpeed = 5.0f;
	}
	SimulationTimer.SetGlobalTimeDialation(SimulationPlaybackSpeed);
}

/*
* Decreases the Simulation speed by contracting it's accumulation of time.
*/
void MiningTruckController::DecreasePlaybackSpeed()
{
	SimulationPlaybackSpeed -= 1.0f;
	if (SimulationPlaybackSpeed < 1.0f) // 72 hours is a long time, no need to go even slower.
	{
		SimulationPlaybackSpeed = 1.0f;
	}
	SimulationTimer.SetGlobalTimeDialation(SimulationPlaybackSpeed);
}

/*
* Increases the speed the Mining Trucks move at.
*/
void MiningTruckController::IncreaseTruckSpeed()
{
	MiningTruckSpeed += 1.0f;

	for (auto iterator : MiningTruckRegistry)
	{
		MiningTruck* miningTruckPtr = reinterpret_cast<MiningTruck*>(iterator.second);
		if (miningTruckPtr)
		{
			miningTruckPtr->SetMiningTruckSpeed(MiningTruckSpeed);
		}
	}
}

/*
* Decreases the speed the Mining Trucks move at.
*/
void MiningTruckController::DecreaseTruckSpeed()
{
	MiningTruckSpeed -= 1.0f;
	if (MiningTruckSpeed < 0.5f)
	{
		MiningTruckSpeed = 0.5f;
	}

	for (auto iterator : MiningTruckRegistry)
	{
		MiningTruck* miningTruckPtr = reinterpret_cast<MiningTruck*>(iterator.second);
		if (miningTruckPtr)
		{
			miningTruckPtr->SetMiningTruckSpeed(MiningTruckSpeed);
		}
	}
}

/*
* Returns the Time Dilation value of the Simulation.
*/
float MiningTruckController::GetPlaybackSpeed() const
{
	return SimulationPlaybackSpeed;
}

/*
* Returns a vector of valid pointers to every Mining Truck in the Simulation.
*/
std::vector<BaseEntity*> MiningTruckController::GetMiningTrucks() const
{
	std::vector<BaseEntity*> miningTrucks;
	for (const auto& iterator : MiningTruckRegistry)
	{
		MiningTruck* miningTruckPtr = reinterpret_cast<MiningTruck*>(iterator.second);
		if (miningTruckPtr)
		{
			miningTrucks.push_back(miningTruckPtr);
		}
	}
	return miningTrucks;
}

/*
* Returns the Total number of Mining Trucks in the Simulation.
*/
unsigned int MiningTruckController::GetNumMiningTrucks() const
{
	return MiningTruckRegistry.size();
}

/*
* Returns the Total number of Unloading Locations in the Simulation.
*/
std::vector<BaseEntity*> MiningTruckController::GetUnloadingLocations() const
{
	std::vector<BaseEntity*> unloadingLocations;
	for (const auto& iterator : UnloadingLocationRegistry)
	{
		UnloadingLocation* unloadingLocationPtr = reinterpret_cast<UnloadingLocation*>(iterator.second);
		if (unloadingLocationPtr)
		{
			unloadingLocations.push_back(unloadingLocationPtr);
		}
	}
	return unloadingLocations;
}

/*
* Returns the total amount of time remaining in the Simulation.
*/
float MiningTruckController::GetGlobalRemainingTime() const
{
	return SimulationTimer.GetRemainingGlobalTime();
}

/*
* Calculates and returns the total Efficiency of the Mining Operation.
* The calculation is as follows:
* Sum the total amount of Helium-3 Unloaded during the Simmulation.
* Divide that value by the total amount of time elapsed since the Simulation started.
* While trucks are not unloading (traveling between locations or waiting in queue) efficiency will drop. Natrually.
*/
float MiningTruckController::GetMiningEfficiency() const
{
	// Sum up how much Helium-3 has been unloaded at this point in time.
	float efficiency = 0.0f;
	for (const auto& iterator : UnloadingLocationRegistry)
	{
		UnloadingLocation* unloadingLocationPtr = reinterpret_cast<UnloadingLocation*>(iterator.second);
		if (unloadingLocationPtr)
		{
			efficiency += unloadingLocationPtr->GetTotalUnloadingTime();
		}
	}

	// Calculation of efficiency = Total Amount of time spent unloading Helium-3 divided by the total elapsed time of the operation (up to 72 hours).
	// Efficiency is expected to drop during moments where no truck us unloading cargo. As time is still elapsing.
	float elapsedTime = SimulationTimer.GetMaxGlobalTime() - SimulationTimer.GetRemainingGlobalTime();
	if (elapsedTime == 0.0f)
	{
		return 0.0f;
	}

	return efficiency / elapsedTime;
}

/*
* Starts the Simulation.
* Spawns Mining Trucks, Mining Locations, and Unloading Locations.
* Followed by finding Mining Locations for each truck to mine at by calling BeginMiningOperation().
*/
void MiningTruckController::StartSimulation(const SimulationConfiguration& simulationConfiguration)
{
	SimConfig = simulationConfiguration;
	SimulationTimer.SetSimulationMaxTime(SimConfig.SimulationMaxTimeSeconds);

	NumMiningTrucksToSpawn = SimConfig.NumMiningTrucksToSpawn;
	NumUnloadingLocationsToSpawn = SimConfig.NumUnloadingLocationsToSpawn;

	SpawnActorsInCircularPattern<MiningTruck>(NumMiningTrucksToSpawn, [this](MiningTruck* miningTruck) {
		if (miningTruck)
		{
			miningTruck->SetMiningAndUnloadingTimes(SimConfig.MiningAndUnloadingTimes);
			MiningTruckRegistry.emplace(miningTruck->GetUniqueId(), miningTruck);
		}
	}, MiningTruckSpawnRadius);

	SpawnActorsInCircularPattern<UnloadingLocation>(NumUnloadingLocationsToSpawn, [this](UnloadingLocation* unloadingLocation) {
		if (unloadingLocation)
		{
			// Pre-bind to the OnRequestMiningTruckStartUnloading delegate. As there is no need to ever have it unbound.
			unloadingLocation->OnRequestMiningTruckStartUnloading.Bind([this](unsigned int truckUniqueId) {
				OnRequestUnloadMiningTruck(truckUniqueId);
			});

			UnloadingLocationRegistry.emplace(unloadingLocation->GetUniqueId(), unloadingLocation);
		}
	}, UnloadingLocationSpawnRadius);

	SpawnMiningLocations();
	BeginMiningOperation();

	// Set the initial simulation speed.
	SimulationTimer.SetGlobalTimeDialation(SimulationPlaybackSpeed);
}

/*
* Cleans up all Simulation Values and resets any Speed changes back to defualt and restarts the simulation.
*/
void MiningTruckController::RestartSimulation()
{
	Teardown();

	MiningTruckSpeed = 1.0f;
	SimulationPlaybackSpeed = 5.0f;

	StartSimulation(SimConfig);
}

/*
* Allows you to change the number of Trucks to Spawn the next time Restart Simulation is called.
* First you set the number of Trucks using this function, then call Restart Simulation which will use the new value.
*/
void MiningTruckController::ChangeNumTrucks(unsigned int amount)
{
	NumMiningTrucksToSpawn += amount;
	if (NumMiningTrucksToSpawn < 1)
	{
		NumMiningTrucksToSpawn = 1;
	}
}

/*
* Allows you to change the number of Unloading Stations to Spawn the next time Restart Simulation is called.
* First you set the number of unloading stations using this function, then call Restart Simulation which will use the new value.
*/
void MiningTruckController::ChangeNumUnloadingStations(unsigned int amount)
{
	NumUnloadingLocationsToSpawn += amount;
	if (NumUnloadingLocationsToSpawn < 1)
	{
		NumUnloadingLocationsToSpawn = 1;
	}
}

/*
* Returns the number of Mining Trucks in the Simulation.
*/
unsigned int MiningTruckController::GetTotalMiningTrucks()
{
	return NumMiningTrucksToSpawn;
}

/*
* Returns the number of Unloading Stations in the Simulation.
*/
unsigned int MiningTruckController::GetTotalUnloadingStations()
{
	return NumUnloadingLocationsToSpawn;
}

/*
* Spawns a Single Entity of any type and returns a pointer to it.
* This is a templated function so that we don't have to write the same function over and over just for a different Spawn type.
*/
template<typename T>
T* MiningTruckController::SpawnEntity(const Vector& location)
{
	T* entity = new T();
	if (!entity)
	{
		return nullptr;
	}

	entity->SetLocation(location);

	++NumSpawnedEntities;
	entity->SetUniqueId(NumSpawnedEntities);

	return entity;
}

/*
* Destroys an entity that was spawned.
*/
void MiningTruckController::DestroyEntity(BaseEntity* entity) const
{
	if (entity)
	{
		delete entity;
		entity = nullptr;
	}
}

/*
* Spawns Mining Locations in random locations.
* These are the mining Locations the Mining Trucks will use to gather Helium-3.
* Also stores each Mining Truck in the MiningTruckRegistry map.
*/
void MiningTruckController::SpawnMiningLocations()
{
	// Spawn Mining Locations in a random pattern. Remember, the number of trucks = the number of mining locations, so NumMiningTrucksToSpawn is used here.
	for (unsigned int i = 0; i < NumMiningTrucksToSpawn; ++i)
	{
		Vector randomNavLocation;
		float miningLocationDistance = 0.0f;

		// Pseudo-code. This Vector should be a random vector that is a valid location to spawn the Mining Location at.
		randomNavLocation.Randomize();

		BaseEntity* spawnedEntity = SpawnEntity<MiningLocation>(randomNavLocation);
		if (spawnedEntity)
		{
			MiningLocationRegistry.emplace(spawnedEntity->GetUniqueId(), spawnedEntity );
		}
	}
}

/*
* Spawns any Entity Type (T) in a circular pattern.
* If we spawn 10 Mining Trucks they will spawn around the center defined by BaseLocation, and the angle will divided by 10 for each truck spawn.
* This is a templated function so we can spawn any entity in a circular pattern.
*/
template<typename T>
void MiningTruckController::SpawnActorsInCircularPattern(unsigned int numActorsToSpawn,
														 std::function<void(T*)> onEntitySpawned,
														 float spawnRadius /* = 1.0f */)
{
	Vector baseLocation = Vector(0.0f, 0.0f, 0.0f);
	float spawnAngle = 0.0f;
	float degreeOffset = 360.0f / numActorsToSpawn;
	for (unsigned int i = 0; i < numActorsToSpawn; ++i)
	{
		// Convert the degree angle into radians, as cos / sin expect radians.
		float finalAngle = spawnAngle * PI / 180.0f;

		// Posiiton the actors around the Base Station (X, Y, Z). 
		Vector spawnLocation(cos(finalAngle) * spawnRadius, sin(finalAngle) * spawnRadius, 50.0f);

		// Face the actors away from the base.
		Vector lookAtLocation = (baseLocation - spawnLocation) * -2.0f; // -2.0f to look away from the base.
		lookAtLocation.Z = 0.0f;										 // Prevents Pitch.
		lookAtLocation.Normalize();

		// Spawn Entity.
		T* spawnedEntity = reinterpret_cast<T*>(SpawnEntity<T>(spawnLocation));
		if (!spawnedEntity)
		{
			// Entity failed to spawn.
			continue;
		}

		onEntitySpawned(spawnedEntity);

		// Offset the spawn location in degrees.
		spawnAngle += degreeOffset;
	}
}

/*
* Finds an empty location for each Mining Truck to start mining at. Empty in this case means Idle.
*/
void MiningTruckController::BeginMiningOperation()
{
	for (const auto iterator : MiningTruckRegistry)
	{
		MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(iterator.second);
		if (!miningTruck)
		{
			continue;
		}

		// Found and Idle truck.
		if (miningTruck->GetState() == EMiningTruckState::Idle)
		{
			// Find a suitable mining location.
			FindLocationToMine(miningTruck);

			// Initialize the Mining Truck Speed.
			miningTruck->SetMiningTruckSpeed(MiningTruckSpeed);
		}
	}
}

/*
* Finds and empty mining location for a single mining truck to mine at. Empty in this case means Idle.
*/
void MiningTruckController::FindLocationToMine(BaseEntity* miningTruckPtr)
{
	if (!miningTruckPtr)
	{
		return;
	}

	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckPtr);
	if (!miningTruck)
	{
		return;
	}

	// The only requirement to be a valid mining location is for the mining location to be in the Idle state for the sake of this simulation.
	for (const auto miningLocationIterator : MiningLocationRegistry)
	{
		MiningLocation* miningLocation = reinterpret_cast<MiningLocation*>(miningLocationIterator.second);
		if (!miningLocation)
		{
			continue;
		}

		// Ensure the mining location is idle.
		if (miningLocation->GetState() != EMiningLocationState::Idle)
		{
			continue;
		}

		// Bind callback that will be used to respond to when the truck arrives at the mining location.
		miningTruck->OnMoveToMiningLocationComplete.Bind([this](unsigned int truckUniqueId) {
			OnMoveToMiningLocationComplete(truckUniqueId);
		});

		// Track the truck as moving to a mining location.
		MoveToMiningLocationPending.emplace(miningTruck->GetUniqueId(), miningLocationIterator.first);

		// We've found a match for a truck and a Mining Location, move the truck to the mining location.
		miningTruck->SetState(EMiningTruckState::MovingToMiningLocation);
		miningLocation->SetState(EMiningLocationState::MiningTruckEnRoute);

		// Attempt to start moving the truck to the mining location.
		if (!miningTruck->MoveToLocation(miningLocation, EMiningTruckMovementTarget::MiningLocation))
		{
			continue;
		}

		break;
	}
}

/*
* After a truck is told to move to a mining location, this callback moves the truck into the Mining state and MiningTruckController will now wait for the truck to complete mining.
*/
void MiningTruckController::OnMoveToMiningLocationComplete(unsigned int truckUniqueId)
{
	// Truck and Mining Location Validation.
	const auto truckToMiningIter = MoveToMiningLocationPending.find(truckUniqueId);
	if (truckToMiningIter == MoveToMiningLocationPending.end())
	{
		return;
	}

	const auto miningLocationIter = MiningLocationRegistry.find(truckToMiningIter->second);
	if (miningLocationIter == MiningLocationRegistry.end())
	{
		return;
	}

	const auto miningTruckIter = MiningTruckRegistry.find(truckUniqueId);
	if (miningTruckIter == MiningTruckRegistry.end())
	{
		return;
	}

	MiningLocation* miningLocation = reinterpret_cast<MiningLocation*>(miningLocationIter->second);
	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckIter->second);
	if (!miningLocation || !miningTruck)
	{
		return;
	}

	// Unbind from the Move to Mining Location Delegate.
	miningTruck->OnMoveToMiningLocationComplete.Unbind();

	// Move Truck from the Move-To mining state, to the actively mining state.
	MoveToMiningLocationPending.erase(truckUniqueId);
	ActiveMiningTrucks.emplace(truckUniqueId, miningLocation->GetUniqueId());

	// Bind to the delegate that responds to mining being completed.
	miningTruck->OnMiningCompleted.Bind([this](unsigned int truckUniqueId) {
		OnMiningCompleted(truckUniqueId);
	});

	// Set the Truck and the Mining Location to the "being mined" state.
	miningTruck->SetState(EMiningTruckState::Mining);
	miningLocation->SetState(EMiningLocationState::ActivelyBeingMined);
}

/*
* Once Mining has completed this function will find a suitable unloading station to queue at.
* The rule is to find the station with the shortest queue, and send the truck there.
* If there is no queue at all, naturally that station will be the best selection for efficiency.
* AddMiningTruckToQueue() is called to add the truck to the mining queue of the Unloading Station.
*/
void MiningTruckController::OnMiningCompleted(unsigned int truckUniqueId)
{
	// Truck and Mining Location Validation.
	const auto truckToMiningIter = ActiveMiningTrucks.find(truckUniqueId);
	if (truckToMiningIter == ActiveMiningTrucks.end())
	{
		return;
	}

	const auto miningLocationIter = MiningLocationRegistry.find(truckToMiningIter->second);
	if (miningLocationIter == MiningLocationRegistry.end())
	{
		return;
	}

	const auto miningTruckIter = MiningTruckRegistry.find(truckUniqueId);
	if (miningTruckIter == MiningTruckRegistry.end())
	{
		return;
	}

	MiningLocation* miningLocation = reinterpret_cast<MiningLocation*>(miningLocationIter->second);
	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckIter->second);
	if (!miningLocation || !miningTruck)
	{
		return;
	}

	// Find a suitable Unloading Location that has the shortest queue for efficiency.
	// Linear search O(n) should be sufficient, as it not expected that there will be a million or more unloading locations.
	// If there were going to be a million or more unloading locations, then I would keep a sorted list of Unloading Locations that will be sorted by queue time from lowest to highest.
	// My sort function would have been Quick Sort at O(n^2) worst case and O( n log n ) average case.
	// My search function would have simply taken the Unloading location from the front of the list (Location with the shortest queue).
	float shortestQueueTime = 99999.0f;
	UnloadingLocation* selectedUnloadingLocation = nullptr;
	for (const auto unloadingLocationInfo : UnloadingLocationRegistry)
	{
		BaseEntity* unloadingLocationPtr = unloadingLocationInfo.second;
		if (!unloadingLocationPtr)
		{
			continue;
		}
		
		UnloadingLocation* unloadingLocation = reinterpret_cast<UnloadingLocation*>(unloadingLocationPtr);
		if (!unloadingLocation)
		{
			continue;
		}

		float queueTime = unloadingLocation->GetQueueTime();
		if (queueTime < shortestQueueTime)
		{
			shortestQueueTime = queueTime;
			selectedUnloadingLocation = unloadingLocation;
		}
	}

	// If we return here, we may have not spawned any Unloading Locations.
	if (!selectedUnloadingLocation)
	{
		return;
	}

	// Bind to the delegate that responds to mining truck move to Unload Queue being completed.
	miningTruck->OnMoveToUnloadingQueueComplete.Bind([this](unsigned int truckUniqueId) {
		OnMoveToUnloadingQueueComplete(truckUniqueId);
	});

	// Unbind from the Move to Mining Location Delegate.
	miningTruck->OnMiningCompleted.Unbind();

	// Move Truck from the mining state, to the move-to unloading state.
	ActiveMiningTrucks.erase(truckUniqueId);
	MovingToUnloadingLocationPending.emplace(truckUniqueId, selectedUnloadingLocation->GetUniqueId());
	MiningTrucksPendingUnload.emplace(truckUniqueId, selectedUnloadingLocation->GetUniqueId());

	// Pre-calculate an unloading time. if the move attempt was successful.
	// This way, the Unloading location, can sum all the queue times up from each Mining Truck in the queue to calculate a total queue time.
	// This time is calculated as the truck begins to move toward the unloading queue location.
	miningTruck->CalculateUnloadTimer();

	selectedUnloadingLocation->AddMiningTruckToQueue(miningTruck);

	// Set the Truck and the Mining Location states to moving to unloading, and depleted states respectively.
	miningTruck->SetState(EMiningTruckState::MovingToUnloadingLocation);
	miningLocation->SetState(EMiningLocationState::Idle);

	// Attempt to start moving the truck to the unloading location. 8,000 units is the min radius where the movement can stop as they will join the queue at that point.
	if (!miningTruck->MoveToLocation(selectedUnloadingLocation, EMiningTruckMovementTarget::UnloadingQueue))
	{
		return;
	}
}

/*
* Once a truck arrives in the loading queue it's state is transitioned to "InUnloadingQueue".
* The Mining Truck's Id will be used to Request that it begin unloading when the Unloading Station is ready.
*/
void MiningTruckController::OnMoveToUnloadingQueueComplete(unsigned int truckUniqueId)
{
	// Truck and Mining Location Validation.
	auto truckToUnloadingIter = MovingToUnloadingLocationPending.find(truckUniqueId);
	if (truckToUnloadingIter == MovingToUnloadingLocationPending.end())
	{
		return;
	}

	auto unloadingLocationIter = UnloadingLocationRegistry.find(truckToUnloadingIter->second);
	if (unloadingLocationIter == UnloadingLocationRegistry.end())
	{
		return;
	}

	auto miningTruckIter = MiningTruckRegistry.find(truckUniqueId);
	if (miningTruckIter == MiningTruckRegistry.end())
	{
		return;
	}

	UnloadingLocation* unloadingLocation = reinterpret_cast<UnloadingLocation*>(unloadingLocationIter->second);
	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckIter->second);
	if (!unloadingLocation || !miningTruck)
	{
		return;
	}

	MovingToUnloadingLocationPending.erase(truckUniqueId);

	miningTruck->OnMoveToUnloadingQueueComplete.Unbind();
	miningTruck->SetState(EMiningTruckState::InUnloadingQueue);
}

/*
* Callback function used by the Unloading Station to request that a truck begin unloading at a station.
* This function is called by a Delegate from an Unloading Station during it's ProcessQueue() function.
*/
void MiningTruckController::OnRequestUnloadMiningTruck(unsigned int truckUniqueId)
{
	auto truckToUnloadingIter = MiningTrucksPendingUnload.find(truckUniqueId);
	if (truckToUnloadingIter == MiningTrucksPendingUnload.end())
	{
		return;
	}

	auto unloadingLocationIter = UnloadingLocationRegistry.find(truckToUnloadingIter->second);
	if (unloadingLocationIter == UnloadingLocationRegistry.end())
	{
		return;
	}

	auto miningTruckIter = MiningTruckRegistry.find(truckUniqueId);
	if (miningTruckIter == MiningTruckRegistry.end())
	{
		return;
	}

	UnloadingLocation* unloadingLocation = reinterpret_cast<UnloadingLocation*>(unloadingLocationIter->second);
	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckIter->second);
	if (!unloadingLocation || !miningTruck)
	{
		return;
	}

	// Bind callback function that gets invoked when a mining truck has finished moving into the unloading station from the queue.
	miningTruck->OnMoveToUnloadingLocationComplete.Bind([this](unsigned int truckUniqueId) {
		OnMoveToUnloadingLocationComplete(truckUniqueId);
	});

	MiningTrucksPendingUnload.erase(truckUniqueId);
	MiningTrucksTransitioningToUnload.emplace(truckUniqueId, unloadingLocation->GetUniqueId());

	miningTruck->SetState(EMiningTruckState::TransitioningToUnload);

	// Attempt to start moving the truck to the mining location.
	if (!miningTruck->MoveToLocation(unloadingLocation, EMiningTruckMovementTarget::UnloadingLocation))
	{
		return;
	}
}

/*
* After requesting a Mining Truck move "directly" into the Mining Station to begin unloading, this function will change the state of the mining truck to the "actual" unloading state.
*/
void MiningTruckController::OnMoveToUnloadingLocationComplete(unsigned int truckUniqueId)
{
	// Truck and Mining Location Validation.
	auto truckToUnloadingIter = MiningTrucksTransitioningToUnload.find(truckUniqueId);
	if (truckToUnloadingIter == MiningTrucksTransitioningToUnload.end())
	{
		return;
	}

	auto unloadingLocationIter = UnloadingLocationRegistry.find(truckToUnloadingIter->second);
	if (unloadingLocationIter == UnloadingLocationRegistry.end())
	{
		return;
	}

	auto miningTruckIter = MiningTruckRegistry.find(truckUniqueId);
	if (miningTruckIter == MiningTruckRegistry.end())
	{
		return;
	}

	UnloadingLocation* unloadingLocation = reinterpret_cast<UnloadingLocation*>(unloadingLocationIter->second);
	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckIter->second);
	if (!unloadingLocation || !miningTruck)
	{
		return;
	}

	miningTruck->OnMoveToUnloadingLocationComplete.Unbind();

	MiningTrucksTransitioningToUnload.erase(truckUniqueId);
	ActiveUnloadingTrucks.emplace(truckUniqueId, unloadingLocation->GetUniqueId());

	// Bind callback to notify when unloading for a truck is complete.
	miningTruck->OnUnloadingCompleted.Bind([this](unsigned int truckUniqueId) {
		OnUnloadingCompleted(truckUniqueId);
	});

	// Update the Unloading Location with the change in time (deltaTime) so that UnloadingLocation::GetQueueTime() stays in sync in real time.
	miningTruck->OnUnloadHelium.Bind([unloadingLocation](unsigned int truckUniqueId, float deltaTime) {
		if (unloadingLocation)
		{
			unloadingLocation->UnloadHelium(deltaTime);
		}
	});

	miningTruck->SetState(EMiningTruckState::Unloading);
	unloadingLocation->SetState(EUnloadingLocationState::Unloading);
}

/*
* Callback function to respond to a truck finishing it's unloading of Helium-3.
* After a truck is "emptied" it's then told to go mine a new Mining Location to maintain efficiency.
*/
void MiningTruckController::OnUnloadingCompleted(unsigned int truckUniqueId)
{
	auto truckToUnloadingLocationIter = ActiveUnloadingTrucks.find(truckUniqueId);
	if (truckToUnloadingLocationIter == ActiveUnloadingTrucks.end())
	{
		return;
	}

	auto unloadingLocationIter = UnloadingLocationRegistry.find(truckToUnloadingLocationIter->second);
	if (unloadingLocationIter == UnloadingLocationRegistry.end())
	{
		return;
	}

	auto miningTruckIter = MiningTruckRegistry.find(truckUniqueId);
	if (miningTruckIter == MiningTruckRegistry.end())
	{
		return;
	}

	UnloadingLocation* unloadingLocation = reinterpret_cast<UnloadingLocation*>(unloadingLocationIter->second);
	MiningTruck* miningTruck = reinterpret_cast<MiningTruck*>(miningTruckIter->second);
	if (!unloadingLocation || !miningTruck)
	{
		return;
	}

	ActiveUnloadingTrucks.erase(truckUniqueId);

	unloadingLocation->SetState(EUnloadingLocationState::Idle);
	unloadingLocation->MiningTruckUnloadingFinished(truckUniqueId);
	miningTruck->SetState(EMiningTruckState::Idle);

	// Remove the Mining Location's callback from the Mining Trucks OnUnloadHelium callback.
	miningTruck->OnUnloadHelium.Unbind();

	// Find a new mining location for the truck to mine.
	FindLocationToMine(miningTruck);
}

/*
* Destroys all Spawned Entities.
*/
void MiningTruckController::DestroyAllEntities()
{

	// Destroy all Mining Trucks.
	for (const auto& iterator : MiningTruckRegistry)
	{
		BaseEntity* miningTruckPtr = iterator.second;
		if (miningTruckPtr)
		{
			DestroyEntity(miningTruckPtr);
		}
	}
	MiningTruckRegistry.clear();

	// Destroy all Mining Trucks.
	for (const auto& iterator : MiningLocationRegistry)
	{
		BaseEntity* miningLocationPtr = iterator.second;
		if (miningLocationPtr)
		{
			DestroyEntity(miningLocationPtr);
		}
	}
	MiningLocationRegistry.clear();

	// Destroy all Unloading Locations.
	for (const auto& iterator : UnloadingLocationRegistry)
	{
		BaseEntity* unloadingLocationPtr = iterator.second;
		if (unloadingLocationPtr)
		{
			DestroyEntity(unloadingLocationPtr);
		}
	}
	UnloadingLocationRegistry.clear();
}