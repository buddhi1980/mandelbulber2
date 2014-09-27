#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T11:34:51
#
#-------------------------------------------------

QT       += core gui uitools widgets

TARGET = mandelbulber2
TEMPLATE = app

CONFIG += qt thread debug

SOURCES +=  ../src/algebra.cpp\
			../src/calculate_distance.cpp\
			../src/camera_target.cpp\
			../src/compute_fractal.cpp\
			../src/color_palette.cpp\
	    	../src/cimage.cpp\
	    	../src/common_math.cpp\
	    	../src/dof.cpp\
	    	../src/error_message.cpp\
	    	../src/fractparams.cpp\
	    	../src/files.cpp\
	    	../src/four_fractals.cpp\
	    	../src/fractal.cpp\
	    	../src/fractal_formulas.cpp\
	    	../src/fractal_list.cpp\
	    	../src/initparameters.cpp\
	    	../src/interface.cpp\
	    	../src/lights.cpp\
	    	../src/main.cpp\
	    	../src/my_ui_loader.cpp\
	    	../src/parameters.cpp\
	    	../src/primitives.cpp\
	    	../src/progress_text.cpp\
	    	../src/preview_file_dialog.cpp\
	    	../src/render_image.cpp\
	    	../src/rendered_image_widget.cpp\
	    	../src/render_job.cpp\
	    	../src/render_ssao.cpp\
	    	../src/render_window.cpp\
	    	../src/render_worker.cpp\
	    	../src/scheduler.cpp\
	    	../src/settings.cpp\
	    	../src/shaders.cpp\
	    	../src/system.cpp\
	    	../src/ssao_worker.cpp\
            ../src/texture.cpp\
            ../src/thumbnail.cpp\
            ../src/undo.cpp
            ../qt/mygroupbox.cpp\
            ../qt/mylineedit.cpp\
            ../qt/myspinbox.cpp\
            ../qt/mydoublespinbox.cpp\
            ../qt/mycheckbox.cpp

HEADERS  += ../src/render_window.hpp\
			../src/render_worker.hpp\
			../qt/myscrolledarea.h\
			../qt/mylineedit.h\
			../qt/myspinbox.h\
			../qt/mydoublespinbox.h\
			../qt/mycheckbox.h\
			../qt/mygroupbox.h\
			../src/my_ui_loader.h\
			../src/preview_file_dialog.h\
			../src/ssao_worker.h\
			../qt/color_palette_widget.h\
			../src/rendered_image_widget.hpp

FORMS    += ../qt/render_window.ui

RESOURCES = ../qt/icons.qrc

UI_DIR = ../qt

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
 
QMAKE_LFLAGS_RELEASE -= -O1

QMAKE_CXXFLAGS += -msse2 -ffast-math


LIBS += -lpng
win32:LIBS += -lz

# rh: ugly absolute paths for libpng and libjpeg on my windows system

#lpng
win32:greaterThan(QT_MAJOR_VERSION, 4): INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"
#win32:LIBS += "C:/Qt/Tools/mingw48_32/lib/libpng16"
#win32:INCLUDEPATH += "C:/Qt/Tools/mingw48_32/include/libpng16"


