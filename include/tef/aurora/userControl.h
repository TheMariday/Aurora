#pragma once
#include "tef/aurora/effect.h"
#include <functional>
#include <map>
#include <sstream>
#include <spdlog/spdlog.h>

namespace TEF::Aurora {

	struct Command; // forward definition

	const static std::map<std::string, bool> m_boolOptions = {
		{"no", false}, {"false", false}, {"off", false}, {"disable", false},
		{"yes", true}, {"true",true}, {"on",true}, {"enable", true}
	};

	const static std::map<std::string, int> m_intOptions = {
		{"zero",0}, {"one",1}, {"two",2}, {"three",3}, {"four", 4},
		{"five",5}, {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}, {"ten", 10}
	};

	class UserControl : public Effect {
	public:
		UserControl();
		~UserControl();

		bool RegisterVoid(std::string command, std::function<bool()> cb);

		bool RegisterBool(std::string command, std::function<bool(bool)> cb, std::map<std::string, bool> validArgs = m_boolOptions);

		bool RegisterLimitedInt(std::string command, std::function<bool(int)> cb, std::map<std::string, int> validArgs = m_intOptions);

		bool RegisterString(std::string command, std::vector<std::string> validArgs, std::function<bool(std::string)> cb);

		bool Unregister(std::string command);

		bool ProcessCommand(std::string inputString);

		bool MainLoopCallback() override;


	private:


		std::map<std::string, Command*> m_allCommands;
	};
}
