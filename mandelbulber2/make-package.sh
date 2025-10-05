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

	#clear already created folders
	rm -r "$destNameLinux"
	
	#making directories
	mkdir -vp "$destNameLinux"
	
	#copying README file
	cp -vu "$sourceDir/deploy/README" "$destNameLinux"
	
	#copying NEWS file
	cp -vu "$sourceDir/deploy/NEWS" "$destNameLinux"
	
	#copying COPYING file
	cp -vu "$sourceDir/deploy/COPYING" "$destNameLinux"
	
	#copying install scripts
	cp -vu "$sourceDir/deploy/linux/install" "$destNameLinux"
	cp -vu "$sourceDir/deploy/linux/uninstall" "$destNameLinux"
	#sed -e 's/$/\r/' "$sourceDir/deploy/win32/install.bat" > "$destNameWin/install.bat"

	#copying desktop file
	cp -vu "$sourceDir/deploy/linux/mandelbulber2.desktop" "$destNameLinux"
	
	#copying share folder
	mkdir -vp "$destNameLinux/usr/"
	cp -vurL "$sourceDir/deploy/share" "$destNameLinux/usr/"
	
	#copying fomula files
	cp -vurL "$sourceDir/formula" "$destNameLinux/usr/share/mandelbulber2/"
	
	#moving formulas source code to formula directory
	mkdir -vp "$destNameLinux/formula/"
	mv -v "$destNameLinux/usr/share/mandelbulber2/formula/definition" "$destNameLinux/formula/"
	
	#copying language files
	cp -vurL "$sourceDir/language" "$destNameLinux/usr/share/mandelbulber2/"
	
	#copying opencl files
	cp -vurL "$sourceDir/opencl" "$destNameLinux/usr/share/mandelbulber2/"
	
	#copying source files
	mkdir -vp "$destNameWin/source"
	
	cp -vurL "$sourceDir/src/" "$destNameLinux/"
	
	cp -vurL "$sourceDir/third-party/" "$destNameLinux/"
	
	cp -vurL "$sourceDir/qt/" "$destNameLinux/"
	
	cp -vurL "$sourceDir/opencl/" "$destNameLinux/"
	
	#create bin folder
	mkdir -vp "$destNameLinux/usr/bin"
	
	#copying makefiles
	mkdir -vp "$destNameLinux/makefiles"
	
	cp -vu "$sourceDir/qmake/mandelbulber.pro" "$destNameLinux/makefiles/"
	cp -vu "$sourceDir/qmake/mandelbulber-opencl.pro" "$destNameLinux/makefiles/"
	cp -vu "$sourceDir/qmake/common.pri" "$destNameLinux/makefiles/"
	cp -vu "$sourceDir/qmake/installer.pri" "$destNameLinux/makefiles/"
	
	
	#DOCFILE="$(curl -s https://api.github.com/repos/buddhi1980/mandelbulber_doc/releases | grep browser_download_url | head -n 1 | cut -d '"' -f 4)"
	
	#echo $DOCFILE
	
	#copying documentation files
	mkdir -vp "$destNameLinux/usr/share/doc/mandelbulber2"

	#wget -O "$destNameLinux/usr/share/doc/mandelbulber2/Mandelbulber_Manual.pdf" $DOCFILE
	cp -v "$sourceDir/deploy/doc/Mandelbulber_Manual.pdf" "$destNameLinux/usr/share/doc/mandelbulber2"
	
	cp -vu "$sourceDir/deploy/NEWS" "$destNameLinux/usr/share/doc/mandelbulber2"
	
	cd "$releaseDir"
	
	#delete temporary svn files
	find . -name .svn -prune -exec rm -rf "{}" \;
	
	echo release prepared to ${PWD}
else
	echo syntax: make-package.sh [number_new] [destination]
fi
