#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include "Zone.h"
#include <string>
#include <utility>
#include <vector>

#include "include\rapidjson\document.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

using namespace std;
using namespace rapidjson;

struct BlockEdgeAndNum {
  vector<pair<double, double>> blockEdge;
  int net_num = -1;
};

class Block : public Zone {
public:
  Block();
  Block(Block const &);
  string blkID;
  pair<double, double> coordinate;
  string facingFlip;

  int through_block_net_num;
  BlockEdgeAndNum through_block_edge_net_num;
  vector<pair<double, double>> block_port_region;
  bool is_feedthroughable;
  bool is_tile;

  vector<Block> allBlocks;

  void ParserAllBlocks(int const &case_serial_number);
  void expandVertices();
  void facingAndFlip(string facingFlip, vector<pair<double, double>> &vertices);
  void shiftCoordinate(pair<double, double> shift,
                       vector<pair<double, double>> &vertices);
  void showBlockInfo();
};

#endif // BLOCK_H_INCLUDED