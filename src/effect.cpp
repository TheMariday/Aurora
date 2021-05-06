#include "tef/aurora/effect.h"
#include <stdio.h>
#include <chrono>
#include <thread>

TEF::Aurora::Effect::Effect()
{
	printf("Effect created!\n");
	m_running = false;
}

TEF::Aurora::Effect::~Effect()
{
	printf("Effect destroyed\n");
}

bool TEF::Aurora::Effect::Load()
{
	printf("Effect loaded\n");
}

bool TEF::Aurora::Effect::Start()
{
	printf("Effect starting\n");
}

bool TEF::Aurora::Effect::Pause()
{
	printf("Effect paused\n");
}

bool TEF::Aurora::Effect::Stop()
{
	printf("Effect stopping\n");
}

bool TEF::Aurora::Effect::MainLoopCallback()
{
	printf("default mainloop callback\n");
	return true;
}

bool TEF::Aurora::Effect::MainLoop()
{
	while (m_running)
	{
		MainLoopCallback();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

bool TEF::Aurora::Effect::StartMainLoop()
{
	m_running = true;
	m_mainLoopThread = std::thread(&TEF::Aurora::Effect::MainLoop, this);
	return true;
}
