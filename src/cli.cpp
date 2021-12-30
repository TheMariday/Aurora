#include "tef/aurora/cli.h"
#include <iostream>
#include <string>

bool TEF::Aurora::CLI::MainLoopCallback()
{
	std::string cli_input;
	std::cout << "Aurora: ";
	std::getline(std::cin, cli_input);
	if (m_commandCallback)
	{
		if (cli_input == "yes" || cli_input == "y")
		{
			m_confirmCallback();
		}
		else
		{
			m_commandCallback(cli_input);
		}
		return true;
	}
	
	return false;
}
