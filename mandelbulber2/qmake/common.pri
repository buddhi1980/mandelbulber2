# qt dependencies
QT += core gui uitools widgets network testlib multimedia
macx:QT += svg

CONFIG += link_pkgconfig
CONFIG += c++14

# optional dependecies
qtHaveModule(gamepad){
	QT += gamepad
	DEFINES += USE_GAMEPAD
	message("Use QtGamepad module")
}

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

opencl {
	DEFINES += USE_OPENCL
	!win32:LIBS += -lOpenCL
        macx:LIBS -= -lOpenCL
        macx:LIBS += -framework OpenCL
	message("Use OpenCL library")
}

# required for proper logging output
DEFINES += QT_MESSAGELOGCONTEXT

TARGET = mandelbulber2 
TEMPLATE = app

CONFIG += qt thread

# specify paths to misc ressources
ROOT = $$PWD/..

SOURCES +=  $$ROOT/src/*.cpp
SOURCES +=  $$ROOT/qt/*.cpp

HEADERS += $$ROOT/src/*.hpp
HEADERS += $$ROOT/src/*.h
HEADERS += $$ROOT/qt/*.hpp
HEADERS += $$ROOT/qt/*.h

FORMS += $$ROOT/qt/*.ui

RESOURCES = $$ROOT/qt/icons.qrc

UI_DIR = $$ROOT/qt

INCLUDEPATH += $$ROOT/

TRANSLATIONS = $$ROOT/language/en.ts\
               $$ROOT/language/de.ts\
               $$ROOT/language/pl.ts\
               $$ROOT/language/it.ts\
               $$ROOT/language/nl.ts\
               $$ROOT/language/es.ts

# copy all system flags
QMAKE_CXXFLAGS += $$(CXXFLAGS)
QMAKE_CFLAGS += $$(CFLAGS)
QMAKE_CXXFLAGS += $$(CPPFLAGS)
QMAKE_CFLAGS += $$(CPPFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)

# TODO: is this still required?
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE -= -O1

# compiler build flags
QMAKE_CXXFLAGS += -ffast-math -fopenmp
macx:DEFINES += "SHARED_DIR_IS_APP_DIR" 

# test hardcoded lib path for gsl in travis container 
QMAKE_CXXFLAGS += -I/usr/include/gsl

# library linking
LIBS += -lpng -lgsl -lgslcblas -fopenmp -llzo2
macx:LIBS += -framework CoreFoundation
win32:LIBS += -lz

# mac specific options
macx:QMAKE_CC=/usr/local/opt/llvm/bin/clang
macx:QMAKE_CXX=/usr/local/opt/llvm/bin/clang++
macx:QMAKE_LINK=/usr/local/opt/llvm/bin/clang++
macx:INCLUDEPATH += /usr/local/opt/llvm/include/
macx:LIBS += -L/usr/local/opt/llvm/lib/
macx:ICON = $$ROOT/mac/mandelbulber2.icns

# gsl png osx absolute path
macx:INCLUDEPATH += /usr/local/include/
macx:LIBS += -L/usr/local/lib/




