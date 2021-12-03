#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <iostream>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

inline long long get_time()
{

	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

#endif
