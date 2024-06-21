#ifndef PROBE_H_INCLUDE
#define PROBE_H_INCLUDE

#include "Point.h"
#include "Wall.h"
#include <vector>
#include <string>

class Probe {
public:
	Probe() {}
	Probe(Point coord, string zoneName, bool directionX, int level, Probe *parent);
	Point coord;
	string zoneName;
    bool directionX; // true if extended in X direction, false if in Y direction
	int level; // not sure if really need...
	// not sure if 有可以寫指標的寫法的話還需不需要 level
	Probe* parentProbe;

	Probe extendedProbe(double dx, double dy, int lv) const;
	bool hitWall(vector<Wall> const &walls) const;
	bool alreadyExist (vector<Probe> const &oldProbes);
};

#endif // PROBE_H_INCLUDE