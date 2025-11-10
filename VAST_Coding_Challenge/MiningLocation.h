#pragma once
#include "BaseEntity.h"

enum class EMiningLocationState : size_t
{
	Idle,               // The mining location is idle
	MiningTruckEnRoute, // The mining location is being approached by a truck
	ActivelyBeingMined, // The mining location is actively being mined
};

class MiningLocation : public BaseEntity
{
	using Base = BaseEntity;

public:	
	MiningLocation() = default;
	~MiningLocation() override = default;

	EMiningLocationState GetState() const;
	void SetState(EMiningLocationState newState);

private:

	EMiningLocationState State = EMiningLocationState::Idle;
};
