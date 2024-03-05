#ifndef POINT_H
#define POINT_H

#include <cmath> // For std::sqrt

struct Point {
    double x, y, z;

    double distanceTo(const Point &other) const;
};

// Inline function definition
inline double Point::distanceTo(const Point &other) const {
    return std::sqrt((x - other.x) * (x - other.x) +
                     (y - other.y) * (y - other.y) +
                     (z - other.z) * (z - other.z));
}

#endif // POINT_H
