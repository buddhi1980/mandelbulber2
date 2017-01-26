![Banner](https://raw.githubusercontent.com/buddhi1980/mandelbulber2/wiki/assets/images/mandelbulberBanner.png)

|Coverity Scan|Build Status|Gitter Chat|
|:-:|:-:|:-:|
|[![Coverity Scan](https://scan.coverity.com/projects/4723/badge.svg?flat=1)](https://scan.coverity.com/projects/mandelbulber-v2)|Linux / OSX: [![Build Status Linux / OSX](https://travis-ci.org/buddhi1980/mandelbulber2.svg)](https://travis-ci.org/buddhi1980/mandelbulber2), Windows: [![Build Status Windows](https://ci.appveyor.com/api/projects/status/urd2h30tu7reg4mp?svg=true)](https://ci.appveyor.com/project/buddhi1980/mandelbulber2)|[![Join the chat at https://gitter.im/buddhi1980/mandelbulber2](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/buddhi1980/mandelbulber2?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)|



## Universal Idea

In summary, Mandelbulber generates 3-dimensional fractals.

Explore trigonometric, hyper-complex, Mandelbox, IFS, and many other 3D fractals.

Render with a great palette of customizable materials to create stunning images and videos.

The possibilities are literally **infinite**!

![Test Render](https://raw.githubusercontent.com/buddhi1980/mandelbulber2/wiki/assets/images/mandelbulberTestrender.jpg)

## Features

- Renders trigonometric, hyper-complex, Mandelbox, IFS, and many other 3D fractals
- Complex 3D ray-traced shading: hard shadows, ambient occlusion, depth of field, translucency & refraction, etc.
- Rich GUI in Qt 5 environment
- Unlimited image resolution on 64-bit systems
- Program compiled for x86 and x64 CPUs (Linux, Windows, OSX)
- Simple 3D navigator
- Distributed Network Rendering
- Key-frame animation of all parameters with different interpolations
- Material management
- Texture mapping (color, luminosity, diffusion, normal maps, displacement)
- Rendering queue
- Command line interface for headless systems


![image](https://cloud.githubusercontent.com/assets/11696990/13788910/173cf11a-eae2-11e5-884e-f1d03924a5f3.png)
![image](https://cloud.githubusercontent.com/assets/11696990/16526853/a708e7e2-3fb3-11e6-8136-323bda493604.png)

## Keyboard shortcuts

In render window:

  - <kbd>Up</kbd> / <kbd>Down</kbd>: move camera forward / backward
  - <kbd>Left</kbd> / <kbd>Right</kbd>: move camera left / right
  - <kbd>Ctrl</kbd>+(<kbd>Up</kbd> / <kbd>Down</kbd>): move camera up / down
  - <kbd>Shift</kbd>+(<kbd>Up</kbd> / <kbd>Down</kbd> / <kbd>Left</kbd> / <kbd>Right</kbd>): rotate camera
  - <kbd>Ctrl</kbd>+(<kbd>Left</kbd> / <kbd>Right</kbd>): roll camera left / right

## Building and Deploying

Dowload latest stable version from [Releases] (https://github.com/buddhi1980/mandelbulber2/releases)

Unpack dowloaded file and follow instructions from README file
Please see information in [mandelbulber2/deploy](mandelbulber2/deploy) folder.

## Easy Preparation for Development

Thre program is natively developed using Qt Creator under Linux (Debian or Ubuntu)

The file [mandelbulber2/Release/mandelbulber.pro] (https://github.com/buddhi1980/mandelbulber2/blob/master/mandelbulber2/Release/mandelbulber.pro) provides all needed configuration for Qt Creator. But to be able to compile the program, the system has to be prepared.

Use the following scripts to prepare your Linux environment for development.
These scripts install all required packages, compile the program, and create symbolic links in /usr/share/mandelbulber to your working directory.

[Prepare Debian for Development] (https://github.com/buddhi1980/mandelbulber2/blob/master/mandelbulber2/tools/prepare_for_dev_debian_testing.sh)

[Prepare Ubuntu for Development] (https://github.com/buddhi1980/mandelbulber2/blob/master/mandelbulber2/tools/prepare_for_dev_ubuntu.sh)

### Arch Linux

#### AUR

[Arch Linux AUR Package (builds latest release)]
(https://aur.archlinux.org/packages/mandelbulber2/)

[Arch Linux AUR Package (builds latest git snapshot)]
(https://aur.archlinux.org/packages/mandelbulber2-git/)

#### Binary packages

Add `http://jws.forteholding.ru/archi` to pacman repositories

* /etc/pacman.conf:
```INI
[archi]
Server = http://jws.forteholding.ru/archi
SigLevel = Never
```

* Update index and install mandelbulber2:
```ShellSession
pacman -Syy
pacman -S mandelbulber2
```

### VC++ 2015

In addition there is prepared solution for VC++ 2015 located in root folder of git package. This solution includes all needed nugets to compile the program.

## Resources

[Image Gallery] (http://krzysztofmarczak.deviantart.com/gallery/)

[Forum] (http://www.fractalforums.com/mandelbulber/)
[Forum Gallery] (http://www.fractalforums.com/index.php?action=gallery;cat=51)

[Compiled Binaries](http://sourceforge.net/projects/mandelbulber/)

[Coverity Scan](http://scan.coverity.com/projects/4723?tab=overview)

## License

GNU GPL v3
