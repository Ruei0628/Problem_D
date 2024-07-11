#include "Net.h"
#include "Band.h"

constexpr double DX = 0.01;
constexpr double DY = 0.01;

bool checkIfIntersect(Band *sourceBandsBackTrace, Band *targetBandsBackTrace, 
					  vector<Band*> &compairer, vector<Band*> &compairee) {
	for (Band *t : compairer) {
		for (Band *s : compairee) {
			if (s->intersected(t)) {
				// which means path is found
				sourceBandsBackTrace = s;
				targetBandsBackTrace = t;
				return 1;
			}
		}
	}
	return 0;
}

class RangeCoverage {
private:
    Pair target;
    vector<Pair> uncovered;

public:
    vector<CoveredRange> covered;
    vector<double> splitPoints;
	
    RangeCoverage(Pair target) : target(target) {
        uncovered.push_back(target);
        splitPoints = { target.min, target.max };
    }

    void addSource(Pair source, double fixed) {
        if (source.max <= target.min || source.min >= target.max) {
            return;  // This source is completely outside the target range
        }

        // Clip the source to the target range
        source.min = max(source.min, target.min);
        source.max = min(source.max, target.max);

        vector<Pair> newUncovered;
        vector<CoveredRange> newCovered;

        for (const auto& uncoveredRange : uncovered) {
            if (source.min <= uncoveredRange.min && source.max >= uncoveredRange.max) {
                // This uncovered range is completely covered
                newCovered.push_back(CoveredRange(uncoveredRange, fixed));
            } else if (source.max <= uncoveredRange.min || source.min >= uncoveredRange.max) {
                // This uncovered range is not affected
                newUncovered.push_back(uncoveredRange);
            } else {
                // Partial overlap
                if (source.min > uncoveredRange.min) {
                    newUncovered.push_back(Pair(uncoveredRange.min, source.min));
                }
                if (source.max < uncoveredRange.max) {
                    newUncovered.push_back(Pair(source.max, uncoveredRange.max));
                }
                newCovered.push_back(CoveredRange(Pair(max(source.min, uncoveredRange.min), min(source.max, uncoveredRange.max)), fixed));
            }
        }

        uncovered = newUncovered;
        for (const auto& range : newCovered) {
            covered.push_back(range);
            splitPoints.push_back(range.range.min);
            splitPoints.push_back(range.range.max);
        }
    }

    bool isFullyCovered() const {
        return uncovered.empty();
    }

    void sortSplitPoints() {
        sort(splitPoints.begin(), splitPoints.end());
        splitPoints.erase(unique(splitPoints.begin(), splitPoints.end()), splitPoints.end());

        for (size_t i = 0; i < splitPoints.size(); ++i) {
            cout << splitPoints[i];
            if (i < splitPoints.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
};

void mikami(Net &net, Chip &chip, vector<Band*> recordPath){
	cout << "Start mikami!" << endl;

	//step 1: initializaiotn
	Terminal source = net.TX;
	Terminal target = net.RXs[0]; // 待處理

	vector<Band*> CSB; // stands for current source bands
	vector<Band*> OSB; // stands for old source bands
	vector<Band*> CTB; // stands for current target bands
	vector<Band*> OTB; // stands for old target bands

	vector<Edge*> &edges = chip.totEdge; // make it referenced

	// 把 TX 跟 RX 改成 Bands
	CSB.push_back(new Band(source, 1, edges));
	CSB.push_back(new Band(source, 0, edges));
	CTB.push_back(new Band(target, 1, edges));
	CTB.push_back(new Band(target, 0, edges));

	Band* sourceBandsBackTrace = nullptr;
	Band* targetBandsBackTrace = nullptr;

	cout << "step 1 complete\n";

	while (1) {
		// step 2: check if intersect
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, CTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, OTB)) break;
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CTB, OSB)) break;

		cout << "step 2 complete\n";
		
		// step 3: copy CSB to OSB; copy CTB to OTB
		// current 的點要存回去 old
		OSB.insert(OSB.end(), CSB.begin(), CSB.end());
		OTB.insert(OTB.end(), CTB.begin(), CTB.end());

		// current 的資料應該不能刪掉，因為還要 extend
		// 只是在這之後(、被清除之前)調用 current bands 應該只能 const *
		cout << "step 3 complete\n";

		// step 4. 生成與 current bands 垂直的 extendedProbes，並先把他們暫存在一個 vector 裡面
		// 要分成 from source 跟 from target
		vector<Band*> ESB; // stands for extended source bands
		vector<Band*> ETB; // stands for extended target bands
		// 這些生成出來的 bands 的 level 要 +1

