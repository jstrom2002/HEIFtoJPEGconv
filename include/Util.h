#pragma once
#include <string>
#include <vector>

namespace HEIFtoJPEG
{
	std::string replaceString(std::string str, std::string replace, std::string replacer);
	std::string toLowercase(std::string str);
	std::string getDirectory(std::string str);
	std::string getExtension(std::string str);
	std::string getFilename(std::string str);
}