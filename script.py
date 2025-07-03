import matplotlib.pyplot as plt
import numpy as np

values = []

with open('output_cp.txt', 'r') as file:
    for line in file:
        values.append(float(line.strip()))

fig, ax = plt.subplots()    

plt.ylim(0, 1)  

ax.plot(values, color='black')

plt.xlabel("Number of MC Sweeps")
plt.ylabel(r"$\phi$")
plt.title("Evolution of Order Parameter During Simulation")

plt.savefig('my_plot.png')
plt.show()