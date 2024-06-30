#include "Net.h"

vector<array<double, 4>> a;

void showNetInfo(Net const &n) {
	cout << "ID: " << n.ID << endl
	<< "TX: " << n.TX.TX_NAME << " (" << n.TX.TX_COORD.x << ", " << n.TX.TX_COORD.y << ")" << endl
	<< "RXs: " << endl;
	for(const RX &zone: n.RXs){
		cout << zone.RX_NAME << " (" << zone.RX_COORD.x << ", " << zone.RX_COORD.y << ")" << endl;
	}
	cout << "NUM: " << n.num << endl
	<< "MUST_THROUGH: " << endl;
	if (n.MUST_THROUGHs.size()) {
		for (const MUST_THROUGH &t : n.MUST_THROUGHs) {
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
	if (n.HMFT_MUST_THROUGHs.size()) {
		for (const HMFT_MUST_THROUGH &t : n.HMFT_MUST_THROUGHs){
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

int main() {
	int testCase = 3;
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

		showNetInfo(tempNet);
	}
	file.close();
	system("PAUSE");
}