#include "Wall.h"

Wall::Wall(bool IsVertical, double Fixed, double Range[2]) : isVertical(IsVertical), fixedCoord(Fixed){
	rangeCoord[0] = Range[0];
	rangeCoord[1] = Range[1];
}

pair<double, double> inOrder(double a, double b){
	double small = min(a, b);
	double big = max(a, b);
	return make_pair(small, big);
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
		double y[2] = {inOrder(y1, y2).first, inOrder(y1, y2).second};
		allWalls.push_back(Wall(1, x1, y));
      } else if (y1 == y2) {
        // then it should be the horizontal wall
        double x[2] = {inOrder(x1, x2).first, inOrder(x1, x2).second};
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