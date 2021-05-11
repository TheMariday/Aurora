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
		Command(std::string type, std::string function = "");
		~Command();

		// Register a function that expects a string
		bool Register(std::function<bool(void)> cb);
		bool RegisterWArgs(std::function<bool(std::string)> cb);

		bool SetExpectedArgs(std::vector<std::string> args);

		//Execute a command with arguments
		bool Execute(std::string arg = "");

		bool IsArgValid(std::string arg);

		//unused at the moment
		std::string m_type;
		std::string m_function;

	private:
		std::function<bool(void)> m_cb;
		std::function<bool(std::string)> m_cbwArgs;
		std::vector<std::string> m_expectedArgs;
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
