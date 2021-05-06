#include <stdio.h>
#include <chrono>
#include <thread>
#include <vector>

#include "tef/aurora/effects/simpleEffect.h"
#include "tef/aurora/masterController.h"

int main(int argc, char** argv)
{
	TEF::Aurora::MasterController mc;

	TEF::Aurora::Effects::SimpleEffect se;
	mc.registerEffect(&se);

	mc.StartMainLoop();

	for (int i = 0; i < 10; i++)
	{
		printf("Master Utilisation: %.2f%\n", mc.GetUtilisation()*100);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}





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

