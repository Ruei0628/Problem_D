#include "Block.h"
#include <sstream>
#include <vector>

Point BORDER;

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

void Block::expandVertices() {
	Point tempVertice(vertices[0].x, vertices[1].y);
	vertices.insert(vertices.begin() + 1, tempVertice);
	tempVertice = Point(vertices[2].x, vertices[0].y);
	vertices.push_back(tempVertice);
}

void facingAndFlip(vector<Point> &vertices, string facingFlip) {
	char flip = facingFlip[0];
	char facing = facingFlip[1];

	vector<Point> tempVertices;
	tempVertices = vertices;
	vertices.clear();

	Point min(BORDER.x, BORDER.y);
	for (Point &vertex : tempVertices) {
		if (vertex.x < min.x) min.x = vertex.x;
		if (vertex.y < min.y) min.y = vertex.y;
	}

	// facing
	for (Point &vertex : tempVertices) {
		switch (facing) {
		case 'N': {
		  vertices.push_back(Point(vertex.x, vertex.y));
		  break;
		}
		case 'W': {
		  vertices.push_back(Point(-vertex.y, vertex.x));
		  break;
		}
		case 'S': {
		  vertices.push_back(Point(-vertex.x, -vertex.y));
		  break;
		}
		case 'E': {
		  vertices.push_back(Point(vertex.y, -vertex.x));
		  break;
		}
		}
	}

	// flip
	if (flip == 'F') {
		for (Point &vertex : vertices) {
		  vertex.x = -vertex.x;
		}
	}

	// shift to original min
	Point newMin(BORDER.x, BORDER.y);
	for (Point &vertex : vertices) {
		if (vertex.x < newMin.x) newMin.x = vertex.x;
		if (vertex.y < newMin.y) newMin.y = vertex.y;
	}
	min.x -= newMin.x;
	min.y -= newMin.y;
	for (Point &vertex : vertices) {
		vertex.x += min.x;
		vertex.y += min.y;
	}
}

void shiftCoordinate(vector<Point> &vertices, Point coordinate) {
	for (Point &vertex : vertices) {
		vertex.x += coordinate.x;
		vertex.y += coordinate.y;
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
		cout<< v.x << " " << v.y << "\n";
	}
	cout << "through_block_net_num: " << through_block_net_num << endl
	<< "through_block_edge_net_num: ";
	for (BlockEdgeAndNum const &TBENN : through_block_edge_net_num) {
		cout << endl << "(" << TBENN.edge[0].x << ", "
		<< TBENN.edge[0].y << ") ("
		<< TBENN.edge[1].x << ", "
		<< TBENN.edge[1].y << ") "
		<< TBENN.net_num;
	}
	cout << endl;
	cout << "block_port_region: ";
	for (vector<Point> const &BPR : block_port_region) {
		cout << endl << "(" << BPR[0].x << ", " << BPR[0].y
		<< ") (" << BPR[1].x << ", " << BPR[1].y << ")";
	}
	cout << endl
		   << "is_feedthroughable: " << is_feedthroughable << endl
		   << "is_tile: " << is_tile << endl;
	cout << "----------------------" << endl;
}

