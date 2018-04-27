import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import argparse
import math

filename = "data/test1.txt"
dataset = pd.read_csv(filename, delimiter=' ', header=0)
data = dataset.values


print("file loaded")

t = data[:,0]
l = data[:,1]
v = data[:,2]
#m = data[:,3]


plt.figure(figsize=(12,6))
plt.plot(t, v)
#plt.plot(t, m)
plt.tight_layout()
plt.show()

for i in range(len(v)):
	if (v[i] == v[i-1]):
		print(i)


plt.figure(figsize=(12,6))
plt.plot(t)
#plt.plot(t, m)
plt.tight_layout()
plt.show()


print(len(t))

periods = []
pt = []

for i in range(2, len(v)):
	if v[i] == 0:
		periods.append(t[i] - t[i-2])
		pt.append(t[i])

for i in range(len(periods)):
	if periods[i] > 0.2:
		print("%d %f"%(pt[i], periods[i]))



plt.figure(figsize=(12,6))
plt.plot(t, v)
#plt.plot(t, m)
plt.tight_layout()
plt.show()






print(np.max(periods))

'''
maxp = 0

for i in range(1, len(periods)):
	aux = periods[i] - periods[i-1]
	if aux > maxp and maxp < 200/1000:
		maxp = aux

print(maxp*1000)
'''




lats = [int(math.modf(x/1000)[1]) for x in l]
	
plt.figure(figsize=(12,6))
plt.plot(t, lats, label="Latencies", linewidth=0.8)
plt.legend()
plt.xlabel("Time (s)")
plt.ylabel("Latency (us)")
plt.title("Latency values")
plt.tight_layout()
plt.show()

