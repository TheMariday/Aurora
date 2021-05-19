#pragma once
#include <chrono>
#include <sys/select.h>
#include <atomic>
#include <thread>
#include <sphinxbase/ad.h>
#include <pocketsphinx/pocketsphinx.h>
#include <mutex>
#include <functional>

namespace TEF::Aurora {
	class SpeechRecognition
	{
	public:
		SpeechRecognition();
		~SpeechRecognition();


		bool Start();
		bool Stop(bool saveBuffer=false);

		bool SetJSGF(std::string jsgfFile);

		bool RegisterCommandCallback(std::function<bool(std::string)> cb);

	private:

		bool ListeningLoop(); //temp move

		void SaveBuffer();


		cmd_ln_t* m_pConfig;
		ps_decoder_t* m_pSpeechDecoder;
		ad_rec_t* m_pDevice;

		std::atomic_bool m_listening;
		std::thread m_listeningThread;

		std::mutex m_bufferMutex;
		short m_audioBuffer[16000*10]; // 24*2048
		int m_audioBufferFront;

		std::function<bool(std::string)> m_commandCallback;


	};
}