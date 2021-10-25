#pragma once
#include <string>
#include <optional>
#include <spdlog/spdlog.h>
#include <iostream>
#include "../lib/tomlplusplus/toml.hpp"

namespace TEF::Aurora::Properties
{
	// this all needs removing / cleaning
	template<class T>
	bool SaveProperty(T prop, std::string filename)
	{
		std::string filePath = std::string("/home/pi/Documents/Aurora/") + filename;
		FILE* fout = fopen(filePath.c_str(), "w");
		fwrite(&prop, sizeof(prop), 1, fout);
		fclose(fout);
		return true;
	};

	template<class T>
	bool LoadProperty(T& prop, std::string filename)
	{
		std::string filePath = std::string("/home/pi/Documents/Aurora/") + filename;
		FILE* fin = fopen(filePath.c_str(), "r");
		fread(&prop, sizeof(prop), 1, fin);
		fclose(fin);
		return true;
	};

	struct CurrentMatrix {
		float data[10][10][10] = { 0 };
	};


	template<class T>
	std::optional<T> GetProperty(std::string section, std::string key)
	{
		toml::table tbl;
		try
		{
			tbl = toml::parse_file("/home/pi/aurora_properties.toml");
			return tbl[section][key].value<T>();
		}
		catch (const toml::parse_error& err)
		{
			return {};
		}
	};

};
