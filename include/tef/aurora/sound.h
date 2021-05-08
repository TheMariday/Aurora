#pragma once
#include <string>
#include <list>
#include <mutex>
#include "tef/aurora/effect.h"

namespace TEF::Aurora {

	class Sound : public Effect {
	public:
		explicit Sound(std::string device);
		~Sound();

		bool AddSpeech(std::string speech, bool wait = false);
		bool RemoveSpeech(std::string speech);
		bool RemoveSpeech();
		bool InterruptSpeech(std::string speech);


		bool WaitFor(std::string speech);
		bool WaitFor();

		bool MainLoopCallback() override;

		bool PlayAudio(std::string filename);
		bool StopAudio(std::string filename);
		bool Stop();

	private:

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