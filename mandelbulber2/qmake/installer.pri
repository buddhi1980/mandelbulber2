#installer
unix {
    isEmpty(PREFIX) {
        PREFIX = /usr
    }

	!isEmpty(SHARED_PATH) {
		DEFINES+=CUSTOM_PREFIX=\"$${SHARED_PATH}\"
	}
	
    target.path = $$PREFIX/bin

	sharefiles.files = $$ROOT/deploy/share/mandelbulber2
	sharefiles.path = $$PREFIX/share
	INSTALLS += sharefiles
	
	formulaui.files = $$ROOT/formula/ui
	formulaui.path = $$PREFIX/share/mandelbulber2/formula
	INSTALLS += formulaui
	
	formulaimg.files = $$ROOT/formula/img
	formulaimg.path = $$PREFIX/share/mandelbulber2/formula
	INSTALLS += formulaimg
	
	formulaopencl.files = $$ROOT/formula/opencl
	formulaopencl.path = $$PREFIX/share/mandelbulber2/formula
	INSTALLS += formulaopencl
	
	openclfiles.files = $$ROOT/opencl
	openclfiles.path = $$PREFIX/share/mandelbulber2
	INSTALLS += openclfiles
	
	documentation.files = $$ROOT/deploy/doc/Mandelbulber_Manual.pdf
	documentation.files += $$ROOT/deploy/README
	documentation.files += $$ROOT/deploy/NEWS
	documentation.files += $$ROOT/deploy/COPYING
	documentation.path = $$PREFIX/share/doc/mandelbulber2
	INSTALLS += documentation
	
	desktopfile.files = $$ROOT/deploy/linux/mandelbulber2.desktop
	desktopfile.path = $$PREFIX/share/applications
	INSTALLS += desktopfile
	
	iconfile.files = $$ROOT/deploy/share/mandelbulber2/icons/mandelbulber.png
	iconfile.path = $$PREFIX/share/icons/hicolor/256x256/apps
	INSTALLS += iconfile
}

INSTALLS += target

#example syntax for cutom PREFIX
#qmake SHARED_PATH="/usr/local" PREFIX=/usr/local mandelbulber-opencl.pro

#example for standalone application like appimage
#program will use relative path for shared files
#qmake SHARED_PATH="usr" PREFIX=/tmp/appdir/usr mandelbulber-opencl.pro
