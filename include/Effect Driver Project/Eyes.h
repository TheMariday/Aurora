#pragma once
#include "Effect.h"
#include "Metronome.h"
#include "Easer.h"
#include "Masks.h"
#include "Textures.h"
#include "colors.h"
#include "Effects.h"


inline void SetEyes(TEF::Aurora::EffectRunner* er, timestamp start, duration dur, std::string type, HSV col = WHITE)
{
	er->AddEffect(std::make_shared< GroupSolid>(&er->m_harness, start, start + dur, "eyes", BLACK)); // overrides the default eye effects
	er->AddEffect(std::make_shared< GroupSolid>(&er->m_harness, start, start + dur, type, col));
}

inline void SetEyes(TEF::Aurora::EffectRunner* er, timestamp start, timestamp end, std::string type, HSV col = WHITE)
{
	er->AddEffect(std::make_shared< GroupSolid>(&er->m_harness, start, end, "eyes", BLACK)); // overrides the default eye effects
	er->AddEffect(std::make_shared< GroupSolid>(&er->m_harness, start, end, type, col));
}



class Eyes : public Effect
{
public:
	Eyes(Harness* harness) :
		Effect(harness, Now(), Now() + std::chrono::milliseconds(1000 * 60 * 60 * 10)) // run for 10 hours
	{
		AddEye("eyes_default", 20);
		AddEye("eyes_1", 5);
		AddEye("eyes_2", 5);
		AddEye("eyes_3", 5);
		AddEye("eyes_4", 5);
		AddEye("eyes_5", 5);

		//AddEye("eyes_angry", 1);
		//AddEye("eyes_heart", 1);
		//AddEye("eyes_chevron", 1);
		//AddEye("eyes_cross", 1);
		//AddEye("eyes_lids", 1);
		//AddEye("eyes_big", 1);
		//AddEye("eyes_rings", 1);
		//AddEye("eyes_confused", 1);


		m_harness = harness;
		SetTexture(std::make_shared<SolidTexture>(harness, WHITE));
	}

	void PreRender(timestamp t) override
	{
		int eye_change = rand() % 60; // randomly every second or so?

		if (eye_change == 0)
		{
			int choice = rand() % m_eyes.size();

			ClearMasks();
			SetMask(std::make_shared<GroupMask>(m_harness, m_eyes[choice]));
		}
	}

	void AddEye(std::string name, int prob)
	{
		for (int i = 0; i < prob; ++i)
			m_eyes.push_back(name);
	}

	void Stop() override
	{
		// do nothing if asked to stop
	}
private:
	Harness* m_harness;

	std::vector<std::string> m_eyes;
};