#include "Chip.h"
#include "Edge.h"
#include <memory>

using namespace std;

Chip::Chip(int const &testCase) {
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
			double border_X, border_Y;
			iss >> temp >> temp >> temp >> temp >> temp >> temp >> border_X >> border_Y;
			border_X /= UNITS_DISTANCE_MICRONS;
			border_Y /= UNITS_DISTANCE_MICRONS;
			border = Point(border_X, border_Y);
		}

		// read block informations
		unique_ptr<Block> tempBlock = make_unique<Block>();
		if (line.find(startWithBlock) == 0) {
			// blockName
			if (regex_search(line, m, getBlockName)) {
				tempBlock->name = m.str();
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
					if (tempBlock->name == block["block_name"].GetString()) {
						tempBlock->through_block_net_num = block["through_block_net_num"].GetInt();
						tempBlock->is_feedthroughable = block["is_feedthroughable"].GetString() == string("True");
						tempBlock->is_tile = block["is_tile"].GetString() == string("True");

						// through_block_edge_net_num
						const Value &TBENN = block["through_block_edge_net_num"];
						for (const auto &singleTBENN : TBENN.GetArray()) {
							BlockEdgeAndNum tempOne;
    						if (singleTBENN.Size() == 3 && singleTBENN[0].IsArray() && singleTBENN[1].IsArray()) {
								Point a(singleTBENN[0][0].GetDouble(), singleTBENN[0][1].GetDouble());
								Point b(singleTBENN[1][0].GetDouble(), singleTBENN[1][1].GetDouble());
    						    tempOne.edge = Edge(a, b); /*not sure*/
    						    tempOne.net_num = singleTBENN[2].GetInt();
    						    tempBlock->through_block_edge_net_num.push_back(tempOne); /*here*/
    						}
						}

						// block_port_region
						const Value &BPR = block["block_port_region"];
						for (auto const &thing : BPR.GetArray()) {
							Point a(thing.GetArray()[0][0].GetDouble(), thing.GetArray()[0][1].GetDouble());
							Point b(thing.GetArray()[1][0].GetDouble(), thing.GetArray()[1][1].GetDouble());
							tempBlock->block_port_region.push_back(Edge(a, b)); /*not sure*/
						}
						break;
					}
				}
				file_cfg.close();
			}

			// blkID
			if (regex_search(line, m, getBlkID)) {
				tempBlock->blkID = m.str();

				// Open blk file to get vertices
				ifstream file_blk("cad_case0" + to_string(testCase) + "/case0" + to_string(testCase) + "/" + tempBlock->blkID + ".def");
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
					tempBlock->vertices.push_back(Point(x, y));
					++iter;
				}
				if (tempBlock->vertices.size() == 2) tempBlock->expandVertices();
			}

			// getCoordinate
			if (regex_search(line, m, getCoordinate) && m.size() == 3) {
				double x = stod(m[1]);
				double y = stod(m[2]);
				x /= UNITS_DISTANCE_MICRONS;
				y /= UNITS_DISTANCE_MICRONS;
				tempBlock->coordinate = Point(x, y);
			}

			// getFacingFlip
			tempBlock->facingFlip = line.substr(line.length() - 4, 2);

			// do faceAndFlip and shift to ALL members have vertices
			tempBlock->transposeAllVertices();

			// store the fliped and shifted vertices into edges
			tempBlock->verticesToEdges();

			// collect edges from tempBlock, and write into totEdge
			for (Edge &e : tempBlock->edges) { allEdges.push_back(e); }
			// turn off this when testing

			// write into totZone
			allBlocks.push_back(std::move(tempBlock));
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
			allRegions.push_back(Region(std::move(tempRegion)));
		}
	}
	file_chip_top.close();

	// Edges: already have block Edges
	// here adding the chip border
	unique_ptr<Block> BORDER = make_unique<Block>("border");
	BORDER->vertices = { Point(0, 0), Point(0, border.y), border, Point(border.x, 0) };
	BORDER->verticesToEdges();
	for (Edge &e : BORDER->edges) { allEdges.push_back(e); }
	allBlocks.push_back(std::move(BORDER));

	// make it ordered
	std::sort(allEdges.begin(), allEdges.end(), [](const auto& a, const auto& b) { return a.fixed() < b.fixed(); });
}

Block Chip::getBlock(string blockName) const {
    for (auto const &b : allBlocks) {
        if (b->name == blockName) return *b;
    }
    return Block();
}

Region Chip::getRegion(string regionName) const {
	for (auto const &r : allRegions) {
		if (r.name == regionName) return r;
	}
	return Region();
}

void Chip::showAllZones() const {
	for (auto const &b : allBlocks) { b->showBlockInfo(); }
	for (auto const &r : allRegions) { r.showRegionInfo(); }
}

Chip::~Chip() {}