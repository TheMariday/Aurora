#pragma once
#include <chrono>
#include <functional>

#define timestamp std::chrono::system_clock::time_point
#define duration std::chrono::milliseconds
#define duration_cast std::chrono::duration_cast<duration>

#define Driver std::function<void(timestamp t)>
#define MaxTimestamp std::chrono::time_point<std::chrono::system_clock>::max
#define Now std::chrono::system_clock::now

#define axis int
#define x_axis 0
#define y_axis 1
#define z_axis 2

#define M_PI 3.14159265358979323846  /* pi */


template <class T>
T limit(T v, T v1, T v2)
{
	v = std::min(v, (v1 < v2) ? v2 : v1);
	v = std::max(v, (v1 < v2) ? v1 : v2);
	return v;
}