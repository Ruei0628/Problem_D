#ifndef ALLZONE_H_INCLUDED
#define ALLZONE_H_INCLUDED

#include "Block.h"
#include "Region.h"
#include "Zone.h"

using namespace std;

class AllZone : public Zone {
public:
  AllZone(int const &testCase);
  vector<Zone *> totZone;

  void showAllZone();
  Block getBlock(string) const;
  Region getRegion(string) const;
  ~AllZone();
};

AllZone::AllZone(int const &testCase) {
  Block Blocks;
  Region Regions;

  Blocks.ParserAllBlocks(testCase);
  Regions.ParserAllRegions(testCase);

  for (Block b : Blocks.allBlocks) {
    totZone.push_back(new Block(b));
  }
  for (Region r : Regions.allRegions) {
    totZone.push_back(new Region(r));
  }
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

#endif // ALLZONE_H_INCLUDED