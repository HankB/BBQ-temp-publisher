#!/usr/bin/python3
# fit a fifth degree polynomial to the economic data
from numpy import arange
from pandas import read_csv
from scipy.optimize import curve_fit
from matplotlib import pyplot

# define the true objective function
def objective(x, a, b, c, d, e, f):
	return (a * x) + (b * x**2) + (c * x**3) + (d * x**4) + (e * x**5) + f

# load the dataset
#url = 'https://raw.githubusercontent.com/jbrownlee/Datasets/master/longley.csv'
#dataframe = read_csv(url, header=None)
dataframe = read_csv('2022-05-01,3-points.csv')

data = dataframe.values
# choose the input and output variables
#x, y = data[:, 4], data[:, -1]
x, y = data[:, 0], data[:, 1]
# curve fit
popt, _ = curve_fit(objective, x, y)
# summarize the parameter values
a, b, c, d, e, f = popt
print('y = %.18f * x + %.18f * x^2 + %.18f * x^3 + %.18f * x^4 + %.18f * x^5 + %.18f' % (a, b, c, d, e, f))
print('y = %6e * x + %6e * x^2 + %6e * x^3 + %6e * x^4 + %6e * x^5 + %6e' % (a, b, c, d, e, f))
# plot input vs output
pyplot.scatter(x, y)
# define a sequence of inputs between the smallest and largest known inputs
x_line = arange(min(x), max(x), 1)
# calculate the output for the range
y_line = objective(x_line, a, b, c, d, e, f)
# create a line plot for the mapping function
pyplot.plot(x_line, y_line, '--', color='red')
pyplot.show()