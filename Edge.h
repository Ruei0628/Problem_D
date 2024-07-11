#ifndef EDGE_H_INCLUDED
#define EDGE_H_INCLUDED

#include "Point.h"
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Edge {
public:
	Edge(){}
	Edge(Point First, Point Second) : first(First), second(Second) {}

	Point first, second;

	bool isVertical() const { return first.x == second.x; }
	double fixed() const { return (isVertical() * first.x) + (!isVertical() * first.y); }
	Pair ranged() const { 
		if (isVertical()) { return Pair(first.y, second.y); }
		else { return Pair(first.x, second.x); }
	}

	bool is_feedthroughable;
	// not sure have an variable to record the charactor of this edge (like TBENN, BPR, etc.)

	bool inRange(double const &value) const { return ranged().min <= value && ranged().max >= value; }
};

#endif // EDGE_H_INCLUDED