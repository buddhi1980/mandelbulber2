//Download and install the latest version of Xcode (and Developer Tools) from the Apple developer website or get it using the Mac App Store.
//Download and install the latest version of Qt from http://www.qt.io/download-open-source/
//download source from http://sourceforge.net/projects/libpng/files/libpng16/
//Build the PNG library
cd libpng-1.6.x
./configure --prefix=/usr/local/
make
make test
//Install the PNG library under /usr/local
sudo make install
//download source from ftp://ftp.gnu.org/gnu/gsl/
//Build GNU scientific library
cd gsl-latest
./configure --prefix=/usr/local/ 
make
//Install the GSL library under /usr/local
sudo make install
//Build TIFF library
curl --remote-name http://download.osgeo.org/libtiff/tiff-4.0.6.tar.gz
tar -xzvf tiff-4.0.6.tar.gz
cd tiff-4.0.6
./configure --prefix=/usr/local/libtiff-4.0.6
make
//Install the TIFF library under /usr/local
make install
ln -s libtiff-4.0.6 /usr/local/libtiff
//download source from https://github.com/buddhi1980/mandelbulber2
//Build mandelbulber
cd Release
qmake -spec macx-g++
//xcode can also be used
//qmake -spec macx-xcode mandelbulber.pro
qmake -project
qmake -o makefile mandelbulber.pro
make
//copy mandelbulber's settings files
//download the share folder from https://github.com/buddhi1980/mandelbulber2/tree/master/mandelbulber2/deploy
//copy the contents of the folder inside "/usr/share/mandelbulber2" 
