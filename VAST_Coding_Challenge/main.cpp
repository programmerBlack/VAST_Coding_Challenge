#include "Global.h"
#include "MiningTruckController.h"

#include <chrono>
#include <iostream>
#include <thread>

void RunTestSuite()
{
	// Run Through a Suite of Unit tests here.
}

/*
* This simulation runs (try it out).
* Adjust the values below for Num Truck and Unloading Stations etc + Mining Time and Unloading times and see the various outputs with the Simulation Runtime set to different values.
* When the simulation is done running, it will print out the efficiency of each truck, unload station, and a global overall efficiency.
*/
int main()
{
	// MiningTruckController holds all the logic required to run the simulation as defined in the Coding Challenge.
	MiningTruckController miningTruckSim;

	// Setup the Simulation.
	SimulationConfiguration config;

	// Set Truck and Unloading Station Configuration.
	config.NumMiningTrucksToSpawn = 10;
	config.NumUnloadingLocationsToSpawn = 3;

	// Set min and max mining times, and min and max unloading times.
	config.MiningAndUnloadingTimes.MinMiningTimeHours = 0.0001f;
	config.MiningAndUnloadingTimes.MaxMiningTimeHours = 0.0001f;
	config.MiningAndUnloadingTimes.MinUnloadingTimeMinutes = 5.0f;
	config.MiningAndUnloadingTimes.MaxUnloadingTimeMinutes = 5.0f;

	// 72 - Hours. Change this to see the differences in efficiency outputs.
	config.SimulationMaxTimeSeconds = 259200.0f;

	miningTruckSim.StartSimulation(config);

	bool exit = false;
	while (!exit)
	{
		exit = miningTruckSim.Tick(1.0f);
		std::this_thread::sleep_for(std::chrono::seconds(1));

		system("cls");
	}

	// Clean up the simulation.
	OperationEfficiency operationEfficiency = miningTruckSim.Teardown();
	operationEfficiency.Print();
	return 0;
}