		for (Band *b : CSB) { // 來自 source
			double dx = b->toExtend_isX * DX;
			double dy = !(b->toExtend_isX) * DY;
			double X = dx, Y = dy;
			int levelCSP = b->level;
			//cout << "source directionX: " << p.directionX << endl;
			// 往兩個方向生成新的 Band
			// 正方向
			while (1) {
				Band *positiveProbe = b->extendedBand(X, Y, levelCSP + 1);
				//cout << "positiveProbe: " << positiveProbe.coord.x << ", " << positiveProbe.coord.y << endl;
				// 如果這個 Band 會撞到牆，直接結束這個方向的 extend
				if (positiveProbe->detectEdge(edges)) {
				 	// cout << "EPSP hit edge!" << endl;
				 	break;
				}
				X += dx;
				Y += dy;
				// 如果這個 Band 已經存在 OSB 裡面，跳過這個 Band (但還是會繼續執行 extend)
				if (positiveProbe->alreadyExist(OSB)) continue; // may be time-consuming
				if (positiveProbe->alreadyExist(CSB)) continue; // may be time-consuming
				if (positiveProbe->alreadyExist(ESB)) continue; // may be time-consuming
				ESB.push_back(positiveProbe);
			}
			// cout << "EPSP done!" << endl;
			// 負方向
			X = dx;
			Y = dy;
			while (1) {
				Band *negativeProbe = b->extendedBand(-X, -Y, levelCSP + 1);
				//cout << "negativeProbe: " << negativeProbe.coord.x << ", " << negativeProbe.coord.y << endl;
				// 如果這個 Band 會撞到牆，直接結束這個方向的 extend
				if (negativeProbe->detectEdge(edges)) {
				 	// cout << "ENSP hit edge!" << endl;
				 	break;
				}
				X += dx;
				Y += dy;
				// 如果這個 Band 已經存在 OSB 裡面，跳過這個 Band (但還是會繼續執行 extend)
				if (negativeProbe->alreadyExist(OSB)) continue; // may be time-consuming
				if (negativeProbe->alreadyExist(CSB)) continue; // may be time-consuming
				if (negativeProbe->alreadyExist(ESB)) continue; // may be time-consuming
				ESB.push_back(negativeProbe);
			}
			// cout << "ENSP done!" << endl;
		}

		for (Band *b : CTB) { // 來自 target
			Pair goalPair = b->directionPair();
			RangeCoverage coverageRight(goalPair);
			RangeCoverage coverageLeft(goalPair);

			// 正向搜尋(右手邊)
			for (Edge *e : edges) {
				if (e->isVertical() == b->toExtend_isX() && e->fixed() >= b->extendedPair().max) { // verified V
					coverageRight.addSource(e->ranged(), e->fixed());
					if (coverageRight.isFullyCovered()) break;
				}
			}
			// 負向搜尋(左手邊)
			for (auto it = edges.rbegin(); it != edges.rend(); ++it) {
				Edge *e = *it;
				if (e->isVertical() == b->toExtend_isX() && e->fixed() <= b->extendedPair().max) { // verified V
					coverageLeft.addSource(e->ranged(), e->fixed());
					if (coverageLeft.isFullyCovered()) break;
				}
			}
			coverageRight.sortSplitPoints();
			coverageLeft.sortSplitPoints();
			//
		}

		// 因此我們現在獲得了全部的下一個 level 的 bands (在 extendedProbes 裡)
		CSB.swap(ESB);
		ESB.clear();
		CTB.swap(ETB);
		ETB.clear();

		cout << "step 4 complete\n";
	}
	
	// Step 5: Backtrace
	cout << "Path found!" << endl;
	vector<const Band*> path;

	// Backtrace from source Band
	const Band* band = sourceBandsBackTrace;
	while (band) {
		path.push_back(band);
		band = band->parent; // mistake
	}

	reverse(path.begin(), path.end()); // Reverse to get path from source to target

	// Backtrace from target Band
	band = targetBandsBackTrace;
	while (band) {
		path.push_back(band);
		band = band->parent; // mistake
	}

	// Clean up dynamically allocated memory
	delete sourceBandsBackTrace;
	delete targetBandsBackTrace;

	// Print the path
	cout << "len(Path): " << path.size() << endl;
    if (path.empty()) {
        cout << "Path is empty." << endl;
    } else {
        cout << "Path:" << endl;
        for (Band const *p : path) {
			cout << p->min.x << ", " << p->min.y << endl;
		} 
    }
}