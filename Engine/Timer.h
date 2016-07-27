#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void Stop();
	float GetMicroSeconds();
	float GetMilliSeconds();
	float GetSeconds();

private:
	std::chrono::high_resolution_clock clock;
	std::chrono::time_point<std::chrono::high_resolution_clock > start_time;
	std::chrono::duration<float> elapse;
};

