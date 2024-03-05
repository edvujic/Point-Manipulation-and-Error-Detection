#include <iostream>
#include <vector>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include <iomanip> // For std::fixed and std::setprecision
#include <sstream> // For std::istringstream

struct Point
{
    double x, y, z;

    // Calculate Euclidean distance between two points
    double distanceTo(const Point &other) const
    {
        return std::sqrt((x - other.x) * (x - other.x) +
                         (y - other.y) * (y - other.y) +
                         (z - other.z) * (z - other.z));
    }
};

// Function prototypes
void listFiles();
std::vector<std::string> getSuitablePointFiles();
void checkClosestAndFarthestPoints(const std::vector<std::string> &files);
void identifyCornerPoints(const std::vector<std::string>& files);
void specifySphereAndFindPoints();
void calculateAverageDistance();

bool isHeaderLine(const std::string& line) {
    // List of possible header keywords
    const std::vector<std::string> headerKeywords = {"VERSION", "FIELDS", "SIZE", "TYPE", "COUNT", "WIDTH", "HEIGHT", "VIEWPOINT", "POINTS", "DATA", "FORMAT"};
    for (const auto& keyword : headerKeywords) {
        if (line.find(keyword) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool promptRepeatMenu()
{
    char repeat;
    std::cout << "\nWould you like to see the menu again? (y/n): ";
    std::cin >> repeat;
    return repeat == 'y' || repeat == 'Y';
}

bool checkFileExtension(const std::string &filename)
{
    if (filename.size() >= 3)
    {
        return filename.substr(filename.size() - 3) == ".pt";
    }
    return false;
}

bool checkVersion(const std::string &line)
{
    std::istringstream iss(line);
    std::string versionLabel;
    int version;

    // Read the label and the version number as an integer
    if (iss >> versionLabel >> version && versionLabel == "VERSION")
    {
        // Now check if there is any additional character after reading the integer
        // which would indicate that the version number is not a whole number.
        char extraChar;
        if (iss >> extraChar)
        {
            // Found an extra character after the version number.
            return false;
        }
        // Successfully read "VERSION" followed by a whole number and nothing else.
        return true;
    }
    return false;
}

bool checkFormat(const std::string &line)
{
    return (line == "FORMAT x y z" || line == "FORMAT x y z r g b");
}

bool checkData(const std::string &line)
{
    return (line == "DATA ascii");
}

bool checkPointsCount(const std::string &line, int &count)
{
    std::istringstream iss(line);
    std::string pointsLabel;

    if (iss >> pointsLabel >> count && pointsLabel == "POINTS" && count > 0)
    {
        return true;
    }
    return false;
}

int main()
{
    int choice = -1;

    std::vector<std::string> suitableFiles;
    do
    {
        std::cout << "Menu:\n"
                  << "0. List files present\n"
                  << "1. Check if point files are suitable in format\n"
                  << "2. Check the closest and farthest two points in each file\n"
                  << "3. Identify corner points of the smallest cube for all points\n"
                  << "4. Specify sphere and find points within sphere\n"
                  << "5. Calculate average distance between points\n"
                  << "9. Exit\n"
                  << "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 0:
            listFiles();
            break;
        case 1:
            suitableFiles = getSuitablePointFiles();
            for (const std::string &filePath : suitableFiles)
            {
                std::cout << "Suitable file: " << filePath << std::endl;
            }

            break;
        case 2:
            checkClosestAndFarthestPoints(suitableFiles);
            break;
        case 3:
            identifyCornerPoints(suitableFiles);
            break;
        case 4:
            specifySphereAndFindPoints();
            break;
        case 5:
            calculateAverageDistance();
            break;
        case 9:
            std::cout << "Exiting the program." << std::endl;
            return 0; // Exit the program immediately
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (promptRepeatMenu()); // This will repeat the menu if the user wants to

    return 0;
}
// Function definitions
void listFiles()
{
    const char *directoryPath = "./point_sets";
    DIR *dir = opendir(directoryPath);
    if (dir == nullptr)
    {
        std::cerr << "Error opening directory: " << errno << std::endl;
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        // Skip the "." and ".." entries
        if (entry->d_name[0] != '.')
        {
            std::cout << entry->d_name << std::endl;
        }
    }

    closedir(dir);
}

std::vector<std::string> getSuitablePointFiles()
{
    const char *directoryPath = "./point_sets";
    std::vector<std::string> suitableFiles;
    DIR *dir = opendir(directoryPath);
    if (dir == nullptr)
    {
        std::cerr << "Error opening directory: " << errno << std::endl;
        return suitableFiles;
    }

    dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filename(entry->d_name);
        if (filename[0] != '.' && checkFileExtension(filename))
        {
            std::ifstream file(directoryPath + std::string("/") + filename);
            if (!file.is_open())
            {
                std::cerr << "Error opening file: " << filename << std::endl;
                continue;
            }

            std::string line;
            int lineCount = 0, pointCount = 0;
            bool validHeader = true;
            while (getline(file, line) && lineCount < 4)
            {
                switch (lineCount)
                {
                case 0:
                    if (!checkVersion(line))
                    {
                        std::cerr << "Error in file " << filename << ": Invalid version format." << std::endl;
                        validHeader = false;
                    }
                    break;
                case 1:
                    if (!checkFormat(line))
                    {
                        std::cerr << "Error in file " << filename << ": Invalid format, should be 'x y z' or 'x y z r g b'." << std::endl;
                        validHeader = false;
                    }
                    break;
                case 2:
                    if (!checkPointsCount(line, pointCount))
                    {
                        std::cerr << "Error in file " << filename << ": Invalid points count." << std::endl;
                        validHeader = false;
                    }
                    break;
                case 3:
                    if (!checkData(line))
                    {
                        std::cerr << "Error in file " << filename << ": Data type must be 'ascii'." << std::endl;
                        validHeader = false;
                    }
                    break;
                }
                if (!validHeader)
                {
                    break;
                }
                lineCount++;
            }

            if (!validHeader)
            {
                file.close();
                continue;
            }

            // Check if the number of points matches the count specified in the header
            std::string pointLine;
            int actualPointsCount = 0;
            while (getline(file, pointLine) || !file.eof())
            {
                if (file.fail() && !file.eof())
                {
                    std::cerr << "Error reading file: " << filename << std::endl;
                    validHeader = false;
                    break;
                }

                if (!pointLine.empty() && !file.eof())
                {
                    actualPointsCount++;
                }
                else if (file.eof())
                {
                    // Handle the last line if it did not end with a newline
                    std::istringstream iss(pointLine);
                    std::string point;
                    if (iss >> point)
                    { // Check if there's at least one value in the last line
                        actualPointsCount++;
                    }
                    break;
                }
            }

            file.close();

            if (actualPointsCount == pointCount)
            {
                suitableFiles.push_back(directoryPath + std::string("/") + filename);
            }
        }
        else if (filename[0] != '.')
        {
            std::cerr << "Error: File " << filename << " does not have a .pt extension and will not be analyzed." << std::endl;
        }
    }

    closedir(dir);
    return suitableFiles;
}
void checkClosestAndFarthestPoints(const std::vector<std::string> &files)
{
    double maxDistance = std::numeric_limits<double>::min();
    double minDistance = std::numeric_limits<double>::max();
    Point maxPointA, maxPointB, minPointA, minPointB;

    for (const std::string &filename : files)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Could not open file: " << filename << std::endl;
            continue;
        }

        std::vector<Point> points;
        std::string line;
        while (getline(file, line))
        {
            std::istringstream iss(line);
            Point p;
            if (iss >> p.x >> p.y >> p.z)
            {
                points.push_back(p);
                // Skip the rest of the line in case there are RGB values or other extra data
                iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        file.close();

        // Calculate distances between all pairs of points
        for (size_t i = 0; i < points.size(); ++i)
        {
            for (size_t j = i + 1; j < points.size(); ++j)
            {
                double distance = points[i].distanceTo(points[j]);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    minPointA = points[i];
                    minPointB = points[j];
                }
                if (distance > maxDistance)
                {
                    maxDistance = distance;
                    maxPointA = points[i];
                    maxPointB = points[j];
                }
            }
        }
    }

    // Output the results
    std::cout << "Closest points: (" << minPointA.x << ", " << minPointA.y << ", " << minPointA.z
              << ") and (" << minPointB.x << ", " << minPointB.y << ", " << minPointB.z
              << ") with distance " << minDistance << std::endl;

    std::cout << "Farthest points: (" << maxPointA.x << ", " << maxPointA.y << ", " << maxPointA.z
              << ") and (" << maxPointB.x << ", " << maxPointB.y << ", " << maxPointB.z
              << ") with distance " << maxDistance << std::endl;
}

void identifyCornerPoints(const std::vector<std::string>& files) {
    for (const std::string& filename : files) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filename << std::endl;
            continue;
        }

        Point minPoint{std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
        Point maxPoint{std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()};

        std::string line;
        while (getline(file, line)) {
            if (!isHeaderLine(line)) { // Skip header lines
                std::istringstream iss(line);
                Point point;
                if (iss >> point.x >> point.y >> point.z) {
                    minPoint.x = std::min(minPoint.x, point.x);
                    minPoint.y = std::min(minPoint.y, point.y);
                    minPoint.z = std::min(minPoint.z, point.z);
                    maxPoint.x = std::max(maxPoint.x, point.x);
                    maxPoint.y = std::max(maxPoint.y, point.y);
                    maxPoint.z = std::max(maxPoint.z, point.z);
                }
            }
        }
        file.close();

        // Set the precision for floating-point values to three decimal places
        std::cout << std::fixed << std::setprecision(3);

        // Output the results for this file
        std::cout << "File: " << filename << std::endl;
        std::cout << "Smallest cube corner points:" << std::endl;
        std::cout << "(" << minPoint.x << ", " << minPoint.y << ", " << minPoint.z << ")" << std::endl;
        std::cout << "(" << maxPoint.x << ", " << minPoint.y << ", " << minPoint.z << ")" << std::endl;
        std::cout << "(" << minPoint.x << ", " << maxPoint.y << ", " << minPoint.z << ")" << std::endl;
        std::cout << "(" << maxPoint.x << ", " << maxPoint.y << ", " << minPoint.z << ")" << std::endl;
        std::cout << "(" << minPoint.x << ", " << minPoint.y << ", " << maxPoint.z << ")" << std::endl;
        std::cout << "(" << maxPoint.x << ", " << minPoint.y << ", " << maxPoint.z << ")" << std::endl;
        std::cout << std::endl;

        // Reset the precision if needed elsewhere with default behavior
        std::cout.unsetf(std::ios_base::fixed);
        std::cout.precision(6);
    }
}

void specifySphereAndFindPoints()
{
    // Implement sphere specification and points within sphere identification
}

void calculateAverageDistance()
{
    // Implement average distance calculation
}