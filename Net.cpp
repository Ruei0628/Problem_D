#include "Net.h"

Net::Net(Terminal tx, Terminal rx) : TX(tx) {
	RXs.push_back(rx);
}

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
			Terminal RX(tempRXNameArray[i], tempRXCoordArray[i]);
			RX.absoluteCoord(chip);
	    	tempNet.RXs.push_back(RX);
	    }

		// NUM:
		tempNet.num = net["NUM"].GetInt();

		// HMFT_MUST_THROUGH:
        for (const auto &hmftmt : net["HMFT_MUST_THROUGH"].GetObject()) {
            MUST_THROUGH tempHmftmt;
            tempHmftmt.blockName = hmftmt.name.GetString();
			Point bc = chip.getBlock(tempHmftmt.blockName).coordinate;
			for (auto const &coord : hmftmt.value.GetArray()) {
				Point first(coord[0].GetDouble() + bc.x, coord[1].GetDouble() + bc.y);
				Point second(coord[2].GetDouble() + bc.x, coord[3].GetDouble() + bc.y);
				tempHmftmt.edges.push_back(Edge(first, second));
			}
            tempNet.HMFT_MUST_THROUGHs.push_back(tempHmftmt);
        }

        // MUST_THROUGH:
        for (const auto &mt : net["MUST_THROUGH"].GetObject()) {
            MUST_THROUGH tempMt;
            tempMt.blockName = mt.name.GetString();
			Point bc = chip.getBlock(tempMt.blockName).coordinate;
			for (auto const &coord : mt.value.GetArray()) {
				Point first(coord[0].GetDouble() + bc.x, coord[1].GetDouble() + bc.y);
				Point second(coord[2].GetDouble() + bc.x, coord[3].GetDouble() + bc.y);
				tempMt.edges.push_back(Edge(first, second));
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
			Edge c = t.edges[0];
			cout << " (" << c.first.x << ", " << c.first.y << ") (" << c.second.x << ", " << c.second.y << ")\n";
			if (t.edges.size() == 1) continue;
			else {
				for (int i = 0; i < t.blockName.size() + 3; i++) { cout << " "; }
				Edge c = t.edges[1];
				cout << " (" << c.first.x << ", " << c.first.y << ") (" << c.second.x << ", " << c.second.y << ")\n";
			}
		}
	}
	cout << "HMFT_MUST_THROUGH: " << endl;
	if (HMFT_MUST_THROUGHs.size()) {
		for (const MUST_THROUGH &t : HMFT_MUST_THROUGHs){
			cout << " - " << t.blockName;
			Edge c = t.edges[0];
			cout << " (" << c.first.x << ", " << c.first.y << ") (" << c.second.x << ", " << c.second.y << ")\n";
			if (t.edges.size() == 1) { continue; }
			else {
				for (int i = 0; i < t.blockName.size() + 3; i++) { cout << " "; }
				Edge c = t.edges[1];
				cout << " (" << c.first.x << ", " << c.first.y << ") (" << c.second.x << ", " << c.second.y << ")\n";
			}
		}
	}
	cout << "----------------------" << endl;
}