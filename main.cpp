#include "Net.h" // include AllZone.h -> Block.h, Region.h -> Zone.h, Wall.h
#include "Probe.h" // included Wall.h

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// enum class Direction { L, R, U, D };
constexpr double DX = 0.001;
constexpr double DY = 0.001;

bool compareNetBoundBoxArea(const Net& a, const Net& b) {
	return a.boundBoxArea < b.boundBoxArea;
}

void mikami (TX const &source, RX const &target, AllZone const &allZone) {
	cout << "Start mikami!" << endl;
	//step 1: initializaiotn
	vector<Probe> CSP; // stands for current source probes
	vector<Probe> OSP; // stands for old source probes
	vector<Probe> CTP; // stands for current target probes
	vector<Probe> OTP; // stands for old target probes

	// 把 TX 跟 RX 改成 probes
    CSP.push_back(Probe(source.TX_COORD, source.TX_NAME, 1, 0, nullptr));
    CSP.push_back(Probe(source.TX_COORD, source.TX_NAME, 0, 0, nullptr));
    CTP.push_back(Probe(target.RX_COORD, target.RX_NAME, 1, 0, nullptr));
    CTP.push_back(Probe(target.RX_COORD, target.RX_NAME, 0, 0, nullptr));

    vector<Wall> const walls = allZone.Walls.allWalls;

	Probe* sourceProbeForBacktrace = nullptr;
	Probe* targetProbeForBacktrace = nullptr;

	cout << "step 1 complete\n";

	while(1){
		// step 2: check if intersect
		bool pathFound = 0;
		for(Probe const &s : CSP){ 
			for(Probe const &t : CTP){ 
				if (s.coord == t.coord){
					// which means path is found
					sourceProbeForBacktrace = new Probe(s);
					targetProbeForBacktrace = new Probe(t);
					pathFound = 1;
					break;
				}
			}
			for(Probe const &t : OTP){
				if (s.coord == t.coord){
					// which means path is found
					sourceProbeForBacktrace = new Probe(s);
					targetProbeForBacktrace = new Probe(t);
					pathFound = 1;
					break;
				}
			}
			if (pathFound) break;
		}
		if (pathFound) break;

		for (Probe const& s : CTP) {
			for (Probe const& t : OSP) {
				if (s.coord == t.coord) {
					// which means path is found
					sourceProbeForBacktrace = new Probe(s);
					targetProbeForBacktrace = new Probe(t);
					pathFound = 1;
					break;
				}
			}
			if (pathFound) break;
		}
		if (pathFound) break;

		cout << "step 2 complete\n";

		// step 3: copy CSP to OSP; copy CTP to OTP
		// current 的點要存回去 old
		OSP.insert(OSP.end(), CSP.begin(), CSP.end());
		OTP.insert(OTP.end(), CTP.begin(), CTP.end());

		// current 的資料應該不能刪掉，因為還要 extend
		// 只是在這之後(、被清除之前)調用 current probes 應該只能 const &

		cout << "step 3 complete\n";

		// 4. 生成與 current probes 垂直的 extendedProbes，並先把他們暫存在一個 vector 裡面
		// 要分成 from source 跟 from target
		vector<Probe> ESP; // stands for extended source probes
		vector<Probe> ETP; // stands for extended target probes
		// 這些生成出來的 probes 的 level 要 +1

		for (Probe const &p : CSP) { // 來自 source
			double dx = p.directionX * DX;
			double dy = !(p.directionX) * DY;
			double X = dx, Y = dy;
			int levelCSP = p.level;
			
			// 往兩個方向生成新的 probe
			// 正方向
			while (1) {
				Probe positiveProbe = p.extendedProbe(X, Y, levelCSP + 1);
				// 如果這個 probe 會撞到牆，直接結束這個方向的 extend
				if (positiveProbe.hitWall(walls)) {
					cout << "EPSP hit wall!" << endl;
					break;
				}
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OSP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (positiveProbe.alreadyExist(OSP)) continue; // may be time-consuming
				ESP.push_back(positiveProbe);
			}
			cout << "EPSP done!" << endl;
			// 負方向
			while (1) {
				Probe negativeProbe = p.extendedProbe(-X, -Y, levelCSP + 1);
				// 如果這個 probe 會撞到牆，直接結束這個方向的 extend
				if (negativeProbe.hitWall(walls)) {
					cout << "ENSP hit wall!" << endl;
					break;
				}
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OSP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (negativeProbe.alreadyExist(OSP)) continue; // may be time-consuming
				ESP.push_back(negativeProbe);
			}
		}
		
		for (Probe const &p : CTP) { // 來自 target
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
					cout << "EPTP hit wall!" << endl;
					break;
				}
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OTP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (positiveProbe.alreadyExist(OTP)) continue; // may be time-consuming
				ETP.push_back(positiveProbe);
			}
			// 負方向
			while (1) {
				Probe negativeProbe = p.extendedProbe(-X, -Y, levelCTP + 1);
				// 如果這個 probe 會撞到牆，直接結束這個方向的 extend
				if (negativeProbe.hitWall(walls)) {
					cout << "ENTP hit wall!" << endl;
					break;
				}
				X += dx;
				Y += dy;
				// 如果這個 probe 已經存在 OTP 裡面，跳過這個 probe (但還是會繼續執行 extend)
				if (negativeProbe.alreadyExist(OTP)) continue; // may be time-consuming
				ETP.push_back(negativeProbe);
			}
		}

		// 因此我們現在獲得了全部的下一個 level 的 probes (在 extendedProbes 裡)
		CSP.swap(ESP);
		ESP.clear();
		CTP.swap(ETP);
		ETP.clear();

		cout << "step 4 complete\n";
	}

	// Step 5: Backtrace
    vector<Probe> path;

    // Backtrace from source probe
    Probe* probe = sourceProbeForBacktrace;
    while (probe) {
        path.push_back(*probe);
        probe = probe->parentProbe;
    }

    reverse(path.begin(), path.end()); // Reverse to get path from source to target

    // Backtrace from target probe
    probe = targetProbeForBacktrace;
    while (probe) {
        path.push_back(*probe);
        probe = probe->parentProbe;
    }

    // Clean up dynamically allocated memory
    delete sourceProbeForBacktrace;
    delete targetProbeForBacktrace;

    // Print the path
    for (Probe const &p : path) {
		cout << "Probe at (" << p.coord.x << ", " << p.coord.y << ")" << endl;
	}
}

