#pragma once
#include "tef/aurora/effect.h"
#include <functional>
#include <vector>

/*
std::function<void(int,int)> f = std::bind(&Foo::doSomethingArgs, this, std::placeholders::_1, std::placeholders::_2);

*/

namespace TEF::Aurora {

	class Command {
	public:
		Command() = default;
		~Command() = default;
		
		// Register a function that expects a string
		void Register(void* context, std::function<bool(std::string)> function);
		// Register a function that expects nothing
		void Register(void* context, std::function<bool(void)> function);
		// Register a function with a default argument
		void Register(void* context, std::function<bool(std::string)> function, std::string argument);

		//Execute a command with no arguments
		bool Execute();
		//Execute a command with arguments
		bool Execute(std::string arg);

		std::string type;
		std::string function;
		std::string argument;

	private:
		std::function<bool(void)> callback;
		std::function<bool(std::string)> callbackWArgs;
		std::string defaultArg;
		void* context = nullptr;
	};

	class UserControl : public Effect {
	public:
		UserControl();
		~UserControl();

		bool RegisterCommand(Command command);

		bool MainLoopCallback() override;

		bool ProcessCommand(std::string command);

	private:

		std::vector<Command> m_commands;

	};
};
