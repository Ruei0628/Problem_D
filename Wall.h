#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED

#include "Block.h"
#include <vector>
#include <algorithm>

using namespace std;

class Wall {
public:
  Wall(){}
  Wall(bool, double, double[2]);

  bool isVertical; // 0: horizontal, 1: vertical
  double fixedCoord;
  double rangeCoord[2];

  vector<Wall> allWalls;

  void getBlockVertices(Block const &block);
  void rearrangement();
};

#endif // WALL_H_INCLUDED