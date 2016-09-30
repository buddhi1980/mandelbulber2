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
REM '"C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /%tbs_arch%'
@echo on

REM # Clean Build Tree #
if defined INCREMENTDISABLE ( 
	echo "Incremental Build disabled"
    rd /s /q %BUILDTREE%
) else (
	echo "Incremental Build enabled"
)
mkdir %BUILDTREE%
cd %BUILDTREE%\deps

REM # GSL Dependencies #
if exist %BUILDTREE%\deps\gsl\Release\gsl.lib (
    if exist %BUILDTREE%\deps\gsl\Release\gslcblas.lib (
		ECHO GSL Libs Found
		GOTO:nuget_Dep
	)
)
ECHO %cmake_platform%
mkdir %BUILDTREE%\deps
cd %BUILDTREE%\deps
git clone git://github.com/ampl/gsl.git
cd %BUILDTREE%\deps\gsl
cmake -G %cmake_platform% ^
-DCMAKE_CXX_FLAGS_RELEASE="/MD" ^
-DCMAKE_CXX_FLAGS_DEBUG="/MDd" ^
-DCMAKE_C_FLAGS_RELEASE="/MD" ^
-DCMAKE_C_FLAGS_DEBUG="/MDd" ^
-DCMAKE_BUILD_TYPE="Release" .
msbuild gsl.sln /p:Configuration=Release /m

:nuget_Dep
REM # packages from nuget #
cd %BUILDTREE%\deps

nuget install zlib-msvc14-%tbs_arch%-master -Version 1.2.8.4 -Source https://ci.appveyor.com/nuget/zlib-shallow-x0n34b3pfq1b
powershell -Command move zlib*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\zlibstatic.lib %BUILDTREE%\deps\zlib.lib
nuget install libpng-msvc14-%tbs_arch%-master -Version 1.6.18.44 -Source https://ci.appveyor.com/nuget/libpng-7hwq4pmmrc48
powershell -Command move libpng*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\libpng16_static.lib %BUILDTREE%\deps\png.lib
nuget install libtiff-msvc14-%tbs_arch%-master -Version 4.0.6.85 -Source https://ci.appveyor.com/nuget/libtiff-i3h8tqqy7o7b
powershell -Command move libtiff*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\tiff_static.lib %BUILDTREE%\deps\tiff.lib
nuget install libjpeg-msvc14-%tbs_arch%-master -Version 1.4.80.21 -Source https://ci.appveyor.com/nuget/libjpegturbo-o6k4js4y7pjw
powershell -Command move libjpeg*\* %BUILDTREE%\deps -force
move %BUILDTREE%\deps\jpeg-static.lib %BUILDTREE%\deps\jpeg.lib

REM # build_script #
cd %BUILDTREE%
SET INCLUDE=%INCLUDE%;C:\projects\mandelbulber2\gsl
SET LIB=%LIB%;C:\projects\mandelbulber2\gsl\Release
cmake -G %cmake_platform% ^
-DPNG_LIBRARY=%BUILDTREE%\deps\png.lib -DPNG_PNG_INCLUDE_DIR=%BUILDTREE%\deps ^
-DZLIB_LIBRARY=%BUILDTREE%\deps\zlib.lib -DZLIB_INCLUDE_DIR=%BUILDTREE%\deps ^
-DTIFF_LIBRARY=%BUILDTREE%\deps\tiff.lib -DTIFF_INCLUDE_DIR=%BUILDTREE%\deps ^
-DJPEG_LIBRARY=%BUILDTREE%\deps\jpeg.lib -DJPEG_INCLUDE_DIR=%BUILDTREE%\deps ^
-DGSL_CBLAS_LIBRARY=%BUILDTREE%\deps\gsl\Release\gslcblas.lib ^
-DGSL_LIBRARY=%BUILDTREE%\deps\gsl\Release\gsl.lib ^
-DGSL_INCLUDE_DIR=%BUILDTREE%\deps\gsl ^
-DUSE_GAMEPAD=1 ^
-DCMAKE_BUILD_TYPE=Release %SRC%


GOTO:eof

msbuild mandelbulber2.sln /p:Configuration=Release /m

REM --- exit ----
GOTO:eof
