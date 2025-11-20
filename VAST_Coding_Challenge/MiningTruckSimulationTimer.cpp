#include "MiningTruckSimulationTimer.h"
#include <iostream>

/*
* Updates the Global Simulation Timer (the 72 hour countdown).
*/
bool MiningTruckSimulationTimer::Tick(float deltaTime)
{
	if (GlobalRemainingTime <= 0.0f)
	{
		return true;
	}

	GlobalRemainingTime -= deltaTime;
	std::cout << "Simulation Time Left: " << GlobalRemainingTime << " seconds." << std::endl;

	if (GlobalRemainingTime <= 0.0f)
	{
		// Once our sim timer reaches 0, the simulation is complete, we can exit the sim now.
		GlobalRemainingTime = 0.0f;
		return true;
	}

	return false;
}

/*
* Returns how much Simulation Time is left.
*/
double MiningTruckSimulationTimer::GetRemainingGlobalTime() const
{
	return GlobalRemainingTime;
}

/*
* Returns the total Simulation Time we started the simulation with. (72-hours)
*/
double MiningTruckSimulationTimer::GetMaxGlobalTime() const
{
	return GlobalMaxTime;
}

/*
* Returns how much Simulation time has passed.
*/
double MiningTruckSimulationTimer::GetElapsedSimulationTime() const
{
	return GlobalMaxTime - GlobalRemainingTime;
}

/*
* Speeds up and slows down the Global Simulation time.
* This dilation value simply scales delta time throughout the simulation.
*/
void MiningTruckSimulationTimer::SetGlobalTimeDialation(float dilation)
{
	TimeDilation = dilation;
}

/*
* Returns Global Time Dilation value.
*/
float MiningTruckSimulationTimer::GetGlobalTimeDilation() const
{
	return TimeDilation;
}


/*
* Sets the Max time (runtime) of the global simulation in seconds.
*/
void MiningTruckSimulationTimer::SetSimulationMaxTime(float maxTime)
{
	GlobalRemainingTime = maxTime;
	GlobalMaxTime = maxTime;
}