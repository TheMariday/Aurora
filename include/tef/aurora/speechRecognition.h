#pragma once
#include <chrono>
#include <sys/select.h>
#include <sphinxbase/ad.h>
#include <pocketsphinx/pocketsphinx.h>

namespace TEF::Aurora {
	class SpeechRecognition
	{
	public:
		SpeechRecognition();
		~SpeechRecognition();

		bool ConnectToDevice();

		bool Continuous();

	private:
		cmd_ln_t* m_pConfig;
		ps_decoder_t* m_pSpeechDecoder;
		ad_rec_t* m_pDevice;
	};
}