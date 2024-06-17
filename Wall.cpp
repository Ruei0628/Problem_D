#include "Wall.h"

Wall::Wall(bool IsVertical, double Fixed, double Range[2]) : isVertical(IsVertical), fixedCoord(Fixed){
	rangeCoord[0] = Range[0];
	rangeCoord[1] = Range[1];
}

void Wall::getBlockVertices(Block const &block) {
  for (Block const &b : block.allBlocks) {
	if (b.is_feedthroughable) continue;
    for (int i = 0; i < b.vertices.size(); i++) {
      double x1 = b.vertices[i].first;
      double x2 = b.vertices[(i + 1) % b.vertices.size()].first;
      double y1 = b.vertices[i].second;
      double y2 = b.vertices[(i + 1) % b.vertices.size()].second;
      if (x1 == x2) {
		// then it should be the vertical wall
		double y[2] = {y1, y2};
		allWalls.push_back(Wall(1, x1, y));
      } else if (y1 == y2) {
        // then it should be the horizontal wall
        double x[2] = {x1, x2};
        allWalls.push_back(Wall(0, y1, x));
      } else {
		break; // this should not happend
	  }
    }
  }
}

bool compare(const Wall &a, const Wall &b) {
  return a.fixedCoord < b.fixedCoord;
}
void Wall::rearrangement() { sort(allWalls.begin(), allWalls.end(), compare); }