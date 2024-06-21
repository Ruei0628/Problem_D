#include <vector>
#include "AllZone.h"

using namespace std;

constexpr double DX = 0.001;
constexpr double DY = 0.001;

class Point {
public:
	double x, y;
	Point(double x = 0, double y = 0) : x(x), y(y) {}

	bool operator ==(const Point& other) const {
		return (x == other.x && y == other.y);
	}
};

class Probe {
public:
	Probe() {}
	Probe(Point coord, bool directionX, int level, Probe* parent = nullptr)
		: coord(coord), directionX(directionX), level(level), parentProbe(parent) {}

	Point coord;
	bool isFromSource; // true if from source, false if from target
	// is "isFromSource" important now?
	bool directionX;   // true if extended in X direction, false if in Y direction
	int level; // not sure if really need...
	// not sure if 有可以寫指標的寫法的話還需不需要 level
	Probe* parentProbe;
	string zoneName;

	Probe extendedProbe(double dx, double dy, int lv) const {
		Point newPoint(coord.x + dx, coord.y + dy);
		return Probe(newPoint, !directionX, lv, const_cast<Probe*>(this));
	}

	// Return 1 if execute hit the wall or the boundary of layout.
	bool hitWall(vector<Wall> const& walls) const {
		// 兩種情況要停下來
		// 1. 碰到不是自己來源的 non-feedthroughable block
		// (自己來源的意思是，如果是 source 的話就是 RX 的那個 block 或 region)
		// (如果是 target 的話就是 TX 的那個 block 或 region)
		for (Wall const& w : walls) {
			// 如果是起點而且碰到起點 zone 的牆壁則忽略
			if (zoneName == w.name) continue;
			// 走橫向的 probes 碰到垂直的牆壁
			if (directionX && w.isVertical) {
				// 好像就不用判斷說是往右還是往左
				if (coord.x == w.fixedCoord && w.inRange(coord.y)) {
					return 1;
				}
			}
			// 走縱向的 probes 碰到水平的牆壁
			if (!directionX && !w.isVertical) {
				// 好像就不用判斷說是往上還是往下
				if (coord.x == w.fixedCoord && w.inRange(coord.y)) {
					return 1;
				}
			}
		}

		// 2. 碰到 chip 邊界
		// Ruei : I found that we can get this info in chip_top.def.
		const double boundaryL = 0.0;
		const double boundaryR = 10000000.0; // assumed
		const double boundaryD = 0.0;
		const double boundaryU = 10000000.0; // assumed
		if (directionX && (coord.x > boundaryR || coord.x < boundaryL)) {
			return 1;
		}
		if (!directionX && (coord.y > boundaryU || coord.y < boundaryD)) {
			return 1;
		}
		return 0;
	}

	bool alreadyExist(vector<Probe> const& oldProbes) {
		for (Probe const& p : oldProbes) {
			if (coord == p.coord) return 1;
		}
		return 0;
	}
};

