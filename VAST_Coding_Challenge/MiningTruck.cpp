#include "MiningTruck.h"

/*
* Update function to update the Mining and Unloading states.
*/
void MiningTruck::Tick(float deltaTime)
{
	if (State == EMiningTruckState::Mining)
	{
		MineLocation(deltaTime);
	}
	else if (State == EMiningTruckState::Unloading)
	{
		UnloadHelium(deltaTime);
	}
}

/*
* Returns the state of the Mining Truck.
*/
EMiningTruckState MiningTruck::GetState() const
{
	return State;
}

/*
* Returns how much time is left to Mine a Location.
*/
float MiningTruck::GetRemainingMiningTime() const
{
	return MiningTimeLeft;
}

/*
* Returns how much time is left to Unload Helim 3 at a Location.
*/
float MiningTruck::GetRemainingUnloadingTime() const
{
	return UnloadingTimeLeft;
}

/*
* This function returns how much time has been spend unloading Helium-3 at any location during the Simulation.
* If a Mining Truck unloads at Station A, Station B, and Station C during the Sim, this is the total time spent unloading at all 3 stations.
* This value is used to calculate the efficiency of a Mining Truck.
* The Calculation is: TotalHeliumUnloaded / TotalSimRunTime.
* This gives an efficiency rating per truck that will slowly diminish while the truck is traveling between mining locations and unloading stations, and while waiting in queue.
*/
float MiningTruck::GetTotalHeliumUnloaded() const
{
	return TotalHeliumUnloaded;
}

/*
* Allows you to change the state of the Mining Truck.
*/
void MiningTruck::SetState(EMiningTruckState newState)
{
	if (newState != State)
	{
		State = newState;
		if (State == EMiningTruckState::Mining)
		{
			CalculateMiningTimer();
		}
	}
}

/*
* Controls the Movement speed of a Mining Truck.
* Since the challenge says a Truck taks 30 minutes to move between locations, this allow you to "hack" to Sim and have the truck move faster so we don't have to wait.
*/
void MiningTruck::SetMiningTruckSpeed(float miningTruckSpeed)
{
	MiningTruckSpeedMultiplier = miningTruckSpeed;
	if (MiningTruckSpeedMultiplier < 0.0f)
	{
		MiningTruckSpeedMultiplier = 0.0f;
	}
}

/*
* This function ensures the Travel Speed is correct given the current location of the Mining Truck and it's target destination.
* The function makes up a random path of 20 points that's used as a "fake path" then calculates it's length segment by segment.
*/
float MiningTruck::CalculateDistanceToFinalLocation() const
{
	float pathLength = 0.0f;
	std::vector<Vector> pathPoints;

	// Pseudo-code. Fill the path poitns with a random set of points.
	for (unsigned int i = 0; i < 20; ++i)
	{
		Vector randomVector;
		randomVector.Randomize();
		pathPoints.push_back(randomVector);
	}

	for (unsigned int i = 1; i < pathPoints.size(); ++i)
	{
		const Vector& pathSegmentEnd = pathPoints[i];
		const Vector& pathSegmentStart = pathPoints[i - 1];

		// The length of the path to the end point is sum of the lengths of each segement in the path.
		Vector segment = pathSegmentEnd - pathSegmentStart;
		pathLength += segment.Size();
	}
	return pathLength;
}

/*
* Given a path length from the function above, we calculate a travel speed to ensure the travel time takes 30 minutes along the path.
* The formula used is: Speed = Distance / Time
*/
void MiningTruck::CalculateMiningTruckSpeed(EMiningTruckMovementTarget movementTarget)
{
	// Don't change / update the movement speed when we are moving from the Unloading Queue into the actual Unloading location.
	if (movementTarget == EMiningTruckMovementTarget::UnloadingLocation)
	{
		return;
	}

	// Use the distance to travel to the end point to calculate the speed required to get there in exactly 30 min.
	float distanceToTravel = CalculateDistanceToFinalLocation();
	float thirtyMinutesAsSeconds = 1800.0f;

	// Speed = Distance / Time.
	float travelSpeed = distanceToTravel / thirtyMinutesAsSeconds;
	MiningTruckTravelSpeed = travelSpeed;
}

/*
* A Psuedo Move to Location function that moves the Mining Truck along a path (it doesn't really, it just instantly returns a successful value).
*/
bool MiningTruck::MoveToLocation(BaseEntity* location, EMiningTruckMovementTarget movementTarget)
{
	if (!location)
	{
		return false;
	}

	Vector targetLocation = GetLocation() + (GetForwardVector() * 10000.0f);
	if (movementTarget == EMiningTruckMovementTarget::MiningLocation)
	{
		// Execute Movement Code here.

		// Notify when arrived at Mining Location.
		MoveCompleted.Bind([this]() { 
			OnArrivedAtMiningLocation();
		});
	}
	else if (movementTarget == EMiningTruckMovementTarget::UnloadingQueue)
	{
		// Execute Movement Code here.

		// Notify when arrived at Unloading Queue
		// This should be a different function as we may need to queue, or commence unloading.
		// Queuing would mean other trucks are queued up or possibly unloading there already.
		MoveCompleted.Bind([this]() {
			OnArrivedAtUnloadingQueue();
		});
	}
	else if (movementTarget == EMiningTruckMovementTarget::UnloadingLocation)
	{
		// And Execute Movement Code here.

		// Notify when arrived at the Unloading Station (to actually unload).
		MoveCompleted.Bind([this]() {
			OnArrivedAtUnloadingLocation();
		});
	}

	CalculateMiningTruckSpeed(movementTarget);

	// Usually we should wait until the move actually completes, but for the sake of this coding challenge. Let's invoke the callback immediately.
	MoveCompleted.ExecuteIfBound();
	return true;
}

