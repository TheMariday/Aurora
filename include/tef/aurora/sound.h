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

		bool Connect(std::string device);
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

	private:

		bool MainLoopCallback() override;

		std::string SpeechToCommand(std::string speech);
		std::string AudioToCommand(std::string speech);

		bool StopCommand(std::string command);

		bool ForceSpeechStop(std::string speech);
		bool ForceAudioStop(std::string audio);


		std::string m_device = "";

		std::mutex m_speechesMutex;
		std::list<std::string> m_speeches;

	};
}
