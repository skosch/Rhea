import numpy as np
import scipy.linalg as scl
import csv
np.set_printoptions(threshold=np.nan)
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm

reader = csv.reader(open("outfield_A", "rb"))
lr = list(reader)
matrix = np.zeros((len(lr), len(lr[0])))

# matrix
for i, l in enumerate(lr):
    l = l[:-1]
    line = np.asarray(map(lambda x: int(x), l))
    matrix[i, 0:len(line)] = line

fig = plt.figure()

ax = fig.add_subplot(111)
ims = ax.imshow(matrix, origin="lower",
                interpolation="nearest", vmin=-20, vmax=+20)
ax.grid(False)
ax.set_title("b=0")
fig.colorbar(ims)
plt.show()

