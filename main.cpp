#include <spdlog/spdlog.h>
#include "tef/aurora/button.h"
#include "tef/aurora/userControl.h"
#include "tef/aurora/speechRecognition.h"
#include "tef/aurora/sound.h"

#define Sleep(x) std::this_thread::sleep_for(std::chrono::seconds(x))
/*
class ControlSystem
{
public:
	ControlSystem() :
		m_recordButton(2),
		m_confirmButton(3),
		m_headset("sysdefault:CARD=Device") {
	};
	~ControlSystem() = default;

	bool RegisterVoid(std::string command, std::function<bool()> cb)
	{
		m_userControl.RegisterVoid(command, [this, &command, &cb]() {
			PreloadCommand
				return true;
			});
		return true;
	}

	bool PreloadCommand()
	{
		m_loadedCommand = cmd
	}

	/*
	std::stringstream ss;
	ss << command << "?";
	m_headset.AddSpeech(ss);
	m_confirmButton.RegisterCallbackDown(cb);
	return true;
	

	bool RunCallback()
	{
		TEF::Aurora::Command* pCommand;
		bool commandRecognised = m_userControl.FetchCommand("command", pCommand);
		if (commandRecognised) {
			bool success = m_loadedCommand->run();
			if (success)
			{
				m_headset.AddSpeech("command success");
			}
			else
			{
				m_headset.AddSpeech("command failed");
			}

		}
		else
		{
			m_headset.AddSpeech("Cannot recognise command");
		}
		// run the last registered callback
		// 
	}


	bool Start() {
		m_speechRecognition.SetRecordFile("/home/pi/projects/Aurora/bin/ARM/Debug/raw.dat");

		RegisterVoid("cancel that", [this]() {
			m_confirmButton.RegisterCallbackDown();
			m_headset.AddSpeech("cancled command");
			return true;
			});

		std::string jsgfFilepath = "/home/pi/temp/pocketsphinx/test.gram";
		m_userControl.GenerateJSGF(jsgfFilepath);
		m_speechRecognition.SetJSGF(jsgfFilepath);

		m_recordButton.RegisterCallbackDown([this]() { return m_speechRecognition.Start(); });
		m_recordButton.RegisterCallbackUp([this]() { return m_speechRecognition.Stop(); });

		m_speechRecognition.RegisterCommandCallback([this](std::string command) {m_userControl.ProcessCommand(command); return true; });



		m_headset.StartMainLoop();
		m_recordButton.StartMainLoop();
		m_confirmButton.StartMainLoop();
		return true;
	}

	TEF::Aurora::Command* m_loadedCommand;


	TEF::Aurora::Button m_recordButton;
	TEF::Aurora::Button m_confirmButton;
	TEF::Aurora::UserControl m_userControl;
	TEF::Aurora::SpeechRecognition m_speechRecognition;
	TEF::Aurora::Sound m_headset;
};

*/

void Test() {

	TEF::Aurora::UserControl userControl;

	userControl.RegisterVoid("system reboot");
	userControl.RegisterVoid("system reboot", []() {return false; });
	userControl.RegisterVoid("system reboot", []() {spdlog::debug("system rebooted"); return true; });

	userControl.RegisterBool("system reboot"); //should fail

	userControl.RegisterString("set name to", { "bob", "rob" });
	userControl.RegisterString("set name to", [](std::string s) {spdlog::debug("setting name to {}", s); return true; });


	userControl.RegisterString("set name to", { "sam" }, [](std::string s) {spdlog::debug("setting name to {}", s); return true; }); //should fail
	userControl.RegisterString("set name to", { "sam" }); //should also fail

	TEF::Aurora::Command* command;
	userControl.FetchCommand("system reboot", command);
	command->Run();

	userControl.FetchCommand("set name to bob", command);
	command->Run();
}

