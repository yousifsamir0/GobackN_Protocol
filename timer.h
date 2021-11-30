#include <chrono>
#include <iostream>
#include <ctime>
#include<windows.h>

using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

long long get_time(void) {

	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

