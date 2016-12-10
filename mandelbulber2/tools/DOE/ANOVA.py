#!/usr/bin/env python
from __future__ import print_function
import math
####
import numpy
# TODO: Python ANOVA logic requires functionality review
# TODO: Python ANOVA logic requires reformat and refactor

def Main ( ):
	# Analysis of Variance #
	# Reference: https://msdn.microsoft.com/magazine/mt742873 #
	print ( "\nBegin ANOVA using python \n" )
	print ( "The goal is to infer if 3 or more population" )
	print ( "means are all equal based on samples. \n" )
	print ( "The sample data is: " )
	fname = 'ANOVA.csv'
	csv = numpy.genfromtxt ( fname, dtype=str, delimiter="," )
	num_rows = csv.shape [0]
	num_cols = csv.shape [1]
	names = csv [:,0]
	data = numpy.genfromtxt ( fname, usecols = range ( 1,num_cols ), delimiter="," )
	ShowData ( data, names )
	print ( "\nCalculating F-statistic" )
	# degrees of freedom
	df = numpy.zeros ( 2 )
	[ F, df ] = Fstat ( data )
	print ( "F = " + str ( F ) )
	print ( "The degrees of freedom are " )
	print ( str ( df [0] ) + ", " + str ( df [1] ) )
	print ( "\nCalculating p-value" )
	pValue = QF ( df [0], df [1], F )
	print ( "p-value = " )
	print ( str ( pValue ) )
	print ( "\nThe p-value is the probability that all " )
	print ( "group means are equal." )
	print ( "Interpreting the p-value is subtle." )
	print ( "\nEnd ANOVA \n" )

def Fstat ( data ):
	# calculate F statistic and degrees freedom num, denom
	# assumes data has specific structure:
	#  unlimited groups (rows)
	#  with equal number of items (columns)
	K = data.shape [0] # number groups
	n = numpy.zeros ( K ) # number items each group
	N = 0 # total number data points
	i = 0
	while i < K:
		n [i] = data.shape [1]
		N += data.shape [1]
		i += 1
	# 1.  group means and grand mean
	means = numpy.zeros ( K )
	gMean = 0.0
	i = 0
	while i < K:
		j = 0
		while j < data.shape [1]:
			means [i] += data [i] [j]
			gMean += data [i] [j]
			j += 1
		means [i] /= n [i]
		print ( "Group [" + str ( i ) + "] mean = " + str ( means [i] ) )
		i += 1
	gMean /= N
	print ( "Overall mean = " + str ( gMean ) )
	# 2.  SSb and MSb
	SSb = 0.0
	MSb = 0.0
	i = 0
	while i < K:
		SSb += n [i] * ( means [i] - gMean ) * ( means [i] - gMean )
		i += 1
	MSb = SSb / ( K - 1 )
	# 3.  SSw and MSw
	SSw = 0.0
	i = 0
	while i < K:
		j = 0
		while j < data.shape [1]:
			SSw += ( data [i] [j] - means [i] ) * ( data [i] [j] - means [i] )
			j += 1
		i += 1
	MSw = SSw / ( N - K )
	# Demonstrate calculation
	print ( "Calculated SSb = " + str ( SSb ) )
	print ( "Calculated MSb = " + str ( MSb ) )
	print ( "Calculated SSw = " + str ( SSw ) )
	print ( "Calculated MSw = " + str ( MSw ) )
	print ( "F = MSb / MSw" )
	print ( " = " + str ( MSb ) + " / " + str ( MSw ) )
	# store df values
	df = numpy.zeros ( 2 )
	df [0] = K - 1
	df [1] = N - K
	F = MSb / MSw
	return [ F, df ]

def LogGamma ( z ):
	# Lanczos approximation g=5, n=7
	coef = [ 1.000000000190015, 76.18009172947146, -86.50532032941677, 24.01409824083091, -1.231739572450155, 0.1208650973866179e-2, -0.5395239384953e-5 ]
	LogSqrtTwoPi = 0.91893853320467274178
	if z < 0.5: # Gamma(z) = Pi / (Sin(Pi*z))* Gamma(1-z))
		return math.log ( math.PI / math.sin ( math.PI * z ) ) - LogGamma ( 1.0 - z )
	zz = z - 1.0
	b = zz + 5.5 # g + 0.5
	sum = coef [0]
	i = 1
	while i < len ( coef ):
		sum += coef [i] / ( zz + i )
		i += 1
	return ( LogSqrtTwoPi + math.log ( sum ) - b ) + ( math.log ( b ) * ( zz + 0.5 ) )

def BetaInc ( a, b, x ):
	# Incomplete Beta function
	# A & S 6.6.2 and 26.5.8
	if x == 0.0 or x == 1.0:
		bt = 0.0
	else:
		bt = math.exp ( LogGamma ( a + b ) - LogGamma ( a ) - LogGamma ( b ) + a * math.log ( x ) + b * math.log ( 1.0 - x ) )
	if x < ( a + 1.0 ) / ( a + b + 2.0 ):
		return bt * BetaIncCf ( a, b, x ) / a
	else:
		return 1.0 - bt * BetaIncCf ( b, a, 1.0 - x ) / b

def BetaIncCf ( a, b, x ):
	# Approximate Incomplete Beta computed by
	# continued fraction
	# A & S 26.5.8
	max_it = 100
	epsilon = 3.0e-7
	small = 1.0e-30 # 2*m
	qab = a + b
	qap = a + 1.0
	qam = a - 1.0
	c = 1.0
	d = 1.0 - qab * x / qap
	if math.fabs ( d ) < small:
		d = small
	d = 1.0 / d
	result = d
	m = 1
	while m <= max_it:
		m2 = 2 * m
		aa = m * ( b - m ) * x / ( ( qam + m2 ) * ( a + m2 ) )
		d = 1.0 + aa * d
		if math.fabs ( d ) < small:
			d = small
		c = 1.0 + aa / c
		if math.fabs ( c ) < small:
			c = small
		d = 1.0 / d
		result *= d * c
		aa = -( a + m ) * ( qab + m ) * x / ( ( a + m2 ) * ( qap + m2 ) )
		d = 1.0 + aa * d
		if math.fabs ( d ) < small:
			d = small
		c = 1.0 + aa / c
		if math.fabs ( c ) < small:
			c = small
		d = 1.0 / d
		de = d * c
		result *= de
		if math.fabs ( de - 1.0 ) < epsilon:
			break
		m += 1 # for
	if m > max_it:
		raise Exception ( "BetaIncCf() failure " )
	return result

def PF ( a, b, x ):
	# approximate lower tail of F-dist
	# (area from 0.0 to x)
	# equivalent to the R pf() function
	# only accurate to about 8 decimals
	z = ( a * x ) / ( a * x + b )
	return BetaInc ( a / 2, b / 2, z )

def QF ( a, b, x ):
	# area from x to +infinity under F
	# for ANOVA = prob(all means equal)
	return 1.0 - PF ( a, b, x )

def ShowData ( data, names ):
	i = 0
	while i < data.shape [0]:
		configData = names [i] + ": "
		j = 0
		while j < data.shape [1]:
			configData += str ( data [i] [j] ) + ' '
			j += 1
		print ( configData )
		i += 1

if __name__ == "__main__":
	# ENTRY #
	Main ( )
	exit ( 0 )
	try:
		Main ( )
		exit ( 0 )
	except KeyboardInterrupt:
		print ( "KeyboardInterrupt Detected" )
	finally:
		exit ( )
