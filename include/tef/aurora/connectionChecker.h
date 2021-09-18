#pragma once

#include "tef/aurora/runnable.h"
#include "tef/aurora/smartFuse.h"
#include "tef/aurora/button.h"

namespace TEF::Aurora {

	struct SubsystemState
	{
		bool power = false;
		bool fadecandy = false;
	};

	class ConnectionChecker : public Runnable
	{
	public:
		ConnectionChecker();

		bool AddFuse(SmartFuse* smartFuse);

		bool MainLoopCallback() override;

	private:

		std::vector<std::string> GetUSB();

		SmartFuse* m_pSmartFuse;

		std::vector<SubsystemState> m_subsystemStates;
	};


}

