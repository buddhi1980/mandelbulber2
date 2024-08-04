# qt dependencies
QT += core gui uitools widgets network testlib multimedia qml concurrent
macx:QT += svg

CONFIG += opencl
CONFIG += gsl
CONFIG += png
CONFIG += zlib

CONFIG += c++17

# specify paths to misc ressources
ROOT = $$PWD/..
SHARED = $$ROOT/deploy/share/mandelbulber2

QMAKE_FULL_VERSION = 2.32
QMAKE_TARGET_BUNDLE_PREFIX = com.mandelbulber
VERSION = 2.32

#DEFINES += USE_EXR
#message("Use OpenEXR libraries for EXR files")

#LIBS += -ltiff
#DEFINES += USE_TIFF
#message("Use tiff library for TIFF files")

#DEFINES += USE_SNDFILE
message("Use sndfile library for WAV and MP3 files")



# required for proper logging output
DEFINES += QT_MESSAGELOGCONTEXT

TARGET = mandelbulber2 
TEMPLATE = app

CONFIG += qt thread

SOURCES +=  $$files($$ROOT/src/*.cpp, false)
SOURCES +=  $$files($$ROOT/qt/*.cpp, false)
SOURCES +=  $$files($$ROOT/formula/definition/*.cpp, false)

HEADERS += $$files($$ROOT/src/*.hpp, false)
HEADERS += $$files($$ROOT/src/*.h, false)
HEADERS += $$files($$ROOT/qt/*.hpp, false)
HEADERS += $$files($$ROOT/qt/*.h, false)
HEADERS += $$files($$ROOT/formula/definition/*.h, false)
HEADERS += $$files($$ROOT/formula/definition/*.hpp, false)
HEADERS += $$files($$ROOT/third-party/stb/*.h, false)

FORMS += $$files($$ROOT/qt/*.ui, false)

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
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE += -O2
QMAKE_CXXFLAGS += /GS /Zc:rvalueCast /TP /Qpar /GL /fp:precise /Zc:forScope /GR /Gd /MD /openmp
DEFINES -= ENABLE_EXTENDED_ALIGNED_STORAGE
DEFINES += WIN32

PREFIX = $$OUT_PWD

message(desitination dir = $$OUT_PWD)

opencl {
    DEFINES += USE_OPENCL
    INCLUDEPATH += $$ROOT/../extlib/opencl/include
    LIBS += -L$$ROOT/../extlib/opencl/lib/x64/ -lOpenCL
    
    opencldll.files = $$ROOT/../extlib/opencl/bin
    opencldll.path = $$PREFIX
    INSTALLS += opencldll    
    message("Use OpenCL library")   
}

gsl {
    INCLUDEPATH += $$ROOT/../extlib/gsl/include
    CONFIG(release, debug|release) {
        LIBS += -L$$ROOT/../extlib/gsl/lib/release/ -lgsl
        gsldll.files = $$ROOT/../extlib/gsl/bin/release
    }
    else:CONFIG(debug, debug|release) {
        LIBS += -L$$ROOT/../extlib/gsl/lib/debug/ -lgsl
        gsldll.files = $$ROOT/../extlib/gsl/bin/debug
    }
    gsldll.path = $$PREFIX
    INSTALLS += gsldll  
    message("Use GSL library")
}

png {
    INCLUDEPATH += $$ROOT/../extlib/png/include
    CONFIG(release, debug|release) {
        LIBS += -L$$ROOT/../extlib/png/lib/release/ -llibpng16
        pngdll.files = $$ROOT/../extlib/png/bin/release
    }
    else:CONFIG(debug, debug|release) {
        LIBS += -L$$ROOT/../extlib/png/lib/debug/ -llibpng16d
        pngdll.files = $$ROOT/../extlib/png/bin/debug
    }
    pngdll.path = $$PREFIX
    INSTALLS += pngdll 
    message("Use PNG library")
}

zlib {
    INCLUDEPATH += $$ROOT/../extlib/zlib/include
    CONFIG(release, debug|release) {
        LIBS += -L$$ROOT/../extlib/zlib/lib/release/ -lzlib
        zlibdll.files = $$ROOT/../extlib/zlib/bin/release
    }
    else:CONFIG(debug, debug|release) {
        LIBS += -L$$ROOT/../extlib/zlib/lib/debug/ -lzlibd
        zlibdll.files = $$ROOT/../extlib/zlib/bin/debug
    }
    zlibdll.path = $$PREFIX
    INSTALLS += zlibdll 
    message("Use ZLIB library")
}



#INCLUDEPATH += "C:\Users\buddh\Git\zlib"
#CONFIG(release, debug|release): LIBS += -LC:/Users/buddh/Git/zlib/Release/ -lzlib
#else:CONFIG(debug, debug|release): LIBS += -LC:/Users/buddh/Git/zlib/Debug/ -lzlibd

# INCLUDEPATH += "C:\Qt\6.7.2\msvc2019_64\include\QtPng"
# CONFIG(release, debug|release): LIBS += -LC:/Qt/6.7.2/msvc2019_64/lib/ -lQt6BundledLibpng
# else:CONFIG(debug, debug|release): LIBS += -LC:/Qt/6.7.2/msvc2019_64/lib/ -lQt6BundledLibpngd

#INCLUDEPATH += "C:\Users\buddh\include"
#LIBS += -LC:/Users/buddh/lib/ -lOpenCL





INCLUDEPATH += "C:\Users\buddh\Git\lzo-2.10\include"
CONFIG(release, debug|release): LIBS += -LC:/Users/buddh/Git/lzo-2.10/build/Release/ -llzo2
else:CONFIG(debug, debug|release): LIBS += -LC:/Users/buddh/Git/lzo-2.10/build/Debug/ -llzo2

INCLUDEPATH += C:\Users\buddh\lib\libsndfile-1.2.2-win64\include
CONFIG(release, debug|release): LIBS += -LC:/Users/buddh/lib/libsndfile-1.2.2-win64/lib -lsndfile
else:CONFIG(debug, debug|release): LIBS += -LC:/Users/buddh/lib/libsndfile-1.2.2-win64/lib -lsndfile

sharefiles.files = $$ROOT/deploy/share/mandelbulber2/data 
sharefiles.files += $$ROOT/deploy/share/mandelbulber2/examples 
sharefiles.files += $$ROOT/deploy/share/mandelbulber2/icons 
sharefiles.files += $$ROOT/deploy/share/mandelbulber2/materials 
sharefiles.files += $$ROOT/deploy/share/mandelbulber2/scripts
sharefiles.files += $$ROOT/deploy/share/mandelbulber2/sounds
sharefiles.files += $$ROOT/deploy/share/mandelbulber2/textures
sharefiles.files += $$ROOT/deploy/share/mandelbulber2/toolbar
sharefiles.path = $$PREFIX
INSTALLS += sharefiles

formulaui.files = $$ROOT/formula/ui
formulaui.path = $$PREFIX/formula
INSTALLS += formulaui

formulaimg.files = $$ROOT/formula/img
formulaimg.path = $$PREFIX/formula
INSTALLS += formulaimg

formulaopencl.files = $$ROOT/formula/opencl
formulaopencl.path = $$PREFIX/formula
INSTALLS += formulaopencl

openclfiles.files = $$ROOT/opencl
openclfiles.path = $$PREFIX
INSTALLS += openclfiles

documentation.files = $$ROOT/deploy/doc/Mandelbulber_Manual.pdf
documentation.files += $$ROOT/deploy/README
documentation.files += $$ROOT/deploy/NEWS
documentation.files += $$ROOT/deploy/COPYING
documentation.path = $$PREFIX/doc
INSTALLS += documentation

desktopfile.files = $$ROOT/deploy/linux/mandelbulber2.desktop
desktopfile.path = $$PREFIX/applications
INSTALLS += desktopfile

iconfile.files = $$ROOT/deploy/share/mandelbulber2/icons/mandelbulber.png
iconfile.path = $$PREFIX/icons
INSTALLS += iconfile

target.path = $$PREFIX/bin
INSTALLS += target

