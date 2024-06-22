#include "Probe.h"
#include <cmath>

Probe::Probe(Point coord, string zoneName, bool directionX, int level, Probe *parent = nullptr){
	this->coord = coord;
	this->zoneName = zoneName;
	this->directionX = directionX;
	this->level = level;
	this->parentProbe = parent;
}

Probe Probe::extendedProbe(double dx, double dy, int lv) const {
	Point newPoint(coord.x + dx, coord.y + dy);
	return Probe(newPoint, zoneName, !directionX, lv, const_cast<Probe*>(this));
}
bool Probe::hitWall(vector<Wall> const &walls) const {
	cout << coord.x << ", " << coord.y << ", "<<directionX<<"\n";
	// 兩種情況要停下來
	// 1. 碰到不是自己來源的 non-feedthroughable block
	// (自己來源的意思是，如果是 source 的話就是 RX 的那個 block 或 region)
	// (如果是 target 的話就是 TX 的那個 block 或 region)
	for(Wall const &w : walls) {
		// 如果是起點而且碰到起點 zone 的牆壁則忽略
		if (zoneName == w.name) continue;
		// 走橫向的 probes 碰到垂直的牆壁
		if (directionX && w.isVertical){
			// 好像就不用判斷說是往右還是往左
			if(coord.x == w.fixedCoord && w.inRange(coord.y)){
				return 1;
			}
		}
		// 走縱向的 probes 碰到水平的牆壁
		if (!directionX && !w.isVertical){
			// 好像就不用判斷說是往上還是往下
			if(coord.x == w.fixedCoord && w.inRange(coord.y)){
				return 1;
			}
		}
	}
	// 2. 碰到 chip 邊界
	const double boundaryL = 0.0;
	const double boundaryR = 6220.068;
	const double boundaryD = 0.0;
	const double boundaryU = 5184.36;
	if (directionX && (coord.x >= boundaryR || coord.x <= boundaryL)){
		return 1;
	}
	if (!directionX && (coord.y >= boundaryU || coord.y <= boundaryD)){
		return 1;
	}
	return 0;
}
bool Probe::alreadyExist (vector<Probe> const &oldProbes){
	for (Probe const &p : oldProbes){
		if (coord == p.coord) return 1;
	}
	return 0;
}