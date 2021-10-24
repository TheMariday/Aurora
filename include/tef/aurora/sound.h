#pragma once
#include <string>
#include <list>
#include <mutex>
#include "tef/aurora/runnable.h"

namespace TEF::Aurora {

	class Sound : public Runnable {
	public:
		Sound();
		~Sound();

		bool Connect(std::string card = "sndrpihifiberry", std::string device = "softvol", std::string volControl="SoftMaster");

		bool IsConnected();

		bool AddSpeech(std::string speech, bool wait = false);
		bool AddSpeech(std::stringstream& speech, bool wait = false);
		bool RemoveSpeech(std::string speech);
		bool RemoveSpeech();
		bool InterruptSpeech(std::string speech);

		void WaitFor(std::string speech);
		void WaitFor();

		bool PlayAudio(std::string filename);
		bool StopAudio(std::string filename);
		bool Stop();

		bool SetVolume(float volume);
		float GetVolume();

	private:

		bool MainLoopCallback() override;

		std::string SpeechToCommand(std::string speech);
		std::string AudioToCommand(std::string speech);

		bool StopCommand(std::string command);

		bool ForceSpeechStop(std::string speech);
		bool ForceAudioStop(std::string audio);


		std::string m_device = "";
		std::string m_card = "";
		std::string m_volControl = "";

		std::mutex m_speechesMutex;
		std::list<std::string> m_speeches;

		float m_volume = 0.5;

	};
}
