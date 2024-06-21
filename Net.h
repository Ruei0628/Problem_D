#ifndef NET_H_INCLUDE
#define NET_H_INCLUDE

#include "AllZone.h"
#include "Point.h"
#include "include\rapidjson\document.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace rapidjson;

struct TX {
	string TX_NAME; 
	Point TX_COORD;
};

struct RX {
	string RX_NAME;
	Point RX_COORD;

	RX(const string &name, const Point &coord) : RX_NAME(name), RX_COORD(coord) {}
};

struct MUST_THROUGH {
	string blockName;
	double edgeIn[4];
	double edgeOut[4];
};

struct HMFT_MUST_THROUGH {
	string blockName;
	double edgeIn[4];
	double edgeOut[4];
};

class Net {
public:
	Net() {}

	int ID;
	int num;
	TX TX;
	vector<RX> RXs;
	vector<MUST_THROUGH> MUST_THROUGHs;
	vector<HMFT_MUST_THROUGH> HMFT_MUST_THROUGHs;

	vector<Point> allNodes;
	vector<Net> allNets;

	double boundBoxArea;
	void getBoundBoxArea(AllZone const &allZone);
	void ParserAllNets(int const &testCase, AllZone const &allZone);
	Net getNet(int const &ID) const;
	void showNetInfo();
};

#endif // NET_H_INCLUDE
