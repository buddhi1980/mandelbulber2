#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T11:34:51
#
#-------------------------------------------------

QT       += core gui uitools widgets network testlib multimedia

macx:QT += svg

!win32 {
  qtHaveModule(gamepad){
    QT += gamepad
    DEFINES += USE_GAMEPAD
    message("Use QtGamepad module")
  }
}

CONFIG += link_pkgconfig

packagesExist(IlmBase){
  PKGCONFIG += IlmBase
  LIBS += -lIlmImf -lHalf
	DEFINES += USE_EXR
	message("Use IlmBase library for EXR files")
}

packagesExist(libtiff-4){
  PKGCONFIG += libtiff-4
}
win32|packagesExist(libtiff-4) {
  LIBS += -ltiff
  DEFINES += USE_TIFF
  message("Use tiff library for TIFF files")
}

packagesExist(sndfile){
  PKGCONFIG += sndfile
}
win32|packagesExist(sndfile) {
  LIBS += -lsndfile
  DEFINES += USE_SNDFILE
  message("Use sndfile library for WAV files")
}

DEFINES += QT_MESSAGELOGCONTEXT

TARGET = mandelbulber2 
TEMPLATE = app

CONFIG += qt thread

CONFIG += console

SOURCES +=  ../src/*.cpp
SOURCES +=  ../qt/*.cpp

HEADERS += ../src/*.hpp
HEADERS += ../src/*.h
HEADERS += ../qt/*.hpp
HEADERS += ../qt/*.h

FORMS   += ../qt/*.ui

RESOURCES = ../qt/icons.qrc

UI_DIR = ../qt

INCLUDEPATH += ../qt/

TRANSLATIONS = ../language/en.ts\
               ../language/de.ts\
               ../language/pl.ts\
               ../language/it.ts\
               ../language/nl.ts

QMAKE_CXXFLAGS += $$(CXXFLAGS)
QMAKE_CFLAGS += $$(CFLAGS)
QMAKE_CXXFLAGS += $$(CPPFLAGS)
QMAKE_CFLAGS += $$(CPPFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
 
QMAKE_LFLAGS_RELEASE -= -O1

unix:!mac:QMAKE_CXXFLAGS += -ffast-math -fopenmp -std=c++11
macx:QMAKE_CXXFLAGS += -ffast-math -openmp -std=c++11
win32:QMAKE_CXXFLAGS += -ffast-math -fopenmp -std=c++11
clang:QMAKE_CXXFLAGS -= -fopenmp

# test hardcoded lib path for gsl in travis container 
QMAKE_CXXFLAGS += -I/usr/include/gsl

unix:!mac:LIBS += -lpng -lgsl -lgslcblas -fopenmp
macx:LIBS += -lpng -lgsl -lgslcblas -openmp
win32:LIBS += -lz -lpng -lgsl -lgslcblas -fopenmp
clang:LIBS -= -fopenmp
#win32:LIBS -= +fopenmp


# gsl png osx absolute path

macx:INCLUDEPATH += /usr/local/include/
macx:LIBS += -L/usr/local/lib/


# rh: ugly absolute paths for libpng and libjpeg on my windows system

#lpng
win32:greaterThan(QT_MAJOR_VERSION, 4): INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"
#win32:LIBS += "C:/Qt/Tools/mingw48_32/lib/libpng16"
#win32:INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"
