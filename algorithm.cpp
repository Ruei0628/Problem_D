#include "Net.h"
#include "Band.h"

constexpr double DX = 0.5;
constexpr double DY = 0.5;

bool checkIfIntersect(Band *sourceBandsBackTrace, Band *targetBandsBackTrace, 
					  vector<Band*> &CSB, vector<Band*> &OSB, vector<Band*> &CTB, vector<Band*> &OTB) {
	for (Band *s : CSB) { 
		for (Band *t : CTB) { 
			if (s->intersected(t)) {
				// which means path is found
				sourceBandsBackTrace = s;
				targetBandsBackTrace = t;
				return 1;
			}	
		}
		for (Band *t : OTB) {
			if (s->intersected(t)) {
			 	// which means path is found
				sourceBandsBackTrace = s;
				targetBandsBackTrace = t;
			 	return 1;
			}
		}
	}
	for (Band *t : CTB) {
		for (Band *s : OSB) {
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

void mikami(TX const &source, RX const &target, Chip const &chip, vector<Band*> recordPath){
	cout << "Start mikami!" << endl;

	//step 1: initializaiotn
	vector<Band*> CSB; // stands for current source bands
	vector<Band*> OSB; // stands for old source bands
	vector<Band*> CTB; // stands for current target bands
	vector<Band*> OTB; // stands for old target bands

	// 把 TX 跟 RX 改成 Bands
	CSB.push_back(new Band(source.TX_NAME, 1, source.TX_COORD));
	CSB.push_back(new Band(source.TX_NAME, 0, source.TX_COORD));
	CTB.push_back(new Band(target.RX_NAME, 1, target.RX_COORD));
	CTB.push_back(new Band(target.RX_NAME, 0, target.RX_COORD));

	vector<Wall> const walls = chip.Walls.allWalls;

	Band* sourceBandsBackTrace = nullptr;
	Band* targetBandsBackTrace = nullptr;
	cout << "step 1 complete\n";

	while (1) {
		// step 2: check if intersect
		if (checkIfIntersect(sourceBandsBackTrace, targetBandsBackTrace, CSB, OSB, CTB, OTB)) break;

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

		for (Band *p : CSB) { // 來自 source
			double dx = p->toExtend_isX * DX;
			double dy = !(p->toExtend_isX) * DY;
			double X = dx, Y = dy;
			int levelCSP = p->level;
			//cout << "source directionX: " << p.directionX << endl;
			// 往兩個方向生成新的 Band
			// 正方向
			while (1) {
				Band *positiveProbe = p->extendedBand(X, Y, levelCSP + 1);
				//cout << "positiveProbe: " << positiveProbe.coord.x << ", " << positiveProbe.coord.y << endl;
				// 如果這個 Band 會撞到牆，直接結束這個方向的 extend
				if (positiveProbe->detectWall(walls)) {
				 	// cout << "EPSP hit wall!" << endl;
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
				Band *negativeProbe = p->extendedBand(-X, -Y, levelCSP + 1);
				//cout << "negativeProbe: " << negativeProbe.coord.x << ", " << negativeProbe.coord.y << endl;
				// 如果這個 Band 會撞到牆，直接結束這個方向的 extend
				if (negativeProbe->detectWall(walls)) {
				 	// cout << "ENSP hit wall!" << endl;
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

		for (Band const *p : CTB) { // 來自 target
		 	// 第一步先檢查他的direction是x還是y
			// 然後才能知道他的min跟max end是要看x還是y
			// 接下來，從min_end在direction的方向，每隔一段距離toExtend找牆壁
			// 一旦發現牆壁的距離有所改變，則完成這個band
			// 把相關的資訊紀錄好，存在ETB理面。
			// 直到max_end
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
			cout << p->min_X << ", " << p->min_Y << endl;
		} 
    }
}