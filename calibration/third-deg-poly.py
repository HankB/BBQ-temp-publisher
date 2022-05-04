#!/usr/bin/python3
# fit a fifth degree polynomial to the economic data
from numpy import arange
from pandas import read_csv
from scipy.optimize import curve_fit
from matplotlib import pyplot

# define the true objective function
def objective(x, a, b, c, d,):
	return (a * x) + (b * x**2) + (c * x**3)  + d

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
a, b, c, d = popt
print('y = %f * x + %f * x^2 + %f * x^3 + %f' % (a, b, c, d ))
# plot input vs output
pyplot.scatter(x, y)
# define a sequence of inputs between the smallest and largest known inputs
x_line = arange(min(x), max(x), 1)
# calculate the output for the range
y_line = objective(x_line, a, b, c, d)
# create a line plot for the mapping function
pyplot.plot(x_line, y_line, '--', color='red')
pyplot.show()