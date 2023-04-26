#pragma once

#include<chrono>

class Timer
{
public:
	static double GetTime();
private:
	static std::chrono::high_resolution_clock::time_point Start;

private:
	Timer(void);
};