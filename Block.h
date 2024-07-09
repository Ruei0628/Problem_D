#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include "Zone.h"
#include "Edge.h"
#include <iostream>

using namespace std;

struct BlockEdgeAndNum {
	Edge edge;
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
	vector<Edge> block_port_region;
	bool is_feedthroughable;
	bool is_tile;
	vector<Edge> edges;

	void verticesToEdges();
	void showBlockInfo();
};

#endif // BLOCK_H_INCLUDED