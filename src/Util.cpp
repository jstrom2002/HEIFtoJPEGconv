#include "Util.h"
#include <filesystem>

namespace HEIFtoJPEG
{
    std::string getDirectory(std::string str)
    {
        // Check and bail early.
        if (std::filesystem::is_directory(std::filesystem::path(str)) ||
            str.back() == '\\' || str.back() == '/')
            return str;

        if (str.find("/") != std::string::npos)
            str = str.substr(0, str.rfind("/") + 1);
        if (str.find("\\") != std::string::npos)
            str = str.substr(0, str.rfind("\\") + 1);

        // Fix filepath irregularities
        while (str[0] == '\\')
            str = str.substr(1);
        str = replaceString(str, "\\\\", "\\");

        return str;
    }
    std::string replaceString(std::string str, std::string replace, std::string replacer) {

        // Check and bail early.
        if (str.find(replace) == std::string::npos)
            return str;

        for (int i = 0; i <= str.length(); ++i)
        {
            std::string sub = str.substr(i, str.length());
            int pos = sub.find(replace) + i;
            if (sub.find(replace) == std::string::npos || pos < 0 || sub.find(replace) > str.length() - 1) {
                //break if done replacing
                i = str.length() + 1;
                pos = std::string::npos;//set position to NULL for string type
            }
            if (pos >= 0) {
                str.erase(pos, replace.length());
                str.insert(pos, replacer);
                i = pos + replacer.length() - 1;
            }
        }
        return str;
    }

    std::string toLowercase(std::string str)
    {
        for (size_t i = 0; i < str.length(); ++i) { str[i] = std::tolower(str[i]); }
        return str;
    }

    std::string getExtension(std::string str)
    {	// Extensions in Windows systems are case insensitive. This may require special handling for Unix/Linux.
        str = toLowercase(str);
        if (str.rfind(".") == std::string::npos)
            return "";
        str = str.substr(str.rfind("."));
        return str;
    }

    std::string getFilename(std::string str) {
        //if (str.find("\\") != std::string::npos) { str = replaceString(str, "\\", "/"); }
        while (str.rfind("/") != std::string::npos && str.length() > 0) {
            str = str.substr(str.rfind("/") + 1);
        }
        while (str.rfind("\\") != std::string::npos && str.length() > 0) {
            str = str.substr(str.rfind("\\") + 1);
        }
        return str;
    }
}