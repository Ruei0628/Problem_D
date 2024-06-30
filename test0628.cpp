#include "Net.h"

Net Net::getNet(int const &id) const {
	for (const Net &n : allNets) {
		if (n.ID == id) return n;
	}
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