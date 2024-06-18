#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>


using namespace std;

struct Point {
  double x, y;
  Point(double x = 0, double y = 0) : x(x), y(y) {}
};

struct RX {
  string RX_NAME;
  pair<double, double> RX_COORD;
  RX(const string &name, const pair<double, double> &coord)
      : RX_NAME(name), RX_COORD(coord) {}
};

struct TX {
  string TX_NAME;
  pair<double, double> TX_COORD;
};

struct MUST_THROUGH {
  string blockName;
  double edgeIn[4];
  double edgeOut[4];
};

struct HMFT_MUST_THROUGH {
  string blockName;
  double edgeIn[4];
  double edgeOut[4];
};

struct Net {
  int ID;
  TX TX;
  vector<RX> RXs;
  vector<MUST_THROUGH> MUST_THROUGHs;
  vector<HMFT_MUST_THROUGH> HMFT_MUST_THROUGHs;
  vector<pair<double, double>> routing_points;
};

struct BlockEdgeAndNum {
  vector<pair<double, double>> blockEdge;
  int net_num = -1;
};

struct Block {
  string blkID;
  int through_block_net_num;
  vector<pair<double, double>> block_port_region;
  BlockEdgeAndNum through_block_edge_net_num;
  bool is_feedthroughable;
};

struct AllZone {
  unordered_map<string, Block> blocks;
  unordered_map<string, Region> regions;

  Block getBlock(const string &name) const { return blocks.at(name); }

  Region getRegion(const string &name) const { return regions.at(name); }
};

pair<double, double> getCoordinates(const string &name,
                                    const pair<double, double> &offset,
                                    const AllZone &allZone) {
  double x, y;
  if (name[0] == 'B') {
    x = allZone.getBlock(name).coordinate.first;
    y = allZone.getBlock(name).coordinate.second;
  } else if (name[0] == 'R') {
    x = allZone.getRegion(name).vertices[0].first;
    y = allZone.getRegion(name).vertices[0].second;
  }
  double newX = x + offset.first;
  double newY = y + offset.second;
  return make_pair(newX, newY);
}

pair<double, double> getBeginPoint(const RX &thePoint, const AllZone &allZone) {
  return getCoordinates(thePoint.RX_NAME, thePoint.RX_COORD, allZone);
}

pair<double, double> getEndPoint(const TX &thePoint, const AllZone &allZone) {
  return getCoordinates(thePoint.TX_NAME, thePoint.TX_COORD, allZone);
}

double cartesianLength(pair<double, double> tx, pair<double, double> rx) {
  double x_length = abs(tx.first - rx.first);
  double y_length = abs(tx.second - rx.second);
  return x_length + y_length;
}

bool compareBySecond(const pair<Net, double> &a, const pair<Net, double> &b) {
  return a.second < b.second;
}

