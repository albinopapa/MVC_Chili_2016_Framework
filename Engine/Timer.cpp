#include "Timer.h"



Timer::Timer()
{
	Start();
}


Timer::~Timer()
{}

void Timer::Start()
{
	start_time = clock.now();
}

void Timer::Stop()
{
	elapse = clock.now() - start_time;
}

float Timer::GetMicroSeconds()
{
	return elapse.count() * 10000.0f;
}

float Timer::GetMilliSeconds()
{
	return elapse.count() * 1000.0f;
}

float Timer::GetSeconds()
{
	return elapse.count();
}