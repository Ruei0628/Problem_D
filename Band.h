#ifndef BAND_H_INCLUDED
#define BAND_H_INCLUDED

#include "Chip.h"
#include "Net.h"

class Band {
public: 
	Band(){}
	Band(string, bool, double, double, double, double, Band*);
	Band(Terminal, bool);
	string zoneName = "";

	Band *parent = nullptr;
	int level = 0;
	bool direction_isX;
	bool toExtend_isX; // oppsite of direction_isX (!direction_isX)

	Pair x, y;

	vector<Band> toExtendPaths;

	// member fuctions
	Band *extendedBand(double dx, double dy, int lv);
	Pair detectEdge(vector<Edge> const edges, Point coord, bool direction_isX) const;
	bool intersected(Band const *other) const;
	bool operator ==(Band const &other) const;
	bool alreadyExist(vector<Band*> bands);
};

#endif // BAND_H_INCLUDED