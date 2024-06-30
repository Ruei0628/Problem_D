#include "Net.h"
#include <cmath>

void Net::ParserAllNets (int const &testCase) {
	ifstream file("cad_case0" + to_string(testCase) + "/case0" + to_string(testCase) + ".json");
	stringstream buffer;
	buffer << file.rdbuf();
	string jsonString = buffer.str();
	Document document;
	document.Parse(jsonString.c_str());
	for (auto const &net : document.GetArray()) {
		Net tempNet;

		// ID:
		tempNet.ID = net["ID"].GetInt();
		// TX_NAME:
		tempNet.TX.TX_NAME = net["TX"].GetString();
		// TX_COORD:
		tempNet.TX.TX_COORD = Point(net["TX_COORD"][0].GetDouble(), net["TX_COORD"][1].GetDouble());

		// RX_NAMEs:
	    vector<string> tempRXNameArray;
		// Parse RX_NAME array
		for (const auto &rxName : net["RX"].GetArray()) {
			tempRXNameArray.push_back(rxName.GetString());
		}
		vector<Point> tempRXCoordArray;
		// Parse RX_COORD array
		for (const auto &coordinate : net["RX_COORD"].GetArray()) {
			double x = coordinate[0].GetDouble();
			double y = coordinate[1].GetDouble();
			tempRXCoordArray.push_back(Point(x, y));
		}
		// Write into RXs
	    for(int i = 0; i < tempRXNameArray.size(); i++){
	    	tempNet.RXs.push_back(RX{tempRXNameArray[i], tempRXCoordArray[i]});
	    }

		// NUM:
		tempNet.num = net["NUM"].GetInt();

		// HMFT_MUST_THROUGH
        for (const auto &hmftmt : net["HMFT_MUST_THROUGH"].GetObject()) {
            HMFT_MUST_THROUGH tempHmftmt;
            tempHmftmt.blockName = hmftmt.name.GetString();
			for (auto const &coord : hmftmt.value.GetArray()) {
				array<double, 4> tempEdges;
				for (int i = 0; i < 4; i++) {
					tempEdges[i] = coord[i].GetDouble();
				}
				tempHmftmt.edges.push_back(tempEdges);
			}
            tempNet.HMFT_MUST_THROUGHs.push_back(tempHmftmt);
        }

        // MUST_THROUGH
        for (const auto &mt : net["MUST_THROUGH"].GetObject()) {
            MUST_THROUGH tempMt;
            tempMt.blockName = mt.name.GetString();
			// mt.value.Size()
			for (auto const &coord : mt.value.GetArray()) {
				array<double, 4> tempEdges;
				for (int i = 0; i < 4; i++) {
					tempEdges[i] = coord[i].GetDouble();
				}
				tempMt.edges.push_back(tempEdges);
			}
            tempNet.MUST_THROUGHs.push_back(tempMt);
        }
	}
	file.close();
}

Net Net::getNet(int const &id) const {
	for (const Net &n : allNets) {
		if (n.ID == id) return n;
	}
}

struct TX Net::absoluteTX(AllZone const &allZone) const {
    double x = 0, y = 0;
    if(TX.TX_NAME[0] == 'B') {
        x = allZone.getBlock(TX.TX_NAME).coordinate.x;
        y = allZone.getBlock(TX.TX_NAME).coordinate.y;
    }
    x += TX.TX_COORD.x;
    y += TX.TX_COORD.y;
    struct TX newTX;
    newTX.TX_COORD = Point(x, y);
    newTX.TX_NAME = TX.TX_NAME;
	return newTX;
}

struct RX Net::absoluteRX(RX const &rx, AllZone const &allZone) const {
    double x = 0, y = 0;
    if(rx.RX_NAME[0] == 'B') {
        x = allZone.getBlock(rx.RX_NAME).coordinate.x;
        y = allZone.getBlock(rx.RX_NAME).coordinate.y;
    }
    x += rx.RX_COORD.x;
    y += rx.RX_COORD.y;
    struct RX newRX;
    newRX.RX_COORD = Point(x, y);
    newRX.RX_NAME = rx.RX_NAME;
    return newRX;
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
		coords.push_back(this->absoluteRX(rx, allZone).RX_COORD);
	}
	coords.push_back(this->absoluteTX(allZone).TX_COORD);
	boundBoxArea = findBoundBox(coords);
}

void Net::showNetInfo() {
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
			cout << " - " << t.blockName;
			for (auto const &c : t.edges) {
				cout << " [ ";
				for (double const &n : c) {
					cout << n << " ";
				}
				cout << "]";
			}
			cout << endl;
		}
	}
	cout << "HMFT_MUST_THROUGH: " << endl;
	if (HMFT_MUST_THROUGHs.size()) {
		for (const HMFT_MUST_THROUGH &t : HMFT_MUST_THROUGHs){
			cout << " - " << t.blockName;
			for (auto const &c : t.edges) {
				cout << " [ ";
				for (double const &n : c) {
					cout << n << " ";
				}
				cout << "]";
			}
			cout << endl;
		}
	}
	cout << "----------------------" << endl;
}