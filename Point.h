#ifndef POINT_H_INCLUDE
#define POINT_H_INCLUDE

class Point {
public:
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}

    bool operator ==(const Point &other) const {
    	return (x == other.x && y == other.y);
    }
};

#endif // POINT_H_INCLUDE