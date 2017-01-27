REM @echo off

Echo Mandelbulber2 Windoze Build Bot


set INCREMENTDISABLE=true

REM # XEON x64 Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set SRC=%CD%
set BUILDTREE=%SRC%\build-win
SET tbs_arch=x64
SET vcvar_arg=x86_amd64
SET qt_version=Qt5.7.0\5.7\msvc2015_64
SET ms_build_suffix=Bin\amd64
SET cmake_platform="Visual Studio 14 2015 Win64"
set QTDIR=C:\Qt\%qt_version%
set PATH=%QTDIR%\bin;C:\Program Files (x86)\MSBuild\14.0\%ms_build_suffix%;%SRC%;%PATH%

REM # VC Vars #
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" %vcvar_arg%
@echo on

REM # Clean Build Tree #
if defined INCREMENTDISABLE ( 
	echo "Incremental Build disabled"
    rd /s /q %BUILDTREE%
) else (
	echo "Incremental Build enabled"
)
mkdir %BUILDTREE%
mkdir %BUILDTREE%\deps

:nuget_Dep
REM # packages from nuget #
cd %BUILDTREE%\deps
nuget install gsl-msvc14-%tbs_arch% -Version 2.2.1.2577
nuget install zlib-msvc14-%tbs_arch%-master -Version 1.2.8.4 -Source https://ci.appveyor.com/nuget/zlib-shallow-x0n34b3pfq1b
powershell -Command move %BUILDTREE%\..\packages\zlib-msvc14-x64-master*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\zlibstatic.lib %BUILDTREE%\deps\zlib.lib
nuget install libpng-msvc14-%tbs_arch%-master -Version 1.6.18.44 -Source https://ci.appveyor.com/nuget/libpng-7hwq4pmmrc48
powershell -Command move %BUILDTREE%\..\packages\libpng-msvc14-x64-master*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\libpng16_static.lib %BUILDTREE%\deps\png.lib
nuget install libtiff-msvc14-%tbs_arch%-master -Version 4.0.6.85 -Source https://ci.appveyor.com/nuget/libtiff-i3h8tqqy7o7b
powershell -Command move %BUILDTREE%\..\packages\libtiff-msvc14-x64-master*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\tiff_static.lib %BUILDTREE%\deps\tiff.lib
nuget install libjpeg-msvc14-%tbs_arch%-master -Version 1.4.80.21 -Source https://ci.appveyor.com/nuget/libjpegturbo-o6k4js4y7pjw
powershell -Command move %BUILDTREE%\..\packages\libjpeg-msvc14-x64-master*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\jpeg-static.lib %BUILDTREE%\deps\jpeg.lib

REM # build_script #
set GSLDIR=%BUILDTREE%\..\packages\gsl-msvc14-x64.2.2.1.2577
cd %BUILDTREE%
cmake -G %cmake_platform% ^
-DPNG_LIBRARY=%BUILDTREE%\deps\png.lib -DPNG_PNG_INCLUDE_DIR=%BUILDTREE%\deps ^
-DZLIB_LIBRARY=%BUILDTREE%\deps\zlib.lib -DZLIB_INCLUDE_DIR=%BUILDTREE%\deps ^
-DTIFF_LIBRARY=%BUILDTREE%\deps\tiff.lib -DTIFF_INCLUDE_DIR=%BUILDTREE%\deps ^
-DJPEG_LIBRARY=%BUILDTREE%\deps\jpeg.lib -DJPEG_INCLUDE_DIR=%BUILDTREE%\deps ^
-DGSL_CBLAS_LIBRARY=%GSLDIR%\static\gslcblas.lib ^
-DGSL_LIBRARY=%GSLDIR%\static\gsl.lib ^
-DGSL_INCLUDE_DIR=%GSLDIR% ^
-DUSE_GAMEPAD=1 ^
-DCMAKE_BUILD_TYPE=Release %SRC%\mandelbulber2\cmake\

msbuild mandelbulber2.sln /p:Configuration=Release /m

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
