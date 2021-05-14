#include "tef/aurora/speechRecognition.h"
#include <spdlog/spdlog.h>

TEF::Aurora::SpeechRecognition::SpeechRecognition()
{
	static const arg_t cont_args_def[] = { POCKETSPHINX_OPTIONS, {"-adcdev", ARG_STRING, NULL, "Name of audio device to use for input."}, CMDLN_EMPTY_OPTION };
	m_pConfig = cmd_ln_init(NULL, cont_args_def, FALSE, "-inmic", "yes", "-jsgf", "/home/pi/temp/pocketsphinx/test.gram", NULL);
	ps_default_search_args(m_pConfig);

	m_pSpeechDecoder = ps_init(m_pConfig);

	if (m_pSpeechDecoder == NULL) {
		spdlog::error("something is null");
		cmd_ln_free_r(m_pConfig);
	}
}

TEF::Aurora::SpeechRecognition::~SpeechRecognition()
{
	ps_free(m_pSpeechDecoder);
	cmd_ln_free_r(m_pConfig);
}

bool TEF::Aurora::SpeechRecognition::Start()
{

	m_pDevice = ad_open_dev(cmd_ln_str_r(m_pConfig, "-adcdev"), (int)cmd_ln_float32_r(m_pConfig, "-samprate"));
	if (m_pDevice == NULL)
	{
		spdlog::error("Failed to open audio device");
		return false;
	}

	if (ad_start_rec(m_pDevice) < 0)
	{
		spdlog::error("Failed to start recording");
		return false;
	}

	if (ps_start_utt(m_pSpeechDecoder) < 0)
	{
		spdlog::error("Failed to start utterance");
		return false;
	}

	m_listeningThread = std::thread([this]() {ListeningLoop(); });
	return true;
}

bool TEF::Aurora::SpeechRecognition::Stop()
{
	m_running = false;

	m_listeningThread.join();

	if (ps_end_utt(m_pSpeechDecoder) < 0)
	{
		spdlog::error("Failed to stop utterance");
		return false;
	}

	if (ad_stop_rec(m_pDevice) < 0)
	{
		spdlog::error("Failed to stop recording");
		return false;
	}

	if (ad_close(m_pDevice) < 0)
	{
		spdlog::error("Failed to close device");
		return false;
	}
	return true;
}

bool TEF::Aurora::SpeechRecognition::ListeningLoop()
{

	bool listening = false;
	spdlog::info("Ready....");

	short audioBuffer[2048];

	//ad_read(m_pDevice, audioBuffer, 2048); //consume .128 seconds of audio

	while(m_running)
	{
		int k = ad_read(m_pDevice, audioBuffer, 2048);
		if (k < 0)
		{
			spdlog::error("Failed to read audio");
			return false;
		}

		ps_process_raw(m_pSpeechDecoder, audioBuffer, k, false, false);
		bool conainsSpeech = ps_get_in_speech(m_pSpeechDecoder);

		if (conainsSpeech && !listening)
		{
			listening = true;
			spdlog::info("Listening...");
		}

		if (!conainsSpeech && listening)
		{
			/* speech -> silence transition, time to start new utterance  */
			ps_end_utt(m_pSpeechDecoder);
			int score;
			char const* command = ps_get_hyp(m_pSpeechDecoder, &score);
			if (command != NULL) {
				spdlog::info("Command: {}, score: {}", command, score);// -1500 = good -2000 = ok -3000 = rubbish
			}

			// Starts afresh here...

			if (ps_start_utt(m_pSpeechDecoder) < 0)
			{
				spdlog::error("Failed to start utterance");
				return false;
			}

			listening = false;
			spdlog::info("Ready....\n");
		}

		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	return true;
}

bool TEF::Aurora::SpeechRecognition::VoreBuffer()
{
	const int ditchSize = 16384;
	//eat the buffer
	// snd_pcm_readi(pcm_capture_handle, inputBuffer, blksize / BYTESPERFRAME
	short ditchBuffer[ditchSize];
	int d = ad_read(m_pDevice, ditchBuffer, ditchSize);
	spdlog::debug("Vored {} samples from the buffer", d);
	while (d >= 2048)
	{
		d = ad_read(m_pDevice, ditchBuffer, ditchSize);
		spdlog::debug("Vored {} samples from the buffer", d);
	}
}
