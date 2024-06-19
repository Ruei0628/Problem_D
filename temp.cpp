while(1){
	// 如果 probe 已經存在的話不要加
	// 所以要檢查這個 probe 是來自於 source 還是 target
	// 來自於 source 就檢查 sourceProbes，反之亦然
	// 不用檢查對面的是因為如果在的話就代表找到 path 了
	if (currentProbe.isFromSource) {
		for (Probe const &p : sourceProbes) {
			if (!(p.coord == currentProbe.coord)) { // p.directionX == currentProbe.directionX
				// 正方向的增加 probe
				Probe addPositiveProbe = currentProbe.extendProbe(X, Y);
				if (addPositiveProbe.hitWall(allZone.Walls.allWalls)) {
					break;
				} else {
					probeNew.push_back(addPositiveProbe);
				}
				// 負方向的增加 probe
				Probe addNegativeProbe = currentProbe.extendProbe(-X, -Y);
				if (addNegativeProbe.hitWall(allZone.Walls.allWalls)) {
					break;
				} else {
					probeNew.push_back(addNegativeProbe);
				}

				// maybe also level++ ?
			}
		}
	} else {
		for (Probe const &p : targetProbes) {
			if (!(p.coord == currentProbe.coord)) {
				// 正方向的增加 probe
				Probe addPositiveProbe = currentProbe.extendProbe(X, Y);
				if (addPositiveProbe.hitWall(allZone.Walls.allWalls)) {
					break;
				} else {
					probeNew.push_back(addPositiveProbe);
				}
				// 負方向的增加 probe
				Probe addNegativeProbe = currentProbe.extendProbe(-X, -Y);
				if (addNegativeProbe.hitWall(allZone.Walls.allWalls)) {
					break;
				} else {
					probeNew.push_back(addNegativeProbe);
				}
				// maybe also level++ ?
			}
		}
	}
	X += dx;
	Y += dy;
}