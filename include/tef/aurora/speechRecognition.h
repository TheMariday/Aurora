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
		bool Stop();

		bool SetRecordFile(std::string filepath);

		bool SetJSGF(std::string jsgfFile);

		bool RegisterCommandCallback(std::function<bool(std::string)> cb);

	private:

		bool RecordLoop();

		void SaveBuffer(std::string filepath);

		const static int m_sampleRate = 16000;
		const static int m_maxRecordTime = 10; //seconds

		cmd_ln_t* m_pConfig;
		ps_decoder_t* m_pSpeechDecoder;
		ad_rec_t* m_pDevice;

		std::atomic_bool m_recording = false;
		std::thread m_recordingThread;

		std::mutex m_bufferMutex;
		short m_audioBuffer[m_sampleRate * m_maxRecordTime];
		int m_audioBufferFront;

		std::function<bool(std::string)> m_commandCallback;

		std::string m_audioFilepathDebug;
	};
}