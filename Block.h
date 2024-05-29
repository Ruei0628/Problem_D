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
  vector<pair<double, double>> blockEdge;
  int net_num = -1;
};

class Block : public Zone {
public:
  Block();
  Block(Block const &);
  string blkID;
  pair<double, double> coordinate;
  string facingFlip;

  int through_block_net_num;
  BlockEdgeAndNum through_block_edge_net_num;
  vector<pair<double, double>> block_port_region;
  bool is_feedthroughable;
  bool is_tile;

  vector<Block> allBlocks;

  void ParserAllBlocks(int const &case_serial_number);
  void expandVertices();
  void facingAndFlip(string facingFlip, vector<pair<double, double>> &vertices);
  void shiftCoordinate(pair<double, double> shift,
                       vector<pair<double, double>> &vertices);
  void showBlockInfo();
};

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
  pair<double, double> tempVertice(vertices[0].first, vertices[1].second);
  vertices.insert(vertices.begin() + 1, tempVertice);
  tempVertice = make_pair(vertices[2].first, vertices[0].second);
  vertices.push_back(tempVertice);
}

void Block::facingAndFlip(string facingFlip,
                          vector<pair<double, double>> &vertices) {
  char flip = facingFlip[0];
  char facing = facingFlip[1];

  vector<pair<double, double>> tempVertices;
  tempVertices = vertices;
  vertices.clear();

  // facing
  for (pair<double, double> &vertex : tempVertices) {
    switch (facing) {
    case 'N': {
      vertices.push_back(make_pair(vertex.first, vertex.second));
      break;
    }
    case 'W': {
      vertices.push_back(make_pair(-vertex.second, vertex.first));
      break;
    }
    case 'S': {
      vertices.push_back(make_pair(-vertex.first, -vertex.second));
      break;
    }
    case 'E': {
      vertices.push_back(make_pair(vertex.second, -vertex.first));
      break;
    }
    }
  }

  // flip
  if (flip == 'F') {
    for (pair<double, double> &vertex : vertices) {
      vertex.first = -vertex.first;
    }
  }

  // shift to nonnegative
  double minX = 1000;
  double minY = 1000;
  for (pair<double, double> &vertex : vertices) {
    if (vertex.first < minX)
      minX = vertex.first;
    if (vertex.second < minY)
      minY = vertex.second;
  }
  for (pair<double, double> &vertex : vertices) {
    vertex.first -= minX;
    vertex.second -= minY;
  }
}

void Block::shiftCoordinate(pair<double, double> shift,
                            vector<pair<double, double>> &vertices) {
  for (pair<double, double> &vertex : vertices) {
    vertex.first += shift.first;
    vertex.second += shift.second;
  }
}

void Block::showBlockInfo() {
  cout << "blockName: '" << name << "'" << endl
       << "blkID: '" << blkID << "'" << endl
       << "coordinate: (" << coordinate.first << ", " << coordinate.second
       << ") " << endl
       << "facingFlip: '" << facingFlip << "'" << endl;
  cout << "vertices: " << endl;
  for (auto v : vertices) {
    cout << "(" << v.first << ", " << v.second << ")\n";
  }
  cout << "through_block_net_num: " << through_block_net_num << endl
       << "through_block_edge_net_num: ";
  if (through_block_edge_net_num.net_num != -1) {
    cout << "(" << through_block_edge_net_num.blockEdge[0].first << ", "
         << through_block_edge_net_num.blockEdge[0].second << ") ("
         << through_block_edge_net_num.blockEdge[1].first << ", "
         << through_block_edge_net_num.blockEdge[2].second << ") "
         << through_block_edge_net_num.net_num;
  }
  cout << endl;
  cout << "block_port_region: ";
  if (block_port_region.size()) {
    cout << "(" << block_port_region[0].first << ", "
         << block_port_region[0].second << ") (" << block_port_region[1].first
         << ", " << block_port_region[1].second << ")";
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
            tempBlock.through_block_net_num =
                block["through_block_net_num"].GetInt();
            tempBlock.is_feedthroughable =
                block["is_feedthroughable"].GetString() == string("True");
            tempBlock.is_tile = block["is_tile"].GetString() == string("True");
          }

          // through_block_edge_net_num
          const Value &TBENN = block["through_block_edge_net_num"];
          if (TBENN.Size()) {
            tempBlock.through_block_edge_net_num.blockEdge[0] =
                make_pair(TBENN[0][0].GetDouble(), TBENN[0][1].GetDouble());
            tempBlock.through_block_edge_net_num.blockEdge[1] =
                make_pair(TBENN[1][0].GetDouble(), TBENN[1][1].GetDouble());
            tempBlock.through_block_edge_net_num.net_num = TBENN[2].GetInt();
          }
          // block_port_region
          const Value &BPR = block["block_port_region"];
          if (BPR.Size()) {
            tempBlock.block_port_region.push_back(
                make_pair(BPR[0][0].GetDouble(), BPR[0][1].GetDouble()));
            tempBlock.block_port_region.push_back(
                make_pair(BPR[1][0].GetDouble(), BPR[1][1].GetDouble()));
          }
        }
        file_cfg.close();
      }

      // blkID
      if (regex_search(line, m, getBlkID)) {
        tempBlock.blkID = m.str();

        // Open blk file to get vertices
        ifstream file_blk("case" + to_string(testCase) + "_def/" +
                          tempBlock.blkID + ".def");
        int lineNum = 1;
        string verticesInfo;
        while (getline(file_blk, verticesInfo)) {
          if (lineNum == 8)
            break;
          lineNum++;
        }
        file_blk.close();
        sregex_iterator iter(verticesInfo.begin(), verticesInfo.end(),
                             getCoordinate);
        sregex_iterator end;
        while (iter != end) {
          smatch match = *iter;
          double x = stod(match[1].str());
          double y = stod(match[2].str());
          tempBlock.vertices.push_back(make_pair(x, y));
          ++iter;
        }
        if (tempBlock.vertices.size() == 2)
          tempBlock.expandVertices();
      }

      // getCoordinate
      if (std::regex_search(line, m, getCoordinate) && m.size() == 3) {
        tempBlock.coordinate = make_pair(stod(m[1]), stod(m[2]));
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

#endif // BLOCK_H_INCLUDED