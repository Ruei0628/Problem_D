#ifndef NET_H_INCLUDE
#define NET_H_INCLUDE

#include "Chip.h"
#include "include\rapidjson\document.h"
#include <array>

using namespace std;
using namespace rapidjson;

class Terminal {
public:
	Terminal() {}
	Terminal(string Name, Point Coord) : name(Name), coord(Coord) {}
	string name;
	Point coord;
	void absoluteCoord(Chip const &chip);
};

struct MUST_THROUGH {
	string blockName;
	vector<Edge> edges;
};

class Net {
public:
	Net() {}
	Net(Terminal TX, Terminal RX);

	int ID;
	int num;
	Terminal TX;
	vector<Terminal> RXs;
	vector<MUST_THROUGH> MUST_THROUGHs;
	vector<MUST_THROUGH> HMFT_MUST_THROUGHs;

	vector<Point> allNodes;
	vector<Net> allNets;

	double boundBoxArea;
	Edge getBoundBoxArea() const;

	void ParserAllNets(int const &testCase, Chip const &chip);
	Net getNet(int const &ID) const;
	void showNetInfo() const;
};

#endif // NET_H_INCLUDE
