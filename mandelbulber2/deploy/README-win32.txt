MANDELBULBER 2.06

project leader: Krzysztof Marczak
project members: knighty, makemeunsee, Marius Schilder, Ryan Hitchman, Jeff Epler, Martin Reinecke, Quazgaa, Sebastian Jennen, Graeme McLaren

contact: buddhi1980@gmail.com
www: http://www.mandelbulber.com
download site: http://sourceforge.net/projects/mandelbulber/
GIT: https://github.com/buddhi1980/mandelbulber2

LICENCE: GNU GPL v3.0

1. REQUIREMENTS

- Windows XP or newer
- very fast CPU (recommended 2GHz, 4 cores)
- 2GB RAM

2. INSTALLATION

Copy this folder somewhere and run Mandelbulber2.exe or run Mandelbulber v2-2.xx-Setup.exe if it is provided.

3. Mandelbulber v2 is also available for Linux

4. TESTED ON:

- Windows Vista
- Windows 7
- Windows 8
- Windows 10

5. USAGE

Please visit: http://wiki.mandelbulber.com/index.php?title=Main_Page

Actually there is only available documentation for old Mandelbulber (v1)

A lot of useful information you can find in tooltips.

6. COMMAND LINE INTERFACE
-------------------------

Usage: ./mandelbulber2 [options] settings_file  

Options:                                                                                                                                                                                                     
  -h, --help                  Displays this help.                                                                                                                                                            
  -v, --version               Displays version information.                                                                                                                                                  
  -n, --nogui                 Start program without GUI.                                                                                                                                                     
  -o, --output <N>            Save rendered image(s) to this file / folder.                                                                                                                                  
  -K, --keyframe              Render keyframe animation                                                                                                                                                             
  -F, --flight                Render flight animation                                                                                                                                                               
  -s, --start <N>             Start rendering from frame number <N>.                                                                                                                                                
  -e, --end <N>               Stop rendering on frame number <N>.                                                                                                                                                   
  -O, --override <KEY=VALUE>  Override item '<KEY>' from settings file with new                                                                                                                                     
                              value '<value>'.                                                                                                                                                                      
                              Specify multiple KEY=VALUE pairs by separating                                                                                                                                        
                              with a '#' (KEY1=VALUE1#KEY2=VALUE2). Quote whole                                                                                                                                     
                              expression to avoid whitespace parsing issues                                                                                                                                         
                              Override fractal parameter in the form                                                                                                                                                   
                              'fractal<N>_KEY=VALUE' with <N> as index of                                                                                                                                              
                              fractal                                                                                                                                                                                  
  -L, --list                  List all possible parameters '<KEY>' with                                                                                                                                                
                              corresponding default value '<VALUE>'.                                                                                                                                                   
  -f, --format <FORMAT>       Image output format:                                                                                                                                                                     
                              jpg - JPEG format                                                                                                                                                                        
                              png - PNG format                                                                                                                                                                         
                              png16 - 16-bit PNG format                                                                                                                                                                
                              png16alpha - 16-bit PNG with alpha channel format                                                                                                                                        
                              exr - EXR format                                                                                                                                                                         
  -r, --res <WIDTHxHEIGHT>    Override image resolution.                                                                                                                                                                
  --fpk <N>                   Override frames per key parameter.                                                                                                                                                        
  -S, --server                Set application as a server listening for                                                                                                                                                 
                              clients.                                                                                                                                                                                  
  -H, --host <N.N.N.N>        Set application as a client connected to server                                                                                                                                           
                              of given Host address (Host can be of type IPv4,                                                                                                                                          
                              IPv6 and Domain name address).                                                                                                                                                            
  -p, --port <N>              Set network port number for Netrender (default
                              5555).
  -C, --no-cli-color          Start program without ANSI colors, when execution
                              on CLI.
  -q, --queue                 Render all images from common queue.
  --stats                     Show statistics while renderering in CLI mode.
  --help-input                Show help about input.

Arguments:
  settings_file               file with fractal settings (program also tries
                              to find file in ./mandelbulber/settings directory)
                              When settings_file is put as a command line
                              argument then program will start in noGUI
                              mode<settings_file> can also be specified as a
                              list, see all options with --help-input