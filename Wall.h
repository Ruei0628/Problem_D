#ifndef WALL_H_INCLUDED
#define WALL_H_INCLUDED

#include "Block.h"
#include <vector>

using namespace std;

class Wall {
public:
  Wall(bool, double, double[2]);

  bool isVertical; // 0: horizontal, 1: vertical
  double fixedCoord;
  double rangeCoord[2];

  vector<Wall> allWalls;

  vector<Wall> getBlockVertices(Block const &block);
  void translateIntoWalls();
};

#endif // WALL_H_INCLUDED