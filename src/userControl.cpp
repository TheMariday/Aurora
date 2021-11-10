#include "tef/aurora/userControl.h"
#include <spdlog/spdlog.h>
#include <fstream>


std::map<std::string, bool> boolOptions = {
	{"no", false}, {"false", false}, {"off", false}, {"disable", false},
	{"yes", true}, {"true",true}, {"on",true}, {"enable", true}
};

std::map<std::string, int> intOptions = {
	{"zero",0}, {"one",1}, {"two",2}, {"three",3}, {"four", 4},
	{"five",5}, {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}, {"ten", 10}
};

bool Split(std::string& command, std::string& argument)
{
	std::istringstream iss(command);
	std::vector<std::string> vec((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

	if (vec.size() < 2)
	{
		argument = "";
		return true;
	}

	argument = vec.back();

	size_t pos = command.find(argument);
	if (pos == std::string::npos)
		return false;

	command.replace(pos - 1, command.length(), ""); // the -1 here is to remove the space
	return true;
};


struct CommandVoid : public TEF::Aurora::Command
{
	CommandVoid(std::string command, std::function<std::string()>& callback, TEF::Aurora::LaunchAction launchAction) : m_callback(callback) {
		m_type = "void";
		m_command = command;
		m_launchAction = launchAction;
	};

	std::function<std::string()> m_callback;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		return true;
	}

	std::string Run() override 
	{
		return m_callback();
	}
};

struct CommandBool : public TEF::Aurora::Command
{
	CommandBool(std::string command, std::function<std::string(bool)>& callback, TEF::Aurora::LaunchAction launchAction) : m_callback(callback) {
		m_argMap = boolOptions;
		m_type = "bool";
		m_command = command;
		m_launchAction = launchAction;
	};

	std::function<std::string(bool)> m_callback;
	std::map<std::string, bool> m_argMap;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		for (auto const& [key, val] : m_argMap)
			valid.push_back(key);
		return true;
	}

	std::string Run() override 
	{
		return m_callback(m_argMap[m_arg]);
	}
};

struct CommandInt : public TEF::Aurora::Command
{
	CommandInt(std::string command, std::function<std::string(int)>& callback, TEF::Aurora::LaunchAction launchAction) :
		m_callback(callback) {
		m_argMap = intOptions;
		m_type = "int";
		m_command = command;
		m_launchAction = launchAction;
	};

	std::function<std::string(int)> m_callback;
	std::map<std::string, int> m_argMap;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		for (auto const& [key, val] : m_argMap)
			valid.push_back(key);
		return true;
	}

	std::string Run() override 
	{
		return m_callback(m_argMap[m_arg]);
	}
};

struct CommandStr : public TEF::Aurora::Command
{
	CommandStr(std::string command, std::function<std::string(std::string)>& callback, std::vector<std::string> argMap, TEF::Aurora::LaunchAction launchAction) :
		m_callback(callback),
		m_argMap(argMap) {
		m_type = "string";
		m_command = command;
		m_launchAction = launchAction;
	};

	std::function<std::string(std::string)> m_callback;
	std::vector<std::string> m_argMap;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		valid = m_argMap;
		return true;
	}

	std::string Run() override
	{
		return m_callback(m_arg);
	}

};


bool TEF::Aurora::UserControl::RegisterVoid(std::string command, LaunchAction launchAction, std::function<std::string()> callback)
{
	spdlog::debug("User Control registering void command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		spdlog::error("overwriting commands has been disabled");
		return false;
	}

	m_allCommands.emplace_back(std::make_shared<CommandVoid>(command, callback, launchAction));

	return true;
}


bool TEF::Aurora::UserControl::RegisterBool(std::string command, LaunchAction launchAction, std::function<std::string(bool)> callback)
{
	spdlog::debug("User Control registering bool command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		spdlog::error("overwriting commands has been disabled");
		return false;
	}

	m_allCommands.emplace_back(std::make_shared<CommandBool>(command, callback, launchAction));

	return true;
}


bool TEF::Aurora::UserControl::RegisterLimitedInt(std::string command, LaunchAction launchAction, std::function<std::string(int)> callback)
{
	spdlog::debug("User Control registering int command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		spdlog::error("overwriting commands has been disabled");
		return false;
	}

	m_allCommands.emplace_back(std::make_shared<CommandInt>(command, callback, launchAction));

	return true;
}


bool TEF::Aurora::UserControl::RegisterString(std::string command, std::vector<std::string> validArgs, LaunchAction launchAction, std::function<std::string(std::string)> callback)
{
	spdlog::debug("User Control registering string command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		spdlog::error("overwriting commands has been disabled");
		return false;
	}

	m_allCommands.emplace_back(std::make_shared<CommandStr>(command, callback, validArgs, launchAction));

	return true;
}


bool TEF::Aurora::UserControl::Unregister(std::string command)
{
	spdlog::debug("User Control unregistering command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		spdlog::error("User Control cannot unregister command {} as it does not exist", command);
		return false;
	}

	std::remove(m_allCommands.begin(), m_allCommands.end(), pCommand);

	return true;
}


bool TEF::Aurora::UserControl::FetchCommand(std::string inputString, std::shared_ptr<Command>& pCommand)
{
	spdlog::debug("User Control processing input '{}'", inputString);

	std::string argument;
	std::string command = inputString;

	// See if the command exists as it is
	if (FindCommand(command, pCommand)) {
		spdlog::debug("User Control recognised void command '{}'", inputString);
		return true;
	}

	// Nope so lets split it
	if (!Split(command, argument))
	{
		spdlog::error("User Control failed to split input '{}'", inputString);
		return false;
	}

	// And try again!
	if (FindCommand(command, pCommand)) {
		spdlog::debug("User Control recognised command '{}' '{}'", command, argument);
		pCommand->SetArg(argument);
		return true;
	}

	spdlog::error("User Control failed to find callback for command '{}'", inputString);
	return false;
}

bool TEF::Aurora::UserControl::FindCommand(std::string command, std::shared_ptr<Command>& pCommand)
{
	for (std::shared_ptr<Command> s : m_allCommands)
	{
		if (s->GetCommand() == command)
		{
			pCommand = s;
			return true;
		}
	}

	return false;
}

void TEF::Aurora::UserControl::GenerateJSGF(std::string& filepath)
{
	std::ofstream ss;

	ss.open(filepath.c_str(), std::ofstream::out | std::ofstream::trunc);

	ss << "#JSGF V1.0;\n";
	ss << "grammar highbeam;\n\n";

	std::vector<std::string> commands;
	for (std::shared_ptr<Command>& command : m_allCommands)
	{
		std::string commandString = command->GetCommand();
		commands.push_back(commandString);
		std::vector<std::string> validArgs;
		command->GetValidArgs(validArgs);
		ss << "<" << commandString << "> = " << commandString;
		if (validArgs.empty())
		{
			ss << ";\n";
		}
		else
		{
			ss << " ( ";
			for (unsigned int i = 0; i < validArgs.size() - 1; i++)
			{
				ss << validArgs[i] << " | ";
			}
			ss << validArgs.back() << " );\n";
		}

	}

	ss << "\n\n";
	ss << "public <main> = ";
	if (commands.size() == 1)
	{
		ss << "<" << commands[0] << ">;";
	}
	else
	{
		ss << "( ";
		for (unsigned int i = 0; i < commands.size() - 1; i++)
		{
			ss << "<" << commands[i] << "> | ";
		}
		ss << "<" << commands.back() << "> );";
	}

	ss.close();
}