#include "tef/aurora/speechRecognition.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>

TEF::Aurora::SpeechRecognition::SpeechRecognition()
{
	static const arg_t cont_args_def[] = { POCKETSPHINX_OPTIONS, {"-adcdev", ARG_STRING, NULL, "Name of audio device to use for input."}, CMDLN_EMPTY_OPTION };
	m_pConfig = cmd_ln_init(NULL, cont_args_def, FALSE, "-inmic", "yes", "-jsgf", "/home/pi/temp/pocketsphinx/test.gram", NULL);
	ps_default_search_args(m_pConfig);

	m_pSpeechDecoder = ps_init(m_pConfig);

	spdlog::debug("Setting up decoder");

	if (m_pSpeechDecoder == NULL) {
		spdlog::error("something is null");
		cmd_ln_free_r(m_pConfig);
	}

	spdlog::debug("Opening device");

	m_pDevice = ad_open_dev(cmd_ln_str_r(m_pConfig, "-adcdev"), (int)cmd_ln_float32_r(m_pConfig, "-samprate"));

	if (m_pDevice == NULL)
	{
		spdlog::error("Failed to open audio device");
	}

	m_listening = false;

}

TEF::Aurora::SpeechRecognition::~SpeechRecognition()
{
	spdlog::debug("Stopping recording");

	m_listening = false;

	if (m_listeningThread.joinable())
		m_listeningThread.join();

	spdlog::debug("Closing device");
	if (ad_close(m_pDevice) < 0)
	{
		spdlog::error("Failed to close device");
	}


	spdlog::debug("Destroying decoder");
	ps_free(m_pSpeechDecoder);
	cmd_ln_free_r(m_pConfig);
	spdlog::debug("Speech recognition destroyed correctly");
}

bool TEF::Aurora::SpeechRecognition::Start()
{
	spdlog::debug("starting");



	m_listening = true;
	m_listeningThread = std::thread([this]() {ListeningLoop(); });

	return true;
}

bool TEF::Aurora::SpeechRecognition::Stop(bool saveBuffer)
{
	spdlog::debug("stopping");
	m_listening = false;

	if (m_listeningThread.joinable())
		m_listeningThread.join();




	spdlog::debug("Starting utterance");

	if (ps_start_utt(m_pSpeechDecoder) < 0)
	{
		spdlog::error("Failed to start utterance");
		return false;
	}

	spdlog::debug("Processing speech");

	{
		std::scoped_lock lock(m_bufferMutex);

		if (ps_process_raw(m_pSpeechDecoder, m_audioBuffer, m_audioBufferFront, false, true) < 0)
		{
			spdlog::error("cannot process audio");
			return false;
		}

		if (saveBuffer)
		{
			spdlog::debug("Saving buffer");
			SaveBuffer();
		}
	}

	spdlog::debug("Ending utterance");

	if (ps_end_utt(m_pSpeechDecoder) < 0)
	{
		spdlog::error("Failed to stop utterance");
		return false;
	}
	spdlog::debug("Recognition complete");

	int score;
	char const* command = ps_get_hyp(m_pSpeechDecoder, &score);
	if (command != NULL) {
		spdlog::info("Command: {}, score: {}", command, score);// -1500 = good -2000 = ok -3000 = rubbish
	}

	return true;
}

bool TEF::Aurora::SpeechRecognition::ListeningLoop()
{
	spdlog::info("Listening loop started");

	{
		std::scoped_lock lock(m_bufferMutex);
		m_audioBufferFront = 0;
	}

	if (ad_start_rec(m_pDevice) < 0)
	{
		spdlog::error("Failed to start recording");
	}

	int samples = 1;


	//easy flush
	int c = 0;
	while (samples != 0)
	{
		samples = ad_read(m_pDevice, &m_audioBuffer[0], 16000);
		spdlog::debug("flushed {} frames", samples);
		c++;
	}


	spdlog::debug("listening...");

	while (m_listening)
	{
		{
			std::scoped_lock lock(m_bufferMutex);
			int newSamples = 0;
			while (newSamples == 0)
				newSamples = ad_read(m_pDevice, &m_audioBuffer[m_audioBufferFront], 16000);

			if (newSamples < 0)
			{
				spdlog::error("Failed to read audio\n");
				return false;
			}

			m_audioBufferFront += newSamples;

		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	if (ad_stop_rec(m_pDevice) < 0)
	{
		spdlog::error("Failed to stop recording\n");
		return false;
	}

	return true;
}

void TEF::Aurora::SpeechRecognition::SaveBuffer()
{
	FILE* file = fopen("/home/pi/projects/Aurora/bin/ARM/Debug/file.raw", "a+");
	fwrite(m_audioBuffer, sizeof(short), m_audioBufferFront, file);
	fclose(file);
}