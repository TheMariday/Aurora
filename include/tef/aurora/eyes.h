#pragma once
#include <vector>
#include <string>
#include <functional>
#include "runnable.h"
#include "effectRunner.h"


namespace TEF::Aurora {
	class Eyes : public Runnable
	{
	public:
		Eyes(TEF::Aurora::EffectRunner* er) : m_effectRunner(er)
		{
		}

		bool MainLoopCallback() override
		{
			m_effectRunner.
			return true;
		}

	private:
		TEF::Aurora::EffectRunner* m_effectRunner;
	};
}