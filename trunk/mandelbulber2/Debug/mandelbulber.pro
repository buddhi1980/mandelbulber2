#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T11:34:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mandelbulber2
TEMPLATE = app

CONFIG += qt thread debug

SOURCES +=  ../src/algebra.cpp\
			../src/calculate_distance.cpp\
	    	../src/cimage.cpp\
	    	../src/common_math.cpp\
	    	../src/fractparams.cpp\
	    	../src/files.cpp\
	    	../src/four_fractals.cpp\
	    	../src/fractal.cpp\
	    	../src/fractal_list.cpp\
	    	../src/initparameters.cpp\
	    	../src/interface.cpp\
	    	../src/main.cpp\
	    	../src/parameters.cpp\
	    	../src/primitives.cpp\
	    	../src/render_window.cpp\
	    	../src/system.cpp\
            ../src/texture.cpp

HEADERS  += ../src/render_window.hpp

FORMS    += ../qt/render_window.ui

RESOURCES = ../qt/icons.qrc

UI_DIR = ../qt

greaterThan(QT_MAJOR_VERSION, 4):LIBS += -lQt5UiTools
else:LIBS += -lQtUiTools
LIBS += -lpng
LIBS += -ljpeg
win32:LIBS += -lz

# rh: ugly absolute paths for libpng and libjpeg on my windows system

#lpng
win32:greaterThan(QT_MAJOR_VERSION, 4): INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"
#win32:LIBS += "C:/Qt/Tools/mingw48_32/lib/libpng16"
#win32:INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"

#ljpeg
win32:LIBS += -L"C:/libjpeg-turbo-gcc/lib/" -ljpeg
win32:INCLUDEPATH += "C:/libjpeg-turbo-gcc/include"
win32:DEPENDPATH += "C:/libjpeg-turbo-gcc/include"

