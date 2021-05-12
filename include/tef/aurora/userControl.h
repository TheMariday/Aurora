#pragma once
#include "tef/aurora/effect.h"
#include <functional>
#include <map>
#include <sstream>
#include <spdlog/spdlog.h>

namespace TEF::Aurora {

	class UserControl {
	public:
		UserControl() {};
		~UserControl() {};


		bool RegisterVoid(std::string command, std::function<bool()> cb)
		{
			m_voidCommands[command] = cb;
		};

		bool RegisterBool(std::string command, std::function<bool(bool)> cb)
		{
			m_boolCommands[command] = cb;
		};

		bool RegisterLimitedInt(std::string command, std::function<bool(int)> cb)
		{
			m_intCommands[command] = cb;
		};

		bool RegisterString(std::string command, std::vector<std::string> options, std::function<bool(std::string)> cb)
		{
			m_stringCommands[command] = cb;
			m_stringOptions[command] = options;
		};

		bool ProcessCommand(std::string inputString)
		{
			if (m_voidCommands.count(inputString) == 1)
			{
				spdlog::debug("User Control recognised Void command: {}", inputString);
				return m_voidCommands[inputString]();
			}

			std::string argument;
			std::string command = inputString;
			if (!Split(command, argument)) {
				spdlog::warn("User Control failed to split command");
				return false;
			};

			if (m_boolCommands.count(command) == 1)
			{
				if (FindIndex(argument, m_trueStrings) != -1)
				{
					spdlog::debug("User Control recognised true command: {}", inputString);
					return m_boolCommands[command](true);
				}

				if (FindIndex(argument, m_falseStrings) != -1)
				{
					spdlog::debug("User Control recognised false command: {}", inputString);
					return m_boolCommands[command](false);
				}

				spdlog::warn("User Control recognised bool command but argument is invalid: {}", inputString);
				return false;

			}

			if (m_intCommands.count(command) == 1)
			{
				int arg = FindIndex(argument, m_numberStrings);
				if (arg == -1) {
					spdlog::warn("User Control recognised int command but argument is invalid: {}", inputString);
					return false;
				}

				spdlog::debug("User Control recognised int command: {}", inputString);
				return m_intCommands[command](arg);
			}

			if (m_stringCommands.count(command) == 1 && m_stringOptions.count(command) == 1)
			{
				std::vector<std::string> options = m_stringOptions[command];

				bool valid = FindIndex(argument, options) != -1;

				if (valid)
				{
					spdlog::warn("User Control recognised string command but argument is invalid: {}", inputString);
					return false;
				}

				spdlog::debug("User Control recognised string command: {}", inputString);
				return m_stringCommands[command](argument);

			}

			spdlog::warn("User Control does not recognise command: {}", inputString);
			return false;

		};



	private:

		int FindIndex(std::string& str, std::vector<std::string>& vec)
		{
			auto it = std::find(vec.begin(), vec.end(), str);

			if (it != vec.end())
				return it - vec.begin();

			return -1;
		}

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

			size_t pos = command.find(argument); // the -1 here is to remove the space
			if (pos == -1)
			{
				return false;
			}
			command.replace(pos - 1, command.length(), "");
			return true;

		}

		std::vector<std::string> m_falseStrings = { "no", "false", "off", "disable" };
		std::vector<std::string> m_trueStrings = { "yes", "true", "on", "enable" };
		std::vector<std::string> m_numberStrings = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten" };

		std::map<std::string, std::function<bool()>> m_voidCommands;
		std::map<std::string, std::function<bool(bool)>> m_boolCommands;
		std::map<std::string, std::function<bool(int)>> m_intCommands;
		std::map<std::string, std::function<bool(std::string)>> m_stringCommands;
		std::map<std::string, std::vector<std::string>> m_stringOptions;
	};

}