void mikami_tabuchi(pair<double, double> beginPoint,
                    vector<pair<double, double>> endPoints,
                    const vector<Net> &Nets, const AllZone &allZone) {
  struct Probe {
    pair<double, double> coord;
    bool fromSource; // true if from source, false if from target
  };

  vector<Probe> currentSourceProbes, currentTargetProbes;
  vector<Probe> oldSourceProbes, oldTargetProbes;
  vector<pair<double, double>> intersections;

  auto addProbes = [](const pair<double, double> &point, bool fromSource,
                      vector<Probe> &probes) {
    probes.push_back({point, fromSource});
    probes.push_back({{point.first, point.second + 1}, fromSource});
    probes.push_back({{point.first + 1, point.second}, fromSource});
    probes.push_back({{point.first, point.second - 1}, fromSource});
    probes.push_back({{point.first - 1, point.second}, fromSource});
  };

  addProbes(beginPoint, true, currentSourceProbes);
  for (const auto &endPoint : endPoints) {
    addProbes(endPoint, false, currentTargetProbes);
  }

  for (const auto &net : Nets) {
    for (const auto &mt : net.MUST_THROUGHs) {
      pair<double, double> edgeInStart = {mt.edgeIn[0], mt.edgeIn[1]};
      pair<double, double> edgeInEnd = {mt.edgeIn[2], mt.edgeIn[3]};
      pair<double, double> edgeOutStart = {mt.edgeOut[0], mt.edgeOut[1]};
      pair<double, double> edgeOutEnd = {mt.edgeOut[2], mt.edgeOut[3]};
      addProbes(edgeInStart, true, currentSourceProbes);
      addProbes(edgeOutEnd, false, currentTargetProbes);
    }
    for (const auto &hmft : net.HMFT_MUST_THROUGHs) {
      pair<double, double> edgeInStart = {hmft.edgeIn[0], hmft.edgeIn[1]};
      pair<double, double> edgeInEnd = {hmft.edgeIn[2], hmft.edgeIn[3]};
      pair<double, double> edgeOutStart = {hmft.edgeOut[0], hmft.edgeOut[1]};
      pair<double, double> edgeOutEnd = {hmft.edgeOut[2], hmft.edgeOut[3]};
      addProbes(edgeInStart, true, currentSourceProbes);
      addProbes(edgeOutEnd, false, currentTargetProbes);
    }
  }

  int level = 0;
  bool pathFound = false;

  while (!pathFound) {
    oldSourceProbes.insert(oldSourceProbes.end(), currentSourceProbes.begin(),
                           currentSourceProbes.end());
    oldTargetProbes.insert(oldTargetProbes.end(), currentTargetProbes.begin(),
                           currentTargetProbes.end());

    currentSourceProbes.clear();
    currentTargetProbes.clear();

    for (const auto &sourceProbe : oldSourceProbes) {
      for (const auto &targetProbe : oldTargetProbes) {
        if (sourceProbe.coord == targetProbe.coord) {
          intersections.push_back(sourceProbe.coord);
          pathFound = true;
          break;
        }
      }
      if (pathFound)
        break;
    }

    if (pathFound)
      break;

    for (const auto &sourceProbe : oldSourceProbes) {
      addProbes(sourceProbe.coord, true, currentSourceProbes);
    }
    for (const auto &targetProbe : oldTargetProbes) {
      addProbes(targetProbe.coord, false, currentTargetProbes);
    }

    ++level;
  }

  if (pathFound) {
    vector<pair<double, double>> path;
    path.push_back(intersections[0]);

    while (level > 0) {
      pair<double, double> current = path.back();
      for (const auto &sourceProbe : oldSourceProbes) {
        if (sourceProbe.coord == current) {
          path.push_back(sourceProbe.coord);
          break;
        }
      }
      --level;
    }

    reverse(path.begin(), path.end());
    level = 0;

    while (level > 0) {
      pair<double, double> current = path.back();
      for (const auto &targetProbe : oldTargetProbes) {
        if (targetProbe.coord == current) {
          path.push_back(targetProbe.coord);
          break;
        }
      }
      --level;
    }

    for (const auto &point : path) {
      cout << "(" << point.first << ", " << point.second << ")" << endl;
    }
  } else {
    cout << "No path found." << endl;
  }
}

int main() {
  vector<Net> Nets;
  vector<Block> Blocks;
  vector<Region> Regions;
  AllZone allZone;

  // Parser functions to read Nets, Blocks, and Regions would be implemented
  // here Nets.ParserAllNets(testCase); Blocks.ParserAllBlocks(testCase);

  vector<pair<Net, double>> netMinLength;

  for (const Net &n : Nets) {
    double len = 0;
    for (const RX &rx : n.RXs) {
      len += cartesianLength(n.TX.TX_COORD, rx.RX_COORD);
    }
    int sizeOfRXs = n.RXs.size();
    netMinLength.push_back(make_pair(n, len / sizeOfRXs));
  }

  sort(netMinLength.begin(), netMinLength.end(), compareBySecond);

  for (const auto &netInfo : netMinLength) {
    Net net = netInfo.first;
    pair<double, double> beginPoint = getEndPoint(net.TX, allZone);
    vector<pair<double, double>> endPoints;
    for (const RX &rx : net.RXs) {
      endPoints.push_back(getBeginPoint(rx, allZone));
    }
    mikami_tabuchi(beginPoint, endPoints, Nets, allZone);
  }

  return 0;
}