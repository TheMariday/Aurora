#pragma once
#include "tef/aurora/button.h"
#include "tef/aurora/userControl.h"
#include "tef/aurora/speechRecognition.h"
#include "tef/aurora/sound.h"

namespace TEF::Aurora {

	class MasterController
	{
	public:
		MasterController();
		~MasterController();

		bool Start();

		Sound* GetSound() { return &m_headset; };

		UserControl* GetUserControl() { return &m_userControl; };

	private:
		bool RunCallback(Command* command);
		bool LoadCommand(std::string command);

		Command* m_loadedCommand;
		Button m_recordButton;
		Button m_confirmButton;
		UserControl m_userControl;
		SpeechRecognition m_speechRecognition;
		Sound m_headset;
	};
}

