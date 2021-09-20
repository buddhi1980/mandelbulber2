# qt dependencies
QT += core gui uitools widgets network testlib multimedia
macx:QT += svg

CONFIG += link_pkgconfig

unix:!macx:CONFIG += c++14
macx:!m1:CONFIG += c++14
macx:m1:CONFIG += c++17

m1: {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.5
}

QMAKE_FULL_VERSION = 2.26-dev
QMAKE_TARGET_BUNDLE_PREFIX = com.mandelbulber
VERSION = 2.26

m1:DEFINES += NO_AUDIO_OUTPUT
m1:DEFINES += NO_QT_MULTIMEDIA_AUDIO

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
macx|win32|packagesExist(libtiff-4) {
	LIBS += -ltiff
	DEFINES += USE_TIFF
	message("Use tiff library for TIFF files")
}

packagesExist(sndfile){
	PKGCONFIG += sndfile
}
macx|win32|packagesExist(sndfile) {
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
SOURCES +=  $$ROOT/formula/definition/*.cpp

HEADERS += $$ROOT/src/*.hpp
HEADERS += $$ROOT/src/*.h
HEADERS += $$ROOT/qt/*.hpp
HEADERS += $$ROOT/qt/*.h
HEADERS += $$ROOT/formula/definition/*.h
HEADERS += $$ROOT/formula/definition/*.hpp
HEADERS += $$ROOT/third-party/stb/*.h

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
unix:!macx:QMAKE_CXXFLAGS += -ffast-math -fopenmp
macx:!m1:QMAKE_CXXFLAGS += -ffast-math -fopenmp
macx:m1:QMAKE_CXXFLAGS += -ffast-math

macx:DEFINES += "SHARED_DIR_IS_APP_DIR" 

# test hardcoded lib path for gsl in travis container 
#QMAKE_CXXFLAGS += -I/usr/include/gsl

QMAKE_CXXFLAGS += -I/usr/include/gsl
m1:QMAKE_CXXFLAGS += -I/opt/homebrew/include

# library linking
unix:!macx:LIBS += -lpng -lgsl -lgslcblas -llzo2 -fopenmp
macx:!m1:LIBS += -lpng -lgsl -lgslcblas -llzo2 -fopenmp
macx:m1:LIBS += -lpng -lgsl -lgslcblas -llzo2

macx:!m1:LIBS += -framework CoreFoundation
win32:LIBS += -lz

# mac specific options
macx:!m1:QMAKE_CC=/usr/local/opt/llvm/bin/clang
macx:!m1:QMAKE_CXX=/usr/local/opt/llvm/bin/clang++
macx:!m1:QMAKE_LINK=/usr/local/opt/llvm/bin/clang++
macx:!m1:INCLUDEPATH += /usr/local/opt/llvm/include/
macx:!m1:LIBS += -L/usr/local/opt/llvm/lib/
macx:LIBS += -L/opt/homebrew/lib/
macx:ICON = $$ROOT/mac/mandelbulber2.icns

# gsl png osx absolute path
macx:!m1:INCLUDEPATH += /usr/local/include/
macx:!m1:LIBS += -L/usr/local/lib/




