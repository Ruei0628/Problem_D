#ifndef REGION_H_INCLUDED
#define REGION_H_INCLUDED

#include "Zone.h"
#include <string>
#include <utility>
#include <vector>

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

using namespace std;

class Region : public Zone {
public:
  Region();
  Region(Region const &);

  vector<Region> allRegions;

  void ParserAllRegions(int const &case_serial_number);
  void expandVertices();
  void divide_UNITS_DISTANCE_MICRONS();
  void showRegionInfo();
};

#endif // REGION_H_INCLUDED