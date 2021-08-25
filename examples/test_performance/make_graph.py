#!/usr/bin/python

### This script creates a graph, using data from the "data.txt" file, measuring
### how the latency and throughput of an NF chain changes based on length

from matplotlib import pyplot as plt

x_axis = []

y_axis_latency = []

y_axis_throughput = []

file = open("data.txt", "r")

for line in file.readlines():
	linedata = line.split(",")
	x_axis.append(int(linedata[0]))
	y_axis_latency.append(int(linedata[1]))
	y_axis_throughput.append(int(linedata[2]))

fig,ax = plt.subplots()
# make a plot
ax.plot(x_axis, y_axis_latency, color="red", marker="o")
# set x-axis label
ax.set_xlabel("NFs",fontsize=14)
# set y-axis label
ax.set_ylabel("Latency (ns)",color="red",fontsize=14)

ax2=ax.twinx()
# make a plot with different y-axis using second axis object
ax2.plot(x_axis, y_axis_throughput,color="blue",marker="o")
ax2.set_ylabel("TX pkts per second",color="blue",fontsize=14)
plt.show()
# save the plot as a file
fig.savefig('graph.png',
            format='png',
            dpi=100,
            bbox_inches='tight')
