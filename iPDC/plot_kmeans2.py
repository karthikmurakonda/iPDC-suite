# 0 : 50.284426, 65451.237705
# 1 : 50.370104, 65476.164948
# 2 : 50.302515, 65464.320755

import csv
import numpy as np
import matplotlib.pyplot as plt
from sklearn import preprocessing
x=[]
y=[]
x1=[]
y1=[]
x2=[]
y2=[]
with open('kmeans.csv', mode ='r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        #print(lines)
        if(int(lines[2])==0):
            x.append(float(lines[0]))
            y.append(float(lines[1]))
        elif(int(lines[2])==1):
            x1.append(float(lines[0]))
            y1.append(float(lines[1]))
        else:
            x2.append(float(lines[0]))
            y2.append(float(lines[1]))

x.append(50.284426)
y.append(65451.237705)
x1.append(50.370104)
y1.append(65476.164948)
x2.append(50.302515)
y2.append(65464.320755)

x_array = np.array(x+x1+x2)
y_array = np.array(y+y1+y2)
# x1_array = np.array(x1)
# y1_array = np.array(y1)
# x2_array = np.array(x2)
# y2_array = np.array(y2)

x_arr = preprocessing.normalize([x_array])
y_arr = preprocessing.normalize([y_array])

# x1_arr = preprocessing.normalize([x1_array])
# y1_arr = preprocessing.normalize([y1_array])

# x2_arr = preprocessing.normalize([x2_array])
# y2_arr = preprocessing.normalize([y2_array])

plt.scatter(x_arr, y_arr)
# plt.scatter(x1_arr, y1_arr)
# plt.scatter(x2_arr, y2_arr)

plt.show()