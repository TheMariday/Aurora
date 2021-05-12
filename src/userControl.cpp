#include "tef/aurora/userControl.h"
#include <spdlog/spdlog.h>
#include <type_traits>





bool Split(std::string& command, std::string& argument)
{
	std::istringstream iss(command);
	std::vector<std::string> vec((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

	if (std::size(vec) < 2)
	{
		argument = "";
		return true;
	}

	argument = vec.back();

	size_t pos = command.find(argument);
	if (pos == -1)
		return false;

	command.replace(pos - 1, command.length(), ""); // the -1 here is to remove the space
	return true;
};


struct TEF::Aurora::Command
{
	virtual bool isValid(std::string arg) = 0;
	virtual bool run(std::string arg) = 0;
};


struct CommandVoid : public TEF::Aurora::Command
{

	CommandVoid(std::function<bool()>& c) : cb(c) {};

	std::function<bool()> cb;

	bool isValid(std::string arg) override { return arg.empty(); }

	bool run(std::string arg) override { return cb(); }
};


struct CommandBool : public TEF::Aurora::Command
{
	CommandBool(std::function<bool(bool)>& c, std::map<std::string, bool>& va) : cb(c), validArgs(va) {};

	std::function<bool(bool)> cb;
	std::map<std::string, bool> validArgs;

	bool isValid(std::string arg) { return validArgs.count(arg) == 1; }

	bool run(std::string arg) override { return cb(validArgs[arg]); }
};

struct CommandInt : public TEF::Aurora::Command
{
	CommandInt(std::function<bool(int)>& c, std::map<std::string, int>& va) : cb(c), validArgs(va) {};

	std::function<bool(int)> cb;
	std::map<std::string, int> validArgs;

	bool isValid(std::string arg) { return validArgs.count(arg) > 0; }

	bool run(std::string arg) override { return cb(validArgs[arg]); }
};


struct CommandStr : public TEF::Aurora::Command
{
	CommandStr(std::function<bool(std::string)>& c, std::vector<std::string>& va) : cb(c), validArgs(va) {};

	std::function<bool(std::string)> cb;
	std::vector<std::string> validArgs;

	bool isValid(std::string arg) override
	{
		return std::find(validArgs.begin(), validArgs.end(), arg) != validArgs.end();
	}

	bool run(std::string arg) override {return cb(arg);}
};


TEF::Aurora::UserControl::UserControl()
{
	SetFPS(0);
}

TEF::Aurora::UserControl::~UserControl()
{
	for (auto const& [mapCommand, mapCallback] : m_allCommands)
		delete m_allCommands[mapCommand];
}


bool TEF::Aurora::UserControl::RegisterVoid(std::string command, std::function<bool()> cb)
{
	spdlog::debug("User Control registering void command {}", command);

	if (m_allCommands.count(command) != 0)
	{
		spdlog::error("User Control cannot register command {} as it overrides an existing command", command);
		return false;
	}

	m_allCommands[command] = new CommandVoid(cb);

	return true;
}


bool TEF::Aurora::UserControl::RegisterBool(std::string command, std::function<bool(bool)> cb, std::map<std::string, bool> validArgs)
{
	spdlog::debug("User Control registering bool command {}", command);

	if (m_allCommands.count(command) != 0)
	{
		spdlog::error("User Control cannot register command {} as it overrides an existing command", command);
		return false;
	}

	m_allCommands[command] = new CommandBool(cb, validArgs);
	return true;
}


bool TEF::Aurora::UserControl::RegisterLimitedInt(std::string command, std::function<bool(int)> cb, std::map<std::string, int> validArgs)
{
	spdlog::debug("User Control registering int command {}", command);


	if (m_allCommands.count(command) != 0)
	{
		spdlog::error("User Control cannot register command {} as it overrides an existing command", command);
		return false;
	}

	m_allCommands[command] = new CommandInt(cb, validArgs);
	return true;
}


bool TEF::Aurora::UserControl::RegisterString(std::string command, std::vector<std::string> validArgs, std::function<bool(std::string)> cb)
{
	spdlog::debug("User Control registering string command {}", command);

	if (m_allCommands.count(command) != 0)
	{
		spdlog::error("User Control cannot register command {} as it overrides an existing command", command);
		return false;
	}

	m_allCommands[command] = new CommandStr(cb, validArgs);
	return true;
}


bool TEF::Aurora::UserControl::Unregister(std::string command)
{
	spdlog::debug("User Control unregistering command {}", command);

	if (m_allCommands.count(command) == 0)
	{
		spdlog::error("User Control cannot unregister command {} as it does not exist", command);
		return false;
	}

	delete m_allCommands[command];
	m_allCommands.erase(command);

	return true;
}


bool TEF::Aurora::UserControl::ProcessCommand(std::string inputString)
{
	spdlog::debug("User Control processing input '{}'", inputString);

	std::string argument;
	std::string command = inputString;

	if (m_allCommands.count(inputString) == 1)
	{
		spdlog::debug("User Control recognised void command '{}'", inputString);
		Command* callback = m_allCommands[inputString];
		return callback->run("");
	}

	if (!Split(command, argument))
	{
		spdlog::error("User Control failed to split input '{}'", inputString);
		return false;
	};

	for (auto const& [mapCommand, mapCallback] : m_allCommands)
	{
		if (command == mapCommand)
		{
			if (mapCallback->isValid(argument))
			{
				spdlog::debug("User Control recognised command '{}' '{}'", command, argument);
				return mapCallback->run(argument);
			}
			else
			{
				spdlog::error("User Control recognised command '{}' but argument '{}' is invalid", command, argument);
				return false;
			}
		}
	}

	spdlog::error("User Control failed to find callback for command '{}'", inputString);
	return false;
}

bool TEF::Aurora::UserControl::MainLoopCallback()
{
	std::string input;
	std::cout << "Command: ";

	getline(std::cin, input);
	ProcessCommand(input);
}
