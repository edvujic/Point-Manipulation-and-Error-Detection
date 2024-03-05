#include <iostream>
#include <vector>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <fstream>
#include <sstream>

// Function prototypes
void listFiles();
bool arePointFilesSuitable();
void checkClosestAndFarthestPoints();
void identifyCornerPoints();
void specifySphereAndFindPoints();
void calculateAverageDistance();

bool promptRepeatMenu() {
    char repeat;
    std::cout << "\nWould you like to see the menu again? (y/n): ";
    std::cin >> repeat;
    return repeat == 'y' || repeat == 'Y';
}

bool checkFileExtension(const std::string& filename) {
    if (filename.size() >= 3) {
        return filename.substr(filename.size() - 3) == ".pt";
    }
    return false;
}

bool checkVersion(const std::string& line) {
    std::istringstream iss(line);
    std::string versionLabel;
    int version;

    if (iss >> versionLabel >> version && versionLabel == "VERSION") {
        return true;
    }
    return false;
}

bool checkFormat(const std::string& line) {
    return (line == "FORMAT x y z" || line == "FORMAT x y z r g b");
}

bool checkData(const std::string& line) {
    return (line == "DATA ascii");
}

bool checkPointsCount(const std::string& line, int& count) {
    std::istringstream iss(line);
    std::string pointsLabel;

    if (iss >> pointsLabel >> count && pointsLabel == "POINTS" && count > 0) {
        return true;
    }
    return false;
}

int main() {
    int choice = -1;

    do {
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

        switch (choice) {
            case 0:
                listFiles();
                break;
            case 1:
                if (arePointFilesSuitable()) {
                    std::cout << "Point files are suitable in format." << std::endl;
                } else {
                    std::cout << "Point files are not suitable in format." << std::endl;
                }
                break;
            case 2:
                checkClosestAndFarthestPoints();
                break;
            case 3:
                identifyCornerPoints();
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
void listFiles() {
    const char* directoryPath = "./point_sets";
    DIR* dir = opendir(directoryPath);
    if (dir == nullptr) {
        std::cerr << "Error opening directory: " << errno << std::endl;
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip the "." and ".." entries
        if (entry->d_name[0] != '.') {
            std::cout << entry->d_name << std::endl;
        }
    }
    
    closedir(dir);
}

bool arePointFilesSuitable() {
    const char* directoryPath = "./point_sets";
    DIR* dir = opendir(directoryPath);
    if (dir == nullptr) {
        std::cerr << "Error opening directory: " << errno << std::endl;
        return false;
    }
    
    dirent* entry;
    bool allFilesSuitable = true;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename(entry->d_name);
        if (filename[0] != '.' && checkFileExtension(filename)) {
            std::ifstream file(directoryPath + std::string("/") + filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                allFilesSuitable = false;
                continue;
            }

            std::string line;
            int lineCount = 0, pointCount = 0;
            bool validHeader = true;
            while (getline(file, line) && lineCount < 4) {
                switch (lineCount) {
                    case 0:
                        if (!checkVersion(line)) {
                            std::cerr << "Error in file " << filename << ": Invalid version format." << std::endl;
                            validHeader = false;
                        }
                        break;
                    case 1:
                        if (!checkFormat(line)) {
                            std::cerr << "Error in file " << filename << ": Invalid format, should be 'x y z' or 'x y z r g b'." << std::endl;
                            validHeader = false;
                        }
                        break;
                    case 2:
                        if (!checkPointsCount(line, pointCount)) {
                            std::cerr << "Error in file " << filename << ": Invalid points count." << std::endl;
                            validHeader = false;
                        }
                        break;
                    case 3:
                        if (!checkData(line)) {
                            std::cerr << "Error in file " << filename << ": Data type must be 'ascii'." << std::endl;
                            validHeader = false;
                        }
                        break;
                }
                if (!validHeader) {
                    break;
                }
                lineCount++;
            }

            if (!validHeader) {
                allFilesSuitable = false;
                file.close();
                continue;
            }

            // Check if the number of points matches the count specified in the header
            std::string pointLine;
            int actualPointsCount = 0;
            while (getline(file, pointLine) || !file.eof()) {
                if (file.fail() && !file.eof()) {
                    std::cerr << "Error reading file: " << filename << std::endl;
                    validHeader = false;
                    break;
                }
                
                if (!pointLine.empty() && !file.eof()) {
                    actualPointsCount++;
                } else if (file.eof()) {
                    // Handle the last line if it did not end with a newline
                    std::istringstream iss(pointLine);
                    std::string point;
                    if (iss >> point) { // Check if there's at least one value in the last line
                        actualPointsCount++;
                    }
                    break;
                }
            }

            if (actualPointsCount != pointCount) {
                std::cerr << "Error in file " << filename << ": Points count mismatch (expected "
                          << pointCount << ", got " << actualPointsCount << ")." << std::endl;
                allFilesSuitable = false;
            } else {
                std::cout << "File " << filename << " is suitable." << std::endl;
            }

            file.close();
        } else if (filename[0] != '.') {
            std::cerr << "Error: File " << filename << " does not have a .pt extension and will not be analyzed." << std::endl;
            allFilesSuitable = false;
        }
    }
    
    closedir(dir);
    return allFilesSuitable;
}
void checkClosestAndFarthestPoints() {
    // Implement closest and farthest points check
}

void identifyCornerPoints() {
    // Implement smallest cube corner points identification
}

void specifySphereAndFindPoints() {
    // Implement sphere specification and points within sphere identification
}

void calculateAverageDistance() {
    // Implement average distance calculation
}