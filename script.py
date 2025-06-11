import matplotlib.pyplot as plt
import numpy as np

values = []

with open('output_cp.txt', 'r') as file:
    for line in file:
        values.append(float(line.strip()))

fig, ax = plt.subplots()    

plt.ylim(-1, 1)  

ax.plot(values)

plt.savefig('my_plot.png')
plt.show()