#include "Band.h"

Band::Band(string terminalName, bool direction_isX, double min_x, double max_x, double min_y, double max_y, Band* parent) 
		  : zoneName(terminalName), direction_isX(direction_isX), toExtend_isX(!direction_isX),
		    x(min_x, min_y), y(max_x, max_y), parent(parent) {}

Band::Band(Terminal terminal, bool direction_isX) 
		  : zoneName(terminal.name), direction_isX(direction_isX), toExtend_isX(!direction_isX), parent(nullptr) {
	if (direction_isX) {
		Pair limit = this->detectEdge(edges, terminal.coord, direction_isX);
		x.min = limit.min;
		x.max = limit.max;
		y.min = y.max = terminal.coord.y;
	} else {
		Pair limit = this->detectEdge(edges, terminal.coord, direction_isX);
		y.min = limit.min;
		y.max = limit.max;
		x.min = x.max = terminal.coord.x;
	}
}

Pair Band::detectEdge(vector<Edge> const edges, Point coord, bool direction_isX) const {
	double min = 0, max = 0;
	for (Edge const &w : edges) {
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
	return Pair(min, max);
}

bool Band::intersected(Band const *other) const{
	bool x_intersect = !(x.min > other->x.max || x.max < other->x.min);
	bool y_intersect = !(y.min > other->y.max || y.max < other->y.min);
	return x_intersect && y_intersect;
}

bool Band::operator ==(Band const &other) const {
	return x.min == other.x.min && x.max == other.x.max && y.min == other.y.min && y.max == other.y.max;
}

bool Band::alreadyExist(vector<Band*> bands) {
	for(Band* const b : bands) {
		if (this == b) return 1;
	}
}