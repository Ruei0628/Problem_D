#ifndef NET_H_INCLUDE
#define NET_H_INCLUDE

#include "Chip.h"
#include "Point.h"
#include "include\rapidjson\document.h"
#include <array>
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
};

struct MUST_THROUGH {
	string blockName;
	vector<array<double, 4>> edges;
};

struct HMFT_MUST_THROUGH {
	string blockName;
	vector<array<double, 4>> edges;
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
	void getBoundBoxArea(Chip const &chip);

	struct TX absoluteTX(Chip const &chip) const;
	struct RX absoluteRX(RX const &rx, Chip const &chip) const;

	void ParserAllNets(int const &testCase);
	Net getNet(int const &ID) const;
	void showNetInfo();
};

#endif // NET_H_INCLUDE
