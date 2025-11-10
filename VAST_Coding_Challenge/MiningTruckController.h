#pragma once
#include "BaseEntity.h"
#include "Delegate.h"
#include "MiningTruckSimulationTimer.h"

#include <functional>
#include <vector>
#include <unordered_map>

class MiningTruckController
{

public:
    MiningTruckController() = default;
    bool Tick(float deltaTime);
    OperationEfficiency Teardown();
    OperationEfficiency GetOperationEfficiency() const;

    // Allows changing of the Playback Speed of the Simulation.
    void IncreasePlaybackSpeed();
    void DecreasePlaybackSpeed();
    float GetMiningTruckSpeed();

    // Allows changing of the speed of the Mining Trucks Movement.
    void IncreaseTruckSpeed();
    void DecreaseTruckSpeed();
    float GetPlaybackSpeed() const;

    // Getter functions.
    std::vector<BaseEntity*> GetMiningTrucks() const;
    unsigned int GetNumMiningTrucks() const;
    std::vector<BaseEntity*> GetUnloadingLocations() const;
    float GetGlobalRemainingTime() const;
    float GetMiningEfficiency() const;
    void StartSimulation(const SimulationConfiguration& simulationConfiguration);
    void RestartSimulation();
    void ChangeNumTrucks(unsigned int amount);
    void ChangeNumUnloadingStations(unsigned int amount);
    unsigned int GetTotalMiningTrucks();
    unsigned int GetTotalUnloadingStations();

    Delegate<> OnMiningTruckStateChanged;

private:

    MiningTruckSimulationTimer SimulationTimer;

    // All actors could be placed into the same registry, but for organizational purposes, I'm placing each type of Actor into their own registry for even faster access.
    std::unordered_map<unsigned int, BaseEntity*> MiningTruckRegistry;
    std::unordered_map<unsigned int, BaseEntity*> MiningLocationRegistry;
    std::unordered_map<unsigned int, BaseEntity*> UnloadingLocationRegistry;

    // Hash maps to track the state of the simulation.
    std::unordered_map<unsigned int, unsigned int> MoveToMiningLocationPending;
    std::unordered_map<unsigned int, unsigned int> ActiveMiningTrucks;
    std::unordered_map<unsigned int, unsigned int> MovingToUnloadingLocationPending;
    std::unordered_map<unsigned int, unsigned int> MiningTrucksPendingUnload;
    std::unordered_map<unsigned int, unsigned int> MiningTrucksTransitioningToUnload;
    std::unordered_map<unsigned int, unsigned int> ActiveUnloadingTrucks;

    template<typename T>
    T* SpawnEntity(const Vector& location);
    void DestroyEntity(BaseEntity* entity) const;

    void SpawnMiningLocations();

    // Spawns actors in a circular pattern around the world.
    // This function accepts a lambda function that's called whenever an actor is spawned.
    // Used to Spawn Mining Trucks, and Unloading Locations.
    template<typename T>
    void SpawnActorsInCircularPattern(unsigned int numActorsToSpawn, 
                                      std::function<void(T*)> onEntitySpawned,
                                      float spawnRadius = 1.0f);
    void BeginMiningOperation();
    void FindLocationToMine(BaseEntity* miningTruckPtr);

    // Callback handle to set a truck and a mining state to "being mined".
    void OnMoveToMiningLocationComplete(unsigned int truckUniqueId);

    // Callback to handle mining a location. Will remove reserves from the mining location until the reserves are depleted.
    void OnMiningCompleted(unsigned int truckUniqueId);

    // Callback handling trucks arriving at an unloading queuelocation.
    void OnMoveToUnloadingQueueComplete(unsigned int truckUniqueId);

    // Callback handling a request from an unloading station to have a truck start unloading cargo.
    void OnRequestUnloadMiningTruck(unsigned int truckUniqueId);

    // Callback handling trucks arriving at an unloading location.
    void OnMoveToUnloadingLocationComplete(unsigned int truckUniqueId);

    // Callback responding to Unloading being completed for a Mining Truck.
    void OnUnloadingCompleted(unsigned int truckUniqueId);

    void DestroyAllEntities();

    // The task says to assume there are infinite amounts of mining locations.
    // So far now Num Trucks = Num Mining Locations. Use the same variable to spawn both.
    unsigned int NumMiningTrucksToSpawn = 10;
    unsigned int NumUnloadingLocationsToSpawn = 3;

    Vector SpawnOrigin = Vector(0.0f, 0.0f, 0.0f); // World Zero.
    float MiningLocationSpawnRadius = 30000.0f;
    float MiningLocationMinimumSpawnRadius = 10000.0f;

    float MiningTruckSpawnRadius = 5000.0f;
    float UnloadingLocationSpawnRadius = 45000.0f;

    float MiningTruckSpeed = 1.0f;
    float SimulationPlaybackSpeed = 1.0f;

    // Count the number of spawned entities.
    int NumSpawnedEntities = 0;

    // Last Used Simulation Configurations.
    SimulationConfiguration SimConfig;
};