void Block::ParserAllBlocks(int const &testCase) {
	// Open chip_top.def to get
	// blockName, blkID, coordinate, facingFlip
	ifstream file_chip_top("cad_case0" + to_string(testCase) + "/case0" + to_string(testCase) + "/chip_top.def");

	string line;
	string startWith = "- BLOCK_";
	regex getBlockName(R"(BLOCK_\d+)");
	regex getBlkID(R"(blk_\d+)");
	regex getCoordinate(R"(\(\s*([\d.]+)\s+([\d.]+)\s*\))");
	smatch m;
	string getFaceFlip;

	int lineNumber = 0;
	int componentNumber = 1;
	int componentNumberCount = 0;

	while (getline(file_chip_top, line)) {
		lineNumber++;
		string temp;

		// get UNITS DISTANCE MICRONS
		if (lineNumber == 7) {
			istringstream iss(line);
			iss >> temp >> temp >> temp >> UNITS_DISTANCE_MICRONS;
		}

		// get borders
		if (lineNumber == 8) {
			istringstream iss(line);
			iss >> temp >> temp >> temp >> temp >> temp >> temp >> border_X >> border_Y;
			border_X /= UNITS_DISTANCE_MICRONS;
			border_Y /= UNITS_DISTANCE_MICRONS;
			BORDER.x = border_X;
			BORDER.y = border_Y;
		}

		// get component number
		if (lineNumber == 10) {
			istringstream iss(line);
			iss >> temp >> componentNumber;
		}

		Block tempBlock;
		if (line.find(startWith) == 0) {
			// blockName
			if (regex_search(line, m, getBlockName)) {
				tempBlock.name = m.str();
				// Open caseOO_cfg.json to get
				// through_block_net_num, through_block_edge_net_num
				// block_port_region, is_feedthroughable, is_tile
				ifstream file_cfg("cad_case0" + to_string(testCase) + "/case0" + to_string(testCase) + "_cfg.json");
				stringstream buffer;
				buffer << file_cfg.rdbuf();
				string jsonString = buffer.str();
				Document document;
				document.Parse(jsonString.c_str());
				for (const auto &block : document.GetArray()) {
					if (tempBlock.name == block["block_name"].GetString()) {
						tempBlock.through_block_net_num = block["through_block_net_num"].GetInt();
						tempBlock.is_feedthroughable = block["is_feedthroughable"].GetString() == string("True");
						tempBlock.is_tile = block["is_tile"].GetString() == string("True");

						// through_block_edge_net_num
						const Value &TBENN = block["through_block_edge_net_num"];
						for (const auto &singleTBENN : TBENN.GetArray()) {
							BlockEdgeAndNum tempOne;
    						if (singleTBENN.Size() == 3 && singleTBENN[0].IsArray() && singleTBENN[1].IsArray()) {
    						    tempOne.edge.push_back(Point(singleTBENN[0][0].GetDouble(), singleTBENN[0][1].GetDouble()));
    						    tempOne.edge.push_back(Point(singleTBENN[1][0].GetDouble(), singleTBENN[1][1].GetDouble()));
    						    tempOne.net_num = singleTBENN[2].GetInt();
    						    tempBlock.through_block_edge_net_num.push_back(tempOne);
    						}
						}

						// block_port_region
						const Value &BPR = block["block_port_region"];
						for (auto const &thing : BPR.GetArray()) {
							vector<Point> tempOne;
							tempOne.push_back(Point(thing.GetArray()[0][0].GetDouble(), thing.GetArray()[0][1].GetDouble()));
							tempOne.push_back(Point(thing.GetArray()[1][0].GetDouble(), thing.GetArray()[1][1].GetDouble()));
							tempBlock.block_port_region.push_back(tempOne);
						}
						break;
					}
				}
				file_cfg.close();
			}

			// blkID
			if (regex_search(line, m, getBlkID)) {
				tempBlock.blkID = m.str();

				// Open blk file to get vertices
				ifstream file_blk("cad_case0" + to_string(testCase) + "/case0" + to_string(testCase) + "/" + tempBlock.blkID + ".def");
				int lineNum = 1;
				string verticesInfo;
				while (getline(file_blk, verticesInfo)) {
					if (lineNum == 8)
						break;
					lineNum++;
				}
				file_blk.close();
				sregex_iterator iter(verticesInfo.begin(), verticesInfo.end(), getCoordinate);
				sregex_iterator end;
				while (iter != end) {
					smatch match = *iter;
					double x = stod(match[1].str());
					double y = stod(match[2].str());
					x /= UNITS_DISTANCE_MICRONS;
					y /= UNITS_DISTANCE_MICRONS;
					tempBlock.vertices.push_back(Point(x, y));
					++iter;
				}
				if (tempBlock.vertices.size() == 2) tempBlock.expandVertices();
			}

			// getCoordinate
			if (regex_search(line, m, getCoordinate) && m.size() == 3) {
				double x = stod(m[1]);
				double y = stod(m[2]);
				x /= UNITS_DISTANCE_MICRONS;
				y /= UNITS_DISTANCE_MICRONS;
				tempBlock.coordinate = Point(x, y);
			}

			// getFacingFlip
			tempBlock.facingFlip = line.substr(line.length() - 4, 2);	
			// do facingAndFlip
			facingAndFlip(tempBlock.vertices, tempBlock.facingFlip);
			
			for (BlockEdgeAndNum &TBENN : tempBlock.through_block_edge_net_num) {
				facingAndFlip(TBENN.edge, tempBlock.facingFlip);
			}
			for (vector<Point> &BPR : tempBlock.block_port_region) {
				facingAndFlip(BPR, tempBlock.facingFlip);
			}
			
			// do shiftCoordinate
			shiftCoordinate(tempBlock.vertices, tempBlock.coordinate);
			
			for (BlockEdgeAndNum &TBENN : tempBlock.through_block_edge_net_num) {
				shiftCoordinate(TBENN.edge, tempBlock.coordinate);
			}
			for (vector<Point> &BPR : tempBlock.block_port_region) {
				shiftCoordinate(BPR, tempBlock.coordinate);
			}
			
			// write into totBLOCK
			this->allBlocks.push_back(tempBlock);

			componentNumberCount++;
		}
		if (componentNumberCount == componentNumber) break;
	}
	file_chip_top.close();
}