/*
* Callback function called when the Mining Truck arrives at a Mining Location.
*/
void MiningTruck::OnArrivedAtMiningLocation()
{
	MoveCompleted.Unbind();
	OnMoveToMiningLocationComplete.ExecuteIfBound(GetUniqueId());
}

/*
* Callback function called when the Mining Truck arrives into an Unloading Queue (It's in line).
*/
void MiningTruck::OnArrivedAtUnloadingQueue()
{
	MoveCompleted.Unbind();
	OnMoveToUnloadingQueueComplete.ExecuteIfBound(GetUniqueId());
}

/*
* Callback function called when the Mining Truck arrives in the "actual" unloading station. (Since it's one at a time).
*/
void MiningTruck::OnArrivedAtUnloadingLocation()
{
	MoveCompleted.Unbind();
	OnMoveToUnloadingLocationComplete.ExecuteIfBound(GetUniqueId());
}

/*
* Sets the Min and Max Mining Times, and the Min and Max Unloading times.
*/
void MiningTruck::SetMiningAndUnloadingTimes(const MiningAndUnloadingTimes& miningAndUnloadingTimes)
{
	MinMiningTimeHours = miningAndUnloadingTimes.MinMiningTimeHours;
	MaxMiningTimeHours = miningAndUnloadingTimes.MaxMiningTimeHours;

	MinUnloadingTimeMinutes = miningAndUnloadingTimes.MinUnloadingTimeMinutes;
	MaxUnloadingTimeMinutes = miningAndUnloadingTimes.MaxUnloadingTimeMinutes;
}

/*
* Calculates how long to take to Mine a Location.
*/
void MiningTruck::CalculateMiningTimer()
{
	// The calculations here could all have been on one line, but I felt like splitting hours, minutes, and seconds out made the code clearer.
	float lambda = static_cast<float>(rand()) / RAND_MAX;
	float hours = (1.0f - lambda) * MinMiningTimeHours + lambda * MaxMiningTimeHours;
	float minutes = hours * 60.0f;
	float seconds = minutes * 60.0f;
	MiningTimeLeft = seconds;
}
/*
* Calculates how long to take to Unlaod Helium-3 at a Unloading Location.
*/
void MiningTruck::CalculateUnloadTimer()
{
	// The calculations here could all have been on one line, but I felt like splitting hours, minutes, and seconds out made the code clearer.
	float lambda = static_cast<float>(rand()) / RAND_MAX;
	float minutes = (1.0f - lambda) * MinUnloadingTimeMinutes + lambda * MaxUnloadingTimeMinutes;
	float seconds = minutes * 60.0f;
	UnloadingTimeLeft = seconds;
}

/*
* Performs minining of Helium-3.
* Mining 1 unit of Helium-3 is equivalent to 1 second of time.
* 1 Helium-3 = 1 Second. In this case, Time is our currency / commodity.
*/
void MiningTruck::MineLocation(float deltaTime)
{
	if (MiningTimeLeft > 0.0f)
	{
		MiningTimeLeft -= deltaTime;
		if (MiningTimeLeft <= 0.0f)
		{
			// To keep things neat and tidy, ensure we reset the remaining mining time to the default value.
			MiningTimeLeft = 0.0f;

			// Mining has completed.
			OnMiningCompleted.ExecuteIfBound(GetUniqueId());
		}
	}
}

/*
* Performs unloading of Helium-3.
* Unloading 1 unit of Helium-3 is equivalent to 1 second of time.
* 1 Helium-3 = 1 Second. In this case, Time is our currency / commodity. :)
*/
void MiningTruck::UnloadHelium(float deltaTime)
{
	if (UnloadingTimeLeft > 0.0f)
	{
		UnloadingTimeLeft -= deltaTime;
		if (UnloadingTimeLeft <= 0.0f)
		{
			// To keep things neat and tidy, ensure we reset the remaining mining time to the default value.
			UnloadingTimeLeft = 0.0f;

			// Mining has completed.
			OnUnloadingCompleted.ExecuteIfBound(GetUniqueId());
		}

		// Notify Observers (Unloading Locations) that the amount of helium has changed.
		OnUnloadHelium.ExecuteIfBound(GetUniqueId(), deltaTime);
		TotalHeliumUnloaded += deltaTime;
	}
}