#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include "utils.h"

class Metronome
{
public:
	Metronome(timestamp start, float bpm)
	{
		m_start = start;
		m_bpm = bpm;
	}

	timestamp Beat(float i)
	{
		return m_start + std::chrono::milliseconds(static_cast<int>(1000 * (60.0f / m_bpm) * i));
	}

	duration Beats(float i)
	{
		return duration(std::chrono::milliseconds(static_cast<int>(1000 * (60.0f / m_bpm) * i)));
	}

private:
	timestamp m_start;
	float m_bpm;
};