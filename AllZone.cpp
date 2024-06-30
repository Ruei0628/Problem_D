#include "AllZone.h"
#include "Wall.h"

using namespace std;

AllZone::AllZone(int const &testCase) {
	Block Blocks;
	Region Regions;

	Blocks.ParserAllBlocks(testCase);
	Regions.ParserAllRegions(testCase);
		
	for (Block b : Blocks.allBlocks) {
		Walls.getBlockVertices(b);
		totZone.push_back(new Block(b));
	}
	for (Region r : Regions.allRegions) {
		totZone.push_back(new Region(r));
	}
	Walls.allWalls.push_back(Wall(1, 0, 9, 31, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 8, 9, 24, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 14, 24, 27, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 19, 22, 25, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 23, 0, 19, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 29, 0, 14, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 35, 4, 8, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 35, 14, 19, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 41, 22, 25, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 44, 4, 25, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 45, 27, 31, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 48, 27, 31, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 54, 6, 25, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 57, 9, 12, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 58, 14, 18, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 60, 6, 31, "nonFeed"));
	Walls.allWalls.push_back(Wall(1, 66, 6, 28, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 4, 35, 44, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 6, 44, 54, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 6, 60, 66, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 8, 35, 44, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 9, 57, 60, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 12, 57, 60, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 14, 29, 35, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 14, 54, 58, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 18, 54, 58, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 9, 0, 8, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 19, 23, 35, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 22, 19, 41, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 24, 8, 14, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 25, 19, 41, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 25, 44, 54, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 27, 14, 45, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 27, 48, 60, "nonFeed"));
	Walls.allWalls.push_back(Wall(0, 48, 27, 31, "nonFeed"));

	Walls.allWalls.push_back(Wall(0, 5, 8, 23, "testS"));
	Walls.allWalls.push_back(Wall(0, 16, 8, 23, "testS"));
	Walls.allWalls.push_back(Wall(1, 8, 5, 16, "testS"));
	Walls.allWalls.push_back(Wall(1, 23, 5, 16, "testS"));
	
	Walls.allWalls.push_back(Wall(0, 4, 54, 60, "testT"));
	Walls.allWalls.push_back(Wall(0, 9, 54, 60, "testT"));
	Walls.allWalls.push_back(Wall(1, 54, 4, 9, "testT"));
	Walls.allWalls.push_back(Wall(1, 60, 4, 9, "testT"));
}

Block AllZone::getBlock(string blockName) const {
	for (Zone *z : totZone) {
		if (Block *bPtr = dynamic_cast<Block *>(z)) {
			if (bPtr->name == blockName)
				return *bPtr;
		}
	}
}

Region AllZone::getRegion(string regionName) const {
	for (Zone *z : totZone) {
		if (Region *rPtr = dynamic_cast<Region *>(z)) {
			if (rPtr->name == regionName) {
				return *rPtr;
			}
		}
	}
}

void AllZone::showAllZone() {
	for (Zone *z : totZone) {
		if (Block *bPtr = dynamic_cast<Block *>(z)) {
			bPtr->showBlockInfo();
		} else if (Region *rPtr = dynamic_cast<Region *>(z)) {
			rPtr->showRegionInfo();
		}
	}
}

AllZone::~AllZone() {
	for (Zone *z : totZone) {
		delete z;
	}
	totZone.clear();
}