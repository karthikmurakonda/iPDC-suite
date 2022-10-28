import csv
import pandas as pd
import matplotlib.pyplot as plt

x=[]
y=[]
x1=[]
y1=[]
x2=[]
y2=[]
with open('points.csv', mode ='r') as file:
    csvFile = csv.reader(file)
    for lines in csvFile:
        print(lines)
        x.append(float(lines[0]))
        y.append(0)
        x1.append(float(lines[1]))
        y1.append(0)
        x2.append(float(lines[2]))
        y2.append(0)


plt.scatter(x, y)

plt.scatter(x1, y1)

plt.scatter(x2,y2)

plt.show()
