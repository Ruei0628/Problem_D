#ifndef BAND_H_INCLUDED
#define BAND_H_INCLUDED

#include "Chip.h"

class Band {
public: 
	Band(){}
	Band(string, bool, double, double, double, double, Band*);
	Band(string, bool, Point);
	string zoneName = "";

	Band *parent = nullptr;
	int level = 0;
	bool direction_isX;
	bool toExtend_isX; // oppsite of direction_isX (!direction_isX)

	double min_X, max_X, min_Y, max_Y;

	vector<Band> toExtendPaths;

	// member fuctions
	Band *extendedBand(double dx, double dy, int lv);
	pair<double, double> detectWall(vector<Wall> const walls, Point coord, bool direction_isX) const;
	bool intersected(Band const *other) const;
	bool operator ==(Band const &other) const;
	bool alreadyExist(vector<Band*> bands);
};

#endif // BAND_H_INCLUDED