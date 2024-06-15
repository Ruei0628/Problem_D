#include "Net.h"

void Net::ParserAllNets(int const &testCase) {
  ifstream file("test" + to_string(testCase) + ".json");

  stringstream buffer;
  buffer << file.rdbuf();
  string jsonString = buffer.str();
  Document document;
  document.Parse(jsonString.c_str());
  for (const auto &net : document.GetArray()) {
    Net tempNet;
    tempNet.ID = net["ID"].GetInt();
    tempNet.TX.TX_NAME = net["TX"].GetString();
    tempNet.num = net["NUM"].GetInt();

	vector<string> tempRXNameArray;
    // Parse RX array
    const Value &RX = net["RX"];
    for (const auto &rxName : RX.GetArray()) {
		struct RX tempRX;
      tempNet.RXs.push_back(rxName.GetString());
    }
    // Parse RX_COORD array
    const Value &RX_COORD = net["RX_COORD"];
    for (const auto &coordinate : RX_COORD.GetArray()) {
      if (coordinate.IsArray() && coordinate.Size() == 2) {
        double x = coordinate[0].GetDouble();
        double y = coordinate[1].GetDouble();
        tempNet.rxCoord.push_back(make_pair(x, y));
      }
    }

    // Parse TX_COORD
    const Value &TX_COORD = net["TX_COORD"];
    if (TX_COORD.IsArray() && TX_COORD.Size() == 2) {
      tempNet.TX.TX_COORD = make_pair(TX_COORD[0].GetDouble(), TX_COORD[1].GetDouble());
    }

    // Parse HMFT_MUST_THROUGH
    const Value &HMFTMT = net["HMFT_MUST_THROUGH"];
    if (HMFTMT.IsArray()) {
      for (const auto &coordinate : HMFTMT.GetArray()) {
        if (coordinate.IsArray() && coordinate.Size() == 2) {
          double x = coordinate[0].GetDouble();
          double y = coordinate[1].GetDouble();
          tempNet.hmftMustThrough.push_back(make_pair(x, y));
        }
      }
    }

    // Parse MUST_THROUGH
    const Value &MT = net["MUST_THROUGH"];
    if (MT.IsArray()) {
      for (const auto &coordinate : MT.GetArray()) {
        if (coordinate.IsArray() && coordinate.Size() == 2) {
          double x = coordinate[0].GetDouble();
          double y = coordinate[1].GetDouble();
          tempNet.mustThrough.push_back(make_pair(x, y));
        }
      }
    }

	this->allNets.push_back(tempNet);
  }
  file.close();
}

Net Net::getNet(int const &id) const {
	for (const Net &n : allNets) {
		if (n.ID == id) return n;
	}
}

void Net::showNetInfo(){
	cout << "ID: " << ID << endl
	<< "TX: " << TX.TX_NAME << endl
	<< "RX: " << endl;
	for(const RX &zone: RXs){
		cout << zone.RX_NAME << endl;
	}
    cout << "NUM: " << num << endl
	<< "MUST_THROUGH: " << endl;
	if (MUST_THROUGHs.size()) {
        for (const MUST_THROUGH &t : MUST_THROUGHs) {
        	cout << t.blockName << ", (" << t.edgeIn.first << ", "
            << t.edgeIn.second << "), (" << t.edgeOut.first << ", "
        	<< t.edgeOut.second << endl;
        }
    }
    cout << "HMFT_MUST_THROUGH: " << endl;
    if (HMFT_MUST_THROUGHs.size()) {
    	for (const HMFT_MUST_THROUGH &t : HMFT_MUST_THROUGHs){
			cout << t.blockName << ", (" 
			<< t.edgeIn.first << ", " << t.edgeIn.second<< "), ("
			<< t.edgeOut.first << ", " << t.edgeOut.second << endl;
		}
    }
    cout << "TX_COORD: " << TX.TX_COORD.first << ", " << TX.TX_COORD.second << endl;
    cout << "RX_COORD: " << endl;
	for (const RX &coord : RXs){
		cout << coord.RX_COORD.first << ", " << coord.RX_COORD.second << endl;
    }
    cout << "----------------------" << endl;
}