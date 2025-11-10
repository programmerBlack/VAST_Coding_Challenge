#pragma once
#include "Global.h"

/*
* Base class for all entities that are spawned to do work during the Simulation.
* Houses basic functions to change the location and set Ids of the Entities etc.
*/
class BaseEntity
{
public:
	BaseEntity() = default;
	virtual ~BaseEntity() = default;

	Vector GetForwardVector()
	{
		Vector returnValue;
		returnValue.Randomize();
		return returnValue;
	}

	Vector GetLocation()
	{
		return Location;
	}

	void SetLocation(const Vector& newLocation)
	{
		Location = newLocation;
	}

	void SetUniqueId(int id)
	{
		UniqueId = id;
	}

	int GetUniqueId()
	{
		return UniqueId;
	}

private:
	int UniqueId = -1;
	Vector Location;
};
