#include <Timer.h>

std::chrono::high_resolution_clock::time_point Timer::Start = std::chrono::high_resolution_clock::now();

Timer::Timer(void)
{
}


double Timer::GetTime()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Start).count() / 1000000.0;
}
