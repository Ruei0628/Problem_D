#include "AllZone.h"
#include "Net.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	int testCase = 4;
	AllZone allZone(testCase);
	allZone.getBlock("BLOCK_1").showBlockInfo();
	allZone.getRegion("REGION_64").showRegionInfo();

	Net Nets;
	Nets.ParserAllNets(testCase);

	for(int i = 0; i<Nets.allNets.size(); i++){
		Nets.allNets[i].showNetInfo();
	}

	// 這段主要是在講說，因為TX可能是 block 或 region，所以沒辦法直接使用 getZone.getCoordinate
	// 所以才要去判斷現在要
    string determine = Nets.allNets[1159].TX.TX_NAME;
	double x, y;
	if(determine[0] == 'B'){
		x = allZone.getBlock(determine).coordinate.first;
		y = allZone.getBlock(determine).coordinate.second;
	}
	else if(determine[0] == 'R'){
		x = allZone.getRegion(determine).vertices[0].first;
		y = allZone.getRegion(determine).vertices[0].second;
    }
    x += Nets.allNets[1159].TX.TX_COORD.first;
    y += Nets.allNets[1159].TX.TX_COORD.second;
    cout << x << ", " << y << endl;
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp AllZone.cpp Block.cpp Net.cpp Region.cpp -o main} ; if ($?) { .\main }
// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Net.cpp -o main} ; if ($?) { .\main }
