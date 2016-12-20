REM @echo off

Echo Mandelbulber2 Windoze Build Bot

REM # XEON x64 Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set SRC=%CD%
set BUILDTREE=%SRC%\build-win
SET tbs_arch=x64
SET vcvar_arg=x86_amd64
SET qt_version=Qt5.7.0\5.7\msvc2015_64
set PACKAGESDIR=%SRC%\mandelbulber2\msvc14-x64\packages

REM # build_script #
set QTDIR=%PACKAGESDIR%\bulbqt-msvc14-x64.5.7.0.11077
set GSLDIR=%PACKAGESDIR%\gsl-msvc14-x64.2.2.1.2677
set PNGDIR=%PACKAGESDIR%\libpng.1.6.26.1
set ZLIBDIR=%PACKAGESDIR%\zlib.1.2.8.8
set TIFFDIR=%PACKAGESDIR%\libtiff.4.0.6.2
set JPEGDIR=%PACKAGESDIR%\libjpeg.9.2.0.1
cd %BUILDTREE%

:copy_files
set BINDIR=%BUILDTREE%\Release\
echo %BINDIR%	
xcopy /B /S %SRC%\mandelbulber2\deploy\share\mandelbulber2 %BINDIR%
xcopy /I %QTDIR%\plugins\gamepads %BINDIR%gamepads
xcopy /I %QTDIR%\plugins\iconengines %BINDIR%iconengines
xcopy /I %QTDIR%\plugins\imageformats %BINDIR%imageformats
xcopy /I %QTDIR%\plugins\platforms %BINDIR%platforms
xcopy %QTDIR%\bin\Qt5Gamepad*.dll %BINDIR%
xcopy %QTDIR%\bin\Qt5Gui*.dll %BINDIR%
xcopy %QTDIR%\bin\Qt5Network*.dll %BINDIR%
xcopy %QTDIR%\bin\Qt5Svg*.dll %BINDIR%
xcopy %QTDIR%\bin\Qt5Test*.dll %BINDIR%
xcopy %QTDIR%\bin\Qt5Widgets*.dll %BINDIR%
xcopy %QTDIR%\bin\Qt5Core*.dll %BINDIR%
xcopy %QTDIR%\bin\Qt5Multimedia*.dll %BINDIR%
xcopy deps\zlib*.dll %BINDIR%
xcopy deps\libpng*.dll %BINDIR%
xcopy deps\tiff*.dll %BINDIR%
del %BINDIR%language
del %BINDIR%qt_data
del %BINDIR%data
xcopy /I %SRC%\mandelbulber2\language %BINDIR%language
xcopy /I %SRC%\mandelbulber2\qt_data %BINDIR%qt_data
xcopy /I %SRC%\mandelbulber2\data %BINDIR%data

GOTO:eof

REM --- exit ----
GOTO:eof
