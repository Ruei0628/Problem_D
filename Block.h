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
	vector<Point> blockEdge;
	int net_num = -1;
};

class Block : public Zone {
public:
	Block();
	Block(Block const &);
	string blkID;
	Point coordinate;
	string facingFlip;

	int through_block_net_num;
	BlockEdgeAndNum through_block_edge_net_num;
	vector<Point> block_port_region;
	bool is_feedthroughable;
	bool is_tile;

	vector<Block> allBlocks;

	void ParserAllBlocks(int const &case_serial_number);
	void expandVertices();
	void facingAndFlip(vector<Point> &vertices);
	void shiftCoordinate(vector<Point> &vertices);
	void showBlockInfo();
};

#endif // BLOCK_H_INCLUDED