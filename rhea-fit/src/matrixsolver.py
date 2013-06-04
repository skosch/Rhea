import numpy as np
import csv

reader = csv.reader(open("training", "rb"))

matrix = np.zeros((26**3, 2000))

for l in reader:
  fullline = np.asarray(l)

matrix = np.reshape(fullline, 26**3)
print matrix[50][50]
  
