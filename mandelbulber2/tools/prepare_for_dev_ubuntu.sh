#!/bin/sh
#

echo "Warning! This script pulls, compiles and installs development version of mandelbulber2
THERE IS NO WARRANTY THAT THE PROGRAM WILL WORK PROPERLY,
because the code can contain partially developed functions

This script will ask about root password to perform installation of system packages and the program

To update the program do not remove already downloaded files. This script will pull only the latest changes."

export CXXFLAGS="-msse2"
MANDELBULBER_SHARE="/usr/share/mandelbulber2"

set -e # if any of the commands fail the script will exit immediately

echo "
installing all needed packages

"

# Qt6 packages: qt6-base-dev provides Core/Gui/Widgets/Network/Test/Concurrent/OpenGLWidgets,
# qt6-tools-dev provides UiTools, qt6-declarative-dev provides Qml,
# qmake6 is the Qt6 qmake binary
sudo apt-get install build-essential qmake6 qt6-base-dev qt6-base-dev-tools libqt6gui6t64 libqt6widgets6t64 \
	qt6-tools-dev qt6-multimedia-dev qt6-declarative-dev \
	libpng16-16t64 libpng-dev libgomp1 libgsl-dev libsndfile1-dev liblzo2-2 liblzo2-dev
sudo apt-get install qtcreator git
# OpenCL headers and ICD loader development files are required to build the
# mandelbulber-opencl.pro target
sudo apt-get install opencl-headers ocl-icd-libopencl1 ocl-icd-opencl-dev

# optional format support (TIFF, EXR, JPEG XL); the build enables each
# automatically when pkg-config finds the library
sudo apt-get install libtiff-dev libopenexr-dev libjxl-dev

echo "
getting fresh source files from git repository

"

if [ ! -d mandelbulber2 ]; then
	git clone https://github.com/buddhi1980/mandelbulber2
	cd mandelbulber2/mandelbulber2
else
	cd mandelbulber2
	git pull
	cd mandelbulber2
fi

echo "
compiling the program

"

cd qmake
qmake6 mandelbulber-opencl.pro
make -j4

echo "
installing the program

"

sudo install mandelbulber2 /usr/bin

echo "
creating links to files from formula and deploy 
folders in /usr/share/mandelbulber2 directory
if you change anything in that folder you will
not need to reinstall the program
You have to remember to not delete mandelbulber2 folder located here
"

sudo rm -f -r $MANDELBULBER_SHARE
sudo mkdir $MANDELBULBER_SHARE
cd ..
sudo ln -s ${PWD}/formula $MANDELBULBER_SHARE/formula
sudo ln -s ${PWD}/deploy/share/mandelbulber2/data $MANDELBULBER_SHARE/data
sudo ln -s ${PWD}/language $MANDELBULBER_SHARE/language
sudo ln -s ${PWD}/deploy/share/mandelbulber2/materials $MANDELBULBER_SHARE/materials
sudo ln -s ${PWD}/deploy/share/mandelbulber2/examples $MANDELBULBER_SHARE/examples
sudo ln -s ${PWD}/deploy/share/mandelbulber2/icons $MANDELBULBER_SHARE/icons
sudo ln -s ${PWD}/deploy/share/mandelbulber2/textures $MANDELBULBER_SHARE/textures
sudo ln -s ${PWD}/deploy/share/mandelbulber2/toolbar $MANDELBULBER_SHARE/toolbar
sudo ln -s ${PWD}/deploy/doc $MANDELBULBER_SHARE/doc
sudo ln -s ${PWD}/deploy/share/mandelbulber2/sounds $MANDELBULBER_SHARE/sounds
sudo ln -s ${PWD}/opencl $MANDELBULBER_SHARE/opencl

echo "To start development of Mandelbulber you can open
mandelbulber2/mandelbulber2/qmake/mandelbulber-opencl.pro
with QtCreator which is already in your system"

echo "To run the program you need to launch folowing file:
mandelbulber2/mandelbulber2/qmake/mandelbulber2"



