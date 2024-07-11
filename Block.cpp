#include "Block.h"
#include <vector>

Block::Block() {}

Block::Block(Block const &b) {
	this->name = b.name;
	this->blkID = b.blkID;
	this->coordinate = b.coordinate;
	this->vertices = b.vertices;
	this->facingFlip = b.facingFlip;

	this->through_block_net_num = b.through_block_net_num;
	this->through_block_edge_net_num = b.through_block_edge_net_num;
	this->block_port_region = b.block_port_region;
	this->is_feedthroughable = b.is_feedthroughable;
	this->is_tile = b.is_tile;
}

void Block::verticesToEdges() {
  	//if (b.is_feedthroughable) return; // should be revise, TODO!!
	for (int i = 0; i < vertices.size(); i++) {
		edges.push_back(Edge(vertices[i], vertices[(i + 1) % vertices.size()]));
	}
}

void Block::showBlockInfo() {
	cout << "blockName: '" << name << "'" << endl
	<< "blkID: '" << blkID << "'" << endl
	<< "coordinate: (" << coordinate.x << ", " << coordinate.y
	<< ") " << endl
	<< "facingFlip: '" << facingFlip << "'" << endl;
	
	cout << "vertices: " << endl;
	for (auto v : vertices) {
		cout<< v.x << "\t" << v.y << "\n";
	}

	cout << "through_block_net_num: " << through_block_net_num << endl
	<< "through_block_edge_net_num: ";
	for (BlockEdgeAndNum const &TBENN : through_block_edge_net_num) {
		cout << endl << "(" << TBENN.edge.first.x << ", " << TBENN.edge.first.y << ") ("
		<< TBENN.edge.second.x << ", " << TBENN.edge.second.y << ") " << TBENN.net_num;
	}
	cout << endl;

	cout << "block_port_region: ";
	for (Edge const &BPR : block_port_region) {
		cout << endl << "(" << BPR.first.x << ", " << BPR.first.y
		<< ") (" << BPR.second.x << ", " << BPR.second.y << ")";
	}
	cout << endl;
	
	cout << "is_feedthroughable: " << is_feedthroughable << endl
	<< "is_tile: " << is_tile << endl;
	cout << "----------------------" << endl;
}