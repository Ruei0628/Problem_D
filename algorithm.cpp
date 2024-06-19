#include "AllZone.h"
#include "Net.h"
#include "Wall.h"

#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <string>
#include <optional>

using namespace std;

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

double findBoundBox(vector<Point> const &coords){
	double x_min = coords[0].x, x_max = coords[0].x,
		   y_min = coords[0].y, y_max = coords[0].y;
	for (auto const &c : coords) {
		double x_this = c.x, y_this = c.y;
		if (x_this > x_max) x_max = x_this;
		if (x_this < x_min) x_min = x_this;
		if (y_this > y_max) y_max = y_this;
		if (y_this < y_min) y_min = y_this;
	}
	return (x_max - x_min) * (y_max - y_min);
}

bool compareBySecond(const pair<Net, double> &a, const pair<Net, double> &b) {
  return a.second < b.second;
}

pair<double, double> getCoordinates(const string &name, const pair<double, double> &offset, const AllZone &allZone) {
    double x, y;
    if(name[0] == 'B') {
        x = allZone.getBlock(name).coordinate.first;
        y = allZone.getBlock(name).coordinate.second;
    }
    else if(name[0] == 'R') {
        x = allZone.getRegion(name).vertices[0].first;
        y = allZone.getRegion(name).vertices[0].second;
    }
    double absoluteX = x + offset.first;
    double absoluteY = y + offset.second;
    return make_pair(absoluteX, absoluteY);
}

// Updated getBeginPoint function
pair<double, double> getBeginPoint(const TX &thePoint, const AllZone &allZone) {
    return getCoordinates(thePoint.TX_NAME, thePoint.TX_COORD, allZone);
}

// Updated getEndPoint function
pair<double, double> getEndPoint(const RX &thePoint, const AllZone &allZone) {
    return getCoordinates(thePoint.RX_NAME, thePoint.RX_COORD, allZone);
}

/* TODO: addProbes
1.  Should plus dx dy(0.001)
2.  現在一次只有加一個格子點，照理來說要一直加到碰到wall為止
	(wall要考慮isFeedThroughable, blockPortRegion照理來說也要考慮但現在可以先不用)
*/

constexpr double DX = 0.001;
constexpr double DY = 0.001;

// 1.如果撞到 nonfeedthroughable block
// 2. x, y 方向的距離超過 兩倍的cartesion length  (10, 10) (20, 20) x, y 2 framing
// TODO: 新的 probes 跟 舊的 probes 重疊的情況不要做。
void addProbes(const pair<double, double>& point, bool fromSource, vector<Probe>& probes, bool extendX) {
	// if (撞牆)
    if (extendX) {
        probes.push_back({ {point.first + DX, point.second}, fromSource });
        probes.push_back({ {point.first - DX, point.second}, fromSource });
    }
    else {
        probes.push_back({ {point.first, point.second + DY}, fromSource });
        probes.push_back({ {point.first, point.second - DY}, fromSource });
    }
}

void addEdgeProbes(const pair<double, double>& start, const pair<double, double>& end,
				   bool fromSource, vector<Probe>& probes) {
    if (start.first == end.first) { // Vertical edge
        double x = start.first;
        double minY = min(start.second, end.second);
        double maxY = max(start.second, end.second);
        for (double y = minY; y <= maxY; y += DY) {
            probes.push_back({ {x, y}, fromSource, false });
        }
    }
    else if (start.second == end.second) { // Horizontal edge
        double y = start.second;
        double minX = min(start.first, end.first);
        double maxX = max(start.first, end.first);
        for (double x = minX; x <= maxX; x += DX) {
            probes.push_back({ {x, y}, fromSource, true });
        }
    }
}

