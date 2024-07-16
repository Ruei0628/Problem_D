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
	Block(string);
	Block(Block const &);

	Point coordinate;
	string facingFlip;
	string blkID;

	int through_block_net_num;
	vector<BlockEdgeAndNum> through_block_edge_net_num;
	vector<Edge> block_port_region;
	bool is_feedthroughable;
	bool is_tile;
	vector<Edge> edges;

	void verticesToEdges();
	void showBlockInfo() const;
	// 還沒寫net穿過block會減少through_block_net_num
};

#endif // BLOCK_H_INCLUDED