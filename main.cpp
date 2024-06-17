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

double cartesianLength(pair<double, double> tx, pair<double, double> rx){
	double x_length = abs(tx.first - rx.first);
	double y_length = abs(tx.second - rx.second);
	return x_length + y_length;
}

bool compareBySecond(const pair<Net, double> &a, const pair<Net, double> &b) {
  return a.second < b.second;
}

bool routing(Net const &n, vector<Wall> const &walls){
	for(Wall const &w : walls){
		// start point go N, S, W, E
		// when go W or E, trigger "goHorizontally"
		// goHorizontally(){
		// which means will hit vertical walls
		// so for (wall in walls)
		// if (wall.isVertical)
		// 
		// }
		// 
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

	vector<pair<Net, double>> netMinLength;

	// 計算笛卡爾距離
	for(Net const &n : Nets.allNets){
		double len = 0;
		for(RX const &rx : n.RXs){
			len += cartesianLength(n.TX.TX_COORD, rx.RX_COORD);
		}
		int sizeOfRXs = n.RXs.size();
		netMinLength.push_back(make_pair(n, len / sizeOfRXs));
	}

	// 把 net 用 minLength 重新排序
	sort(netMinLength.begin(), netMinLength.end(), compareBySecond);
	
	// 轉換成絕對座標
	for(Net const &n : Nets.allNets){
		string determine = n.TX.TX_NAME;
		double x, y;
		if(determine[0] == 'B'){
			x = allZone.getBlock(determine).coordinate.first;
			y = allZone.getBlock(determine).coordinate.second;
		}
		else if(determine[0] == 'R') {
			x = allZone.getRegion(determine).vertices[0].first;
			y = allZone.getRegion(determine).vertices[0].second;
		}
		x += n.TX.TX_COORD.first;
		y += n.TX.TX_COORD.second;

	}
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp AllZone.cpp Block.cpp Net.cpp Region.cpp Wall.cpp -o main} ; if ($?) { .\main }
// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Net.cpp -o main} ; if ($?) { .\main }
