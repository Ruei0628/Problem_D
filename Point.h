#ifndef POINT_H_INCLUDE
#define POINT_H_INCLUDE

#include <algorithm>

class Point {
public:
    Point(double x = 99, double y = 99) : x(x), y(y) {}
    double x, y;
    bool operator ==(const Point &other) const {
    	return (x == other.x && y == other.y);
    }
};

class Pair {
public:
	Pair() : min(0), max(0) {}
	Pair(double Min, double Max) {
		// Min should smaller than Max, 
		// but in case we still check that
		this->min = std::min(Min, Max);
		this->max = std::max(Min, Max);
	}
	double min, max;
	void operator =(Pair const &other) {
		this->min = other.min;
		this->max = other.max;
	}
};

#endif // POINT_H_INCLUDE