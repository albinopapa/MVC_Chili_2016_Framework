#pragma once

#include <chrono>

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void Stop();
	float Reset();
	float GetMilli();

private:
	bool isStopped = true;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start, m_stop;
	float m_elapsed;
};

