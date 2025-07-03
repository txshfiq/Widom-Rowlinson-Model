import matplotlib.pyplot as plt
import numpy as np

def acf(x, length=20):
    return np.array([1]+[np.corrcoef(x[:-i], x[i:])[0,1]  \
        for i in range(1, length)])

values = []

with open('output_auto_cp.txt', 'r') as file:
    for line in file:
        values.append(float(line.strip()))

y = acf(values, length=1000)
log_y = np.log(y)

x = np.arange(len(y)) + 1

slope, intercept = np.polyfit(x, log_y, 1)

a = -1.0 / slope

print(a)

## plotting code (optional)

fig, ax = plt.subplots()    

plt.ylim(-1, 1)  

ax.plot(x, y, marker='o', linestyle='-', color='b', label='ACF')
ax.plot(x, fit, linestyle='--', color='r', label='Exponential Fit')
ax.set_xlabel('Lag')
ax.set_ylabel('Autocorrelation')
ax.set_title('Autocorrelation Function')

plt.savefig('my_plot_auto.png')
plt.show()
