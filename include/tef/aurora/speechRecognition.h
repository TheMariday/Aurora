#pragma once
#include <chrono>
#include <sys/select.h>
#include <atomic>
#include <thread>
#include <sphinxbase/ad.h>
#include <pocketsphinx/pocketsphinx.h>

namespace TEF::Aurora {
	class SpeechRecognition
	{
	public:
		SpeechRecognition();
		~SpeechRecognition();


		bool Start();
		bool Stop();

	private:

		bool ListeningLoop();

		bool VoreBuffer();

		cmd_ln_t* m_pConfig;
		ps_decoder_t* m_pSpeechDecoder;
		ad_rec_t* m_pDevice;

		std::atomic_bool m_running;
		std::thread m_listeningThread;

		short m_audioBuffer[49152*2]; // 24*2048
		int m_audioBufferLen;


	};
}