/*
*
*
*
*
*

	ControlSystem cs;

	cs.RegisterVoid("system reboot", [&cs]() {cs.Notify("system rebooted"); return true; });

	cs.Notify("System starting up!");

	cs.Start();

	userControl.RegisterBool("set safety", [&headset](bool safety) {
		headset.AddSpeech(safety ? "safeties enabled, you are safe ish" : "safties off, you are not safe");
		return true;
		});
	userControl.RegisterLimitedInt("set brightness to", [&headset](int brightness) {
		std::stringstream ss;
		ss << "brightness set to " << brightness;
		headset.AddSpeech(ss);
		return true;
		});

	userControl.RegisterString("set name to", { "bob", "rob" }, [&headset](std::string name) {
		std::stringstream ss;
		ss << "Name set to " << name;
		headset.AddSpeech(ss);
		return true;
		});
*
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <vector>

	char* port = "/dev/ttyUSB0"; //Serial Device Address

	int levelSensor = serialOpen(port, 57600);
	wiringPiSetup();
	//serialPuts(levelSensor, "DP"); //Send command to the serial device

	while (1)
	{
		char buffer[100];
		ssize_t length = read(levelSensor, &buffer, sizeof(buffer));
		if (length == -1)
		{
			spdlog::error("Error reading from serial port");
			break;
		}
		else if (length == 0)
		{
			spdlog::error("No more data");
			break;
		}
		else
		{
			buffer[length] = '\0';
			std::cout << buffer; //Read serial data
		}
	}


	Sleep(1000);

	return 0;

* 	TEF::Aurora::Button button(2);
	TEF::Aurora::UserControl userControl;
	TEF::Aurora::SpeechRecognition speechRecognition;


	//userControl.RegisterVoid("system reboot", []() {spdlog::debug("system rebooted"); return true; });
	//userControl.RegisterBool("set safety", [](bool safety) {spdlog::debug("safety set to {}", safety); return true; });
	userControl.RegisterLimitedInt("set brightness to", [](int brightness) {spdlog::debug("brightess set to {}", brightness); return true; });
	//userControl.RegisterString("set name to", { "bob", "rob" }, [](std::string name) {spdlog::debug("name set to {}", name); return true; });

	std::string jsgfFilepath = "/home/pi/temp/pocketsphinx/test.gram";
	userControl.GenerateJSGF(jsgfFilepath);
	speechRecognition.SetJSGF(jsgfFilepath);

	speechRecognition.RegisterCommandCallback([&userControl](std::string command) {userControl.ProcessCommand(command); return true; });

	button.RegisterCallbackDown([&speechRecognition]() { return speechRecognition.Start(); });
	button.RegisterCallbackUp([&speechRecognition]() { return speechRecognition.Stop(); });

	button.StartMainLoop();

	//userControl.StartMainLoop(); this just enables cin


	TEF::Aurora::SpeechRecognition speechRecognition;
	TEF::Aurora::Button button(2);


	button.RegisterCallbackDown([&speechRecognition]() {
		spdlog::debug("Button down callback hit");
		return speechRecognition.Start();
		});
	button.RegisterCallbackUp([&speechRecognition]() {
		spdlog::debug("Button up callback hit");
		return speechRecognition.Stop();
		});

	button.StartMainLoop();

	speechRecognition.ListeningLoop();

	Sleep(1000);

	spdlog::debug("done");

	TEF::Aurora::SpeechRecognition sr;

	spdlog::debug("Device connected, sleeping");

	for (int i = 10; i > 0; i--)
	{
		spdlog::debug("Audio capture starting in {}", i);
		Sleep(1);
	}

	sr.Start();

	for (int i = 3; i > 0; i--)
	{
		spdlog::debug("Audio capture stopping in {}", i);
		Sleep(1);
	}

	sr.Stop();

struct SystemDummy
{
	bool Reboot() {
		spdlog::debug("rebooted");
		return true;
	}

	bool SetBrightness(int i) {
		spdlog::debug("setting brightness to {}", i);
		return true;
	}

	bool SetSafety(bool b)
	{
		spdlog::debug("setting safety to {}", b);
		return true;
	}

	bool SetName(std::string s) {
		spdlog::debug("setting name to {}", s);
		return true;
	}
};

*
*
*
	SystemDummy sys;

	TEF::Aurora::UserControl uc;

	std::atomic_bool running = true;

	uc.RegisterVoid("system reboot", [&sys]() {return sys.Reboot(); });
	uc.RegisterBool("set safety", [&sys](bool b) {return sys.SetSafety(b); });
	uc.RegisterLimitedInt("set brightness to", [&sys](int i) {return sys.SetBrightness(i); });
	uc.RegisterString("set name to", { "bob", "rob" }, [&sys](std::string s) {return sys.SetName(s); });

	uc.RegisterVoid("system stop", [&running]() {running = false; return true; });

	uc.StartMainLoop();

	spdlog::debug("spinning...");

	while (running) {
		sleep(1);
	}
* 	uc.RegisterVoid("system reboot", [&sys]() {return sys.Reboot(); });
	//uc.RegisterVoid("system reboot 2", [&sys]() {return sys.Reboot(); });
	uc.RegisterBool("system safety", [&sys](bool b) {return sys.SetSafety(b); });
	uc.RegisterLimitedInt("system set brightness to", [&sys](int i) {return sys.SetBrightness(i); });
	//uc.RegisterVoid("system set brightness to firve", [&sys]() {return sys.SetBrightness(5); });
	uc.RegisterString("system set name to", validNames, [&sys](std::string s) {return sys.SetName(s); });
	uc.RegisterString("system set narm to", validNames, [&sys](std::string s) {return sys.SetName(s); });

	uc.ProcessCommand("system"); // = false
	uc.ProcessCommand("bananas"); // = false
	uc.ProcessCommand("system reboot");
	uc.ProcessCommand("system reboot 2");
	uc.ProcessCommand("system bananas"); // = false
	uc.ProcessCommand("system safety on");
	uc.ProcessCommand("system safety disable");
	uc.ProcessCommand("system safety bananas"); // = false
	uc.ProcessCommand("system set brightness to five");
	uc.ProcessCommand("system set brightness to eleven"); // = false
	uc.ProcessCommand("system set name to bobby");
	uc.ProcessCommand("system set name to alex"); // = false
	TEF::Aurora::UserControl uc;

	TEF::Aurora::Command rebootCMD("system", "reboot");
	TEF::Aurora::Command setVarCMD("system", "setvar");
	TEF::Aurora::Command incrementByOneCMD("system", "increment");

	rebootCMD.Register([&sys]() {return sys.Reboot(); });
	setVarCMD.RegisterWArgs([&sys](std::string arg) {return sys.SetVar(std::stoi(arg)); });
	incrementByOneCMD.Register([&sys]() {return sys.incrementVar(1); });

	uc.RegisterCommand(rebootCMD);
	uc.RegisterCommand(setVarCMD);
	uc.RegisterCommand(incrementByOneCMD);

	uc.Register("System setvar %i", [&sys](std::string arg) {return sys.SetVar(std::stoi(arg)); })


	bool success;

	success = uc.ProcessCommand("system reboot");
	spdlog::debug(success ? "success" : "failed");

	success = uc.ProcessCommand("system setvar 12");
	spdlog::debug(success ? "success" : "failed");

	success = uc.ProcessCommand("system increment");
	spdlog::debug(success ? "success" : "failed");


TEF::Aurora::MasterController mc;

mc.GetExternalSound()->AddSpeech("starting", true);

TEF::Aurora::Effects::SimpleEffect se;

mc.registerEffect(&se);

se.StartMainLoop();

std::this_thread::sleep_for(std::chrono::seconds(10));
TEF::Aurora::Sound plantronics("sysdefault:CARD=Audio");
TEF::Aurora::Sound tail("");

//plantronics.StartMainLoop();
tail.StartMainLoop();
plantronics.StartMainLoop();

std::string longLorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
std::string shortLorem = "Lorem ipsum dolor sit amet";

tail.AddSpeech("Starting audio test", true);
plantronics.AddSpeech("Headset test connection", true);

tail.AddSpeech("Playing audio file for 2 seconds", true);

std::string testAudio = "/home/pi/media/test.wav";

tail.PlayAudio(testAudio);
pause(2);
tail.StopAudio(testAudio);

tail.AddSpeech("testing async audio and speech", true);
tail.PlayAudio(testAudio);
tail.AddSpeech(longLorem);
pause(2)
tail.StopAudio(testAudio);
tail.RemoveSpeech(longLorem);

tail.AddSpeech("Testing interrupts", true);

tail.AddSpeech(longLorem);
pause(2);
tail.InterruptSpeech("This is an interruption, resuming");
pause(5);
tail.RemoveSpeech(longLorem);

tail.AddSpeech("testing headset", true);

//stalls here

plantronics.AddSpeech("this is a headset test", true);

tail.AddSpeech("testing headset and tail", true);

plantronics.AddSpeech("this is a concurrent headset test");
tail.AddSpeech("this is a concurrent tail test");

plantronics.WaitFor();
tail.WaitFor();

tail.AddSpeech("Audio test complete", true);

//TEF::Aurora::MasterController mc;

//TEF::Aurora::Effects::SimpleEffect se;
//mc.registerEffect(&se);

//mc.StartMainLoop();

//for (int i = 0; i < 10; i++)
//{
//	printf("Master Utilisation: %.2f%\n", mc.GetUtilisation()*100);
//	std::this_thread::sleep_for(std::chrono::seconds(1));
//}
*/






//#include "tef/aurora/effectRunner.h"
//#include "lib/fadecandy/opc_client.h"
//#include "lib/svl/SVL.h"
//#include "lib/rapidjson/reader.h"


//rapidjson::ParseFlag pf;
//Vec3 v3;
//TEF::Aurora::Effects::SimpleEffect se;
//se.Start();
//OPCClient opc;

//TEF::Aurora::Effects::SimpleEffect se;

//se.Start();


//printf("herro!\n");
//std::this_thread::sleep_for(std::chrono::seconds(10));


int main(int argc, char* argv[])
{

	spdlog::set_level(spdlog::level::debug);

	Test();

	Sleep(1000);
	return 0;
}