int main()
{
	cout << fixed << setprecision(3);
	int testCase = 4;
	AllZone allZone(testCase);
	/*
	for (Zone *z : allZone.totZone) {
    	if (Block *bPtr = dynamic_cast<Block *>(z)) {
          	Block n = *bPtr;
			cout << n.name << endl;
		  	for (Point const &b : n.vertices) {
				cout << b.x <<","<<b.y<<endl;
		  	}
        }
		if (Region *rPtr = dynamic_cast<Region *>(z)) {
			Region n = *rPtr;
			cout << n.name << endl;
    	  	for (Point const &b : n.vertices) {
				cout << b.x <<","<<b.y<<endl;
		  	}
    	}
  	}*/

	Net Nets;
	Nets.readFile(testCase);
	// 把 net 用 bound box 大小重新排序
	// sort(Nets.allNets.begin(), Nets.allNets.end(), compareNetBoundBoxArea);
	// Nets.allNets[3].showNetInfo();

  	vector<Wall> walls = allZone.Walls.allWalls;

	Probe mo(Point(1,2), "A", 1, 0, nullptr);
	Probe m1 = mo.extendedProbe(2, 0, 1);
    cout << m1.coord.x << ", " << m1.coord.y << ", prnt:("
         << m1.parentProbe->coord.x << ", " << m1.parentProbe->coord.y
         << ") " << m1.directionX << endl;
    Probe m2 = m1.extendedProbe(0, 2, 2);
    cout << m2.coord.x << ", " << m2.coord.y << ", prnt:("
		 << m2.parentProbe->coord.x << ", " << m2.parentProbe->coord.y
		 << ") " << m2.directionX << endl;

    /*
    for (Wall const &w : walls) {
        if(w.isVertical){
            cout << "(" << w.fixedCoord << ", ["
            << w.rangeCoord[0] << ", " << w.rangeCoord[1] << "])\t"
            << w.name << endl;
        } else {
                cout << "([" << w.rangeCoord[0] << ", " << w.rangeCoord[1] <<"],
    "
                << w.fixedCoord << ")\t" << w.name << endl;
        }
    }*/

    /*
for (Net const &n : Nets.allNets) {
  TX const &source = n.absoluteTX(allZone);
  for (RX const &rx : n.RXs) {
    RX const &target = n.absoluteRX(rx, allZone);
    mikami(source, target, allZone);
  }
}*/
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp AllZone.cpp Block.cpp Net.cpp Probe.cpp Region.cpp Wall.cpp -o main} ; if ($?) { .\main }
// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Net.cpp -o main} ; if ($?) { .\main }