bool mikami(TX source, RX target, AllZone const& allZone) {

	//step 1: initializaiotn
	vector<Probe> CSP;  //  Current Source Probes
	vector<Probe> OSP;  //  Old Target Probes
	vector<Probe> CTP;  //  Current Target Probes
	vector<Probe> OTP;  //  Old Target Probes
	
	Probe sX(TX_coord, 1, 0, nullptr);  //Level 可以刪掉
	Probe sY(TX_coord, 0, 0, nullptr);
	Probe tX(RX_coord, 1, 0, nullptr);
	Probe tY(RX_coord, 0, 0, nullptr);
	
	// 往兩個方向生成新的 probe
	// Positive Direction
	while (1) {
		double dx = p.directionX * DX;
		double dy = !(p.directionX) * DY;
		double X = dx, Y = dy;
		Probe positiveProbe = sX.extendedProbe(X, Y, levelCSP);
		if (positiveProbe.hitWall(walls)) break;
		X += dx;
		Y += dy;
		extendedProbes.push_back(positiveProbe);
	}

	// Negative Direction
	while (1) {
		double dx = p.directionX * DX;
		double dy = !(p.directionX) * DY;
		double X = dx, Y = dy; 
		Probe positiveProbe = sY.extendedProbe(X, Y, levelCSP);
		if (positiveProbe.hitWall(walls)) break;
		X += dx;
		Y += dy;
		extendedProbes.push_back(positiveProbe);
	}

	while (1) {
		double dx = p.directionX * DX;
		double dy = !(p.directionX) * DY;
		double X = dx, Y = dy;
		Probe positiveProbe = tX.extendedProbe(X, Y, levelCSP);
		if (positiveProbe.hitWall(walls)) break;
		X += dx;
		Y += dy;
		extendedProbes.push_back(positiveProbe);
	}

	while (1) {
		double dx = p.directionX * DX;
		double dy = !(p.directionX) * DY;
		double X = dx, Y = dy;
		Probe positiveProbe = tY.extendedProbe(X, Y, levelCSP);
		if (positiveProbe.hitWall(walls)) break;
		X += dx;
		Y += dy;
		extendedProbes.push_back(positiveProbe);
	}
	
	vector<Wall> const walls = allZone.Walls.allWalls;

	Probe sourceProbeForBacktrace;
	Probe targetProbeForBacktrace;

	while (1) {
		// step 2: check if intersect
		for (Probe const& s : CSP) {
			for (Probe const& t : CTP) {
				if (s.coord == t.coord) {
					// which means path is found
					sourceProbeForBacktrace = s;
					targetProbeForBacktrace = t;
					break;
				}
			}
			for (Probe const& t : OTP) {
				if (s.coord == t.coord) {
					// which means path is found
					sourceProbeForBacktrace = s;
					targetProbeForBacktrace = t;
					break;
				}
			}
		}

		// Ruei : Didn't consider this situation.
		// The current method break the loop is wrong.
		for (Probe const& s : CTP) {
			for (Probe const& t : OSP) {
				if (s.coord == t.coord) {
					// which means path is found
					sourceProbeForBacktrace = s;
					targetProbeForBacktrace = t;
					break;
				}
			}
		}

		// step 3: copy CSP to OSP; copy CTP to OTP
		// current 的點要存回去 old
		OSP.insert(OSP.end(), CSP.begin(), CSP.end());
		OTP.insert(OTP.end(), CTP.begin(), CTP.end());

		// current 的資料應該不能刪掉，因為還要 extend
		// 只是在這之後(、被清除之前)調用 current probes 應該只能 const &

		// 4. 生成與 current probes 垂直的 extendedProbes，並先把他們暫存在一個 vector 裡面
		vector<Probe> extendedProbes;
		// 這些生成出來的 probes 的 level 要 +1

		for (Probe const& p : CSP) { // 來自 source
			double dx = p.directionX * DX;
			double dy = !(p.directionX) * DY;
			double X = dx, Y = dy;
			int levelCSP = p.level;  //Ruei : This line write outside.

			// 往兩個方向生成新的 probe
			// 正方向
			while (1) {
				Probe positiveProbe = p.extendedProbe(X, Y, levelCSP + 1);
				if (positiveProbe.hitWall(walls)) break;
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OSP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (positiveProbe.alreadyExist(OSP)) continue; // may be time-consuming 
				// Ruei : The situation that (sometimes this probe is already existed while the next isn't) is existed. Maybe find an alternative? 
				extendedProbes.push_back(positiveProbe);
			}
			// 負方向
			while (1) {
				Probe negativeProbe = p.extendedProbe(-X, -Y, levelCSP + 1);
				// 如果這個 probe 會撞到牆，直接結束這個方向的 extend
				if (negativeProbe.hitWall(walls)) {
					break;
				}
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OSP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (negativeProbe.alreadyExist(OSP)) continue; // may be time-consuming
				extendedProbes.push_back(negativeProbe);
			}
		}

		for (Probe const& p : CTP) { // 來自 target
			double dx = p.directionX * DX;
			double dy = !(p.directionX) * DY;
			double X = dx, Y = dy;
			int levelCTP = p.level;

			// 往兩個方向生成新的 probe
			// 正方向
			while (1) {
				Probe positiveProbe = p.extendedProbe(X, Y, levelCTP + 1);
				// 如果這個 probe 會撞到牆，直接結束這個方向的 extend
				if (positiveProbe.hitWall(walls)) {
					break;
				}
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OSP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (positiveProbe.alreadyExist(OTP)) continue; // may be time-consuming
				extendedProbes.push_back(positiveProbe);
			}
			// 負方向
			while (1) {
				Probe negativeProbe = p.extendedProbe(-X, -Y, levelCTP + 1);
				// 如果這個 probe 會撞到牆，直接結束這個方向的 extend
				if (negativeProbe.hitWall(walls)) {
					break;
				}
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OSP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (negativeProbe.alreadyExist(OTP)) continue; // may be time-consuming
				extendedProbes.push_back(negativeProbe);
			}
		}

		// 因此我們現在獲得了全部的下一個 level 的 probes (在 extendedProbes 裡)
		CSP.swap(extendedProbes);
		extendedProbes.clear();
		// Ruei : Why not just push back to CSP directly ?
	}

	// step 5. backtrace
	// backtrace 的實作想法:
	// 反正我剛剛問 chatGPT 寫了一個指標的寫法，而且看起來也不難
	// 那就是利用指標的方式去一直找 現在的 Probe 的 parent
	// (也就是上一個 level extend 出這個 probe 的 probe)
	// 就可以 trace 出整條線，應該啦
}

int main() { // 或是說 mikami 的前置作業，不一定是在 main，可能是獨立的函式
	AllZone allZone(4);

	vector<Probe> fromSource;
	vector<Probe> fromTarget;

	Point TX(10, 10);
	fromSource.push_back(Probe(TX, 0, 0));
	fromSource.push_back(Probe(TX, 1, 0));

	Point RX_0(20, 20);
	Point RX_1(30, 30);
	// 應該要用迴圈實現但隨便啦
	fromTarget.push_back(Probe(RX_0, 0, 0));
	fromTarget.push_back(Probe(RX_0, 1, 0));
	fromTarget.push_back(Probe(RX_1, 0, 0));
	fromTarget.push_back(Probe(RX_1, 1, 0));

	mikami(fromSource, fromTarget, allZone);
}