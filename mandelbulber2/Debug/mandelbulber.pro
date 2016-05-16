#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T11:34:51
#
#-------------------------------------------------

QT       += core gui uitools widgets network testlib

macx:QT += svg

!win32 {
  qtHaveModule(gamepad){
    QT += gamepad
    DEFINES += USE_GAMEPAD
  }
}

CONFIG += link_pkgconfig

packagesExist(IlmBase){
  PKGCONFIG += IlmBase
  LIBS += -lIlmImf -lHalf
	DEFINES += USE_EXR
}

packagesExist(libtiff-4){
  PKGCONFIG += libtiff-4
}
win32|packagesExist(libtiff-4) {
  LIBS += -ltiff
  DEFINES += USE_TIFF
}

TARGET = mandelbulber2 
TEMPLATE = app

CONFIG += qt thread debug

SOURCES +=  ../src/*.cpp
SOURCES +=  ../qt/*.cpp

HEADERS += ../src/*.hpp
HEADERS += ../src/*.h
HEADERS += ../qt/*.hpp
HEADERS += ../qt/*.h

FORMS   += ../qt/*.ui

RESOURCES = ../qt/icons.qrc

UI_DIR = ../qt

TRANSLATIONS = ../language/en.ts\
               ../language/de.ts\
               ../language/pl.ts\
               ../language/it.ts

QMAKE_CXXFLAGS += $$(CXXFLAGS)
QMAKE_CFLAGS += $$(CFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
 
QMAKE_LFLAGS_RELEASE -= -O1 -fopenmp

QMAKE_CXXFLAGS += -ffast-math -fopenmp -std=c++11
# test hardcoded lib path for gsl in travis container 
QMAKE_CXXFLAGS += -I/usr/include/gsl

LIBS += -lpng -lgsl -lgslcblas -fopenmp
win32:LIBS += -lz

# rh: ugly absolute paths for libpng and libjpeg on my windows system

#lpng
win32:greaterThan(QT_MAJOR_VERSION, 4): INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"
#win32:LIBS += "C:/Qt/Tools/mingw48_32/lib/libpng16"
#win32:INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"
