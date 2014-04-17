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
	    	../src/cimage.cpp\
	    	../src/common_math.cpp\
	    	../src/fractparams.cpp\
	    	../src/files.cpp\
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

HEADERS  += ../src/render_window.hpp\

FORMS    += ../qt/render_window.ui

RESOURCES = ../qt/icons.qrc

UI_HEADERS_DIR = ../qt

LIBS += -lQtGui
LIBS += -lQtUiTools
LIBS += -lQtCore
LIBS += -lQtXml
LIBS += -lpng
LIBS += -ljpeg
