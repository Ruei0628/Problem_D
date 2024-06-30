#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED

#include "Block.h"
#include <vector>
#include <algorithm>

using namespace std;

class Wall : public Block{
public:
	Wall(){}
	Wall(bool, double, double[2], string);
	Wall(bool, double, double, double, string);

	bool isVertical; // 0: horizontal, 1: vertical
	double fixedCoord;
	double rangeCoord[2];

	vector<Wall> allWalls;

	void getBlockVertices(Block const &block);
	void rearrangement();
	bool inRange(double const &value) const;
};

#endif // WALL_H_INCLUDED3