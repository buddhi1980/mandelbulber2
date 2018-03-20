
                                 MANDELBULBER 2.13

1. What is Mandelbulber
-----------------------

In summary, Mandelbulber generates three-dimensional fractals.
Explore  trigonometric, hyper-complex, Mandelbox, IFS, Kleinian  and many other
3D fractals.  Render with a great palette  of customizable materials  to create
stunning images and videos. The possibilities are literally   i n f i n i t e !

2. Project Members and Links
----------------------------

PROJECT LEADER:  Krzysztof Marczak
PROJECT MEMBERS: Sebastian Jennen, Graeme McLaren, Bernardo Martelli,
                 Robert Pancoast, knighty, makemeunsee, Marius Schilder,
                 Ryan Hitchman, Jeff Epler, Martin Reinecke, Quazgaa

CONTACT:         buddhi1980@gmail.com
DOWNLOAD:        https://sourceforge.net/projects/mandelbulber/
DEVELOPMENT:     https://github.com/buddhi1980/mandelbulber2/
COMMUNITY:       http://www.fractalforums.com/mandelbulber/
                 https://www.facebook.com/groups/mandelbulber/
FANSITE:         http://www.mandelbulber.com
                 http://www.mandelbulber.org

LICENCE:         GNU GPL v3.0

3. REQUIREMENTS
---------------

- Windows XP or newer
- very fast CPU (recommended 2GHz, 4 cores)
- 2GB RAM

4. INSTALLATION
---------------

Copy this folder somewhere and run Mandelbulber2.exe
or run Mandelbulber v2-2.xx-Setup.exe if it is provided.

3. Mandelbulber v2 is also available for Linux

5. TESTED ON
------------

- Windows 7 32-bit
- Windows 8 64-bit
- Windows 10 64-bit

- msvc16
- Qt 5.7.1

6. USAGE
--------

The main documentation is the PDF file  Mandelbulber_Manual.pdf,  which you can
find in this very folder.
Also you can open it from Mandelbulber under: [Menu > Help > User Manual]
A lot of useful information you can find  in the tooltips.  Just hover with the
mouse pointer over the edit fields.
More useful material can be found in the wiki documentation for old
Mandelbulber (v1) here: http://wiki.mandelbulber.com/index.php?title=Main_Page


7. COMMAND LINE INTERFACE
-------------------------

The program can be used "headless" from the console. This way the program can
be integrated in scripts or toolchains and even be run without a x-server
completely.

Usage: ./mandelbulber2 [options] settings_file

Options:
  -h, --help             Displays this help.
  -v, --version          Displays version information.
  -n, --nogui            Starts the program without a GUI.
  -o, --output <N>       Saves rendered image(s) to this file / folder.
  --logfilepath <N>      Specify custom system log filepath (default is:
                         ~/.mandelbulber_log.txt).
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
  -b, --benchmark        Runs benchmarks on the mandelbulber instance, specify
                         optional parameter difficulty (1 -> very easy, > 20 ->
                         very hard, 10 -> default). When [output] option is set
                         to a folder, the example-test images will be stored
                         there.
  -T, --touch            Resaves a settings file (can be used to update a
                         settings file)
  -V, --voxel <FORMAT>   Renders the voxel volume. Output formats are:
                          slice - stack of PNG images into one folder (default)
                          ply - Polygon File Format (single 3d file)

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
  --help-opencl          Shows commands regarding OpenCL.

Arguments:
  settings_file          file with fractal settings (program also tries
                         to find file in ./mandelbulber/settings directory)
                         When settings_file is put as a command line argument
                         then program will start in noGUI mode<settings_file>
                         can also be specified as a list, see all options with
                         --help-input
