#ifndef __NET_H__
#define __NET_H__

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>


using namespace std;

struct TX {
  // First determine block or region. Second determine ID.
  pair<int, int> TX_info; 
  pair<double, double> TX_COORD;
};

struct RX {
  // First determine block or region. Second determine ID.
  pair<int, int> RX_info; 
  pair<double, double> RX_COORD;
};

struct MUST_THROUGH {
  // 2D coord determine the edge the net should go in.v
  pair<double, double> edgeIn;
  // 2D coord determine the edge the net should go out.
  pair<double, double> edgeOut;
  // First determine block or region. Second determine ID.
  pair<int, int> block_info; 
};

struct HMFT_MUST_THROUGH {
  // 2D coord determine the edge the net should go in.
  pair<double, double> edgeIn;
  // 2D coord determine the edge the net should go out.
  pair<double, double> edgeOut;
  // First determine block or region. Second determine ID.
  pair<int, int> block_info;
};

class Net {
public:
  Net() {}

  int ID;
  int num;
  // Direct stack allocation
  TX TX;
  // Changed to direct objects instead of pointers
  vector<RX> RXs;
  // Changed to direct objects instead of pointers
  vector<MUST_THROUGH> MUST_THROUGHs;
  // Changed to direct objects instead of pointers
  vector<HMFT_MUST_THROUGH> HMFT_MUST_THROUGHs;
};

#endif // __NET_H__
