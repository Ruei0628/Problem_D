#ifndef REGION_H_INCLUDED
#define REGION_H_INCLUDED

#include "Zone.h"
#include <iostream>

using namespace std;

class Region : public Zone {
public:
	Region();
	Region(Region const &);
	
	void showRegionInfo() const;
};

#endif // REGION_H_INCLUDED