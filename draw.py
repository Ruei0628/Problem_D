import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection, LineCollection
import numpy as np

def read_polygons(filename):
    df = pd.read_csv(filename)
    polygons = []
    colors = []
    for group in df['group'].unique():
        polygon = df[df['group'] == group]
        poly = Polygon(polygon[['x', 'y']].values, closed=True)
        polygons.append(poly)
        is_feed = polygon['is_feed'].iloc[0]
        colors.append('cornflowerblue' if is_feed==1 else ('gray' if is_feed==0 else 'salmon'))
    return polygons, colors

def read_must_throughs(filename):
    df = pd.read_csv(filename)
    lines = df[['x1', 'y1', 'x2', 'y2']].values.reshape(-1, 2, 2)
    colors = ['gold' if h==1 else ('turquoise' if h==2 else ('mediumpurple' if h==3 else 'orange')) for h in df['mt']]
    return lines, colors

def read_net(filename):
    df = pd.read_csv(filename)
    tx = df[df['type'] == 'TX'][['x', 'y']].values
    rx = df[df['type'] == 'RX'][['x', 'y']].values
    return tx, rx

# 創建圖表
fig, ax = plt.subplots(figsize=(15, 10))

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
ax.scatter(tx[:, 0], tx[:, 1], color='red', s=5, label='TX')
ax.scatter(rx[:, 0], rx[:, 1], color='blue', s=3, label='RX')

# 設置軸限制
ax.autoscale()

# 設置標題和軸標籤
plt.title('Net TX RXs on Chip')
plt.xlabel('X coordinate')
plt.ylabel('Y coordinate')

# 添加圖例
# feed = plt.Rectangle((0, 0), 1, 1, fc="cornflowerblue", alpha=0.4)
# non_feed = plt.Rectangle((0, 0), 1, 1, fc="gray", alpha=0.4)
# region = plt.Rectangle((0, 0), 1, 1, fc="salmon", alpha=0.4)
# tbenn = plt.Line2D([0], [0], color="gold", linewidth=2)
# bpr = plt.Line2D([0], [0], color="orange", linewidth=2)
# tx = plt.Line2D([0], [0], color="red", linewidth=2)
# rx = plt.Line2D([0], [0], color="blue", linewidth=2)
# mt = plt.Line2D([0], [0], color="turquoise", linewidth=2)
# hmft = plt.Line2D([0], [0], color="mediumpurple", linewidth=2)
# plt.legend([feed, non_feed, region, tbenn, bpr, tx, rx, mt, hmft], 
#            ['Feed', 'Non-Feed', 'region', 'TBENN', 'BPR', 'TX', 'RX', 'MT', 'HMFT MT'], 
#            loc='upper left')

# 顯示圖表
plt.show()

# 如果您想保存圖表，可以取消註釋下面的行：
# plt.savefig('combined_plot.png', dpi=300, bbox_inches='tight')