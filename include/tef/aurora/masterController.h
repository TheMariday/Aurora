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

		bool StartHeadset();
		bool StartTail();
		bool StartBattery();
		bool StartFuse();
		bool StartButtons();
		bool StartSpeech();

		Sound* GetNotifier();

		UserControl* GetUserControl();

		bool CriticalFault();

		bool ClearFault();

		bool Report(Error e);

		void SetupVoiceCommands();

		bool HasQuit() { return m_quit; };

	private:

		bool RunCallback(std::shared_ptr<Command> command);
		bool LoadCommand(std::string command);

		bool m_fault = false;
		Error m_faultError;

		std::vector<Runnable*> m_connectedRunnable;

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

		std::atomic_bool m_quit = false;
	};
}

