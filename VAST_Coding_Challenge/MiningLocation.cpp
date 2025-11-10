#include "MiningLocation.h"

/*
* Returns the current state of a Mining Location.
*/
EMiningLocationState MiningLocation::GetState() const
{
	return State;
}

/*
* Allows you to change the state of a Mining Locations.
*/
void MiningLocation::SetState(EMiningLocationState newState)
{
	if (newState != State)
	{
		State = newState;
	}
}