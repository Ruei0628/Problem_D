#include "Chip.h"
#include "Edge.h"

using namespace std;

Point BORDER;

vector<Point> edgeToPoint(Edge const &e) {
	return vector<Point>{e.first, e.second};
}

void shiftCoordinate(vector<Point> &vertices, Point coordinate) {
	for (Point &vertex : vertices) {
		vertex.x += coordinate.x;
		vertex.y += coordinate.y;
	}
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
		case 'N': { vertices.push_back(Point(vertex.x, vertex.y)); break; }
		case 'W': { vertices.push_back(Point(-vertex.y, vertex.x)); break; }
		case 'S': { vertices.push_back(Point(-vertex.x, -vertex.y)); break; }
		case 'E': { vertices.push_back(Point(vertex.y, -vertex.x)); break; }
		}
	}

	// flip
	if (flip == 'F') {
		for (Point &vertex : vertices) { vertex.x = -vertex.x; }
	}

	// shift to original min
	Point newMin(BORDER.x, BORDER.y);
	for (Point &vertex : vertices) {
		if (vertex.x < newMin.x) newMin.x = vertex.x;
		if (vertex.y < newMin.y) newMin.y = vertex.y;
	}
	min.x -= newMin.x;
	min.y -= newMin.y;
	shiftCoordinate(vertices, min);
}

Chip::Chip(int const &testCase) {
	/*
	Edges.allEdges.push_back(Edge(1, 0, 0, border_Y, ""));
	Edges.allEdges.push_back(Edge(0, 0, 0, border_X, ""));
	Edges.allEdges.push_back(Edge(1, border_X, 0, border_Y, ""));
	*/

	// Open chip_top.def to get
	// blockName, blkID, coordinate, facingFlip
	ifstream file_chip_top("cad_case0" + to_string(testCase) + "/case0" + to_string(testCase) + "/chip_top.def");

	string line;
	string startWithBlock = "- BLOCK_";
	regex getBlockName(R"(BLOCK_\d+)");
	regex getBlkID(R"(blk_\d+)");
	regex getCoordinate(R"(\(\s*([\d.]+)\s+([\d.]+)\s*\))");
	
	string startWithRegion = "- REGION_";
	regex getRegionName(R"(REGION_\d+)");

	smatch m;
	string getFaceFlip;

	int lineNumber = 0;

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

		// read block informations
		Block tempBlock;
		if (line.find(startWithBlock) == 0) {
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
								Point a(singleTBENN[0][0].GetDouble(), singleTBENN[0][1].GetDouble());
								Point b(singleTBENN[1][0].GetDouble(), singleTBENN[1][1].GetDouble());
    						    tempOne.edge = Edge(a,b);
    						    tempOne.net_num = singleTBENN[2].GetInt();
    						    tempBlock.through_block_edge_net_num.push_back(tempOne);
    						}
						}

						// block_port_region
						const Value &BPR = block["block_port_region"];
						for (auto const &thing : BPR.GetArray()) {
							Point a(thing.GetArray()[0][0].GetDouble(), thing.GetArray()[0][1].GetDouble());
							Point b(thing.GetArray()[1][0].GetDouble(), thing.GetArray()[1][1].GetDouble());
							tempBlock.block_port_region.push_back(Edge(a,b));
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
					if (lineNum == 8) break;
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
				tempBlock.verticesToEdges();
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
				vector<Point> convert = edgeToPoint(TBENN.edge);
				facingAndFlip(convert, tempBlock.facingFlip);
				TBENN.edge.first = convert[0];
				TBENN.edge.second = convert[1];
			}
			for (Edge &BPR : tempBlock.block_port_region) {
				vector<Point> convert = edgeToPoint(BPR);
				facingAndFlip(convert, tempBlock.facingFlip);
				BPR.first = convert[0];
				BPR.second = convert[1];
			}
			
			// do shiftCoordinate
			shiftCoordinate(tempBlock.vertices, tempBlock.coordinate);
			
			for (BlockEdgeAndNum &TBENN : tempBlock.through_block_edge_net_num) {
				vector<Point> convert = edgeToPoint(TBENN.edge);
				shiftCoordinate(convert, tempBlock.coordinate);
				TBENN.edge.first = convert[0];
				TBENN.edge.second = convert[1];
			}
			for (Edge &BPR : tempBlock.block_port_region) {
				vector<Point> convert = edgeToPoint(BPR);
				shiftCoordinate(convert, tempBlock.coordinate);
				BPR.first = convert[0];
				BPR.second = convert[1];
			}
			
			// write into totZone
			totZone.push_back(new Block(tempBlock));
		}

		// read region information
		Region tempRegion;
		if (line.find(startWithRegion) == 0) {
			// regionName
			if (regex_search(line, m, getRegionName)) {
				tempRegion.name = m.str();
			}
			// vertices
			sregex_iterator iter(line.begin(), line.end(), getCoordinate);
			sregex_iterator end;
			while (iter != end) {
				smatch match = *iter;
				double x = stod(match[1].str());
				double y = stod(match[2].str());
				tempRegion.vertices.push_back(Point(x / UNITS_DISTANCE_MICRONS, y / UNITS_DISTANCE_MICRONS));
				++iter;
			}
			tempRegion.expandVertices();
			totZone.push_back(new Region(tempRegion));
		}
	}
	file_chip_top.close();
}

Block Chip::getBlock(string blockName) const {
	for (Zone *z : totZone) {
		if (Block *bPtr = dynamic_cast<Block *>(z)) {
			if (bPtr->name == blockName) return *bPtr;
		}
	}
}

Region Chip::getRegion(string regionName) const {
	for (Zone *z : totZone) {
		if (Region *rPtr = dynamic_cast<Region *>(z)) {
			if (rPtr->name == regionName) return *rPtr;
		}
	}
}

void Chip::showAllZones() {
	for (Zone *z : totZone) {
		if (Block *bPtr = dynamic_cast<Block *>(z)) {
			if (bPtr->vertices.size() == 4) {
				bPtr->showBlockInfo();
			}
		} else if (Region *rPtr = dynamic_cast<Region *>(z)) {
			//rPtr->showRegionInfo();
		}
	}
}

Chip::~Chip() {
	for (Zone *z : totZone) {
		delete z;
	}
	for (Edge *e : totEdge) {
		delete e;
	}
	totZone.clear();
	totEdge.clear();
}