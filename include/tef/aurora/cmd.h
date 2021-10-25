#pragma once
#include <string>
#include <spdlog/spdlog.h>

namespace TEF::Aurora::CMD {

	struct CMD
	{
		std::string command = "";
		std::string response = "";
		int status = 0;
	};

	inline bool execCommand(CMD* cmd)
	{
		if (cmd->command.empty())
		{
			spdlog::error("Cannot call empty command");
			return false;
		}

		spdlog::debug("Command Executor: {}", cmd->command.c_str());
		cmd->status = 0;
		auto pPipe = ::popen(cmd->command.c_str(), "r");
		if (pPipe == nullptr)
		{
			spdlog::error("Command Executor cannot open pipe");
			return false;
		}

		std::array<char, 256> buffer;

		while (not std::feof(pPipe))
		{
			auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
			cmd->response.append(buffer.data(), bytes);
		}

		auto rc = ::pclose(pPipe);

		if (WIFEXITED(rc))
		{
			cmd->status = WEXITSTATUS(rc);
		}

		return true;
	}

	inline bool quickCommand(std::string command)
	{
		CMD cmd;
		cmd.command = command;
		return execCommand(&cmd);
	}

}