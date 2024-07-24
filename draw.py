import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection, LineCollection
import numpy as np

FEED = '#454570'
NON_FEED = '#525553'
REGION = '#704545'
BPR = '#40C080'
TBENN = '#C08040'
MT = '#80FFFF'
HMFT = '#FF80FF'

def read_polygons(filename):
    df = pd.read_csv(filename)
    polygons = []
    colors = []
    for group in df['group'].unique():
        polygon = df[df['group'] == group]
        poly = Polygon(polygon[['x', 'y']].values, closed=True)
        polygons.append(poly)
        is_feed = polygon['is_feed'].iloc[0]
        colors.append(FEED if is_feed==1 else (NON_FEED if is_feed==0 else REGION))
    return polygons, colors

def read_must_throughs(filename):
    df = pd.read_csv(filename)
    lines = df[['x1', 'y1', 'x2', 'y2']].values.reshape(-1, 2, 2)
    colors = [TBENN if h==1 else (MT if h==2 else (HMFT if h==3 else BPR)) for h in df['mt']]
    return lines, colors

def read_net(filename):
    df = pd.read_csv(filename)
    tx = df[df['type'] == 'TX'][['x', 'y']].values
    rx = df[df['type'] == 'RX'][['x', 'y']].values
    return tx, rx

# 創建圖表
fig, ax = plt.subplots(figsize=(15, 10))
ax.set_facecolor('k')

# 讀取並繪製多邊形數據
polygons, poly_colors = read_polygons('zzb.csv')
p = PatchCollection(polygons, edgecolor='darkgrey', alpha=0.4)
p.set_facecolors(poly_colors)
ax.add_collection(p)

# 讀取並繪製must_throughs數據
must_through_lines, mt_colors = read_must_throughs('zzm.csv')
lc = LineCollection(must_through_lines, colors=mt_colors, linewidths=1)
ax.add_collection(lc)

# 讀取並繪製net數據
tx, rx = read_net('zzn.csv')
ax.scatter(rx[:, 0], rx[:, 1], c='b', s=5, marker='s')
ax.scatter(tx[:, 0], tx[:, 1], c='r', s=5)

# 設置軸限制
ax.autoscale()

# 設置標題和軸標籤
plt.title('Net TX RXs on Chip')
plt.xlabel('X coordinate')
plt.ylabel('Y coordinate')

# 添加圖例
# feed = plt.Rectangle((0, 0), 1, 1, fc=FEED)
# non_feed = plt.Rectangle((0, 0), 1, 1, fc=NON_FEED)
# region = plt.Rectangle((0, 0), 1, 1, fc=REGION)
# tbenn = plt.Line2D([0], [0], color=TBENN, linewidth=2)
# bpr = plt.Line2D([0], [0], color=BPR, linewidth=2)
# tx = plt.Line2D([0], [0], color='r', linewidth=2)
# rx = plt.Line2D([0], [0], color='b', linewidth=2)
# mt = plt.Line2D([0], [0], color=MT, linewidth=2)
# hmft = plt.Line2D([0], [0], color=HMFT, linewidth=2)
# plt.legend([feed, non_feed, region, tbenn, bpr, tx, rx, mt, hmft], 
        #    ['Feed', 'Non-Feed', 'region', 'TBENN', 'BPR', 'TX', 'RX', 'MT', 'HMFT MT'], 
        #    loc='upper left')

# 顯示圖表
plt.show()

# 如果您想保存圖表，可以取消註釋下面的行：
# plt.savefig('combined_plot.svg', dpi=300, bbox_inches='tight')