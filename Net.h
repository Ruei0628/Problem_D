#ifndef __NET_H__
#define __NET_H__

#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>

using namespace std;

struct TX {
    pair<int, int> TX_info; // First determine block or region. Second determine ID.
    pair<double, double> TX_COORD;
};

struct RX {
    pair<int, int> RX_info; // First determine block or region. Second determine ID.
    pair<double, double> RX_COORD;
};

struct MUST_THROUGH {
    pair<double, double> edgeIn; // 2D coord determine the edge the net should go in.
    pair<double, double> edgeOut; // 2D coord determine the edge the net should go out.
    pair<int, int> block_info; // First determine block or region. Second determine ID.
};

struct HMFT_MUST_THROUGH {
    pair<double, double> edgeIn; // 2D coord determine the edge the net should go in.
    pair<double, double> edgeOut; // 2D coord determine the edge the net should go out.
    pair<int, int> block_info; // First determine block or region. Second determine ID.
};

class Net {
public:
    Net() {}

    int ID;
    int num;
    TX TX; // Direct stack allocation
    vector<RX> RXs; // Changed to direct objects instead of pointers
    vector<MUST_THROUGH> MUST_THROUGHs; // Changed to direct objects instead of pointers
    vector<HMFT_MUST_THROUGH> HMFT_MUST_THROUGHs; // Changed to direct objects instead of pointers

    // Destructor not needed for vector of objects
};

#endif // __NET_H__
