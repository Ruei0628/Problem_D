#include "Net.h"
#include "Band.h"
#include <iomanip>
#include <memory>

constexpr double DX = 0.01;
constexpr double DY = 0.01;

bool debugInfo = 0;

bool checkIfIntersect(shared_ptr<Band> &source, shared_ptr<Band> &target, 
                      vector<shared_ptr<Band>> &vec_source, 
                      vector<shared_ptr<Band>> &vec_target) {
    for (const auto &s : vec_source) {
        for (const auto &t : vec_target) {
            if (s->intersected(t)) {
                // which means path is found
                source = s;
                target = t;
                return true;
            }
        }
    }
    return false;
}

void printBands(const vector<shared_ptr<Band>>& bands) {
    for (const auto& band : bands) { 
        cout << " > " << *band << endl; 
    }
}

void bandSearchAlgorithm(Net &net, Chip &chip, vector<shared_ptr<Band>> &recordPath) {
    if (debugInfo) cout << "Start band search!" << endl;

    //step 1: initializaion
    Terminal source = net.TX;
    Terminal target = net.RXs[0]; // waited to be fix

    vector<shared_ptr<Band>> CSB; // stands for current source bands
    vector<shared_ptr<Band>> OSB; // stands for old source bands
    vector<shared_ptr<Band>> CTB; // stands for current target bands
    vector<shared_ptr<Band>> OTB; // stands for old target bands

    vector<Edge> &edges = chip.allEdges; // make it referenced

    // change TX and RX into Bands
    CSB.push_back(make_shared<Band>(source, 1, edges));
    CSB.push_back(make_shared<Band>(source, 0, edges));
    CTB.push_back(make_shared<Band>(target, 1, edges));
    CTB.push_back(make_shared<Band>(target, 0, edges));

    shared_ptr<Band> sourceBandsBackTrace = nullptr;
    shared_ptr<Band> targetBandsBackTrace = nullptr;

    if (debugInfo) cout << "step 1 complete\n";

    int levelOfIteration = 0;
    while (true) {
        if (debugInfo) cout << "=== level Of Iteration: " << ++levelOfIteration << " ===\n";

        if (debugInfo) {
            cout << "> CSB:\n";
            printBands(CSB);
            cout << "> CTB:\n";
            printBands(CTB);
            cout << "> OSB:\n";
            printBands(OSB);
            cout << "> OTB:\n";
            printBands(OTB);
        }
		
        // step 2: check if intersect
        if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, CTB)) break;
        if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, OTB)) break;
        if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, OSB, CTB)) break;

        if (debugInfo) cout << "step 2 complete\n";

        // step 3: copy CSB to OSB; copy CTB to OTB
        // current bands stores to old
        OSB.insert(OSB.end(), CSB.begin(), CSB.end());
        OTB.insert(OTB.end(), CTB.begin(), CTB.end());

        if (debugInfo) cout << "step 3 complete\n";

        // step 4. generate extended bands that perpendicular to current bands
        vector<shared_ptr<Band>> ESB; // stands for extended source bands
        vector<shared_ptr<Band>> ETB; // stands for extended target bands

        if (debugInfo) cout << "< from source >\n";
        for (const auto &b : CSB) { // from source
            if (debugInfo) cout << " # band " << *b << endl;

            vector<Edge> coverageRight = b->generateCoveredRanges(edges, 1);
            vector<Edge> coverageLeft = b->generateCoveredRanges(edges, 0);

            vector<unique_ptr<Band>> tempESB = b->mergeCoveredRanges(coverageLeft, coverageRight);
            for (auto &esb : tempESB) {
                if (debugInfo) cout << "  + adding " << *esb;
                if (esb->alreadyExist(OSB) || esb->alreadyExist(CSB) || esb->alreadyExist(ESB)) {
                    if (debugInfo) cout << endl;
                    continue;
                }
                ESB.push_back(std::move(esb));
                if (debugInfo) cout << endl;
            }
            if (debugInfo) cout << endl;
        }

        if (debugInfo) cout << "< from target >\n";
        for (const auto &b : CTB) { // from target
            if (debugInfo) cout << " # band " << *b << endl;

            vector<Edge> coverageRight = b->generateCoveredRanges(edges, 1);
            vector<Edge> coverageLeft = b->generateCoveredRanges(edges, 0);

            vector<unique_ptr<Band>> tempETB = b->mergeCoveredRanges(coverageLeft, coverageRight);
            for (auto &etb : tempETB) {
                if (debugInfo) cout << "  + adding " << *etb;
                if (etb->alreadyExist(OTB) || etb->alreadyExist(CTB) || etb->alreadyExist(ETB)) {
                    if (debugInfo) cout << endl;
                    continue;
                }
                ETB.push_back(std::move(etb));
                if (debugInfo) cout << endl;
            }
            if (debugInfo) cout << endl;
        }

        // now we obtain all next level bands inside extendedBands
        // now clears CSB, CTB
        CSB.clear();
        CTB.clear();

        // moves ESB, ETB's data into CSB, CTB
        CSB = std::move(ESB);
        CTB = std::move(ETB);

        if (levelOfIteration > 10) {
            cout << "can't find :(\n";
            return;
        }
		if (debugInfo) cout << "step 4 complete\n\n";
    }

    // Step 5: Backtrace
    cout << "Path found! ";

    // Backtrace from source Band
    while (sourceBandsBackTrace) {
        recordPath.push_back(sourceBandsBackTrace);
        sourceBandsBackTrace = sourceBandsBackTrace->parent;
    }

    reverse(recordPath.begin(), recordPath.end()); // Reverse to get path from source to target

    // Backtrace from target Band
    while (targetBandsBackTrace) {
        recordPath.push_back(targetBandsBackTrace);
        targetBandsBackTrace = targetBandsBackTrace->parent;
    }

    // Print the path
    cout << "length of Path: " << recordPath.size() << endl;
    printBands(recordPath);

    // No need to manually delete smart pointers
    return;
}

