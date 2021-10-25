#pragma once
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <memory>

namespace TEF::Aurora {

	class Command
	{
	public:
		virtual bool GetValidArgs(std::vector<std::string>& valid) = 0;
		virtual bool Run() = 0;
		std::string GetType() { return m_type; };
		std::string GetCommand() { return m_command; }

		void SetArg(std::string arg) { m_arg = arg; };
		std::string GetArg() { return m_arg; };

		std::string GetCommandAndArgs() {
			std::stringstream ss;
			ss << GetCommand() << " " << GetArg();
			return ss.str();
		}

		void SetConfirmationRequired(bool requiresConfirm) { m_requiresConfirmation = requiresConfirm; };
		bool IsConfirmationRequired() { return m_requiresConfirmation; };

	protected:
		std::string m_command;
		std::string m_arg;
		std::string m_type;
		bool m_requiresConfirmation;
	};

	const static std::map<std::string, bool> m_boolOptions = {
		{"no", false}, {"false", false}, {"off", false}, {"disable", false},
		{"yes", true}, {"true",true}, {"on",true}, {"enable", true}
	};

	const static std::map<std::string, int> m_intOptions = {
		{"zero",0}, {"one",1}, {"two",2}, {"three",3}, {"four", 4},
		{"five",5}, {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}, {"ten", 10}
	};

	class UserControl {
	public:
		UserControl();
		~UserControl();

		bool RegisterVoid(std::string command, std::function<bool()> cb = {}, bool requiresConfirmation = true);

		bool RegisterBool(std::string command, std::function<bool(bool)> cb = {}, bool requiresConfirmation = true);

		bool RegisterLimitedInt(std::string command, std::function<bool(int)> cb = {}, bool requiresConfirmation = true);

		bool RegisterString(std::string command, std::vector<std::string> validArgs, std::function<bool(std::string)> cb = {}, bool requiresConfirmation = true);

		bool RegisterString(std::string command, std::function<bool(std::string)> cb = {}, bool requiresConfirmation = true);

		void GenerateJSGF(std::string& filepath);

		bool FetchCommand(std::string inputString, std::shared_ptr<Command>& pCommand);

	private:
		bool FindCommand(std::string command, std::shared_ptr<Command>& pCommand);

		bool Unregister(std::string command);

		
		std::vector<std::shared_ptr<Command>> m_allCommands;

	};
}
