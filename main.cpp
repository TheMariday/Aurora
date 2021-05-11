#include <stdio.h>
#include <chrono>
#include <thread>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "tef/aurora/effects/simpleEffect.h"
#include "tef/aurora/sound.h"
#include "tef/aurora/masterController.h"

#define pause(x) std::this_thread::sleep_for(std::chrono::seconds(x));

class SomeClass {

public:
	static bool funcStatic() { printf("Hello from static CB!\n"); return true; }
	bool func() { printf("%s\n", member.c_str()); return true; }
	bool funcArgs(std::string arg) { printf("%s:%s\n", member.c_str(), arg.c_str()); }

private:
	std::string member = "Hello from non static member callback!";
};


bool func() { return printf("Hello from non member function!\n"); return true; }

bool Run(std::function<bool(void)> f) { return f(); }

class Caller
{
public:
	void Register(std::function<bool(void)> cb) { m_cb = cb; };
	bool Run() { return m_cb(); }
private:
	std::function<bool(void)> m_cb;
	std::string arg;
};


int main(int argc, char** argv)
{
	Caller caller;

	{
		caller.Register(func);
		printf(caller.Run() ? "success\n" : "failed\n");
	}

	{
		caller.Register(SomeClass::funcStatic);
		printf(caller.Run() ? "success\n" : "failed\n");
	}

	{
		SomeClass someClass;
		caller.Register(std::bind(&SomeClass::func, someClass));
		printf(caller.Run() ? "success\n" : "failed\n");
	}

	{
		SomeClass someClass;
		caller.Register(std::bind(&SomeClass::funcArgs, someClass, "argument"));
		printf(caller.Run() ? "success\n" : "failed\n");
	}

	sleep(10);
}

















/*
*
*
class SomeClass {

public:
	static std::string funcStatic() { return "Hello from static CB!"; }
	std::string func() { return member; }
	std::string funcArgs(std::string arg) { return member + arg; }

private:
	std::string member = "Hello from non static member callback!";
};


std::string func() { return "Hello from non member function!"; }

std::string Run(std::function<std::string(void)> f) { return f(); }

class Caller
{
public:
	void RegisterAndBind(std::function<std::string(void)> f) { m_cb = std::bind(f); };

	void Register(std::function<std::string(void)> cb) { m_cb = cb; };
	std::string Run() { return m_cb(); }
private:
	std::function<std::string(void)> m_cb;
	std::string arg;
};


int main(int argc, char** argv)
{
	Caller caller;

	{
		caller.RegisterAndBind(func);
		printf("%s\n", caller.Run().c_str());
	}

	{
		caller.RegisterAndBind(SomeClass::funcStatic);
		printf("%s\n", caller.Run().c_str());
	}

	{
		SomeClass someClass;
		caller.Register(std::bind(&SomeClass::func, someClass));
		printf("%s\n", caller.Run().c_str());
	}

	{
		SomeClass someClass;
		caller.Register(std::bind(&SomeClass::funcArgs, someClass, "argument"));
		printf("%s\n", caller.Run().c_str());
	}

	sleep(10);
spdlog::set_level(spdlog::level::debug);

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

