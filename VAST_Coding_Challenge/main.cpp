#include "Global.h"
#include "MiningTruckController.h"

#include <chrono>
#include <iostream>
#include <thread>

/**************************/
/* VAST Coding Challenge. */
/**************************/
/*
* This is a fully functional Simulation. :) I wrote this version here that you can run, and I wrote a second version in Unreal Engine 5.
* I'll be sending this project over along with the Unreal Engine 5 Build (I made it just for you) and a video of the build running if you want to view that instead.
* You should check them out, they're pretty cool. :)
*/

/***********************/
/* TESTING STRATEGIES. */
/***********************/
/*
* Foreword:
* Usually scale would be considered in this case. Scale in terms of Network bandwidth / latency, persistence size (non-volatile memory usage), and memory (volatile memory usage).
* But since this simulation runs locally the only considerations for scale I'm making are #1. The memory footprint of the simulation and #2. It's runtime performance (no crashes etc).
*/

/*
* Strategy #1. Ensure (through unit testing) when we add more Mining Trucks with same amount of Unloading Locations we experience global efficiency gains, while our efficiencies per truck would be expected to fall
* due to an increasing queue time. Unloading station efficiency should be maximized as they will be at full capacity at all times.
*/

/*
* Strategy #2. Ensure that when we reduce the number of Mining Trucks and increase the number of loading stations our efficiency per truck would be higher than in Strategy #1,
* while our unloading station efficiency would be reduced, as there are less trucks occupying a station at any given time - unloading Helium-3.
*/

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

