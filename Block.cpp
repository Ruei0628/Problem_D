#include "Block.h"

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

void Block::facingAndFlip(string facingFlip,
                          vector<Point> &vertices) {
  char flip = facingFlip[0];
  char facing = facingFlip[1];

  vector<Point> tempVertices;
  tempVertices = vertices;
  vertices.clear();

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

  // shift to nonnegative
  double minX = 6220;
  double minY = 5180;
  for (Point &vertex : vertices) {
    if (vertex.x < minX)
      minX = vertex.x;
    if (vertex.y < minY)
      minY = vertex.y;
  }
  for (Point &vertex : vertices) {
    vertex.x -= minX;
    vertex.y -= minY;
  }
}

void Block::shiftCoordinate(Point shift, vector<Point> &vertices) {
  for (Point &vertex : vertices) {
    vertex.x += shift.x;
    vertex.y += shift.y;
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
    cout << "(" << v.x << ", " << v.y << ")\n";
  }
  cout << "through_block_net_num: " << through_block_net_num << endl
       << "through_block_edge_net_num: ";
  if (through_block_edge_net_num.net_num != -1) {
    cout << "(" << through_block_edge_net_num.blockEdge[0].x << ", "
         << through_block_edge_net_num.blockEdge[0].y << ") ("
         << through_block_edge_net_num.blockEdge[1].x << ", "
         << through_block_edge_net_num.blockEdge[2].y << ") "
         << through_block_edge_net_num.net_num;
  }
  cout << endl;
  cout << "block_port_region: ";
  if (block_port_region.size()) {
    cout << "(" << block_port_region[0].x << ", "
         << block_port_region[0].y << ") (" << block_port_region[1].x
         << ", " << block_port_region[1].y << ")";
  }
  cout << endl
       << "is_feedthroughable: " << is_feedthroughable << endl
       << "is_tile: " << is_tile << endl;
  cout << "----------------------" << endl;
}

void Block::ParserAllBlocks(int const &testCase) {
  // Open chip_top.def to get
  // blockName, blkID, coordinate, facingFlip
  ifstream file_chip_top("case" + to_string(testCase) + "_def/chip_top.def");

  string line;
  string startWith = "- BLOCK_";
  regex getBlockName(R"(BLOCK_\d+)");
  regex getBlkID(R"(blk_\d+)");
  regex getCoordinate(R"(\(\s*([\d.]+)\s+([\d.]+)\s*\))");
  smatch m;
  string getFaceFlip;
  while (getline(file_chip_top, line)) {
    Block tempBlock;
    if (line.find(startWith) == 0) {
		// blockName
		if (regex_search(line, m, getBlockName)) {
			tempBlock.name = m.str();
			// Open caseOO_cfg.json to get
			// through_block_net_num, through_block_edge_net_num
			// block_port_region, is_feedthroughable, is_tile
			ifstream file_cfg("case" + to_string(testCase) + "_cfg.json");
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
				}

				// through_block_edge_net_num
				const Value &TBENN = block["through_block_edge_net_num"];
				if (TBENN.Size()) {
					tempBlock.through_block_edge_net_num.blockEdge[0] = Point(TBENN[0][0].GetDouble(), TBENN[0][1].GetDouble());
					tempBlock.through_block_edge_net_num.blockEdge[1] = Point(TBENN[1][0].GetDouble(), TBENN[1][1].GetDouble());
					tempBlock.through_block_edge_net_num.net_num = TBENN[2].GetInt();
				}
				// block_port_region
				const Value &BPR = block["block_port_region"];
				if (BPR.Size()) {
					tempBlock.block_port_region.push_back(Point(BPR[0][0].GetDouble(), BPR[0][1].GetDouble()));
            		tempBlock.block_port_region.push_back(Point(BPR[1][0].GetDouble(), BPR[1][1].GetDouble()));
        		}
        	}
        	file_cfg.close();
    	}

		// blkID
		if (regex_search(line, m, getBlkID)) {
			tempBlock.blkID = m.str();

			// Open blk file to get vertices
			ifstream file_blk("case" + to_string(testCase) + "_def/" + tempBlock.blkID + ".def");
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
				x /= 2000;
				y /= 2000;
        		tempBlock.vertices.push_back(Point(x, y));
        		++iter;
        	}
        	if (tempBlock.vertices.size() == 2)
        		tempBlock.expandVertices();
    	}

    	// getCoordinate
    	if (std::regex_search(line, m, getCoordinate) && m.size() == 3) {
        	double x = stod(m[1]);
        	double y = stod(m[2]);
			x /= 2000;
			y /= 2000;
    		tempBlock.coordinate = Point(x, y);
    	}

    	// getFacingFlip
    	tempBlock.facingFlip = line.substr(line.length() - 4, 2);	
    	// do facingAndFlip
    	tempBlock.facingAndFlip(tempBlock.facingFlip, tempBlock.vertices);
    	tempBlock.facingAndFlip(tempBlock.facingFlip,
    	                        tempBlock.through_block_edge_net_num.blockEdge);
    	tempBlock.facingAndFlip(tempBlock.facingFlip,
    	                        tempBlock.block_port_region);	
    	// do shiftCoordinate
    	tempBlock.shiftCoordinate(tempBlock.coordinate, tempBlock.vertices);
    	tempBlock.shiftCoordinate(tempBlock.coordinate,
    	                          tempBlock.through_block_edge_net_num.blockEdge);
    	tempBlock.shiftCoordinate(tempBlock.coordinate,
    	                          tempBlock.block_port_region);	
    	// read into totBLOCK
    	this->allBlocks.push_back(tempBlock);
    }
  }
  file_chip_top.close();
}