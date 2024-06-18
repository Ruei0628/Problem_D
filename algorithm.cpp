#include "AllZone.h"
#include "Net.h"

#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <string>
#include <optional>

using namespace std;

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

double findBoundBox(vector<pair<double, double>> const &coords){
	double x_min = coords[0].first, x_max = coords[0].first,
		   y_min = coords[0].second, y_max = coords[0].second;
	for (auto const &c : coords) {
		double x_this = c.first, y_this = c.second;
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
    double newX = x + offset.first;
    double newY = y + offset.second;
    return make_pair(newX, newY);
}

// Updated getBeginPoint function
pair<double, double> getBeginPoint(const TX &thePoint, const AllZone &allZone) {
    return getCoordinates(thePoint.TX_NAME, thePoint.TX_COORD, allZone);
}

// Updated getEndPoint function
pair<double, double> getEndPoint(const RX &thePoint, const AllZone &allZone) {
    return getCoordinates(thePoint.RX_NAME, thePoint.RX_COORD, allZone);
}

void mikami_tabuchi(pair<double, double> beginPoint, vector<pair<double, double>> endPoints,
					const vector<Net> &Nets, const AllZone &allZone) {
    struct Probe {
        pair<double, double> coord;
        bool fromSource; // true if from source, false if from target
    };

    vector<Probe> currentSourceProbes, currentTargetProbes;
    vector<Probe> oldSourceProbes, oldTargetProbes;
    vector<pair<double, double>> intersections;

    auto addProbes = [](const pair<double, double> &point, bool fromSource, vector<Probe> &probes) {
        double dx = 1, dy = 1;
		probes.push_back({point, fromSource});
        probes.push_back({{point.first, point.second + dy}, fromSource});
        probes.push_back({{point.first + dx, point.second}, fromSource});
        probes.push_back({{point.first, point.second - dy}, fromSource});
        probes.push_back({{point.first - dx, point.second}, fromSource});
    };

    addProbes(beginPoint, true, currentSourceProbes);
    for (const auto &endPoint : endPoints) {
        addProbes(endPoint, false, currentTargetProbes);
    }

    for (const auto &net : Nets) {
        for (const auto &mt : net.MUST_THROUGHs) {
            pair<double, double> edgeInStart = {mt.edgeIn[0], mt.edgeIn[1]};
            pair<double, double> edgeInEnd = {mt.edgeIn[2], mt.edgeIn[3]};
            pair<double, double> edgeOutStart = {mt.edgeOut[0], mt.edgeOut[1]};
            pair<double, double> edgeOutEnd = {mt.edgeOut[2], mt.edgeOut[3]};
            addProbes(edgeInStart, true, currentSourceProbes);
            addProbes(edgeOutEnd, false, currentTargetProbes);
        }
        for (const auto &hmft : net.HMFT_MUST_THROUGHs) {
            pair<double, double> edgeInStart = {hmft.edgeIn[0], hmft.edgeIn[1]};
            pair<double, double> edgeInEnd = {hmft.edgeIn[2], hmft.edgeIn[3]};
            pair<double, double> edgeOutStart = {hmft.edgeOut[0], hmft.edgeOut[1]};
            pair<double, double> edgeOutEnd = {hmft.edgeOut[2], hmft.edgeOut[3]};
            addProbes(edgeInStart, true, currentSourceProbes);
            addProbes(edgeOutEnd, false, currentTargetProbes);
        }
    }

    int level = 0;
    bool pathFound = false;

    while (!pathFound) {
        oldSourceProbes.insert(oldSourceProbes.end(), currentSourceProbes.begin(), currentSourceProbes.end());
        oldTargetProbes.insert(oldTargetProbes.end(), currentTargetProbes.begin(), currentTargetProbes.end());

        currentSourceProbes.clear();
        currentTargetProbes.clear();

        for (const auto &sourceProbe : oldSourceProbes) {
            for (const auto &targetProbe : oldTargetProbes) {
                if (sourceProbe.coord == targetProbe.coord) {
                    intersections.push_back(sourceProbe.coord);
                    pathFound = true;
                    break;
                }
            }
            if (pathFound) break;
        }

        if (pathFound) break;

        for (const auto &sourceProbe : oldSourceProbes) {
            addProbes(sourceProbe.coord, true, currentSourceProbes);
        }
        for (const auto &targetProbe : oldTargetProbes) {
            addProbes(targetProbe.coord, false, currentTargetProbes);
        }

        ++level;
    }

    if (pathFound) {
        vector<pair<double, double>> path;
        path.push_back(intersections[0]);

        while (level > 0) {
            pair<double, double> current = path.back();
            for (const auto &sourceProbe : oldSourceProbes) {
                if (sourceProbe.coord == current) {
                    path.push_back(sourceProbe.coord);
                    break;
                }
            }
            --level;
        }

        reverse(path.begin(), path.end());
        level = 0;

        while (level > 0) {
            pair<double, double> current = path.back();
            for (const auto &targetProbe : oldTargetProbes) {
                if (targetProbe.coord == current) {
                    path.push_back(targetProbe.coord);
                    break;
                }
            }
            --level;
        }

        for (const auto &point : path) {
            cout << "(" << point.first << ", " << point.second << ")" << endl;
        }
    } else {
        cout << "No path found." << endl;
    }
}

int main() {
	int testCase = 4;
    AllZone allZone(testCase);
    Net Nets;
	Nets.ParserAllNets(testCase);

    vector<pair<Net, double>> netMinBox;

	// 計算笛卡爾面積
	for(Net const &n : Nets.allNets){
		vector<pair<double, double>> coords;
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
        mikami_tabuchi(beginPoint, endPoints, Nets.allNets, allZone);
    }

    return 0;
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ algorithm.cpp AllZone.cpp Block.cpp Net.cpp Region.cpp Wall.cpp -o algorithm } ; if ($?) { .\algorithm }