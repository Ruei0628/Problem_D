#include "AllZone.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
  AllZone allZone(4);
  allZone.getBlock("BLOCK_25").showBlockInfo();
  allZone.getRegion("REGION_200").showRegionInfo();
  return 0;
}
