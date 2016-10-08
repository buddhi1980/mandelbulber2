#!/bin/bash

# Build Systems #
source $PWD/local.sh
BUILDTREE=$BUILDNAN

# Clean build dir
rm -rf $BUILDTREE/
mkdir -p $BUILDTREE/

# Generate Source
cat > $BUILDTREE/NaNtest-src.cpp <<EOF
#include <iostream>
#include <math.h>
#include <cmath>
#include <gsl/gsl_sys.h>

//http://stackoverflow.com/questions/570669/checking-if-a-double-or-float-is-nan-in-c
// Modified by mancoast
/*
  Portable warning-free NaN test:
    * Does not emit warning with -Wfloat-equal (does not use float comparisons)
    * Works with -O3 -ffast-math (floating-point optimization)
    * Only call to standard library is memset and memcmp via <cstring>
    * Works for IEEE 754 compliant floating-point representations
    * Also works for extended precision long double
*/
#include <limits>
#include <cstring>
template <class T> bool isNaN(T x)
{

        std::cout << std::endl;
        std::cout << std::endl;

	std::cout << "x\t";
	std::cout << x;
	std::cout << "\n";
	std::cout << "std::isnan(x);\t";
	std::cout << std::isnan(x);
	std::cout << "\n";
	std::cout << "::isnan(x);\t";
	std::cout << ::isnan(x);
	std::cout << "\n";
	std::cout << "gsl_isnan(x);\t";
	std::cout << gsl_isnan(x);
	std::cout << "\n";
	std::cout << "!gsl_finite(x)\t";
	std::cout << !gsl_finite(x);
	std::cout << "\n";

  /*Initialize all bits including those used for alignment to zero. This sets
  all the values to positive zero but does not clue fast math optimizations as
  to the value of the variables.*/
  T z[4];
  memset(z, 0, sizeof(z));
  z[1] = -z[0];
  z[2] = x;
  z[3] = z[0] / z[2];

  /*Rationale for following test:
    * x is 0 or -0                                --> z[2] = 0, z[3] = NaN
    * x is a negative or positive number          --> z[3] = 0
    * x is a negative or positive denormal number --> z[3] = 0
    * x is negative or positive infinity          --> z[3] = 0
      (IEEE 754 guarantees that 0 / inf is zero)
    * x is a NaN                                  --> z[3] = NaN != 0.
  */

  //Do a bitwise comparison test for positive and negative zero.
  bool z2IsZero = memcmp(&z[2], &z[0], sizeof(T)) == 0 ||
                  memcmp(&z[2], &z[1], sizeof(T)) == 0;

  bool z3IsZero = memcmp(&z[3], &z[0], sizeof(T)) == 0 ||
                  memcmp(&z[3], &z[1], sizeof(T)) == 0; 

  //If the input is bitwise zero or negative zero, then it is not NaN.
  return !z2IsZero && !z3IsZero;
}

//NaN test suite
#include <iostream>

/*If printNaN is true then only expressions that are detected as NaN print and
vice versa.*/
template <class T> void test(bool printNaN)
{
  T v[10] = {-0.0, 0.0, -1.0, 1.0,
    std::numeric_limits<T>::infinity(),
    -std::numeric_limits<T>::infinity(),
    std::numeric_limits<T>::denorm_min(),
    -std::numeric_limits<T>::denorm_min(),
    std::numeric_limits<T>::quiet_NaN(),
    std::numeric_limits<T>::signaling_NaN()};
  printNaN = true;
  for(int i = 0; i < 10; i++)
  {
    for(int j = 0; j < 10; j++)
    {
      if(isNaN(v[i] + v[j]) == printNaN)
        std::cout << v[i] << "+" << v[j] << " = " << v[i] + v[j] << std::endl;



      if(isNaN(v[i] - v[j]) == printNaN)
        std::cout << v[i] << "-" << v[j] << " = " << v[i] - v[j] << std::endl;


      if(isNaN(v[i] * v[j]) == printNaN)
        std::cout << v[i] << "*" << v[j] << " = " << v[i] * v[j] << std::endl;


      if(isNaN(v[i] / v[j]) == printNaN)
        std::cout << v[i] << "/" << v[j] << " = " << v[i] / v[j] << std::endl;
    }
  }
}


int main()
{


	std::cout << "\n\n###Hello System!###\n";
	std::cout << "\tTest for NaN cases:\n";

	double x = -1.0; x = x / (x + 1.0); x += 10.0; x = sin(x);
	std::cout << "double x = -1.0; x = x / (x + 1.0); x += 10.0; x = sin(x);\n";

	std::cout << "x\t";
	std::cout << x;
	std::cout << "\n";
	std::cout << "std::isnan(x);\t";
	std::cout << std::isnan(x);
	std::cout << "\n";
	std::cout << "::isnan(x);\t";
	std::cout << ::isnan(x);
	std::cout << "\n";
	std::cout << "gsl_isnan(x);\t";
	std::cout << gsl_isnan(x);
	std::cout << "\n";


	std::cout << "NaNs:" << std::endl;
	test<float>(true);
	test<double>(true);
	test<long double>(true);
	std::cout << std::endl << "Not NaNs:" << std::endl;
	test<float>(false);
	test<double>(false);
	test<long double>(false);


	std::cout << "\nExit\n\n";

	return 0;
}

EOF
cat $BUILDTREE/NaNtest-src.cpp

# Compile KNC
icpc \
-I$MPSSDIR/usr/include \
-I$KNC_LIB/gsl/install-mic/include \
-02 -g -fPIC -mmic \
-wd39,10006 \
-ffast-math -fopenmp -std=c++11 \
-o $BUILDTREE/NaNtest-src.cpp.o \
-c $BUILDTREE/NaNtest-src.cpp

# Link KNC
icpc \
-02 -g -fPIC -mmic -wd39,10006  -ffast-math -fopenmp -std=c++11 \
$BUILDTREE/NaNtest-src.cpp.o \
-o $BUILDTREE/NaNtest.mic \
$KNC_LIB/gsl/install-mic/lib/libgsl.a \
$KNC_LIB/gsl/install-mic/lib/libgslcblas.a \
-Wl,-rpath,$MPSSDIR/usr/lib64

# Execute NaN tests
chmod +x $BUILDTREE/NaNtest.mic
ssh mic0 $BUILDTREE/NaNtest.mic

# Cleanup
rm -rf $BUILDTREE/NaNtest-src.cpp.o

# Compile x64
c++ \
-ffast-math -fopenmp -std=c++11 \
-fPIC -std=gnu++11 \
-o $BUILDTREE/NaNtest-src.cpp.o \
-c $BUILDTREE/NaNtest-src.cpp

# Link x64
c++ \
-ffast-math -fopenmp -std=c++11 \
$BUILDTREE/NaNtest-src.cpp.o \
-o $BUILDTREE/NaNtest \
/usr/lib64/libgsl.so \
/usr/lib64/libgslcblas.so

# Execute NaN tests
chmod +x $BUILDTREE/NaNtest
$BUILDTREE/NaNtest




