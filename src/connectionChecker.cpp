#include <spdlog/spdlog.h>
#include <sstream>

#include "tef/aurora/connectionChecker.h"
#include "tef/aurora/cmd.h"


TEF::Aurora::ConnectionChecker::ConnectionChecker()
{
	SetFPS(1);
	m_subsystemStates.resize(8);
}

bool TEF::Aurora::ConnectionChecker::AddFuse(SmartFuse* smartFuse)
{
	m_pSmartFuse = smartFuse;

	std::vector<bool> channels;
	m_pSmartFuse->CheckConnected(channels);

	for (int i = 0; i < 8; i++)
		m_subsystemStates[i].power = channels[i];
	
	return true;
}

bool TEF::Aurora::ConnectionChecker::MainLoopCallback()
{
	if (m_pSmartFuse)
	{
		std::vector<bool> channels;
		m_pSmartFuse->CheckConnected(channels);

		for (int c = 0; c < 8; c++)
		{
			if (m_subsystemStates[c].power != channels[c])
			{
				spdlog::info("Subsystem {} power has been {}", c, channels[c] ? "connected" : "disconnected");
			}
			//update
			m_subsystemStates[c].power = channels[c];
		}
	}

	GetUSB();

	return false;
}

std::vector<std::string> TEF::Aurora::ConnectionChecker::GetUSB()
{
	std::vector<std::string> devices;
	CMD::CMD lsusb;
	lsusb.command = "lsusb";

	TEF::Aurora::CMD::execCommand(&lsusb);

	std::stringstream ss(lsusb.response);
	std::string to;

	while (std::getline(ss, to, '\n')) {
		spdlog::info("line: {}", to); // Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
	}

}
