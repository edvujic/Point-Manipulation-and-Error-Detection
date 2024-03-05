#include "Utils.h"
#include <vector>
#include <sstream>

bool Utils::isHeaderLine(const std::string &line)
{
    const std::vector<std::string> headerKeywords = {
        "VERSION", "FIELDS", "SIZE", "TYPE", "COUNT",
        "WIDTH", "HEIGHT", "VIEWPOINT", "POINTS", "DATA", "FORMAT"};
    for (const auto &keyword : headerKeywords)
    {
        if (line.find(keyword) != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

bool Utils::checkFileExtension(const std::string &filename)
{
    if (filename.size() >= 3)
    {
        return filename.substr(filename.size() - 3) == ".pt";
    }
    return false;
}

bool Utils::checkVersion(const std::string &line)
{
    std::istringstream iss(line);
    std::string versionLabel;
    int version;

    if (iss >> versionLabel >> version && versionLabel == "VERSION")
    {
        char extraChar;
        if (iss >> extraChar)
        {
            return false;
        }
        return true;
    }
    return false;
}

bool Utils::checkFormat(const std::string &line)
{
    return (line == "FORMAT x y z" || line == "FORMAT x y z r g b");
}

bool Utils::checkData(const std::string &line)
{
    return (line == "DATA ascii");
}

bool Utils::checkPointsCount(const std::string &line, int &count)
{
    std::istringstream iss(line);
    std::string pointsLabel;

    if (iss >> pointsLabel >> count && pointsLabel == "POINTS" && count > 0)
    {
        return true;
    }
    return false;
}
