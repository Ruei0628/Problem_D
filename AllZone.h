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

#endif // ALLZONE_H_INCLUDED