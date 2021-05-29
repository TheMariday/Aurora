#include "tef/aurora/testSuite.h"

#include <spdlog/spdlog.h>
#include <atomic>
#include <string>

#include <tef/aurora/smartFuse.h>
#include <tef/aurora/masterController.h>
#include <tef/aurora/button.h>

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))

bool WaitFor(std::atomic_bool& b, int seconds = 10)
{
	for (int i = 0; i < 10 * seconds; i++)
	{
		Sleep(100);
		if (b) return true;
	}
	spdlog::error("Timeout reached");
	return false;
}

bool WaitForButton()
{
	TEF::Aurora::Button confirmButton(3);
	std::atomic_bool pressed = false;
	confirmButton.RegisterCallbackDown([&pressed]() {pressed = true; return true; });
	confirmButton.Run();
	if (!WaitFor(pressed)) return false;
	return true;
}

void TEF::Aurora::TestSuite::Test(std::string testName, std::function<bool()> func)
{
	TEF::Aurora::Sound headset("sysdefault:CARD=Device");
	headset.Run();

	{
		std::stringstream ss;
		ss << "starting " << testName << " test";
		headset.AddSpeech(ss.str(), true);
	}

	std::stringstream ss;
	if (func())
	{
		ss << testName << " test passed";
	}
	else
	{
		ss << "warning, " << testName << " test failed";
	}
	headset.AddSpeech(ss.str(), true);
}


bool TEF::Aurora::TestSuite::ButtonTest()
{
	for (int buttonId = 2; buttonId <= 3; buttonId++)
	{
		spdlog::debug("testing button {}", buttonId);

		TEF::Aurora::Button button(buttonId);

		std::atomic_bool pressed = false;
		std::atomic_bool released = false;

		button.RegisterCallbackDown([&pressed]() {pressed = true; return true; });
		button.RegisterCallbackUp([&released]() {released = true; return true; });

		button.Run();

		spdlog::debug("please press and release button {}", buttonId);

		if (!WaitFor(pressed)) return false;

		spdlog::debug("button {} pressed", buttonId);

		if (!WaitFor(released)) return false;

		spdlog::debug("button {} released", buttonId);
	}

	return true;
}

bool TEF::Aurora::TestSuite::SoundTest()
{
	TEF::Aurora::Sound headset("sysdefault:CARD=Device");
	headset.Run();

	headset.AddSpeech("Press the confirm button if you can hear me");

	if (!WaitForButton()) return false;

	headset.AddSpeech("Playing audio file for 2 seconds, press the confirm if you can hear it", true);
	std::string testAudio = "/home/pi/media/test.wav";
	headset.PlayAudio(testAudio);
	Sleep(2000);
	headset.StopAudio(testAudio);

	if (!WaitForButton()) return false;

	return true;
}

bool TEF::Aurora::TestSuite::UserControlTest()
{
	TEF::Aurora::UserControl userControl;

	std::shared_ptr<Command> command;
	std::atomic hit = false;
	{
		hit = false;
		userControl.RegisterVoid("void test", [&hit]() {hit = true; return true; });

		userControl.FetchCommand("void test", command);
		if (!command->Run()) return false;
		if (!hit) return false;
	}

	{
		hit = false;
		userControl.RegisterString("argument test", { "success","other" }, [&hit](std::string s) {hit = s == "success"; return true; });

		userControl.FetchCommand("argument test success", command);
		command->Run();
		if (!hit) return false;
	}

	return true;
}

bool TEF::Aurora::TestSuite::MasterControllerTest()
{
	TEF::Aurora::MasterController master;

	std::atomic_bool hit = false;

	master.GetUserControl()->RegisterVoid("passed test", [&hit]() {hit = true;  return true; });

	master.GetUserControl()->RegisterVoid("failed test", [&hit]() {hit = false;  return true; });

	master.GetSound()->AddSpeech("Please say passed test to continue");

	master.Start();

	if (!WaitFor(hit)) return false;

	return true;
}

bool TEF::Aurora::TestSuite::SmartFuseTest()
{
	TEF::Aurora::SmartFuse smartFuse;
	smartFuse.Connect();
	Sleep(1000);
	smartFuse.Print();
	smartFuse.SetFet(0, true);
	Sleep(1000);

	bool fetState;
	smartFuse.GetFet(0, fetState);
	if (!fetState) return false;

	return true;
}

bool TEF::Aurora::TestSuite::AutoTest()
{

	TEF::Aurora::Sound notify("sysdefault:CARD=Device");

	Test("Button", ButtonTest);

	Test("Sound", SoundTest);

	Test("User Control", UserControlTest);

	Test("Master Controller", MasterControllerTest);

	Test("Smart Fuse", SmartFuseTest);

	return true;
}
