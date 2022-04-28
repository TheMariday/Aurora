#pragma once
#include <vector>
#include <functional>
#include "utils.h"

class Drivable
{
public:
	void AddDriver(std::function<void(timestamp t)> d)
	{
		m_drivers.push_back(d);
	}

	void UpdateDrivers(timestamp t)
	{
		for (const Driver& driver : m_drivers)
			driver(t);
	}
private:
	std::vector<Driver> m_drivers;
};