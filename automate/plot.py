#!/bin/python

import matplotlib.pyplot as plt

f = open("processedwindow.txt", "r")

xVals = []
yVals = []

lines = f.readlines()

for line in lines:
    time, val = line.split(";")
    xVals.append(float(time))
    yVals.append(float(val))

plt.plot(xVals, yVals)
plt.ylabel('MB/s')
plt.savefig("data.png")
