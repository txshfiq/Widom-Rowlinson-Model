import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit

def acf(x, length=20):
    return np.array([1]+[np.corrcoef(x[:-i], x[i:])[0,1]  \
        for i in range(1, length)])

def fit_func(x, a):
    return np.exp(-x / a)

values = []

with open('output_auto_de.txt', 'r') as file:
    for line in file:
        values.append(float(line.strip()))

y = acf(values, length=3500)
x = np.arange(len(y)) + 1

constants = curve_fit(fit_func, x, y)

a_fit = constants[0][0]

print(a_fit)

## commented out to save computation time, used to generate fit data
'''
fit = []
for i in x:
    fit.append(fit_func(i, a_fit))
'''

## plotting code (optional)
'''
fig, ax = plt.subplots()    

plt.ylim(-1, 1)  

ax.plot(x, y, marker='o', linestyle='-', color='b', label='ACF')
ax.plot(x, fit, linestyle='--', color='r', label='Exponential Fit')
ax.set_xlabel('Lag')
ax.set_ylabel('Autocorrelation')
ax.set_title('Autocorrelation Function')

plt.savefig('my_plot_auto.png')
plt.show()
'''