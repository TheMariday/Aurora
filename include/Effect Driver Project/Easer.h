#pragma once
#include "utils.h"

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
		v = from + static_cast<T>((to - from) * (std::sin(t * 3.14159 * 2) / 2 + 0.5f));
	}
	break;
	case(CycleType::RANDOM):
	{
		v = from + static_cast<T>((static_cast<double>(to - from) * rand()) / static_cast<double>(RAND_MAX));
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
bool Ease(T* pValue, timestamp now, T from, T to, timestamp start, timestamp end, EaseType easeType = EaseType::LINEAR, MixType mixType = MixType::NORMAL, bool extend = false)
{
	duration d = duration_cast(end - start);
	return Ease(pValue, now, from, to, start, d, easeType, mixType, extend);
}

template <class T>
bool Ease(T* pValue, timestamp now, T from, T to, timestamp start, duration d, EaseType easeType = EaseType::LINEAR, MixType mixType = MixType::NORMAL, bool extend = false)
{
	T v;

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

// forgive me father for this is hot garbage

inline bool LocEase(Loc* pValue, timestamp now, Loc from, Loc to, timestamp start, timestamp end, EaseType easeType = EaseType::LINEAR, MixType mixType = MixType::NORMAL, bool extend = false)
{
	Ease<int>(&pValue->x, now, from.x, to.x, start, end, easeType, mixType, extend);
	Ease<int>(&pValue->y, now, from.y, to.y, start, end, easeType, mixType, extend);
	Ease<int>(&pValue->z, now, from.z, to.z, start, end, easeType, mixType, extend);
	return true;
}