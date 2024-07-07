#include "Net.h"
#include <cmath>

void Net::ParserAllNets (int const &testCase, Chip const &chip) {
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
		tempNet.TX.name = net["TX"].GetString();
		// TX_COORD:
		tempNet.TX.coord = Point(net["TX_COORD"][0].GetDouble(), net["TX_COORD"][1].GetDouble());
		tempNet.TX.absoluteCoord(chip);

		// RX_NAME:
	    vector<string> tempRXNameArray;
		for (const auto &rxName : net["RX"].GetArray()) {
			tempRXNameArray.push_back(rxName.GetString());
		}
		// RX_COORD:
		vector<Point> tempRXCoordArray;
		for (const auto &coord : net["RX_COORD"].GetArray()) {
			tempRXCoordArray.push_back(Point(coord[0].GetDouble(), coord[1].GetDouble()));
		}
		// Write into RXs
	    for(int i = 0; i < tempRXNameArray.size(); i++){
			Terminal write(tempRXNameArray[i], tempRXCoordArray[i]);
			write.absoluteCoord(chip);
	    	tempNet.RXs.push_back(write);
	    }

		// NUM:
		tempNet.num = net["NUM"].GetInt();

		// HMFT_MUST_THROUGH:
        for (const auto &hmftmt : net["HMFT_MUST_THROUGH"].GetObject()) {
            MUST_THROUGH tempHmftmt;
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

        // MUST_THROUGH:
        for (const auto &mt : net["MUST_THROUGH"].GetObject()) {
            MUST_THROUGH tempMt;
            tempMt.blockName = mt.name.GetString();
			for (auto const &coord : mt.value.GetArray()) {
				array<double, 4> tempEdges;
				for (int i = 0; i < 4; i++) {
					tempEdges[i] = coord[i].GetDouble();
				}
				tempMt.edges.push_back(tempEdges);
			}
            tempNet.MUST_THROUGHs.push_back(tempMt);
        }

		// write into net
		allNets.push_back(tempNet);
	}
	file.close();
	return;
}

Net Net::getNet(int const &id) const {
	for (const Net &n : allNets) {
		if (n.ID == id) return n;
	}
}

void Terminal::absoluteCoord (Chip const &chip) {
    double x = 0, y = 0;
    if (name[0] == 'B') {
        x = chip.getBlock(name).coordinate.x;
        y = chip.getBlock(name).coordinate.y;
    }
    coord.x += x;
    coord.y += y;
	return;
}

double findBoundBox(vector<Point> const &coords){
	double x_min = coords[0].x, x_max = coords[0].x, y_min = coords[0].y, y_max = coords[0].y;
	for (auto const &c : coords) {
		double x_this = c.x, y_this = c.y;
		if (x_this > x_max) x_max = x_this;
		if (x_this < x_min) x_min = x_this;
		if (y_this > y_max) y_max = y_this;
		if (y_this < y_min) y_min = y_this;
	}
	return (x_max - x_min) * (y_max - y_min);
}

void Net::getBoundBoxArea(){
	vector<Point> coords;
	for(Terminal const &rx : RXs){
		coords.push_back(rx.coord);
	}
	coords.push_back(TX.coord);
	boundBoxArea = findBoundBox(coords);
}

void Net::showNetInfo() const {
	cout << "ID: " << ID << endl
	<< "TX: " << TX.name << " (" << TX.coord.x << ", " << TX.coord.y << ")" << endl
	<< "RXs: " << endl;
	for(const Terminal &rx: RXs){
		cout << " - " << rx.name << " (" << rx.coord.x << ", " << rx.coord.y << ")" << endl;
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
		for (const MUST_THROUGH &t : HMFT_MUST_THROUGHs){
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