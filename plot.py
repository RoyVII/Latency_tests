import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import argparse
import math

filename_scope = "data/scope_10.csv"
dataset = pd.read_csv(filename_scope, delimiter=',', header=2)
data_scope = dataset.values


filename_lat = "data/preempt_lat10.txt"
dataset = pd.read_csv(filename_lat, delimiter=' ', header=0)
data_lat = dataset.values


print("files loaded")

t_scope = data_scope[:,0]
#l = data[:,1]
v_scope = data_scope[:,1]


count_scope = 0
maxc_scope = 0
ref_scope = 0
refmax_scope = 0
tref_scope = 0



for i in range (len(v_scope)):
	if v_scope[i] == np.max(v_scope):
		count_scope = count_scope + 1
		ref_scope = i
	else:
		if maxc_scope <= count_scope:
			maxc_scope = count_scope
			refmax_scope = ref_scope - (count_scope - 1)
			tref_scope = t_scope[refmax_scope]
		count_scope = 0


t_diff = (tref_scope - t_scope[0]) * 1000


'''
plt.figure(figsize=(12,6))
plt.plot(t_scope, v_scope)
plt.plot(refmax_scope, np.max(v_scope), 'o')
#plt.plot(t, m)
plt.tight_layout()
plt.show()
'''


t_lat = data_lat[:,0]
l_lat = data_lat[:,1]
v_lat = data_lat[:,2]


maxc_lat = maxc_scope / 5
count_lat = 0
ref_lat = 0
refmax_lat = 0
tref_lat = 0


for i in range(len(v_lat)):
	if v_lat[i] == 1:
		count_lat = count_lat + 1
		ref_lat = i
	else:
		if maxc_lat == count_lat:
			refmax_lat = ref_lat - (count_lat - 1)
			tref_lat = t_lat[refmax_lat]
			break
		count_lat = 0




sub = t_diff
add = 10-t_diff



for i in range(len(t_lat)):
	if t_lat[i] >= tref_lat-sub:
		i_sub = i
		break

for i in range(len(t_lat)):
	if t_lat[i] >= tref_lat+add:
		i_add = i
		break


t_seg_lat = t_lat[i_sub:i_add]
v_seg_lat = v_lat[i_sub:i_add]

'''
plt.figure(figsize=(12,6))
plt.plot(t_lat, v_lat, drawstyle='steps-pre')
plt.plot(tref_lat, 1, 'o')
plt.xlim(tref_lat-sub, tref_lat+add)
#plt.plot(t, m)
plt.tight_layout()
plt.show()
'''

zero_t_scope = []
zeros_scope = []

for i in range(len(t_scope[0:refmax_scope])):
	if v_scope[i] == np.min(v_scope) and v_scope[i+1] != np.min(v_scope):
		zero_t_scope.append(t_scope[i])
		zeros_scope.append(np.min(v_scope))




zero_t_lat = []
zeros_lat = []

for i in range(i_sub, refmax_lat):
	if v_lat[i] == np.min(v_lat) and v_lat[i+1] != np.min(v_lat):
		zero_t_lat.append(t_lat[i])
		zeros_lat.append(np.min(v_lat))




periods_scope = []
periods_lat = []

for i in range(1, len(zero_t_scope)):
	periods_scope.append((zero_t_scope[i] - zero_t_scope[i-1]) * 1000)

for i in range(1, len(zero_t_lat)):
	periods_lat.append(zero_t_lat[i] - zero_t_lat[i-1])




print("Oscilloscope measured period:\n\tMin: %f\n\tMax: %f\n\tMean: %f\n\tStd: %f"%(np.min(periods_scope)*1000, np.max(periods_scope)*1000, np.mean(periods_scope)*1000, np.std(periods_scope)*1000))

print("Computer measured period:\n\tMin: %f\n\tMax: %f\n\tMean: %f\n\tStd: %f"%(np.min(periods_lat)*1000, np.max(periods_lat)*1000, np.mean(periods_lat)*1000, np.std(periods_lat)*1000))



plt.figure(figsize=(12,8))


ax1 = plt.subplot(2, 1, 1)
plt.plot(t_scope, v_scope)
plt.plot(tref_scope, np.max(v_scope), 'o')
plt.plot(zero_t_scope, zeros_scope, 'o')
plt.xlim(t_scope[0], t_scope[-1])

ax2 = plt.subplot(2, 1, 2)
plt.plot(t_lat, v_lat, drawstyle='steps-pre')
plt.plot(tref_lat, 1, 'o')
plt.plot(zero_t_lat, zeros_lat, 'o')
plt.xlim(tref_lat-sub, tref_lat+add)

plt.tight_layout()
plt.show()



'''
maxp = 0

for i in range(1, len(periods)):
	aux = periods[i] - periods[i-1]
	if aux > maxp and maxp < 200/1000:
		maxp = aux

print(maxp*1000)
'''


lats = [int(math.modf(x/1000)[1]) for x in l_lat]
	
plt.figure(figsize=(12,6))
plt.plot(t_lat, lats, label="Latencies", linewidth=0.8)
plt.legend()
plt.xlabel("Time (s)")
plt.ylabel("Latency (us)")
plt.title("Latency values")
plt.tight_layout()
plt.show()








