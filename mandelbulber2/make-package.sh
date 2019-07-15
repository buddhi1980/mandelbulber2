#!/bin/bash
#
# bash script to create destination structure of files/directories to be ready for release new version
# arguments: 
# 1: version number
# 2: destination directory
# example: ./make-package.sh 2.05 /home/me/ReleasedMandelbulbers

if [ $# = 2 ]
then
	#source dir as current directory
	sourceDir=${PWD}
	releaseDir="$2"

	if [ ! -f "$sourceDir/make-package.sh" ]; then
		echo "this script has to be executed in mandelbulber root directory";
		exit 2;
	fi

	# create release dir, if not already exists
	mkdir -vp "$releaseDir"
	if [ ! -d "$releaseDir" ]; then
		echo "could not create releaseDir: $releaseDir";
		exit 3;
	fi
	cd "$releaseDir"
	
	#names of destination folders
	destNameLinux="mandelbulber2-"$1
	destNameWin="mandelbulber2-win32-"$1
	destNameWin64="mandelbulber2-win64-"$1

	#clear already created folders
	rm -r "$destNameLinux"
	rm -r "$destNameWin"
	rm -r "$destNameWin64"
	
	#making directories
	mkdir -vp "$destNameLinux"
	mkdir -vp "$destNameWin"
	mkdir -vp "$destNameWin64"
	
	#copying README file
	cp -vu "$sourceDir/deploy/README" "$destNameLinux"
	sed -e 's/$/\r/' "$sourceDir/deploy/README-win32.txt" > "$destNameWin/README.txt"
	sed -e 's/$/\r/' "$sourceDir/deploy/README-win32.txt" > "$destNameWin64/README.txt"
	
	#copying NEWS file
	cp -vu "$sourceDir/deploy/NEWS" "$destNameLinux"
	sed -e 's/$/\r/' "$sourceDir/deploy/NEWS" > "$destNameWin/NEWS.txt"
	sed -e 's/$/\r/' "$sourceDir/deploy/NEWS" > "$destNameWin64/NEWS.txt"
	
	#copying COPYING file
	cp -vu "$sourceDir/deploy/COPYING" "$destNameLinux"
	sed -e 's/$/\r/' "$sourceDir/deploy/COPYING" > "$destNameWin/COPYING.txt"
	sed -e 's/$/\r/' "$sourceDir/deploy/COPYING" > "$destNameWin64/COPYING.txt"
	
	#copying install scripts
	cp -vu "$sourceDir/deploy/linux/install" "$destNameLinux"
	cp -vu "$sourceDir/deploy/linux/uninstall" "$destNameLinux"
	#sed -e 's/$/\r/' "$sourceDir/deploy/win32/install.bat" > "$destNameWin/install.bat"

	#copying desktop file
	cp -vu "$sourceDir/deploy/linux/mandelbulber2.desktop" "$destNameLinux"
	cp -vu "$sourceDir/deploy/win32/mandelbulber2.ico" "$destNameWin"
	cp -vu "$sourceDir/deploy/win64/mandelbulber2.ico" "$destNameWin64"
	
	#copying share folder
	mkdir -vp "$destNameLinux/usr/"
	cp -vurL "$sourceDir/deploy/share" "$destNameLinux/usr/"
	cp -vurL "$sourceDir/deploy/share/mandelbulber2/"* "$destNameWin/"
	cp -vurL "$sourceDir/deploy/share/mandelbulber2/"* "$destNameWin64/"
	
	#copying fomula files
	cp -vurL "$sourceDir/formula" "$destNameLinux/usr/share/mandelbulber2/"
	cp -vurL "$sourceDir/formula"* "$destNameWin/"
	cp -vurL "$sourceDir/formula"* "$destNameWin64/"
	
	#copying language files
	cp -vurL "$sourceDir/language" "$destNameLinux/usr/share/mandelbulber2/"
	cp -vurL "$sourceDir/language"* "$destNameWin/"
	cp -vurL "$sourceDir/language"* "$destNameWin64/"
	
	#copying opencl files
	cp -vurL "$sourceDir/opencl" "$destNameLinux/usr/share/mandelbulber2/"
	cp -vurL "$sourceDir/opencl"* "$destNameWin/"
	cp -vurL "$sourceDir/opencl"* "$destNameWin64/"
	
	#copying source files
	mkdir -vp "$destNameWin/source"
	
	cp -vurL "$sourceDir/src/" "$destNameLinux/"
	cp -vurL "$sourceDir/src/" "$destNameWin/source"
	cp -vurL "$sourceDir/src/" "$destNameWin64/source"
	
	cp -vurL "$sourceDir/qt/" "$destNameLinux/"
	cp -vurL "$sourceDir/qt/" "$destNameWin/source"
	cp -vurL "$sourceDir/qt/" "$destNameWin64/source"
	
  cp -vurL "$sourceDir/opencl/" "$destNameLinux/"
	cp -vurL "$sourceDir/opencl/" "$destNameWin/source"
	cp -vurL "$sourceDir/opencl/" "$destNameWin64/source"
	
	#create bin folder
	mkdir -vp "$destNameLinux/usr/bin"
	
	#copying makefiles
	mkdir -vp "$destNameLinux/makefiles"
	mkdir -vp "$destNameWin/source/makefiles"
	mkdir -vp "$destNameWin64/source/makefiles"
	
	cp -vu "$sourceDir/qmake/mandelbulber.pro" "$destNameLinux/makefiles/"
	cp -vu "$sourceDir/qmake/mandelbulber-opencl.pro" "$destNameLinux/makefiles/"
	cp -vu "$sourceDir/qmake/common.pri" "$destNameLinux/makefiles/"
	cp -vu "$sourceDir/qmake/installer.pri" "$destNameLinux/makefiles/"
	
	cp -vu "$sourceDir/qmake/mandelbulber.pro" "$destNameWin/source/makefiles/"
	cp -vu "$sourceDir/qmake/mandelbulber-opencl.pro" "$destNameWin/source/makefiles/"
	cp -vu "$sourceDir/qmake/common.pri" "$destNameWin/source/makefiles/"
	cp -vu "$sourceDir/qmake/mandelbulber.pro" "$destNameWin64/source/makefiles/"
	cp -vu "$sourceDir/qmake/mandelbulber-opencl.pro" "$destNameWin64/source/makefiles/"
	cp -vu "$sourceDir/qmake/common.pri" "$destNameWin64/source/makefiles/"

	#copying DLL files
	cp -vur "$sourceDir/deploy/win32/dll/"* "$destNameWin"
	cp -vur "$sourceDir/deploy/win64/dll/"* "$destNameWin64"
	
	#copying windows exe files (needed to be compiled before)
	cp -vu "$sourceDir/build-mandelbulber-MinGwQt-Release/release/mandelbulber2.exe" "$destNameWin"
	cp -vu "$sourceDir/build-mandelbulber-MinGw64Qt-Release/release/mandelbulber2.exe" "$destNameWin64"
	
	DOCFILE="$(curl -s https://api.github.com/repos/buddhi1980/mandelbulber_doc/releases | grep browser_download_url | head -n 1 | cut -d '"' -f 4)"
	
	echo $DOCFILE
	
	#copying documentation files
	mkdir -vp "$destNameLinux/usr/share/doc/mandelbulber2"
	mkdir -vp "$destNameWin/doc"
	mkdir -vp "$destNameWin64/doc"
	wget -O "$destNameLinux/usr/share/doc/mandelbulber2/Mandelbulber_Manual.pdf" $DOCFILE
	cp -v "$destNameLinux/usr/share/doc/mandelbulber2/Mandelbulber_Manual.pdf" "$destNameWin/doc/"
	cp -v "$destNameLinux/usr/share/doc/mandelbulber2/Mandelbulber_Manual.pdf" "$destNameWin64/doc/"

	cp -vu "$sourceDir/deploy/NEWS" "$destNameLinux/usr/share/doc/mandelbulber2"
	cp -vu "$sourceDir/deploy/NEWS" "$destNameWin/doc/"
	cp -vu "$sourceDir/deploy/NEWS" "$destNameWin64/doc/"
	
	cd "$releaseDir"
	
	#delete temporary svn files
	find . -name .svn -prune -exec rm -rf "{}" \;
	
	echo release prepared to ${PWD}
else
	echo syntax: make-package.sh [number_new] [destination]
fi
