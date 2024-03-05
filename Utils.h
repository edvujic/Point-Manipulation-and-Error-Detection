#ifndef UTILS_H 
#define UTILS_H
#include <string>

class Utils
{
public:
    static bool isHeaderLine(const std::string &line);
    static bool checkFileExtension(const std::string &filename);
    static bool checkVersion(const std::string &line);
    static bool checkFormat(const std::string &line);
    static bool checkData(const std::string &line);
    static bool checkPointsCount(const std::string &line, int &count);
};

#endif // UTILS_H