#include "AllZone.h"
#include "Net.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;

int main()
{
	AllZone allZone(4);
	allZone.getBlock("BLOCK_1").showBlockInfo();
	allZone.getRegion("REGION_64").showRegionInfo();
/*
    vector<Net> Nets;
    readFile("case4.json", Nets);

    for (const auto &net : Nets) {
    	cout << "ID: " << net.ID << endl;
    	cout << "TX: " << (net.TX.TX_info.first == 0 ? "BLOCK" : "REGION")
		<< "_" << net.TX.TX_info.second << endl;
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
*/
}

// cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp AllZone.cpp Block.cpp Net.cpp Region.cpp -o main} ; if ($?) { .\main }