void mikami_tabuchi(pair<double, double> beginPoint, vector<pair<double, double>> endPoints,
					const Net& net, const AllZone& allZone) {
    vector<Probe> currentSourceProbes, currentTargetProbes;
    vector<Probe> oldSourceProbes, oldTargetProbes;
    vector<pair<double, double>> intersections;

    // Add initial probes
    currentSourceProbes.push_back({ beginPoint, true, true });
    currentSourceProbes.push_back({ beginPoint, true, false });
    for (const auto& endPoint : endPoints) {
        currentTargetProbes.push_back({ endPoint, false, true });
        currentTargetProbes.push_back({ endPoint, false, false });
    }

    // Add MUST_THROUGH and HMFT_MUST_THROUGH as initial probes
    for (const auto& mt : net.MUST_THROUGHs) {
        pair<double, double> edgeInStart = { mt.edgeIn[0], mt.edgeIn[1] };
        pair<double, double> edgeInEnd = { mt.edgeIn[2], mt.edgeIn[3] };
        pair<double, double> edgeOutStart = { mt.edgeOut[0], mt.edgeOut[1] };
        pair<double, double> edgeOutEnd = { mt.edgeOut[2], mt.edgeOut[3] };
        addEdgeProbes(edgeInStart, edgeInEnd, true, currentSourceProbes);
        addEdgeProbes(edgeOutStart, edgeOutEnd, false, currentTargetProbes);
    }
    for (const auto& hmft : net.HMFT_MUST_THROUGHs) {
        pair<double, double> edgeInStart = { hmft.edgeIn[0], hmft.edgeIn[1] };
        pair<double, double> edgeInEnd = { hmft.edgeIn[2], hmft.edgeIn[3] };
        pair<double, double> edgeOutStart = { hmft.edgeOut[0], hmft.edgeOut[1] };
        pair<double, double> edgeOutEnd = { hmft.edgeOut[2], hmft.edgeOut[3] };
        addEdgeProbes(edgeInStart, edgeInEnd, true, currentSourceProbes);
        addEdgeProbes(edgeOutStart, edgeOutEnd, false, currentTargetProbes);
    }

    int levelSource = 0,
        levelTarget = 0; // TODO: 所有 component
                         // (component指的是一條net上的TX, RXs, MUST_THROUGHs, HMFT_MUST_THROUGHs)的要分開寄
    bool pathFound = false;

    while (!pathFound) {
		// 把current丟到old
        oldSourceProbes.insert(oldSourceProbes.end(), currentSourceProbes.begin(), currentSourceProbes.end()); 
        oldTargetProbes.insert(oldTargetProbes.end(), currentTargetProbes.begin(), currentTargetProbes.end());

        currentSourceProbes.clear();
        currentTargetProbes.clear();

        // TODO:他這邊的寫法是只需要任意一個RX跟TX交到就算pathFound
        // 但實際上一個net上要所有components都要交到同一個電路(path)上才能算pathFound

        for (const auto& sourceProbe : oldSourceProbes) {
            for (const auto& targetProbe : oldTargetProbes) {
                if (sourceProbe.coord == targetProbe.coord) {
					// intersection 只有記錄到(source target)交會的那一點
                    intersections.push_back(sourceProbe.coord);
                    pathFound = true;
                    break;
                }
            }
            if (pathFound) break;
        }
        
		if (pathFound) break;
        
        for (const auto& sourceProbe : oldSourceProbes) {
			// if (撞牆)
            addProbes(sourceProbe.coord, true, currentSourceProbes, !sourceProbe.extendX);
        }
        for (const auto& targetProbe : oldTargetProbes) {
            addProbes(targetProbe.coord, false, currentTargetProbes, !targetProbe.extendX);
        }

        // 紀錄現在的 intersection 之類的
        levelSource++;
		levelTarget++;
    }

    if (pathFound) 
    {
        vector<pair<double, double>> path;
        path.push_back(intersections[0]);

        // Traceback from Intersection to Source
        while (levelSource > 0) {
            pair<double, double> current = path.back();
            for (const auto& sourceProbe : oldSourceProbes) {
                if (sourceProbe.coord == current) {
                    path.push_back(sourceProbe.coord);
                    break;
                }
            }
            levelSource--;
        }

        reverse(path.begin(), path.end()); // Reverse the Path(不重要別理)

        // Backtrace from Intersection to Target
        while (levelTarget > 0) {
            pair<double, double> current = path.back();
            for (const auto& targetProbe : oldTargetProbes) {
                if (targetProbe.coord == current) {
                    path.push_back(targetProbe.coord);
                    break;
                }
            }
            levelTarget--; // time oriented programming
        }

        
        for (const auto& point : path) {
            cout << "(" << point.first << ", " << point.second << ")" << endl;
        }
    }
    else {
        cout << "No path found." << endl;
    }
}

int main() {
	int testCase = 4;
    AllZone allZone(testCase);
    Net Nets;
	Nets.ParserAllNets(testCase);
	allZone.Walls;

    vector<pair<Net, double>> netMinBox;

	// 計算笛卡爾面積
	for(Net const &n : Nets.allNets){
		vector<Point> coords;
		for(RX const &rx : n.RXs){
			coords.push_back(getEndPoint(rx, allZone));
		}
		coords.push_back(getBeginPoint(n.TX, allZone));
		double boundBox = findBoundBox(coords);
		netMinBox.push_back(make_pair(n, boundBox));
	}
	// 把 net 用 minBox 重新排序
    sort(netMinBox.begin(), netMinBox.end(), compareBySecond);

    for (const auto &netInfo : netMinBox) {
        Net net = netInfo.first;
        pair<double, double> beginPoint = getBeginPoint(net.TX, allZone);
        vector<pair<double, double>> endPoints;
        for (const RX &rx : net.RXs) {
            endPoints.push_back(getEndPoint(rx, allZone));
        }
        mikami_tabuchi(beginPoint, endPoints, net, allZone);
    }

    return 0;
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ algorithm.cpp AllZone.cpp Block.cpp Net.cpp Region.cpp Wall.cpp -o algorithm } ; if ($?) { .\algorithm }