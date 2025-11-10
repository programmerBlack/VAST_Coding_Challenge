#pragma once

class MiningTruckSimulationTimer
{

public:
	MiningTruckSimulationTimer() = default;

	bool Tick(float DeltaTime);
	double GetRemainingGlobalTime() const;
	double GetMaxGlobalTime() const;
	double GetElapsedSimulationTime() const;
	void SetGlobalTimeDialation(float dilation);
	float GetGlobalTimeDilation() const;
	void SetSimulationMaxTime(float maxTime);

private:
	// 259,200 seconds is 72 hours.
	double GlobalRemainingTime = 259200.0f;
	double GlobalMaxTime = 259200.0f;

	double TimeDilation = 1.0f;
};
