#include "Timer.h"



Timer::Timer()
{}


Timer::~Timer()
{}

void Timer::Start()
{
	m_start = std::chrono::high_resolution_clock::now();
	isStopped = false;
}

void Timer::Stop()
{
	m_stop = std::chrono::high_resolution_clock::now();
	isStopped = true;
}

float Timer::Reset()
{
	Stop();
	float milli = GetMilli();
	Start();
	return milli;
}

float Timer::GetMilli()
{
	float milli = 0.f;

	if( isStopped )
	{
		milli = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - m_start ).count();
	}
	else
	{
		milli = std::chrono::duration_cast<std::chrono::milliseconds>( m_stop - m_start ).count();
	}

	return milli;
}
