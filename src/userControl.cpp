#include "tef/aurora/userControl.h"
#include <spdlog/spdlog.h>
#include <sstream>


std::vector<std::string> Split(std::string str)
{
	std::istringstream iss(str);
	std::vector<std::string> vec((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
	return vec;
}

TEF::Aurora::Command::Command(std::string type, std::string function) :
	m_type(type),
	m_function(function)
{

}

TEF::Aurora::Command::~Command()
{

}

bool TEF::Aurora::Command::Register(std::function<bool(void)> cb)
{
	m_cb = cb;
}

bool TEF::Aurora::Command::RegisterWArgs(std::function<bool(std::string)> cb)
{
	m_cbwArgs = cb;
}

bool TEF::Aurora::Command::SetExpectedArgs(std::vector<std::string> args)
{
	m_expectedArgs = args;
	return true;
}

bool TEF::Aurora::Command::Execute(std::string arg)
{
	if (arg.empty() and m_cb)
		return m_cb();

	if (m_cbwArgs)
		return m_cbwArgs(arg);

	spdlog::error("Command cannot be executed as register has not been called");
	return false;
}

bool TEF::Aurora::Command::IsArgValid(std::string arg)
{
	if (m_expectedArgs.empty()) 
		return true;

	bool valid = std::find(m_expectedArgs.begin(), m_expectedArgs.end(), arg) != m_expectedArgs.end();

	return valid;
}

TEF::Aurora::UserControl::UserControl()
{
	SetFPS(0);
}

TEF::Aurora::UserControl::~UserControl()
{

}

bool TEF::Aurora::UserControl::RegisterCommand(Command command)
{
	// You can currently push back the same command twice. Not sure if this is intentional
	/*
	if (std::find(m_commands.begin(), m_commands.end(), command) != m_commands.end()) {
		spdlog::error("User Control cannot register command as command is already registered");
		return false;
	}
	*/

	m_commands.push_back(command);
	return true;
}


bool TEF::Aurora::UserControl::MainLoopCallback()
{
	//wait for user input
	//pass to processcommand
	return false;
}

bool TEF::Aurora::UserControl::ProcessCommand(std::string commandStr)
{

	std::vector typeFuncArg = Split(commandStr);

	if (std::size(typeFuncArg) == 2)
		typeFuncArg.push_back("");

	if (std::size(typeFuncArg) != 3)
	{
		spdlog::error("User Control cannot process command as unexpected number of arguments");
		return false;
	}
	

	std::string type = typeFuncArg[0];
	std::string function = typeFuncArg[1];
	std::string arg = typeFuncArg[2];

	for (Command& command : m_commands)
	{
		if (command.m_type != type) continue;
		if (command.m_function != function) continue;
		if (arg.empty())
		{
			command.Execute();
		}
		else if (command.IsArgValid(arg))
		{
			command.Execute(arg);
		}
		
	}

	return true;
}
