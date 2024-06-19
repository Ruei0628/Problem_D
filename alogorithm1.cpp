#include <vector>
#include "AllZone.h"

using namespace std;

  double DX = 0.001;
constexpr double DY = 0.001;

class Point {
public:
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}

    bool operator ==(const Point &other) const {
    	return (x == other.x && y == other.y);
    }
};

class Probe {
public:
	Probe(Point coord, bool isFromSource, bool directionX)
		 : coord(coord), isFromSource(isFromSource), directionX(directionX) {}

	Point coord;
	bool isFromSource; // true if from source, false if from target
    bool directionX;   // true if extended in X direction, false if in Y direction
	int level; // not sure if really need...

	Probe extendProbe(double dx, double dy) {
		Point newPoint(coord.x + dx, coord.y + dy);
		return Probe(newPoint, isFromSource, !directionX);
	}

	bool hitWall(vector<Wall> const &walls){
		// 兩種情況要停下來
		// 1. 碰到不是自己來源的 non-feedthroughable block
		// (自己來源的意思是，如果是 source 的話就是 RX 的那個 block 或 region)
		// (如果是 target 的話就是 TX 的那個 block 或 region)
		// 2. 碰到 chip 的 boundary
		for(Wall const &w : walls) {
			// 如果是起點而且碰到起點 zone 的牆壁則忽略
			if (beginZone == w.name) continue;
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
		return 0;
	}
};

bool extend2Probes(Probe& currentProbe, double X, double Y,
				   const AllZone& allZone, vector<Probe>& probeNew) {

    // 正方向的增加 probe
    Probe addPositiveProbe = currentProbe.extendProbe(X, Y);
    if (addPositiveProbe.hitWall(allZone.Walls.allWalls)) return false;
	probeNew.push_back(addPositiveProbe);

    // 負方向的增加 probe
    Probe addNegativeProbe = currentProbe.extendProbe(-X, -Y);
    if (addNegativeProbe.hitWall(allZone.Walls.allWalls)) return false;
    probeNew.push_back(addNegativeProbe);

    return true;
}

bool processProbes(Probe& currentProbe, const vector<Probe>& probes,
				   double& X, double& Y, const AllZone& allZone, vector<Probe>& probeNew) {
	// probes may either from source or target
    for (const Probe& p : probes) { 
        if (!(p.coord == currentProbe.coord)) {
            if (!extend2Probes(currentProbe, X, Y, allZone, probeNew)) return false;
		}
	}
    return true;
}

bool mikami (Probe currentProbe, vector<Probe> probesOnThisPath,
			 vector<Probe> &sourceProbes, vector<Probe> &targetProbes,
			 AllZone const &allZone) {

	// 紀錄從這個點出發所新增的 probes
	vector<Probe> probeNew;
	double dx = currentProbe.directionX * DX;
	double dy = !(currentProbe.directionX) * DY;
	double X = dx, Y = dy;

	while(1){
		// 如果 probe 已經存在的話不要加
		// 所以要檢查這個 probe 是來自於 source 還是 target
		// 來自於 source 就檢查 sourceProbes，反之亦然
		// 不用檢查對面的是因為如果在的話就代表找到 path 了
		if (currentProbe.isFromSource) {
    	    if (!processProbes(currentProbe, sourceProbes, X, Y, allZone, probeNew)) break;
        } else {
    	    if (!processProbes(currentProbe, targetProbes, X, Y, allZone, probeNew)) break;
        }
        X += dx;
        Y += dy;
    }

	// probeNew 的點要存回去 fromSource 或 fromTarget
	for (Probe const &p : probeNew) {
		if(p.isFromSource){
			sourceProbes.push_back(p);
		}
		else {
			targetProbes.push_back(p);
		}
	}

	// 尋找是否有交集
	for(Probe const &s : sourceProbes){ // vector<Probe> fromSource;
		for(Probe const &t : targetProbes){ // vector<Probe> fromTarget;
			if (s.coord == t.coord){
				// push back the probe back to probesOnThisPath
				return true;
			}
		}
	}

	// 用 新的 probes 當作新的點，並遞迴
	for(Probe const &p : probeNew){
		probesOnThisPath.push_back(p);
		mikami (p, probesOnThisPath, sourceProbes, targetProbes, allZone);
	}
}

int main(){ // 或是說 mikami 的前置作業，不一定是在 main，可能是獨立的函式
	AllZone allZone(4);

	vector<Probe> fromSource;
	vector<Probe> fromTarget;

	Point TX(10, 10);
	fromSource.push_back(Probe(TX, 1, 0));
	fromSource.push_back(Probe(TX, 1, 1));

	Point RX_0(20, 20);
	Point RX_1(30, 30);
	// 應該要用迴圈實現但隨便啦
	fromTarget.push_back(Probe(RX_0, 0, 0));
	fromTarget.push_back(Probe(RX_0, 0, 1));
	fromTarget.push_back(Probe(RX_1, 0, 0));
	fromTarget.push_back(Probe(RX_1, 0, 1));
	
	for(Probe &p : fromSource) { // 只有兩個
		vector<Probe> probesOnSourcePath;
		probesOnSourcePath.push_back(p);
		mikami(p, probesOnSourcePath, fromSource, fromTarget, allZone);
	}

	for(Probe &p : fromTarget) {
		vector<Probe> probesOnTargetPath;
		probesOnTargetPath.push_back(p);
		mikami(p, probesOnTargetPath, fromSource, fromTarget, allZone);
	}
}