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
		bool Say(std::string sentence);
		bool Play(std::string filename);

		bool MainLoopCallback();

	private:

		std::string m_device;
		bool m_enabled = true;
		std::mutex m_queueMutex;
		std::list<std::string> m_sentenceQueue;

	};
}
