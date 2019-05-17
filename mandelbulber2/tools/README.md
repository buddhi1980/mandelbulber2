## Tool-scripts for Mandelbulber
This folder contains maintaining and packaging commands & scripts for the mandelbulber project.
This file describes how to work with these commands & scripts and when to use them.

### (1) Ongoing changes
The project should be kept updated and clean with the following steps (when necessary):
- 1. Refactoring of the codebase
  - `mandelbulber2/tools/codeStyleCheck.php # this makes a dry run, check output`
  - `mandelbulber2/tools/codeStyleCheck.php nondry # this applies changes`
- 2. Updating the fractal ui files with code information
  - `mandelbulber2/tools/populateUiInformation.php # this makes a dry run, check output`
  - `mandelbulber2/tools/populateUiInformation.php nondry # this applies changes`
- 3. Updating the opencl files with autogeneration from cpp code
  - `mandelbulber2/tools/populateOpenCL.php # this makes a dry run, check output`
  - `mandelbulber2/tools/populateOpenCL.php nondry # this applies changes`
- 4. `mandelbulber2/tools/dos2unix.sh # fix file formatting issues`
- 5. Add new language snippets, see also "(3) language system"
- 6. Add new formula images, see also "(4) formula images"

### (2) Packaging for new version
When a new version is prepared for release, the following steps should be taken:
- 1. Everything from "(1) ongoing changes"
- 2. Update README / NEWS (check for newer dependencies, supported platforms, ...)
  - `mandelbulber2/tools/updateREADMEandCo.php # update release relevant things`
- 3. Run build CI's / code coverage / any code validating software
- 4. Testing
  - Add test cases for new introduced features
  - Run `mandelbulber --test` with valgrind and check for memory leaks
  - Run `mandelbulber --benchmark` and check for performance regressions 
- 6. Tag current point in master as release

from here on only working on the tag:
- 5. package binaries for different OS's and deploy to sourceforge / github
- 6. contact package maintainer / mailing list / internal publish
- 7. let people know at: fractalforums / google plus / facebook / ...

### (3) language system
The language system requires language tools of qt.

Updating of ts files (first step before editing language entries)

`lupdate mandelbulber2/formula/ui/*.ui -ts mandelbulber2/language/formula_*.ts`

`lupdate mandelbulber2/qmake/mandelbulber-opencl-no-installer.pro`

Modify language files
`linguist mandelbulber2/language/[language to edit].ts`

Generation of binary files used by the program
`lrelease mandelbulber2/language/[language to generate].ts`

### (4) formula images
Each fractal has a big dropdown with all available formulas.
Each entry has a small thumbnail image, to give a hint, what the formula does.

steps to take for adding new thumbnails:
- 1. For each new formula create a "descent" new image, which shows the most striking property of the new formula
  When the formula is a transform, it should base on the menger sponge to be comparable with other transforms
- 2. Save the image as 256x256 png16alpha to the folder mandelbulber2/formula/img
  the filename should equal the formula name
- 3. Test if application shows the new thumbnail in the formula dropdown
- 4. Resave the image with gimp (this reduces the filesize)
- 5. Add the image to git

### (5) new program icons
The used icons are part of the tango theme.
In order to add new icons those need to be shipped for non linux systems

steps to take:
- 1. Get the tango icon pack from here: http://tango.freedesktop.org/Tango_Icon_Library
- 2. Get the desired icon from scalable folder and add it to mandelbulber2/qt/icons
- 3. Add reference analog to other system icons in mandelbulber2/qt/icons.qrc
- 4. Use icon analogue to other icons in ui file
- 5. Add svg file to git

### (6) optimization of images ###
Optimize jpg images and compress to 80%
`find . -name '*.jp*g' -print0 | xargs -0 jpegoptim -ftn -m80`

Optimize png images
`find . -name '*.png' -print0 | xargs -0 optipng -o7`

### (7) new libraries ###
- add the library to ... and check compilation
  - mandelbulber/qmake/common.pri
  - mandelbulber/cmake/CMakeLists.txt
  - mandelbulber/msvc/mandelbulber2.sln
- add information to mandelbulber2/deploy/README
- add dependencies to
  - mandelbulber2/tools/prepare_for_dev_debian_testing.sh
  - mandelbulber2/tools/prepare_for_dev_ubuntu.sh
  - mandelbulber2/deploy/setup-cross-compile-to-windows.sh
- add to package
  - mandelbulber2/deploy/cross-compile-windows.sh
- add to ci and check compilation
  - .appveyor.yml
  - .travis.yml
