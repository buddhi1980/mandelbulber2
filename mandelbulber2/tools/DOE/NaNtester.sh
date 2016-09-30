# Clean build dir
rm -rf /home/NaNtest/
mkdir -p /home/NaNtest/

# Generate Source
cat > /home/NaNtest/NaNtest-src.cpp <<EOF
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
cat /home/NaNtest/NaNtest-src.cpp

# Compile KNC
/opt/intel/compilers_and_libraries_2017.0.064/linux/bin/intel64/icpc \
-DQT_CORE_LIB \
-DQT_GUI_LIB \
-DQT_NETWORK_LIB \
-DQT_NO_DEBUG \
-DQT_TESTLIB_LIB \
-DQT_UITOOLS_LIB \
-DQT_WIDGETS_LIB \
-I/home/mandelbulber2/build-mic \
-I/home/mandelbulber2 \
-I/home/mandelbulber2/mandelbulber2/qt \
-I/home/mandelbulber2/mandelbulber2/src \
-I/home/KNC-Lib/libpng/install-mic/include \
-I/home/mpss-3.7/k1om/usr/include \
-I/home/KNC-Lib/gsl/install-mic/include \
-I/home/KNC-Lib/libjpeg \
-isystem /home/KNC-Lib/qt5/build-mic/qtbase/include \
-isystem /home/KNC-Lib/qt5/build-mic/qtbase/include/QtWidgets \
-isystem /home/KNC-Lib/qt5/build-mic/qtbase/include/QtGui \
-isystem /home/KNC-Lib/qt5/build-mic/qtbase/include/QtCore \
-isystem /home/KNC-Lib/qt5/qtbase/mkspecs/linux-icc-64 \
-isystem /home/KNC-Lib/qt5/build-mic/qtbase/include/QtNetwork \
-isystem /home/KNC-Lib/qt5/build-mic/qttools/include \
-isystem /home/KNC-Lib/qt5/build-mic/qttools/include/QtUiTools \
-isystem /home/KNC-Lib/qt5/build-mic/qtbase/include/QtTest \
-02 -g -fPIC -mmic \
-wd39,10006 \
-ffast-math -fopenmp -std=c++11 \
-o /home/NaNtest/NaNtest-src.cpp.o \
-c /home/NaNtest/NaNtest-src.cpp

# Link KNC
/opt/intel/compilers_and_libraries_2017.0.064/linux/bin/intel64/icpc \
-02 -g -fPIC -mmic -wd39,10006  -ffast-math -fopenmp -std=c++11 \
/home/NaNtest/NaNtest-src.cpp.o \
-o /home/NaNtest/NaNtest.mic \
/home/KNC-Lib/qt5/build-mic/qtbase/lib/libQt5Widgets.so.5.4.3 \
/home/KNC-Lib/qt5/build-mic/qtbase/lib/libQt5Network.so.5.4.3 \
/home/KNC-Lib/qt5/build-mic/qtbase/lib/libQt5Gui.so.5.4.3 \
/home/KNC-Lib/qt5/build-mic/qttools/lib/libQt5UiTools.a \
/home/KNC-Lib/qt5/build-mic/qtbase/lib/libQt5Test.so.5.4.3 \
/home/KNC-Lib/libpng/install-mic/lib/libpng15.so.15.27.0 \
/home/KNC-Lib/gsl/install-mic/lib/libgsl.a \
/home/KNC-Lib/gsl/install-mic/lib/libgslcblas.a \
/home/mpss-3.7/k1om/usr/lib64/libz.so \
/home/KNC-Lib/libjpeg/build-mic/libjpeg.a \
/home/KNC-Lib/qt5/build-mic/qtbase/lib/libQt5Core.so.5.4.3 \
-Wl,-rpath,/home/KNC-Lib/qt5/build-mic/qtbase/lib:/home/mpss-3.7/k1om/usr/lib64

# Execute NaN tests
chmod +x /home/NaNtest/NaNtest.mic
ssh mic0 /home/NaNtest/NaNtest.mic

# Cleanup
rm -rf /home/NaNtest/NaNtest-src.cpp.o

# Compile x64
/bin/c++ \
-DQT_CORE_LIB \
-DQT_GAMEPAD_LIB \
-DQT_GUI_LIB \
-DQT_NETWORK_LIB \
-DQT_NO_DEBUG \
-DQT_TESTLIB_LIB \
-DQT_UITOOLS_LIB \
-DQT_WIDGETS_LIB \
-DUSE_GAMEPAD=1 \
-I/home/mandelbulber2/build \
-I/home/mandelbulber2 \
-I/home/mandelbulber2/mandelbulber2/qt \
-I/home/mandelbulber2/mandelbulber2/src \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include/QtWidgets \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include/QtGui \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include/QtCore \
-isystem /opt/Qt5.7.0/5.7/gcc_64/./mkspecs/linux-g++ \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include/QtNetwork \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include/QtUiTools \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include/QtTest \
-isystem /opt/Qt5.7.0/5.7/gcc_64/include/QtGamepad \
-ffast-math -fopenmp -std=c++11 \
-fPIC -std=gnu++11 \
-o /home/NaNtest/NaNtest-src.cpp.o \
-c /home/NaNtest/NaNtest-src.cpp

# Link x64
/bin/c++ \
-ffast-math -fopenmp -std=c++11 \
/home/NaNtest/NaNtest-src.cpp.o \
-o /home/NaNtest/NaNtest \
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Network.so.5.7.0 \
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5UiTools.a \
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Test.so.5.7.0 \
/lib64/libpng.so /usr/lib64/libgsl.so \
/usr/lib64/libgslcblas.so \
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Gamepad.so.5.7.0 \
/lib64/libz.so \
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Widgets.so.5.7.0 \
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Gui.so.5.7.0 \
/opt/Qt5.7.0/5.7/gcc_64/lib/libQt5Core.so.5.7.0 \
-Wl,-rpath,/opt/Qt5.7.0/5.7/gcc_64/lib

# Execute NaN tests
chmod +x /home/NaNtest/NaNtest
/home/NaNtest/NaNtest




