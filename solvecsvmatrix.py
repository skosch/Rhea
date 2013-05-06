import numpy as np
import numpy.linalg as npl
import matplotlib as mpl
import pylab

A = np.loadtxt(open("bin/out_itrr.csv","rb"),delimiter=",")
print A.shape
A = np.vstack((A, np.ones(A.shape[1])))

B = np.zeros((A.shape[0],1))

B[B.shape[0]-1] = 10000

X = npl.lstsq(A, B)


pylab.plot(X[0][5:])
pylab.show()

# xxxxx = 0
# xxxxx = 0
# xxxxx = 0
# xxxxx = 0
# xxxxx = 0
# xxxxx = 0
# xxxxx = 0
# xxxxx = 0
# xxxxx = 0
# xxxxx = 0 
