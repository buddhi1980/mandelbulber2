
MANDELBULBER 2.10
-----------------

project leader: Krzysztof Marczak
project members: Sebastian Jennen, Graeme McLaren, Bernardo Martelli, Robert Pancoast
                 knighty, makemeunsee, Marius Schilder, Ryan Hitchman, Jeff Epler, Martin Reinecke, Quazgaa

contact: buddhi1980@gmail.com
www: http://www.mandelbulber.com
download site: http://sourceforge.net/projects/mandelbulber/
GIT: https://github.com/buddhi1980/mandelbulber2
Community: http://www.fractalforums.com/mandelbulber

LICENCE: GNU GPL v3.0

1. REQUIREMENTS
---------------

- unknown MacOS version
- very fast CPU (recommended 2GHz, 4 cores)
- 2GB RAM

2. COMPILING
------------

/Download and install the latest version of Xcode (and Developer Tools) from the Apple developer website or get it using the Mac App Store.
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

3. INSTALLATION
---------------

see 2.

4. TESTED ON:
------------

- unknown

5. USAGE
--------

Please visit: http://wiki.mandelbulber.com/index.php?title=Main_Page

Actually there is only available documentation for old Mandelbulber (v1)

A lot of useful information you can find in tooltips.

6. COMMAND LINE INTERFACE
-------------------------

Usage: ./mandelbulber2 [options] settings_file

Options:
  -h, --help             Displays this help.
  -v, --version          Displays version information.
  -n, --nogui            Starts the program without a GUI.
  -o, --output <N>       Saves rendered image(s) to this file / folder.
  -K, --keyframe         Renders keyframe animation.
  -F, --flight           Renders flight animation.
  -X, --never-delete     Never delete data, instead Exit CLI application.
  -s, --start <N>        Starts rendering from frame number <N>.
  -e, --end <N>          Stops rendering on frame number <N>.
  -L, --list             Lists all possible parameters '<KEY>' with
                         corresponding default value '<VALUE>'.
  -f, --format <FORMAT>  Image output format:
                          jpg - JPEG format (default)
                          png - PNG format
                          png16 - 16-bit PNG format
                          png16alpha - 16-bit PNG with alpha channel format
                          exr - EXR format
                          tiff - TIFF format
  -r, --res <WxH>        Overrides image resolution. Specify as width and
                         height separated by 'x'
  --fpk <N>              Overrides frames per key parameter.
  -S, --server           Sets application as a server listening for clients.
  -H, --host <N.N.N.N>   Sets application as a client connected to server of
                         given host address (Host can be of type IPv4, IPv6 and
                         Domain name address).
  -p, --port <N>         Sets network port number for netrender (default 5555).
  -C, --no-cli-color     Starts program without ANSI colors, when execution on
                         CLI.
  -q, --queue            Renders all images from common queue.
  -t, --test             Runs testcases on the mandelbulber instance
  -b, --benchmark        Runs benchmarks on the mandelbulber instance
  -T, --touch            Resaves a settings file (can be used to update a
                         settings file)
  -V, --voxel            Renders the voxel volume in a stack of images.
  -O, --override <...>   <KEY=VALUE> overrides item '<KEY>' from settings file
                         with new value '<VALUE>'.
                         Specify multiple KEY=VALUE pairs by separating with a
                         '#': <KEY1=VALUE1#KEY2=VALUE2>. Quote whole expression
                         to avoid whitespace parsing issues
                         Override fractal parameter in the form
                         'fractal<N>_KEY=VALUE' with <N> being index of fractal
  --stats                Shows statistics while rendering in CLI mode.
  --help-input           Shows help about input.
  --help-examples        Shows example commands.

Arguments:
  settings_file          file with fractal settings (program also tries
                         to find file in ./mandelbulber/settings directory)
                         When settings_file is put as a command line argument
                         then program will start in noGUI mode<settings_file>
                         can also be specified as a list, see all options with
                         --help-input
