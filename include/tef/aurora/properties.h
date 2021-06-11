#pragma once
#include <string>

namespace TEF::Aurora::Properties
{

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
};
