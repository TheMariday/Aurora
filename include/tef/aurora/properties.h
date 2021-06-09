#pragma once
#include <string>

namespace TEF::Aurora::Properties
{

	template<class T>
	bool SaveProperty(T prop)
	{
		std::string filename = std::string("/home/pi/Documents/Aurora/") + prop.filename;
		FILE* fout = fopen(filename.c_str(), "w");
		fwrite(&prop, sizeof(prop), 1, fout);
		fclose(fout);
		return true;
	};

	template<class T>
	bool LoadProperty(T& prop)
	{
		std::string filename = std::string("/home/pi/Documents/Aurora/") + prop.filename;
		FILE* fin = fopen(filename.c_str(), "r");
		fread(&prop, sizeof(prop), 1, fin);
		fclose(fin);
		return true;
	};

	struct CurrentMatrix {
		std::string filename = "CurrentMatrix.bin";
		float data[10][10][10] = {-1};
	};
};
