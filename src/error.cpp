#include "tef/aurora/error.h"

TEF::Aurora::Error::Error(ErrorType t, ErrorLevel l, std::string d):
	type{ t },
	level{ l },
	details{ d }
{
}

std::string TEF::Aurora::Error::str()
{
	std::stringstream ss;

	switch (type)
	{
	case(ErrorType::Electrical):
		ss << "Electrical system ";
		break;
	case(ErrorType::Battery):
		ss << "Power system ";
		break;
	case(ErrorType::System):
		ss << "System ";
		break;
	case(ErrorType::Unknown):
		ss << "Unknown ";
		break;
	}

	switch (level)
	{
	case(ErrorLevel::Critical):
		ss << "critical, ";
		break;
	case(ErrorLevel::Warning):
		ss << "warning, ";
		break;
	case(ErrorLevel::Info):
		ss << "info, ";
		break;
	case(ErrorLevel::Unknown):
		ss << "notice, ";
		break;
	}

	ss << details;

	return ss.str();
}

void TEF::Aurora::Error::log()
{
	switch (level)
	{
	case(ErrorLevel::Critical):
		spdlog::error(str());
		break;
	case(ErrorLevel::Info):
		spdlog::info(str());
		break;
	case(ErrorLevel::Warning):
		spdlog::warn(str());
		break;
	default:
		spdlog::debug(str());
		break;
	}
}
