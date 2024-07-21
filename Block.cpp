#include "Block.h"

Block::Block(){}

Block::Block(string border) {
	this->name = border;
	this->is_feedthroughable = 0;
}

Block::Block(Block const &b) {
	this->name = b.name;
	this->vertices = b.vertices;

	this->coordinate = b.coordinate;
	this->facingFlip = b.facingFlip;
	this->blkID = b.blkID;

	this->through_block_net_num = b.through_block_net_num;
	this->through_block_edge_net_num = b.through_block_edge_net_num;
	this->block_port_region = b.block_port_region;
	this->is_feedthroughable = b.is_feedthroughable;
	this->is_tile = b.is_tile;
	this->edges = b.edges;
}

void Block::verticesToEdges() {
  	//if (b.is_feedthroughable) return; // should be revise, TODO!!
	for (int i = 0; i < vertices.size(); i++) {
		Edge newEdge(vertices[i], vertices[(i + 1) % vertices.size()]);
		newEdge.block = this;
		edges.push_back(newEdge);
	}

	return;
}

void faceAndFlip(vector<Point> &anyVertices, char const &face, char const &flip) {
	vector<Point> tempVertices;
	tempVertices = std::move(anyVertices);
	
	// facing
	for (Point &vertex : tempVertices) {
		switch (face) {
		case 'N': { anyVertices.push_back(vertex); break; }
		case 'W': { anyVertices.push_back(Point(-vertex.y, vertex.x)); break; }
		case 'S': { anyVertices.push_back(Point(-vertex.x, -vertex.y)); break; }
		case 'E': { anyVertices.push_back(Point(vertex.y, -vertex.x)); break; }
		}
	}

	// flip
	if (flip == 'F') { for (Point &vertex : anyVertices) { vertex.x = -vertex.x; } }

	return;
}

void shift(vector<Point> &anyVertices, Point const &min) {
	for (Point &vertex : anyVertices) {
		vertex.x += min.x;
		vertex.y += min.y;
	}

	return;
}

vector<Point> edge2Point(Edge const &e) {
	return vector<Point> { e.first, e.second };
}

void Block::transposeAllVertices() {
	// three things need to transpose:
	// 1. vertices
	// 2. Edges in through_block_edge_net_num
	// 3. Edges in block_port_region

	char flip = facingFlip[0];
	char face = facingFlip[1];

	faceAndFlip(vertices, face, flip);

	// find the new min, later all kinds of vertices if shifted base on this point
	Point min(1e+10, 1e+10);

	for (Point const &vertex : vertices) {
		if (vertex.x < min.x) min.x = vertex.x;
		if (vertex.y < min.y) min.y = vertex.y;
	}
	min.x = - min.x;
	min.y = - min.y;

	// first done the vertices of the block
	shift(vertices, min);
	shift(vertices, coordinate);

	// then all the other
	// note that after experiment, they only need shift base on coordinate
	// NO NEED to faceFlip!!!
	for (BlockEdgeAndNum &TBENN : through_block_edge_net_num) {
		vector<Point> pointsOfEdge = edge2Point(TBENN.edge);
		shift(pointsOfEdge, coordinate);
		TBENN.edge.first = pointsOfEdge[0];
		TBENN.edge.second = pointsOfEdge[1];
	}
	for (Edge &BPR : block_port_region) {
		vector<Point> pointsOfEdge = edge2Point(BPR);
		shift(pointsOfEdge, coordinate);
		BPR.first = pointsOfEdge[0];
		BPR.second = pointsOfEdge[1];
	}

	return;
}

void Block::showBlockInfo() const {
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