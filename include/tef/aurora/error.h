#pragma once
#include <string>
#include <sstream>
#include "spdlog/spdlog.h"

namespace TEF::Aurora {

	enum class ErrorType
	{
		Electrical,
		Battery,
		System,
		Unknown
	};

	enum class ErrorLevel
	{
		Critical,
		Warning,
		Info,
		Unknown
	};

	struct Error
	{
		Error(ErrorType type = ErrorType::Unknown, ErrorLevel level = ErrorLevel::Unknown, std::string details = "");

		std::string str();

		void log();

		ErrorType type;
		ErrorLevel level;
		std::string details;
	};
}