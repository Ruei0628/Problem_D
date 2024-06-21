#include "Net.h"

void Net::ParserAllNets(int const &testCase, AllZone const &allZone) {
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
		vector<Point> tempRXCoordArray;
		// Parse RX_COORD array
		const Value &RX_COORD = net["RX_COORD"];
		for (const auto &coordinate : RX_COORD.GetArray()) {
			double x = coordinate[0].GetDouble();
			double y = coordinate[1].GetDouble();
			tempRXCoordArray.push_back(Point(x, y));
		}
		// Write into RXs
		for(int i = 0; i < tempRXNameArray.size(); i++){
			tempNet.RXs.push_back(RX(tempRXNameArray[i], tempRXCoordArray[i]));
		}
		// Parse TX_COORD
		const Value &TX_COORD = net["TX_COORD"];
		if (TX_COORD.IsArray() && TX_COORD.Size() == 2) {
			tempNet.TX.TX_COORD = Point(TX_COORD[0].GetDouble(), TX_COORD[1].GetDouble());
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
		tempNet.getBoundBoxArea(allZone);

		this->allNets.push_back(tempNet);
	}
	file.close();
}

Net Net::getNet(int const &id) const {
	for (const Net &n : allNets) {
		if (n.ID == id) return n;
	}
}

Point getCoordinates(const string &name, const Point &offset, const AllZone &allZone) {
    double x, y;
    if(name[0] == 'B') {
        x = allZone.getBlock(name).coordinate.first;
        y = allZone.getBlock(name).coordinate.second;
    }
    else if(name[0] == 'R') {
        x = allZone.getRegion(name).vertices[0].first;
        y = allZone.getRegion(name).vertices[0].second;
    }
    double newX = x + offset.x;
    double newY = y + offset.y;
    return Point(newX, newY);
}

// Updated getBeginPoint function
Point getBeginPoint(const TX &thePoint, const AllZone &allZone) {
    return getCoordinates(thePoint.TX_NAME, thePoint.TX_COORD, allZone);
}

// Updated getEndPoint function
Point getEndPoint(const RX &thePoint, const AllZone &allZone) {
    return getCoordinates(thePoint.RX_NAME, thePoint.RX_COORD, allZone);
}

double findBoundBox(vector<Point> const &coords){
	double x_min = coords[0].x, x_max = coords[0].x,
		   y_min = coords[0].y, y_max = coords[0].y;
	for (auto const &c : coords) {
		double x_this = c.x, y_this = c.y;
		if (x_this > x_max) x_max = x_this;
		if (x_this < x_min) x_min = x_this;
		if (y_this > y_max) y_max = y_this;
		if (y_this < y_min) y_min = y_this;
	}
	return (x_max - x_min) * (y_max - y_min);
}

void Net::getBoundBoxArea(AllZone const &allZone){
	vector<Point> coords;
	for(RX const &rx : RXs){
		coords.push_back(getEndPoint(rx, allZone));
	}
	coords.push_back(getBeginPoint(TX, allZone));
	boundBoxArea = findBoundBox(coords);
}

void Net::showNetInfo(){
	cout << "ID: " << ID << endl
	<< "TX: " << TX.TX_NAME << " (" << TX.TX_COORD.x << ", " << TX.TX_COORD.y << ")" << endl
	<< "RXs: " << endl;
	for(const RX &zone: RXs){
		cout << zone.RX_NAME << " (" << zone.RX_COORD.x << ", " << zone.RX_COORD.y << ")" << endl;
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