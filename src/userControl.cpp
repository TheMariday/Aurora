#include "tef/aurora/userControl.h"
#include <spdlog/spdlog.h>
#include <fstream>


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
	CommandVoid(std::string command, std::function<bool()>& callback, bool requiresConfirmation) : m_callback(callback) {
		m_type = "void";
		m_command = command;
		m_requiresConfirmation = requiresConfirmation;
	};

	std::function<bool()> m_callback;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		return true;
	}

	bool Run() override {

		if (!m_callback)
		{
			spdlog::warn("Command \"{}\" ({}) has no associated callback", m_command, m_type);
			return false;
		}

		return m_callback();
	}
};

struct CommandBool : public TEF::Aurora::Command
{
	CommandBool(std::string command, std::function<bool(bool)>& callback, std::map<std::string, bool> argMap, bool requiresConfirmation) : m_callback(callback), m_argMap(argMap) {
		m_type = "bool";
		m_command = command;
		m_requiresConfirmation = requiresConfirmation;
	};

	std::function<bool(bool)> m_callback;
	std::map<std::string, bool> m_argMap;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		for (auto const& [key, val] : m_argMap)
			valid.push_back(key);
		return true;
	}

	bool Run() override {

		if (!m_callback)
		{
			spdlog::warn("Command \"{}\" ({}) has no associated callback", m_command, m_type);
			return false;
		}

		return m_callback(m_argMap[m_arg]);
	}
};

struct CommandInt : public TEF::Aurora::Command
{
	CommandInt(std::string command, std::function<bool(int)>& callback, std::map<std::string, int> argMap, bool requiresConfirmation) :
		m_callback(callback),
		m_argMap(argMap) {
		m_type = "int";
		m_command = command;
		m_requiresConfirmation = requiresConfirmation;
	};

	std::function<bool(int)> m_callback;
	std::map<std::string, int> m_argMap;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		for (auto const& [key, val] : m_argMap)
			valid.push_back(key);
		return true;
	}

	bool Run() override {

		if (!m_callback)
		{
			spdlog::warn("Command \"{}\" ({}) has no associated callback", m_command, m_type);
			return false;
		}

		return m_callback(m_argMap[m_arg]);
	}
};

struct CommandStr : public TEF::Aurora::Command
{
	CommandStr(std::string command, std::function<bool(std::string)>& callback, std::vector<std::string> argMap, bool requiresConfirmation) :
		m_callback(callback),
		m_argMap(argMap) {
		m_type = "string";
		m_command = command;
		m_requiresConfirmation = requiresConfirmation;
	};

	std::function<bool(std::string)> m_callback;
	std::vector<std::string> m_argMap;

	bool GetValidArgs(std::vector<std::string>& valid) override
	{
		valid = m_argMap;
		return true;
	}

	bool Run() override
	{

		if (!m_callback)
		{
			spdlog::warn("Command \"{}\" ({}) has no associated callback", m_command, m_type);
			return false;
		}

		return m_callback(m_arg);
	}
};

TEF::Aurora::UserControl::UserControl()
{
}

TEF::Aurora::UserControl::~UserControl()
{
}


bool TEF::Aurora::UserControl::RegisterVoid(std::string command, std::function<bool()> callback, bool requiresConfirmation)
{
	spdlog::debug("User Control registering void command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		std::string existingType = pCommand->GetType(); // get type failed
		if (existingType != "void")
		{
			spdlog::error("You cannot overwrite command type ({}) with a different type. (void)", existingType);
			return false;
		}

		spdlog::warn("User Control overwriting command {}", command);
		CommandVoid* pCommandVoid = (CommandVoid*)pCommand.get();
		pCommandVoid->m_callback = callback;
		pCommandVoid->SetConfirmationRequired(requiresConfirmation);
	}
	else
	{
		m_allCommands.emplace_back(std::make_shared<CommandVoid>(command, callback, requiresConfirmation));
	}

	return true;
}


bool TEF::Aurora::UserControl::RegisterBool(std::string command, std::function<bool(bool)> callback, bool requiresConfirmation)
{
	spdlog::debug("User Control registering bool command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		std::string existingType = pCommand->GetType();
		if (existingType != "bool")
		{
			spdlog::error("You cannot overwrite command type ({}) with a different type. (bool)", existingType);
			return false;
		}

		spdlog::warn("User Control overwriting command {}", command);
		CommandBool* pCommandBool = (CommandBool*)pCommand.get();
		pCommandBool->m_callback = callback;
		pCommandBool->SetConfirmationRequired(requiresConfirmation);
	}
	else
	{
		m_allCommands.emplace_back(std::make_shared<CommandBool>(command, callback, m_boolOptions, requiresConfirmation));
	}

	return true;
}


bool TEF::Aurora::UserControl::RegisterLimitedInt(std::string command, std::function<bool(int)> callback, bool requiresConfirmation)
{
	spdlog::debug("User Control registering int command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		std::string existingType = pCommand->GetType();
		if (existingType != "int")
		{
			spdlog::error("You cannot overwrite command type ({}) with a different type. (int)", existingType);
			return false;
		}

		spdlog::warn("User Control overwriting command {}", command);
		CommandInt* pCommandInt = (CommandInt*)pCommand.get();
		pCommandInt->m_callback = callback;
		pCommandInt->SetConfirmationRequired(requiresConfirmation);

	}
	else
	{
		m_allCommands.emplace_back(std::make_shared<CommandInt>(command, callback, m_intOptions, requiresConfirmation));
	}

	return true;
}


bool TEF::Aurora::UserControl::RegisterString(std::string command, std::vector<std::string> validArgs, std::function<bool(std::string)> callback, bool requiresConfirmation)
{
	spdlog::debug("User Control registering string command {}", command);

	std::shared_ptr<Command> pCommand;
	if (FindCommand(command, pCommand))
	{
		std::string existingType = pCommand->GetType();
		if (existingType != "string")
		{
			spdlog::error("You cannot overwrite command type ({}) with a different type. (string)", existingType);
			return false;
		}

		if (!validArgs.empty())
		{
			std::vector<std::string> existingArgs;
			pCommand->GetValidArgs(existingArgs);
			if (existingArgs != validArgs)
			{
				spdlog::error("You cannot overwrite command with new string arguments");
				return false;
			}
		}

		spdlog::warn("User Control overwriting command {}", command);
		CommandStr* pCommandString = (CommandStr*)pCommand.get();
		pCommandString->m_callback = callback;
		pCommandString->SetConfirmationRequired(requiresConfirmation);

	}
	else
	{
		m_allCommands.emplace_back(std::make_shared<CommandStr>(command, callback, validArgs, requiresConfirmation));
	}

	return true;
}

bool TEF::Aurora::UserControl::RegisterString(std::string command, std::function<bool(std::string)> cb, bool requiresConfirmation)
{
	return RegisterString(command, {}, cb);
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