#pragma once
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <memory>

namespace TEF::Aurora {

	enum LaunchAction
	{
		CONFIRM,
		INSTANT,
		INSTANT_TRIGGER
	};

	class Command
	{
	public:
		virtual bool GetValidArgs(std::vector<std::string>& valid) = 0;
		virtual std::string Run() = 0;
		std::string GetType() { return m_type; };
		std::string GetCommand() { return m_command; }

		void SetArg(std::string arg) { m_arg = arg; };
		std::string GetArg() { return m_arg; };

		std::string GetCommandAndArgs() {
			std::stringstream ss;
			ss << GetCommand() << " " << GetArg();
			return ss.str();
		}

		LaunchAction GetLaunchAction() { return m_launchAction; };

	protected:
		std::string m_command;
		std::string m_arg;
		std::string m_type;
		LaunchAction m_launchAction;
	};

	class UserControl {
	public:
		UserControl() = default;
		~UserControl() = default;

		bool RegisterVoid(std::string command, LaunchAction launchAction, std::function<std::string()> cb);

		bool RegisterBool(std::string command, LaunchAction launchAction, std::function<std::string(bool)> cb);

		bool RegisterLimitedInt(std::string command, LaunchAction launchAction, std::function<std::string(int)> cb);

		bool RegisterString(std::string command, std::vector<std::string> validArgs, LaunchAction launchAction, std::function<std::string(std::string)> cb);

		void GenerateJSGF(std::string& filepath);

		bool FetchCommand(std::string inputString, std::shared_ptr<Command>& pCommand);

	private:
		bool FindCommand(std::string command, std::shared_ptr<Command>& pCommand);

		bool Unregister(std::string command);

		
		std::vector<std::shared_ptr<Command>> m_allCommands;

	};
}
