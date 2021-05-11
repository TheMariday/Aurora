#include "tef/aurora/userControl.h"
#include <spdlog/spdlog.h>

TEF::Aurora::UserControl::UserControl()
{
	SetFPS(0);
}

TEF::Aurora::UserControl::~UserControl()
{

}

bool TEF::Aurora::UserControl::RegisterCommand(Command command)
{
	if (std::find(m_commands.begin(), m_commands.end(), command) == m_commands.end())
	{
		spdlog::error("User Control cannot register command as command is already in database");
		return false;
	}

	m_commands.push_back(command);
	return true;
}

bool TEF::Aurora::UserControl::MainLoopCallback()
{
	std::string userInput;
	std::cout << "Enter command: ";
	std::cin >> userInput;
	if (!ProcessCommand(userInput))
	{
		spdlog::error("UserControl cannot process command");
		return false;
	}
	return true;
}

bool TEF::Aurora::UserControl::ProcessCommand(std::string command)
{
	return false;
}


void TEF::Aurora::Command::Register(void* context, std::function<bool(std::string)> function)
{
	context = context;
	callbackWArgs = function;
}

void TEF::Aurora::Command::Register(void* context, std::function<bool(void)> function)
{
	context = context;
	callback = function;
}

void TEF::Aurora::Command::Register(void* context, std::function<bool(std::string)> function, std::string argument)
{
	context = context;
	callbackWArgs = function;
	defaultArg = argument;
}

bool TEF::Aurora::Command::Execute()
{
	if (!defaultArg.empty() and callbackWArgs)
		return callbackWArgs(defaultArg);
	
	if (callback)
		return callback();

	return false;
}


bool TEF::Aurora::Command::Execute(std::string arg)
{
	if (!defaultArg.empty())
		return false;

	if (callbackWArgs)
		return callbackWArgs(arg);

	return false;
}
