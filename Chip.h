#ifndef CHIP_H_INCLUDED
#define CHIP_H_INCLUDED

#include "Block.h"
#include "Region.h"
#include "Wall.h"
#include "Zone.h"

using namespace std;

class Chip : public Zone {
public:
	Chip(int const &testCase);
	vector<Zone *> totZone;

	double border_X;
	double border_Y;
	
	Wall Walls;	
	void showAllZones();
	Block getBlock(string) const;
	Region getRegion(string) const;
	~Chip();
};

#endif // CHIP_H_INCLUDED