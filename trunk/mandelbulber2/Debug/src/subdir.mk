################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/algebra.cpp \
../src/cimage.cpp \
../src/common_math.cpp \
../src/files.cpp \
../src/fractal.cpp \
../src/fractparams.cpp \
../src/interface.cpp \
../src/main.cpp \
../src/parameters.cpp \
../src/primitives.cpp \
../src/qt_interface.cpp \
../src/system.cpp \
../src/texture.cpp 

OBJS += \
./src/algebra.o \
./src/cimage.o \
./src/common_math.o \
./src/files.o \
./src/fractal.o \
./src/fractparams.o \
./src/interface.o \
./src/main.o \
./src/parameters.o \
./src/primitives.o \
./src/qt_interface.o \
./src/system.o \
./src/texture.o 

CPP_DEPS += \
./src/algebra.d \
./src/cimage.d \
./src/common_math.d \
./src/files.d \
./src/fractal.d \
./src/fractparams.d \
./src/interface.d \
./src/main.d \
./src/parameters.d \
./src/primitives.d \
./src/qt_interface.d \
./src/system.d \
./src/texture.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/qt4/QtUiTools -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include/qt4/QtXml -I/usr/include/qt4/QtCore -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


