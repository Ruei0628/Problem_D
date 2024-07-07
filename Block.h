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
	vector<Point> edge;
	int net_num;
};

class Block : public Zone {
public:
	Block();
	Block(Block const &);
	string blkID;
	Point coordinate;
	string facingFlip;

	int through_block_net_num;
	vector<BlockEdgeAndNum> through_block_edge_net_num;
	vector<vector<Point>> block_port_region;
	bool is_feedthroughable;
	bool is_tile;

	int UNITS_DISTANCE_MICRONS;
	double border_X;
	double border_Y;

	vector<Block> allBlocks;

	void ParserAllBlocks(int const &case_serial_number);
	void expandVertices();
	void showBlockInfo();
};

#endif // BLOCK_H_INCLUDED