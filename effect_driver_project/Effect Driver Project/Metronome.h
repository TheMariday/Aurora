#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <functional>

#define timestamp std::chrono::system_clock::time_point
#define duration std::chrono::milliseconds
#define duration_cast std::chrono::duration_cast<duration>

template <class T>
T limit(T v, T v1, T v2)
{
	v = std::min(v, (v1 < v2) ? v2 : v1);
	v = std::max(v, (v1 < v2) ? v1 : v2);
	return v;
}

enum class CycleType
{
	SINE,
	RANDOM
};

enum class EaseType
{
	LINEAR,
	BEIZIER
};

enum class MixType
{
	NORMAL,
	ADD,
	SUBTRACT,
	MULTIPLY
};

template <class T>
void Mix(T* pValue, T value, MixType type)
{
	switch (type)
	{
	case(MixType::NORMAL):
	{
		*pValue = value;
	}
	break;
	case(MixType::ADD):
	{
		*pValue += value;
	}
	break;
	case(MixType::SUBTRACT):
	{
		*pValue -= value;
	}
	break;
	case(MixType::MULTIPLY):
	{
		*pValue *= value;
	}
	break;
	}

};

template <class T>
bool Cycle(T* pValue, timestamp now, T from, T to, timestamp start, duration period, CycleType cycleType = CycleType::SINE, MixType mixType = MixType::NORMAL)
{
	T v = 0;

	float t = static_cast<float>(duration_cast(now - start).count()) / static_cast<float>(period.count());

	switch (cycleType)
	{
	case(CycleType::SINE):
	{
		v = from + (to - from) * static_cast<T>(std::sin(t * 3.14159 * 2) / 2 + 0.5f);
	}
	break;
	case(CycleType::RANDOM):
	{
		v = from + ((to - from) * rand()) / static_cast<double>(RAND_MAX);
	}
	break;
	}

	Mix(pValue, v, mixType);

	return true;

}

template <class T>
bool Constant(T* pValue, timestamp now, T value, MixType mixType = MixType::NORMAL)
{
	return Ease(pValue, now, value, value, now, now, EaseType::LINEAR, mixType, true);
}


template <class T>
bool Constant(T* pValue, timestamp now, T value, timestamp start, timestamp end, MixType mixType = MixType::NORMAL, bool extend = false)
{
	return Ease(pValue, now, value, value, start, end, EaseType::LINEAR, mixType, extend);
}

template <class T>
bool Constant(T* pValue, timestamp now, T value, timestamp start, duration d, MixType mixType = MixType::NORMAL, bool extend = false)
{
	return Ease(pValue, now, value, value, start, d, EaseType::LINEAR, mixType, extend);
}

template <class T>
bool Ease(T* pValue, timestamp now, T from, T to, timestamp start, timestamp end, EaseType easeType = EaseType::LINEAR, MixType mixType = MixType::NORMAL, bool extend = true)
{
	duration d = duration_cast(end - start);
	return Ease(pValue, now, from, to, start, d, easeType, mixType, extend);
}

template <class T>
bool Ease(T* pValue, timestamp now, T from, T to, timestamp start, duration d, EaseType easeType = EaseType::LINEAR, MixType mixType = MixType::NORMAL, bool extend = true)
{
	T v = 0;

	if (now < start + d && now > start)
	{
		float t = static_cast<float>(duration_cast(now - start).count()) / static_cast<float>(d.count());

		switch (easeType)
		{
		case EaseType::LINEAR:
		{
			v = static_cast<T>(from + (to - from) * t);
		}
		break;
		case EaseType::BEIZIER:
		{
			v = static_cast<T>(from * (std::pow((1 - t), 3) + 3 * std::pow((1 - t), 2) * t) + to * (3 * (1 - t) * std::pow(t, 2) + std::pow(t, 3)));
		}
		break;
		}

		v = limit<T>(v, from, to);
	}
	else
	{
		if (extend)
		{
			v = (now < start) ? from : to;
		}
		else
		{
			return false;
		}
	}

	Mix(pValue, v, mixType);

	return true;
}

class RangedEffect
{
public:
	RangedEffect() = default;

	~RangedEffect() = default;

	void AddAnimation(std::function<void(timestamp t)> a)
	{
		m_animation = a;
	};

	void Render(timestamp t)
	{
		m_animation(t);
		for (int i = 0; i < completion * 100; i++)
			std::cout << "#";
		std::cout << std::endl;
	}

public:
	float completion = 0;

private:
	std::function<void(timestamp t)> m_animation;
};

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

int msain()
{
	timestamp song_start = std::chrono::system_clock::now();

	Metronome metronome(song_start, 30);

	RangedEffect effect;

	effect.AddAnimation(
		[&effect, &metronome](timestamp t) {

			Cycle<float>(&effect.completion, t, -0.1f, 0.1f, metronome.Beat(0), metronome.Beats(0.5), CycleType::SINE);

			Ease<float>(&effect.completion, t, 0, 1, metronome.Beat(0), metronome.Beats(1.5), EaseType::BEIZIER, MixType::MULTIPLY);

			Ease<float>(&effect.completion, t, 1, 0, metronome.Beat(1.5), metronome.Beats(1.5), EaseType::BEIZIER, MixType::MULTIPLY);

			Constant<float>(&effect.completion, t, 0.5f, MixType::ADD);

			Constant<float>(&effect.completion, t, 1.0f, metronome.Beat(1.5), metronome.Beats(0.1), MixType::NORMAL);
		});

	timestamp now = std::chrono::system_clock::now();
	while (now < metronome.Beat(3))
	{
		now = std::chrono::system_clock::now();

		effect.Render(now);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	return 0;
}