#pragma once
#include <iostream>
#include <math.h>
#include <random>

struct MiningAndUnloadingTimes
{
	float MinMiningTimeHours = 1.0f;
	float MaxMiningTimeHours = 5.0f;

	float MinUnloadingTimeMinutes = 5.0f;
	float MaxUnloadingTimeMinutes = 5.0f;
};

struct SimulationConfiguration
{
	int NumMiningTrucksToSpawn = 10;
	int NumUnloadingLocationsToSpawn = 3;
	MiningAndUnloadingTimes MiningAndUnloadingTimes;

	float SimulationMaxTimeSeconds = 259200.0f;
};

/*
* Operation Report used to calculate the Operation Efficiency per truck, per unloading station, and globally.
*/
struct OperationEfficiency
{
	float GlobalEfficiency = 0.0f;
	std::vector<float> PerTruckEfficiency;
	std::vector<float> PerUnloadingLocationEfficiency;

	void Print()
	{
		std::cout << "Global Efficiency: " << GlobalEfficiency << std::endl;
		for (unsigned int i = 0; i < PerTruckEfficiency.size(); ++i)
		{
			std::cout << "Truck: " << i << " efficiency: " << PerTruckEfficiency[i] << std::endl;
		}

		for (unsigned int i = 0; i < PerUnloadingLocationEfficiency.size(); ++i)
		{
			std::cout << "Unloading Station: " << i << " efficiency: " << PerUnloadingLocationEfficiency[i] << std::endl;
		}
	}
};

// Basic 3D vector for storing locations of Mining Trucks, Unloading Locations, and Mining Locations.
class Vector
{
public:
	Vector() = default;
	Vector(float in_X, float in_Y, float in_Z) : X(in_X), Y(in_Y), Z(in_Z) {};

	Vector operator+(const Vector& rhs) const
	{
		return Vector(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
	};

	// Define subtraction operator for vectors.
	Vector operator-(const Vector& rhs) const
	{
		return Vector(X - rhs.X, Y - rhs.Y, Z - rhs.Z);
	};

	Vector operator*(const float scalar) const
	{
		return Vector(X * scalar, Y * scalar, Z * scalar);
	};

	void Randomize()
	{
		srand(static_cast<unsigned>(time(0))); 

		X = static_cast<float>(rand()) / RAND_MAX;
		Y = static_cast<float>(rand()) / RAND_MAX;
		Z = static_cast<float>(rand()) / RAND_MAX;
	}

	float Size()
	{
		return sqrt(X*X + Y*Y + Z*Z);
	};

	static float Distance(const Vector& lhs, const Vector& rhs)
	{
		Vector result = lhs - rhs;
		return sqrt(result.X * result.X + result.Y * result.Y + result.Z * result.Z);
	};

	void Normalize()
	{
		float size = Size();
		X /= size;
		Y /= size;
		Z /= size;
	}

	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
};

class Rotator
{
public:
	Rotator() = default;
	Rotator(float in_Yaw, float in_Pitch, float in_Roll) : Yaw(in_Yaw), Pitch(in_Pitch), Roll(in_Roll) {};

	void Randomize()
	{
		srand(static_cast<unsigned>(time(0)));

		Yaw = static_cast<float>(rand()) / RAND_MAX;
		Pitch = static_cast<float>(rand()) / RAND_MAX;
		Roll = static_cast<float>(rand()) / RAND_MAX;
	}

	float Yaw = 0.0f;
	float Pitch = 0.0f;
	float Roll = 0.0f;
};