int main() {
    Chip chip(4);
	
	cout << fixed << setprecision(3);

    Net net;
    net.ParserAllNets(4, chip);

    int index = -1;
    for (const auto &n : net.allNets) {
        index++;
        if (index < 810) continue;
		if (index > 850) break;
        Terminal source = n.TX;
        for (const auto &target : n.RXs) {
            Net sole(source, target);
            cout << "\nID: " << n.ID << " (" << sole.TX.coord.x << ", " << sole.TX.coord.y << ") (" 
                 << sole.RXs[0].coord.x << ", " << sole.RXs[0].coord.y << ")\n";

    		vector<shared_ptr<Band>> record;
            bandSearchAlgorithm(sole, chip, record);
        }
    }

    cout << "done" << endl;
    return 0; /*---------------------------------------------------------

	// customed test data
	Chip tesCh;
	for (int i = 0; i < 6; i++) { tesCh.allBlocks.push_back(make_unique<Block>("block" + to_string(i))); }
	tesCh.allBlocks.push_back(make_unique<Block>("testS"));
	tesCh.allBlocks.push_back(make_unique<Block>("testT"));

	// regular borders
	tesCh.allBlocks[0]->vertices = { Point(11, 0), Point(11, 9), Point(17, 9), Point(17, 7), Point(14, 7), Point(14, 0) };
	tesCh.allBlocks[1]->vertices = { Point(9, 10), Point(9, 12), Point(20, 12), Point(20, 10) };
	tesCh.allBlocks[2]->vertices = { Point(17, 2), Point(17, 4), Point(22, 4), Point(22, 12), Point(25, 12), Point(25, 10), Point(28, 10), Point(28, 8), Point(25, 8), Point(25, 3), Point(22, 3), Point(22, 2) };
	tesCh.allBlocks[3]->vertices = { Point(0, 4), Point(4, 4), Point (4, 11), Point(7, 11), Point(7, 13), Point(23, 13), Point(23, 15), Point(0, 15) };
	tesCh.allBlocks[4]->vertices = { Point(24, 13), Point(24, 15), Point(33, 15), Point(33, 3), Point(30, 3), Point(30, 4), Point(27, 4), Point(27, 6), Point(30, 6), Point(30, 13) };
	tesCh.allBlocks[5]->vertices = { Point(0, 0), Point(0, 15), Point(33, 15), Point(33, 0) };

	// the block contains terminal
	tesCh.allBlocks[6]->vertices = { Point(4.9, 3.9), Point(4.9, 6.1), Point(7.1, 6.1), Point(7.1, 3.9) }; // testS
	tesCh.allBlocks[7]->vertices = { Point(25.5, 2), Point(25.5, 3.3), Point(28.8, 3.3), Point(28.8, 2) }; // testT
	for (auto &b : tesCh.allBlocks) { b->verticesToEdges(); for (auto &e : b->edges) { tesCh.allEdges.push_back(e); } }

	// make it ordered
	std::sort(tesCh.allEdges.begin(), tesCh.allEdges.end(), [](const auto& a, const auto& b) { return a.fixed() < b.fixed(); });

	Terminal start("testS", Point(6, 5));
  	Terminal end("testT", Point(26, 3));
	Net n(start, end);

	bandSearchAlgorithm(n, tesCh, record);

	cout << "done" << endl;
    return 0; *---------------------------------------------------------*

    // this tests if edges are linked to block, and is well!
	for (auto e : chip.allEdges) {
		if (e.block) {
			cout << e.block->name << ": ";
			if (e.block->name.length() == 7) { cout << " "; }
		}
		else { cout << "_BORDER_: "; }
		cout << e.isVertical();
		cout << " (" << e.fixed() << ")\t(" << e.ranged().min << ", " << e.ranged().max << ")\n";
	}
    return 0; *---------------------------------------------------------*/
}

/*
cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Band.cpp Chip.cpp Block.cpp Net.cpp Region.cpp -o main} ; if ($?) { .\main }

cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ test0628.cpp Chip.cpp Block.cpp Net.cpp Probe.cpp Region.cpp  Edge.cpp -o test0628} ; if ($?) { .\main }

cd "c:\Users\照元喔\source\repos\Problem_D\" ; if ($?) { g++ main.cpp Net.cpp -o main} ; if ($?) { .\main }
*/