#ifndef BAND_H_INCLUDED
#define BAND_H_INCLUDED

#include "Chip.h"
#include "Net.h"

struct CoveredRange {
    Pair range;
    double fixed;

    CoveredRange(Pair r, double p) : range(r), fixed(p) {}
};

class Band {
public: 
	Band(){}
	Band(Pair, Pair, int, Band*);
	Band(Terminal, bool, vector<Edge*> const&);
	string zoneName = "";

	Band *parent = nullptr;
	int level = 0;
	bool direction_isX;
	bool toExtend_isX() { return !direction_isX; }

	Pair x, y;
	Pair directionPair() { 
		if (direction_isX) return x;
		else return y;
	}
	Pair extendedPair() {
		if (toExtend_isX()) return x;
		else return y;
	}

	// member fuctions
	Pair directionPair(vector<Edge*> const edges, Point coord) const;
	bool intersected(Band const *other) const;

	Band *extendBand(Pair, Pair);
	vector<CoveredRange> generateCoveredRanges(vector<Edge*> &edges, bool right);
	void addSource(Edge *edge, vector<Pair> &uncovered, vector<CoveredRange> &covered);
	vector<Band*> mergeCoveredRanges(vector<CoveredRange> const &left, vector<CoveredRange> const &right);

	bool operator ==(Band const &other) const;
	bool alreadyExist(vector<Band*> bands);
};

#endif // BAND_H_INCLUDED