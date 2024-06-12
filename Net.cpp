#include "Net.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void readFile(const char *filename, vector<Net> &Nets) {
  ifstream file(filename);
  if (!file.is_open()) {
    cerr << "Unable to open file." << endl;
    return;
  }

  string line;
  while (getline(file, line)) {
    if (line.find('{') != string::npos) {
      Net net;
      string txBlockOrRegion, txID;
      getline(file, line);
      sscanf_s(line.c_str(), " \"ID\":%d,", &net.ID);
      getline(file, line);
      size_t pos = line.find('_');
      txBlockOrRegion = line.substr(8, pos - 8);
      txID = line.substr(pos + 1, line.length() - pos - 3);
      net.TX.TX_info.first = (txBlockOrRegion == "BLOCK") ? 0 : 1;
      net.TX.TX_info.second = stoi(txID);
      getline(file, line);
      string rxBlockOrRegion, rxID;
      pos = line.find('[');
      size_t endPos = line.find(']');
      string rxLine = line.substr(pos + 1, endPos - pos - 1);
      istringstream iss(rxLine);
      string rx;
      while (getline(iss, rx, ',')) {
        rx = rx.substr(1, rx.length() - 2);
        size_t pos = rx.find('_');
        rxBlockOrRegion = rx.substr(0, pos);
        rxID = rx.substr(pos + 1);
        RX newRX;
        newRX.RX_info.first = (rxBlockOrRegion == "BLOCK") ? 0 : 1;
        newRX.RX_info.second = stoi(rxID);
        net.RXs.push_back(newRX);
      }
      getline(file, line);
      sscanf_s(line.c_str(), " \"NUM\":%d,", &net.num);
      getline(file, line); // MUST_THROUGH
      getline(file, line); // HMFT_MUST_THROUGH
      getline(file, line);
      sscanf_s(line.c_str(), " \"TX_COORD\":[%lf,%lf],", &net.TX.TX_COORD.first,
               &net.TX.TX_COORD.second);
      getline(file, line);
      pos = line.find('[');
      endPos = line.find_last_of(']');
      string rxCoordLine = line.substr(pos + 1, endPos - pos - 1);
      istringstream issCoord(rxCoordLine);
      string coordPair;
      int rxIndex = 0;
      while (getline(issCoord, coordPair, ']')) {
        size_t coordPos = coordPair.find('[');
        if (coordPos != string::npos) {
          coordPair = coordPair.substr(coordPos + 1);
          double x, y;
          sscanf_s(coordPair.c_str(), "%lf,%lf", &x, &y);
          if (rxIndex < net.RXs.size()) {
            net.RXs[rxIndex].RX_COORD = {x, y}; // Access directly
          } else {
            cerr << "Error: RX index out of range. rxIndex: " << rxIndex
                 << ", RXs size: " << net.RXs.size() << endl;
          }
          rxIndex++;
        }
      }
      Nets.push_back(net);
    }
  }
  file.close();
}

int main(){
  vector<Net> Nets;
  readFile("case4.json", Nets);

  for (const auto &net : Nets) {
    cout << "ID: " << net.ID << endl;
    cout << "TX: " << (net.TX.TX_info.first == 0 ? "BLOCK" : "REGION") << "_"
         << net.TX.TX_info.second << endl;
    cout << "RX: [";
    for (size_t i = 0; i < net.RXs.size(); ++i) {
      cout << (net.RXs[i].RX_info.first == 0 ? "BLOCK" : "REGION") << "_"
           << net.RXs[i].RX_info.second << ", ";
    }
    cout << "\b\b]" << endl;
    cout << "Num: " << net.num << endl;
    cout << "TX_COORD: [" << net.TX.TX_COORD.first << ", "
         << net.TX.TX_COORD.second << "]" << endl;
    cout << "RX_COORD: [";
    for (size_t i = 0; i < net.RXs.size(); ++i) {
      cout << "[" << net.RXs[i].RX_COORD.first << ","
           << net.RXs[i].RX_COORD.second << "], ";
    }
    cout.unsetf(ios::fixed);
    cout << "\b\b]\n-----------------------------------------------\n";
  }
}