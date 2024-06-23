#include "Net.h"
#include <cmath>

void Net::readFile(int const &testCase) {
	ifstream file("test" + to_string(testCase) + ".json");
    if (!file.is_open())
    {
        cerr << "Unable to open file." << endl;
        return;
    }

    string line;
    while (getline(file, line))  //Get the first line of net ( "{" ).
    {
        if (line.find('{') != string::npos)
        {
            Net net;
            string txBlockOrRegion, txID;
            
            getline(file, line);  // Get net_ID
            sscanf_s(line.c_str(), " \"ID\":%d,", &net.ID);

            getline(file, line);  // Get TX_ID 
            net.TX.TX_NAME = line.substr(10, line.length() - 12);
            size_t pos = line.find('_');
            txBlockOrRegion = line.substr(8, line.length() - 2);
            txID = line.substr(pos + 1, line.length() - pos - 3);
            net.TX.TX_COORD.x = (txBlockOrRegion == "BLOCK") ? 0 : 1;
            net.TX.TX_COORD.y = stoi(txID);
                          
            getline(file, line);  //Get RXs.RX_ID
            string rxBlockOrRegion, rxID;
            pos = line.find('[');
            size_t endPos = line.find(']');
            string rxLine = line.substr(pos + 1, endPos - pos - 1);
            istringstream iss(rxLine);
            string rx;
            while (getline(iss, rx, ','))
            {
                rx = rx.substr(1, rx.length() - 2);
                size_t pos = rx.find('_');
                rxBlockOrRegion = rx.substr(0, pos);
                rxID = rx.substr(pos + 1);
                RX newRX;  // Declare a new RX and push back to the vector<RX> RXs
                newRX.RX_COORD.x = (rxBlockOrRegion == "BLOCK") ? 0 : 1;
                newRX.RX_COORD.y = stoi(rxID);
                newRX.RX_NAME.append(rxBlockOrRegion);
                newRX.RX_NAME.append("_");
                newRX.RX_NAME.append(rxID);
                net.RXs.push_back(newRX);
            }

            getline(file, line);  // Get num
            sscanf_s(line.c_str(), " \"NUM\":%d,", &net.num);
            
            getline(file, line); // Get MUST_THROUGH
            pos = line.find('[');
            endPos = line.find_last_of(']');string mustThroughLine = line.substr(pos + 1, endPos - pos - 1);
            string content = mustThroughLine;size_t start = 0;while (start < content.size()) 
            {
                size_t entryStart = content.find("[\"", start);
                size_t entryEnd = content.find("]],", entryStart);
                if (entryEnd == string::npos) 
                {
                    entryEnd = content.find("]]", entryStart);
                }
                
                if (entryStart == string::npos || entryEnd == string::npos) 
                {
                    break; 
                }
                
                string entry = content.substr(entryStart, entryEnd - entryStart + 2);
                start = entryEnd + 3;

                size_t idStart = entry.find("\"") + 1;
                size_t idEnd = entry.find("\"", idStart);
                string block_ID = entry.substr(idStart, idEnd - idStart);
                int type = (block_ID.find("REGION") != string::npos) ? 1 : 0;
                int id = stoi(block_ID.substr(block_ID.find('_') + 1));

                // Find the edgeIn and edgeOut arrays
                size_t edgeInStart = entry.find("[", idEnd) + 1;
                size_t edgeInEnd = entry.find("]", edgeInStart);
                string edgeInStr = entry.substr(edgeInStart, edgeInEnd - edgeInStart);

                size_t edgeOutStart = entry.find("[", edgeInEnd) + 1;
                size_t edgeOutEnd = entry.find("]", edgeOutStart);
                string edgeOutStr = entry.substr(edgeOutStart, edgeOutEnd - edgeOutStart);

                // Parse the coordinates from edgeIn and edgeOut
                double edgeIn[4] = { 0 };
                double edgeOut[4] = { 0 };
                sscanf_s(edgeInStr.c_str(), "%lf,%lf,%lf,%lf", &edgeIn[0], &edgeIn[1], &edgeIn[2], &edgeIn[3]);
                sscanf_s(edgeOutStr.c_str(), "%lf,%lf,%lf,%lf", &edgeOut[0], &edgeOut[1], &edgeOut[2], &edgeOut[3]);

                // Create a MUST_THROUGH struct and populate it
                MUST_THROUGH mustThrough;
                mustThrough.blockName = block_ID;
                for (int i = 0; i < 4; ++i) {
                    mustThrough.edgeIn[i] = edgeIn[i];
                    mustThrough.edgeOut[i] = edgeOut[i];
                }

                net.MUST_THROUGHs.push_back(mustThrough);
            }
            
            getline(file, line); // HMFT_MUST_THROUGH
            pos = line.find('[');
            endPos = line.find_last_of(']'); 
            mustThroughLine = line.substr(pos + 1, endPos - pos - 1);
            content = mustThroughLine; 
            start = 0; 
            while (start < content.size())
            {
                size_t entryStart = content.find("[\"", start);
                size_t entryEnd = content.find("]],", entryStart);
                if (entryEnd == string::npos)
                {
                    entryEnd = content.find("]]", entryStart);
                }

                if (entryStart == string::npos || entryEnd == string::npos)
                {
                    break;
                }

                string entry = content.substr(entryStart, entryEnd - entryStart + 2);
                start = entryEnd + 3;

                size_t idStart = entry.find("\"") + 1;
                size_t idEnd = entry.find("\"", idStart);
                string block_ID = entry.substr(idStart, idEnd - idStart);
                int type = (block_ID.find("REGION") != string::npos) ? 1 : 0;
                int id = stoi(block_ID.substr(block_ID.find('_') + 1));

                // Find the edgeIn and edgeOut arrays
                size_t edgeInStart = entry.find("[", idEnd) + 1;
                size_t edgeInEnd = entry.find("]", edgeInStart);
                string edgeInStr = entry.substr(edgeInStart, edgeInEnd - edgeInStart);

                size_t edgeOutStart = entry.find("[", edgeInEnd) + 1;
                size_t edgeOutEnd = entry.find("]", edgeOutStart);
                string edgeOutStr = entry.substr(edgeOutStart, edgeOutEnd - edgeOutStart);

                // Parse the coordinates from edgeIn and edgeOut
                double edgeIn[4] = { 0 };
                double edgeOut[4] = { 0 };
                sscanf_s(edgeInStr.c_str(), "%lf,%lf,%lf,%lf", &edgeIn[0], &edgeIn[1], &edgeIn[2], &edgeIn[3]);
                sscanf_s(edgeOutStr.c_str(), "%lf,%lf,%lf,%lf", &edgeOut[0], &edgeOut[1], &edgeOut[2], &edgeOut[3]);

                HMFT_MUST_THROUGH mustThrough;
                mustThrough.blockName = block_ID;
                for (int i = 0; i < 4; ++i) {
                    mustThrough.edgeIn[i] = edgeIn[i];
                    mustThrough.edgeOut[i] = edgeOut[i];
                }
                net.HMFT_MUST_THROUGHs.push_back(mustThrough);
            }
            
            getline(file, line);  // Get TX_COORD 
            sscanf_s(line.c_str(), " \"TX_COORD\":[%lf,%lf],", &net.TX.TX_COORD.x, &net.TX.TX_COORD.y);
            
            getline(file, line);  // Get RXs.RX_COORD
            pos = line.find('['); 
            endPos = line.find_last_of(']');
            string rxCoordLine = line.substr(pos + 1, endPos - pos - 1);
            istringstream issCoord(rxCoordLine);
            string coordPair;
            int rxIndex = 0;
            while (getline(issCoord, coordPair, ']'))
            {
                size_t coordPos = coordPair.find('[');
                if (coordPos != string::npos)
                {
                    coordPair = coordPair.substr(coordPos + 1);
                    double x, y;
                    sscanf_s(coordPair.c_str(), "%lf,%lf", &x, &y);
                    if (rxIndex < net.RXs.size())
                    {
                        net.RXs[rxIndex].RX_COORD = { x, y }; // Access directly
                    }
                    else
                    {
                        cerr << "Error: RX index out of range. rxIndex: " << rxIndex << ", RXs size: " << net.RXs.size() << endl;
                    }
                    rxIndex++;
                }
            }
            allNets.push_back(net);
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
    double x, y;
    if(TX.TX_NAME[0] == 'B') {
        x = allZone.getBlock(TX.TX_NAME).coordinate.x;
        y = allZone.getBlock(TX.TX_NAME).coordinate.y;
    }
    else if(TX.TX_NAME[0] == 'R') {
        x = allZone.getRegion(TX.TX_NAME).vertices[0].x;
        y = allZone.getRegion(TX.TX_NAME).vertices[0].y;
    }
    x += TX.TX_COORD.x / 2000;
    y += TX.TX_COORD.y / 2000;
    x = std::ceil(x * 1000.0) / 1000.0;
    y = std::ceil(y * 1000.0) / 1000.0;
    struct TX newTX;
    newTX.TX_COORD = Point(x, y);
    newTX.TX_NAME = TX.TX_NAME;
	return newTX;
}

struct RX Net::absoluteRX(RX const &rx, AllZone const &allZone) const {
    double x, y;
    if(rx.RX_NAME[0] == 'B') {
        x = allZone.getBlock(rx.RX_NAME).coordinate.x;
        y = allZone.getBlock(rx.RX_NAME).coordinate.y;
    }
    else if(rx.RX_NAME[0] == 'R') {
        x = allZone.getRegion(rx.RX_NAME).vertices[0].x;
        y = allZone.getRegion(rx.RX_NAME).vertices[0].y;
    }
    x += rx.RX_COORD.x / 2000;
    y += rx.RX_COORD.y / 2000;
    x = std::ceil(x * 1000.0) / 1000.0;
    y = std::ceil(y * 1000.0) / 1000.0;
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