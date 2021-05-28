#include <spdlog/spdlog.h>
#include "tef/aurora/testSuite.h"

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))


int main(int argc, char* argv[])
{
	TEF::Aurora::TestSuite::AutoTest();
}