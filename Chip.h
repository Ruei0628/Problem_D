#ifndef CHIP_H_INCLUDED
#define CHIP_H_INCLUDED

#include "Block.h"
#include "Region.h"
#include "Edge.h"
#include "Zone.h"

#include "include\rapidjson\document.h"
#include <fstream>
#include <regex>
#include <sstream>

using namespace std;
using namespace rapidjson;

class Chip : public Zone {
public:
	Chip(int const &testCase);

	Point border;
	int UNITS_DISTANCE_MICRONS;
	
	vector<Zone*> totZone;
	vector<Edge> totEdge;
	
	Block getBlock(string) const;
	Region getRegion(string) const;
	void showAllZones() const;
	~Chip();
};

#endif // CHIP_H_INCLUDED