#include "tef/aurora/speechRecognition.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>


TEF::Aurora::SpeechRecognition::~SpeechRecognition()
{
	if (!IsConnected())
		return;

	m_recording = false;

	if (m_recordingThread.joinable())
		m_recordingThread.join();

	if (ad_close(m_pDevice) < 0)
	{
		spdlog::error("Speech Recognition failed to close the default audio device");
	}

	if (ps_free(m_pSpeechDecoder) != 0)
	{
		spdlog::error("Speech Recognition failed to free the speech decoder");
	}

	if (cmd_ln_free_r(m_pConfig) != 0)
	{
		spdlog::error("Speech Recognition failed to free the command line arguments");
	}
}

bool TEF::Aurora::SpeechRecognition::Connect()
{
	static const arg_t cont_args_def[] = { POCKETSPHINX_OPTIONS, {"-adcdev", ARG_STRING, NULL, "Name of audio device to use for input."}, CMDLN_EMPTY_OPTION };
	m_pConfig = cmd_ln_init(NULL, cont_args_def, FALSE, "-inmic", "yes", NULL);
	ps_default_search_args(m_pConfig);

	if (m_pConfig == NULL)
	{
		spdlog::error("Speech Recognition failed to setup config ");
		return false;
	}

	m_pSpeechDecoder = ps_init(m_pConfig);

	if (m_pSpeechDecoder == NULL) {
		spdlog::error("Speech Recognition failed to initialise the speech decoder");
		return false;
	}

	m_pDevice = ad_open_dev(cmd_ln_str_r(m_pConfig, "-adcdev"), (int)cmd_ln_float32_r(m_pConfig, "-samprate"));

	if (m_pDevice == NULL)
	{
		spdlog::error("Speech Recognition Failed to open the default audio device");
		return false;
	}
	return true;
}

bool TEF::Aurora::SpeechRecognition::IsConnected()
{
	return m_pDevice != NULL;
}

bool TEF::Aurora::SpeechRecognition::Start()
{
	if (!IsConnected())
	{
		spdlog::error("Speech Recognition cannot start as it is not connected");
		return false;
	}
	m_recording = true;
	m_recordingThread = std::thread([this]() {RecordLoop(); });

	return true;
}

bool TEF::Aurora::SpeechRecognition::Stop()
{
	if (!IsConnected())
	{
		spdlog::error("Speech Recognition cannot stop as it is not connected");
		return false;
	}

	m_recording = false;

	if (m_recordingThread.joinable())
		m_recordingThread.join();

	if (ps_start_utt(m_pSpeechDecoder) < 0)
	{
		spdlog::error("Speech Recognition failed to start utterance");
		return false;
	}

	{
		std::scoped_lock lock(m_bufferMutex);

		if (ps_process_raw(m_pSpeechDecoder, m_audioBuffer, m_audioBufferFront, false, true) < 0)
		{
			spdlog::error("Speech Recognition failed to process the raw audio");
			return false;
		}

		if (!m_audioFilepathDebug.empty())
		{
			SaveBuffer(m_audioFilepathDebug);
		}
	}

	if (ps_end_utt(m_pSpeechDecoder) < 0)
	{
		spdlog::error("Speech Recognition failed to stop utterance");
		return false;
	}

	int score;
	char const* command = ps_get_hyp(m_pSpeechDecoder, &score);
	if (command == NULL) {
		spdlog::warn("Speech Recognition failed to recognise any audio");
		return false;
	}

	spdlog::debug("Speech Recognition command: {}, score: {}", command, score); // -1500 = good -2000 = ok -3000 = rubbish

	if (!m_commandCallback)
	{
		spdlog::warn("Speech Recognition has no command handler");
		return true;
	}

	m_commandCallback(std::string(command));

	return true;
}

bool TEF::Aurora::SpeechRecognition::SetRecordFile(std::string filepath)
{
	m_audioFilepathDebug = filepath;
	return true;
}

bool TEF::Aurora::SpeechRecognition::SetJSGF(std::string jsgfFile)
{
	static const arg_t cont_args_def[] = { POCKETSPHINX_OPTIONS, {"-adcdev", ARG_STRING, NULL, "Name of audio device to use for input."}, CMDLN_EMPTY_OPTION };
	m_pConfig = cmd_ln_init(NULL, cont_args_def, FALSE, "-inmic", "yes", "-jsgf", jsgfFile.c_str(), NULL);
	ps_default_search_args(m_pConfig);

	if (m_pConfig == NULL)
	{
		spdlog::error("Speech Recognition failed to set config during JSGF refresh");
		return false;
	}

	if (ps_reinit(m_pSpeechDecoder, m_pConfig) != 0)
	{
		spdlog::error("Speech Recognition failed to re initialise the speech decoder after JSGF refresh");
		return false;
	}

	return true;
}

bool TEF::Aurora::SpeechRecognition::RegisterCommandCallback(std::function<bool(std::string)> cb)
{
	if (!cb)
	{
		spdlog::error("Speech Recognition failed to register command as callback is null");
		return false;
	}

	m_commandCallback = cb;
	return true;
}

bool TEF::Aurora::SpeechRecognition::RecordLoop()
{
	// This is hacky and disgusting and I really want to implement my own ALSA recording system as this is very sloppy
	// Main issue is when ad_stop_rec is called, it drops all pending frames instead of draining them
	// This is to clear any spurious remaining frames when the listening loop is called

	if (ad_start_rec(m_pDevice) < 0)
	{
		spdlog::error("Speech Recognition failed to start recording");
		return false;
	}
	//we assume that if we've started, we can stop and restart so these aren't checked.
	ad_stop_rec(m_pDevice);
	ad_start_rec(m_pDevice);

	{
		std::scoped_lock lock(m_bufferMutex);
		m_audioBufferFront = 0;
	}

	spdlog::debug("Speech Recognition recording started");

	while (m_recording)
	{
		{
			std::scoped_lock lock(m_bufferMutex);
			int newSamples = 0;

			while (newSamples == 0)
			{
				newSamples = ad_read(m_pDevice, &m_audioBuffer[m_audioBufferFront], 16000);
			}

			if (newSamples < 0)
			{
				spdlog::error("Speech Recognition failed to read audio");
				return false;
			}

			m_audioBufferFront += newSamples;

		}

		if (m_audioBufferFront + 16000 > m_sampleRate * m_maxRecordTime)
		{
			spdlog::warn("Speech Recognition has completely filled the audio buffer. Forcing recording stop");
			m_recording = false;
		}

		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}

	spdlog::debug("Speech Recognition recording ended");

	if (ad_stop_rec(m_pDevice) < 0)
	{
		spdlog::error("Speech Recognition failed to stop recording");
		return false;
	}

	return true;
}

void TEF::Aurora::SpeechRecognition::SaveBuffer(std::string fp)
{
	FILE* file = fopen(fp.c_str(), "a+");

	if (file == NULL)
	{
		spdlog::error("Speech Recognition cannot open buffer location {}", fp);
		return;
	}

	int bytesWritten = fwrite(m_audioBuffer, sizeof(short), m_audioBufferFront, file);
	if (bytesWritten != m_audioBufferFront)
	{
		spdlog::warn("Speech Recognition failed to write all bytes to disk {}/{} to {}", bytesWritten, m_audioBufferFront, fp);
	}

	if (fclose(file) != 0)
	{
		spdlog::warn("Speech Recognition failed to close the file {}", fp);
	}
}