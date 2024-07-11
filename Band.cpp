#include "Band.h"

Band::Band(string terminalName, bool direction_isX, double min_x, double max_x, double min_y, double max_y, Band* parent) 
		  :zoneName(terminalName), direction_isX(direction_isX),
		   x(min_x, min_y), y(max_x, max_y), parent(parent) {
}

Band::Band(Pair X, Pair Y, int Level, Band *Parent) : x(X), y(Y), level(Level), parent(Parent) {
	// uses in extendBand
}

Band::Band(Terminal terminal, bool direction_isX, vector<Edge*> const &edges)
		  :zoneName(terminal.name), direction_isX(direction_isX), parent(nullptr) {
	if (direction_isX) {
		x = this->directionPair(edges, terminal.coord);
		y = Pair(terminal.coord.y, terminal.coord.y);
	} else {
		y = this->directionPair(edges, terminal.coord);
		x = Pair(terminal.coord.x, terminal.coord.x);
	}
}

bool Band::toExtend_isX() { return !direction_isX; }

Pair Band::directionPair(vector<Edge*> const OrderedEdges, Point coord) const {
	double min = 0, max = 0;
	for (Edge *w : OrderedEdges) {
		double TerminalFixed = direction_isX ? coord.y : coord.x;
		if (direction_isX == w->isVertical() && w->inRange(TerminalFixed)) {
			if (w->fixed() > TerminalFixed) {
				max = w->fixed();
				break;
			}
			min = w->fixed();
		}
	}
	return Pair(min, max);
}

Band* Band::extendBand(Pair x, Pair y) { return new Band(x, y, level + 1, this); }

bool Band::intersected(Band const *other) const{
	bool x_intersect = !(x.min > other->x.max || x.max < other->x.min);
	bool y_intersect = !(y.min > other->y.max || y.max < other->y.min);
	return x_intersect && y_intersect;
}

bool Band::operator ==(Band const &other) const {
	return x.min == other.x.min && x.max == other.x.max && y.min == other.y.min && y.max == other.y.max;
}

bool Band::alreadyExist(vector<Band*> bands) {
	for(Band* const b : bands) { if (this == b) return 1; }
}