#include "AllZone.h"
#include "Net.h"
#include "Wall.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum class Direction { L, R, U, D };

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

bool routing(pair<double, double> beginPoint, string beginZone, 
			 pair<double, double> endPoint, string endZone,
			 vector<Wall> const &walls, Direction dir){
	double y = beginPoint.second;
	double x = beginPoint.first;

	// go left:
	for(auto iter = walls.rbegin(); iter != walls.rend(); iter++){
		Wall const &w = *iter;
		if (w.name == beginZone) continue;
		if (!w.isVertical) continue;
		if (w.fixedCoord < x && w.rangeCoord[0] <= y && w.rangeCoord[1] >= y){
			// we found the left wall
		}
	}
	
	// go right:
	for(Wall const &w : walls){
		if (w.name == beginZone) continue;
		if (!w.isVertical) continue;
		if (w.fixedCoord > x && w.rangeCoord[0] <= y && w.rangeCoord[1] >= y){
			// we found the right wall
		}
	}
	
	// go up:
	for(Wall const &w : walls){
		if (w.name == beginZone) continue;
		if (w.isVertical) continue;
		if (w.fixedCoord > y && w.rangeCoord[0] <= x && w.rangeCoord[1] >= x){
			// we found the up wall
		}
	}

	// go down:
	for(auto iter = walls.rbegin(); iter != walls.rend(); iter++){
		Wall const &w = *iter;
		if (w.name == beginZone) continue;
		if (w.isVertical) continue;
		if (w.fixedCoord < y && w.rangeCoord[0] <= x && w.rangeCoord[1] >= x){
			// we found the down wall
		}
	}
}

int main()
{
	int testCase = 4;
	AllZone allZone(testCase);
	allZone.getBlock("BLOCK_1").showBlockInfo();
	allZone.getRegion("REGION_64").showRegionInfo();

	Net Nets;
	Nets.ParserAllNets(testCase);

	vector<Wall> walls = allZone.Walls.allWalls;

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
	// 把 net 用 minLength 重新排序
    sort(netMinBox.begin(), netMinBox.end(), compareBySecond);

    // 轉換成絕對座標
	
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp AllZone.cpp Block.cpp Net.cpp Region.cpp Wall.cpp -o main} ; if ($?) { .\main }
// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Net.cpp -o main} ; if ($?) { .\main }
