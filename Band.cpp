#include "Band.h"

Band::Band(string terminalName, bool direction_isX, double min_X, double max_X, double min_Y, double max_Y, Band* parent) 
		  : zoneName(terminalName), direction_isX(direction_isX), toExtend_isX(!direction_isX),
		    min_X(min_X), max_X(max_X), min_Y(min_Y), max_Y(max_Y), parent(parent) {}

Band::Band(string terminalName, bool direction_isX, Point coord) 
		  : zoneName(terminalName), direction_isX(direction_isX), toExtend_isX(!direction_isX),
		   min_X(coord.x), max_X(coord.x), min_Y(coord.y), max_Y(coord.y), parent(nullptr) {
	if (direction_isX) {
		
	}
}

pair<double, double> Band::detectWall(vector<Wall> const walls, Point coord, bool direction_isX) const {
	double min = 0, max = 0;
	for (Wall const &w : walls) {
		if (direction_isX && w.isVertical && w.inRange(coord.y)) {
			if (w.fixedCoord > coord.x) {
				max = w.fixedCoord;
				break;
			}
			min = w.fixedCoord;
		}
		if (!direction_isX && !w.isVertical && w.inRange(coord.x)) {
			if (w.fixedCoord > coord.y) {
				max = w.fixedCoord;
				break;
			}
			min = w.fixedCoord;
		}
	}
	return make_pair(min, max);
}

bool Band::intersected(Band const *other) const{
	bool x_intersect = (min_X < other->min_X && other->min_X < max_X) || (other->min_X < min_X && min_X < other->max_X);
	bool y_intersect = (min_Y < other->min_Y && other->min_Y < max_Y) || (other->min_Y < min_Y && min_Y < other->max_Y);
	return x_intersect && y_intersect;
}

bool Band::operator ==(Band const &other) const {
	return min_X == other.min_X && max_X == other.max_X && min_Y == other.min_Y && max_Y == other.max_Y;
}

bool Band::alreadyExist(vector<Band*> bands) {
	for(Band* const b : bands) {
		if (this == b) return 1;
	}
}