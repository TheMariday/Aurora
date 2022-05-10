#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include "utils.h"

class Metronome
{
public:
	Metronome(timestamp start, float bpm, int offset = 0)
	{
		m_start = start;
		m_bpm = bpm;
		m_offset = offset;
	}

	timestamp Beat(float i)
	{
		return m_start + std::chrono::milliseconds(static_cast<int>(1000 * (60.0f / m_bpm) * (i + m_offset)));
	}

	duration Beats(float i)
	{
		return duration(std::chrono::milliseconds(static_cast<int>(1000 * (60.0f / m_bpm) * i)));
	}

	int GetBeat(timestamp t)
	{
		return static_cast<int>((t - m_start).count() / (10000000 * (60.0f / m_bpm))) - m_offset;
	}

private:
	timestamp m_start;
	float m_bpm;
	int m_offset = 0;
};