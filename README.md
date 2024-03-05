# Point Manipulation and Error Detection

## Overview
This C++ program analyzes collections of 3D point data stored in files within a designated directory, performing various calculations such as determining the closest and farthest point pairs, identifying corner points of the smallest containing cube, and finding points within a user-specified sphere.

## Features
- Lists all files in a specified directory.
- Validates point file formats and filters suitable files.
- Identifies closest and farthest point pairs in point files.
- Calculates the corner points of the smallest cube that contains all points.
- Finds points within a user-specified sphere.
- Computes the average distance between points in point files.
- Interactive menu for user to select operations.

## Dependencies
- C++ Standard Library
- dirent.h (for directory operations, may not be available on non-POSIX systems)

## Compilation
Use the following command to compile the program with g++:
```bash
g++ -o point_analyzer main.cpp Utils.cpp Point.h -std=c++11
```

## Usage
Run the compiled executable with:
```bash
./point_analyzer
```
Follow the on-screen instructions to navigate through the menu and choose the desired operations.

## File Structure
Ensure that your point data files are located within the ./point_sets directory relative to the executable. Each point file should have the .pt extension and follow the expected format.

## Expected Point File Format
The first four lines of each point file should contain the following headers:

- `Version`: A line beginning with version followed by the version number.
- `Format`: A line indicating the format, which should be either `x y z` or `x y z r g b`.
- `Points Count`: A line with the total number of points in the file.
- `Data Type`: A line specifying the data type, which must be `ascii`.
- Following the headers, each subsequent line should contain point data corresponding to the format specified in the headers.

## Contributing
Contributions to the project are welcome. Please follow the standard GitHub pull request process to submit your changes.

## Pseudocode

### List Files in a Directory

```pseudocode
FUNCTION listFilesInDirectory(directoryPath)
    filesList <- []
    directory <- OPEN directoryPath

    IF directory is NOT open THEN
        RETURN error("Directory cannot be opened")

    FOR EACH file IN directory DO
        IF file has extension ".pt" THEN
            filesList.APPEND(file)
        END IF
    END FOR

    CLOSE directory
    RETURN filesList
END FUNCTION
```
### Find the Closest Pair of Points
```pseudocode
FUNCTION findClosestPair(points)
    IF points.LENGTH < 2 THEN
        RETURN error("Not enough points to find a pair")

    closestDistance <- INFINITY
    closestPair <- (null, null)

    FOR i FROM 0 TO points.LENGTH - 1 DO
        FOR j FROM i + 1 TO points.LENGTH DO
            distance <- calculateDistance(points[i], points[j])
            IF distance < closestDistance THEN
                closestDistance <- distance
                closestPair <- (points[i], points[j])
            END IF
        END FOR
    END FOR

    RETURN closestPair
END FUNCTION
```
### Find Points Within a Specified Sphere

```pseudocode
FUNCTION findPointsWithinSphere(center, radius, points)
    pointsWithinSphere <- []

    FOR EACH point IN points DO
        distance <- calculateDistance(point, center)
        IF distance <= radius THEN
            pointsWithinSphere.APPEND(point)
        END IF
    END FOR

    RETURN pointsWithinSphere
END FUNCTION
```
### Calculate the Average Distance Between Points
```pseudocode
FUNCTION calculateAverageDistance(points)
    totalDistance <- 0
    count <- 0

    FOR i FROM 0 TO points.LENGTH - 1 DO
        FOR j FROM i + 1 TO points.LENGTH DO
            totalDistance <- totalDistance + calculateDistance(points[i], points[j])
            count <- count + 1
        END FOR
    END FOR

    IF count > 0 THEN
        averageDistance <- totalDistance / count
    ELSE
        averageDistance <- 0
    END IF
    RETURN averageDistance
END FUNCTION
```
### Identify Corner Points of Square

To find the corner points of the smallest axis-aligned cube (also known as an axis-aligned bounding box or AABB) that contains all points in a 3D space, you don't need a complex formula. The process involves finding the minimum and maximum coordinates along each axis (X, Y, and Z) among all the points in the dataset. The corner points of the cube can then be determined by combining these minimum and maximum values.

#### AABB Corner Points Formula

Given a set of points in 3D space, the corner points of the AABB are derived using the following:

- Let `P` be the set of all points `(x, y, z)`.
- Define `minPoint` as `(minX, minY, minZ)` where:
  - `minX = min({x | (x, y, z) ∈ P})`
  - `minY = min({y | (x, y, z) ∈ P})`
  - `minZ = min({z | (x, y, z) ∈ P})`
- Define `maxPoint` as `(maxX, maxY, maxZ)` where:
  - `maxX = max({x | (x, y, z) ∈ P})`
  - `maxY = max({y | (x, y, z) ∈ P})`
  - `maxZ = max({z | (x, y, z) ∈ P})`

The corner points of the AABB are the 8 unique combinations of `minX`, `minY`, `minZ`, `maxX`, `maxY`, and `maxZ`.

```pseudocode
FUNCTION identifyCornerPoints(files)
    FOR EACH filename IN files DO
        file <- OPEN filename for reading
        IF NOT file is open THEN
            OUTPUT "Could not open file: " + filename
            CONTINUE to next file
        END IF

        minPoint <- Point(MAX_VALUE, MAX_VALUE, MAX_VALUE)
        maxPoint <- Point(MIN_VALUE, MIN_VALUE, MIN_VALUE)

        WHILE NOT end of file DO
            line <- READ next line from file
            IF line is a data line THEN
                point <- EXTRACT point coordinates from line
                UPDATE minPoint and maxPoint with point
            END IF
        END WHILE
        CLOSE file

        OUTPUT corner points for filename using minPoint and maxPoint
    END FOR
END FUNCTION
```

