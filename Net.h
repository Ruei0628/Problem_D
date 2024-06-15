#ifndef __NET_H__
#define __NET_H__

#include "include\rapidjson\document.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace rapidjson;

struct TX {
  string TX_NAME; 
  pair<double, double> TX_COORD;
};

struct RX {
  string RX_NAME;
  pair<double, double> RX_COORD;

  RX(const string &name, const pair<double, double> &coord) : RX_NAME(name), RX_COORD(coord) {}
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

  vector<Net> allNets;

  void ParserAllNets(int const &testCase);
  Net getNet(int const &ID) const;
  void showNetInfo();
};

#endif // __NET_H__
