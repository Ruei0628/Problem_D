#include <vector>

using namespace std;

constexpr double DX = 0.001;
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
};

bool hitWall(){
	return 0;
}

bool mikami (Probe currentProbe, vector<Probe> probesOnThisPath,
			 vector<Probe> &sourceProbes, vector<Probe> &targetProbes) {

	// 紀錄從這個點出發所新增的 probes
	vector<Probe> probeNew;
	double dx = currentProbe.directionX * DX;
	double dy = !(currentProbe.directionX) * DY;
	double X = dx, Y = dy;

	while(!hitWall()){
		// 如果 probe 已經存在的話不要加
		// 所以要檢查這個 probe 是來自於 source 還是 target
		// 來自於 source 就檢查 sourceProbes，反之亦然
		// 不用檢查對面的是因為如果在的話就代表找到 path 了
		if (currentProbe.isFromSource) {
			for (Probe const &p : sourceProbes) {
				if (!(p.coord == currentProbe.coord)) { // p.directionX == currentProbe.directionX
					probeNew.push_back(currentProbe.extendProbe(X, Y));
					// maybe also level++
				}
			}
		} else {
			for (Probe const &p : targetProbes) {
				if (!(p.coord == currentProbe.coord)) {
					probeNew.push_back(currentProbe.extendProbe(X, Y));
					// maybe also level++
				}
			}
		}
		if (hitWall()) {
			break;
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
		mikami (p, probesOnThisPath, sourceProbes, targetProbes);
	}
}

int main(){ // 或是說 mikami 的前置作業，不一定是在 main，可能是獨立的函式
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
		mikami(p, probesOnSourcePath, fromSource, fromTarget);
	}

	for(Probe &p : fromTarget) {
		vector<Probe> probesOnTargetPath;
		probesOnTargetPath.push_back(p);
		mikami(p, probesOnTargetPath, fromSource, fromTarget);
	}
}