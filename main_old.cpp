#include <spdlog/spdlog.h>
#include "tef/aurora/connectionChecker.h"
#include <libusb-1.0/libusb.h>

#define Sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))


static void print_devs(libusb_device** devs)
{
	libusb_device* dev;
	int i = 0, j = 0;
	uint8_t path[8];

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return;
		}

		printf("%04x:%04x (bus %d, device %d)",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			printf(" path: %d", path[0]);
			for (j = 1; j < r; j++)
				printf(".%d", path[j]);
		}
		printf("\n");
	}
}



int main(int argc, char* argv[])
{
	spdlog::set_level(spdlog::level::debug);

	libusb_device** devs;
	int r;
	ssize_t cnt;

	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0) {
		libusb_exit(NULL);
		return (int)cnt;
	}

	print_devs(devs);
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
	return 0;

	TEF::Aurora::ConnectionChecker connectionChecker;

	connectionChecker.Run();

	/*
	TEF::Aurora::SmartFuse smartFuse;

	smartFuse.Connect();

	std::vector<bool> connections;
	smartFuse.CheckConnected(connections);

	for(int i = 0; i < 8; i++)
	{
		spdlog::info("Circuit {}: {}", i, connections[i]);
	}
	*/
	

	//TEF::Aurora::Safety safety;

	//safety.LoadCurrentMatrix();

	//safety.FilterCurrentMatrix();

	//safety.PrintCurrentMatrix();
	std::this_thread::sleep_for(std::chrono::seconds(100000));
	return true;

	//currently segfaulint when trying to delete cmLoad;
}

/*
*
*
void waitForEnter()
{
	do {} while (std::cin.get() != '\n');
}
* #include <chrono>
#include "tef/aurora/effectRunner.h"
#include "tef/aurora/effects/debugEffect.h"
#include "tef/aurora/smartFuse.h"
#include <shared_mutex>
#include <atomic>
#include "tef/aurora/testSuite.h"
#include "tef/aurora/safety.h"
#include <iostream>
* 	TEF::Aurora::Safety safety;
	safety.BuildCurrentMatrix();
	safety.PrintCurrentMatrix();
*/
/*
{
	pPaw->r = 10;
	Sleep(sleep);
	smartFuse.GetCurrent(7, r);
	spdlog::debug("Current: {}", r);
	Sleep(5000);
	pPaw->Black();
}

{
	pPaw->g = 10;
	Sleep(sleep);
	smartFuse.GetCurrent(7, g);
	spdlog::debug("Current: {}", g);
	Sleep(5000);
	pPaw->Black();
}

{
	pPaw->b = 10;
	Sleep(sleep);
	smartFuse.GetCurrent(7, b);
	spdlog::debug("Current: {}", b);
	Sleep(5000);
	pPaw->Black();
}
*/

/*
for (int i = 0; i <= 7; i++)
{
	pPaw->r = i;
	pPaw->g = i;
	Sleep(sleep);
	smartFuse.GetCurrent(7, r);

	pPaw->Black();

	pPaw->g = i;
	pPaw->b = i;
	Sleep(sleep);
	smartFuse.GetCurrent(7, g);

	pPaw->Black();

	pPaw->b = i;
	pPaw->r = i;
	Sleep(sleep);
	smartFuse.GetCurrent(7, b);

	pPaw->Black();

	std::cout << i << "," << r << "," << g << "," << b << std::endl;
}
*/



/*
void ShovelDemo()
{
	std::atomic_bool running = true;
	TEF::Aurora::MasterController masterController;
	TEF::Aurora::EffectRunner effectRunner("localhost");

	auto pPaw = std::make_shared<TEF::Aurora::Effects::PawEffect>();

	auto pCrap = std::make_shared<TEF::Aurora::Effects::PawEffect>();

	effectRunner.AddEffect(pPaw);
	effectRunner.AddEffect(pCrap);

	masterController.GetUserControl()->RegisterBool("rainbow effect", [&pCrap](bool running) {
		pCrap->m_running = running;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set red to", [&pPaw](int r) {
		pPaw->r = r;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set green to", [&pPaw](int g) {
		pPaw->g = g;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set blue to", [&pPaw](int b) {
		pPaw->b = b;
		return true;
		});

	masterController.GetUserControl()->RegisterLimitedInt("set all to", [&pPaw](int i) {
		pPaw->r = i;
		pPaw->g = i;
		pPaw->b = i;
		return true;
		});

	effectRunner.Run();
	masterController.Start();

	while (running)
		Sleep(1000);
}
*/
