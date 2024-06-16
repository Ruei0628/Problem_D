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
    const Value &RX_NAME = net["RX"];
    for (const auto &rxName : RX_NAME.GetArray()) {
      tempRXNameArray.push_back(rxName.GetString());
    }
    vector<pair<double, double>> tempRXCoordArray;
    // Parse RX_COORD array
    const Value &RX_COORD = net["RX_COORD"];
    for (const auto &coordinate : RX_COORD.GetArray()) {
      double x = coordinate[0].GetDouble();
      double y = coordinate[1].GetDouble();
      tempRXCoordArray.push_back(make_pair(x, y));
    }
    // Write into RXs
	for(int i = 0; i < tempRXNameArray.size(); i++){
		tempNet.RXs.push_back(RX(tempRXNameArray[i], tempRXCoordArray[i]));
	}

    // Parse TX_COORD
    const Value &TX_COORD = net["TX_COORD"];
    if (TX_COORD.IsArray() && TX_COORD.Size() == 2) {
      tempNet.TX.TX_COORD = make_pair(TX_COORD[0].GetDouble(), TX_COORD[1].GetDouble());
    }

    // Parse HMFT_MUST_THROUGH
    const Value &HMFTMT = net["HMFT_MUST_THROUGH"];
	if (HMFTMT.IsArray()) {
		for (const auto &hmftmt : HMFTMT.GetArray()) {
			const Value &hmftmtData = hmftmt;
			HMFT_MUST_THROUGH tempHMFTMT;
			// First get the zone name
			tempHMFTMT.blockName = hmftmtData[0].GetString();
			// Next get edgeIn coordinates, there are four of them
			const Value &dataEdgeIn = hmftmtData[1];
			for(int i = 0; i < dataEdgeIn.Size(); i++){
        	    tempHMFTMT.edgeIn[i] = dataEdgeIn[i].GetDouble();
        	}
        	// Then get edgeOut coordinates, there are four of them
        	const Value &dataEdgeOut = hmftmtData[2];
        	for (int i = 0; i < dataEdgeOut.Size(); i++) {
        		tempHMFTMT.edgeOut[i] = dataEdgeOut[i].GetDouble();
        	}
			// So we get name, edgeIn, edgeOut happily
			// Finally we write these into the HMFTMTs
        	tempNet.HMFT_MUST_THROUGHs.push_back(tempHMFTMT);
    	}
	}
    

    // Parse MUST_THROUGH
    const Value &MT = net["MUST_THROUGH"];
    if (MT.IsArray()) {
		for (const auto &mt : MT.GetArray()) {
    		const Value &mtData = mt;
    		MUST_THROUGH tempMT;
    		// First get the zone name
    		tempMT.blockName = mtData[0].GetString();
    		// Next get edgeIn coordinates, there are four of them
    		const Value &dataEdgeIn = mtData[1];
    		for (int i = 0; i < dataEdgeIn.Size(); i++) {
    		  tempMT.edgeIn[i] = dataEdgeIn[i].GetDouble();
    		}
    		// Then get edgeOut coordinates, there are four of them
    		const Value &dataEdgeOut = mtData[2];
    		for (int i = 0; i < dataEdgeOut.Size(); i++) {
    		  tempMT.edgeOut[i] = dataEdgeOut[i].GetDouble();
    		}
    		// So we get name, edgeIn, edgeOut happily
    		// Finally we write these into the HMFTMTs
    		tempNet.MUST_THROUGHs.push_back(tempMT);
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
	<< "TX: " << TX.TX_NAME << " (" << TX.TX_COORD.first << ", " << TX.TX_COORD.second << ")" << endl
	<< "RXs: " << endl;
	for(const RX &zone: RXs){
		cout << zone.RX_NAME << " (" << zone.RX_COORD.first << ", " << zone.RX_COORD.second << ")" << endl;
	}
    cout << "NUM: " << num << endl
	<< "MUST_THROUGH: " << endl;
	if (MUST_THROUGHs.size()) {
        for (const MUST_THROUGH &t : MUST_THROUGHs) {
        	cout << t.blockName << ", (";
			for(int i = 0; i < 4; i++){
				cout << t.edgeIn[i] << ", ";
			}
			cout << "), (";
			for(int i = 0; i < 4; i++){
				cout << t.edgeOut[i] << ", ";
			}
			cout << ")" << endl;
        }
    }
    cout << "HMFT_MUST_THROUGH: " << endl;
    if (HMFT_MUST_THROUGHs.size()) {
    	for (const HMFT_MUST_THROUGH &t : HMFT_MUST_THROUGHs){
        	cout << t.blockName << ", (";
			for(int i = 0; i < 4; i++){
				cout << t.edgeIn[i] << ", ";
			}
			cout << "), (";
			for(int i = 0; i < 4; i++){
				cout << t.edgeOut[i] << ", ";
			}
			cout << ")" << endl;
		}
    }
    cout << "----------------------" << endl;
}