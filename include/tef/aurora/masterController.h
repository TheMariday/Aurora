#pragma once
#include "tef/aurora/button.h"
#include "tef/aurora/userControl.h"
#include "tef/aurora/speechRecognition.h"
#include "tef/aurora/sound.h"
#include "tef/aurora/batteryMonitor.h"
#include "tef/aurora/dacMCP3008.h"
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/error.h"
#include "tef/aurora/smartFuse.h"

namespace TEF::Aurora {

	class MasterController
	{
	public:
		MasterController();
		~MasterController();

		bool Start();

		Sound* GetSound() { return &m_headset; };

		UserControl* GetUserControl() { return &m_userControl; };

		bool CriticalFault();

		bool ClearFault();

		bool Report(Error e);

		bool CancelCommand();

		bool WhatsLoaded();

	private:
		bool RunCallback(std::shared_ptr<Command> command);
		bool LoadCommand(std::string command);

		bool m_fault = false;

		std::shared_ptr<Command> m_loadedCommand;
		DacButton m_recordButton;
		DacButton m_confirmButton;
		UserControl m_userControl;
		SpeechRecognition m_speechRecognition;
		Sound m_headset;
		Sound m_tailbass;
		BatteryMonitor m_batteryMonitor;
		DacMCP3008 m_dac;
		EffectRunner m_effectRunner;
		SmartFuse m_smartFuse;
	};
}

