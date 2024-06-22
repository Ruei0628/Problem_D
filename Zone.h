#ifndef ZONE_H_INCLUDED
#define ZONE_H_INCLUDED

#include "Point.h"

#include <string>
#include <utility>
#include <vector>

using namespace std;

class Zone {
public:
  Zone() {}
  string name;
  vector<Point> vertices;
  virtual ~Zone() {}
};

#endif // ZONE_H_INCLUDED