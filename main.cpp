#include <stdio.h>
#include <chrono>
#include <thread>

class A {
public:
	virtual void MainLoop()
	{
		while (true)
		{
			printf("new stuff\n");
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};

	void RunMainLoop() {
		m_mainLoopThread = std::thread(&A::MainLoop, this);
	};

public:
	std::thread m_mainLoopThread;
};

class B : public A {
public:
	void MainLoop() override
	{
		while (true)
		{
			printf("new stuff\n");
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};
};



#include "tef/aurora/effects/simpleEffect.h"

int main(int argc, char** argv)
{
	TEF::Aurora::Effects::SimpleEffect se;

	se.StartMainLoop();

	std::this_thread::sleep_for(std::chrono::seconds(10));
}













//
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

