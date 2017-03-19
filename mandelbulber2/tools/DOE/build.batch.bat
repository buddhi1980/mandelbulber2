REM @echo off

Echo Mandelbulber2 Windoze Build Bot


set INCREMENTDISABLE=true

REM # XEON x64 Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set SRC=%_SCRIPT_FOLDER%\..\..\..\
SET tbs_arch=x64
SET vcvar_arg=x86_amd64
SET ms_build_suffix=Bin\amd64
SET cmake_platform="NMake Makefiles JOM"
SET cmake_platform_tool="Intel C++ Compiler 17.0"
set QTDIR=%SRC%\packages\bulbqt-msvc14-x64.5.8.0.10782\build\native
SET JomDir=%SRC%\packages\jom-nug.1.1.2.777\build\native
set PATH=%QTDIR%\bin;C:\Program Files (x86)\MSBuild\14.0\%ms_build_suffix%;%SRC%;%PATH%

REM # Vars #
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" %vcvar_arg%
call "C:\Program Files (x86)\IntelSWTools\compilers_and_libraries\windows\bin\iclvars.bat" intel64
@echo on

set OutDir=%SRC%\build-win\Release\k1om\
set IntDir=%SRC%\build-win\mandelbulber2.dir\Release\k1om\
set BUILDTREE=%IntDir%

REM # Clean Build Tree #
if defined INCREMENTDISABLE ( 
	echo "Incremental Build disabled"
    rd /s /q %BUILDTREE%
) else (
	echo "Incremental Build enabled"
)
mkdir %BUILDTREE%

:nuget_Dep
REM # packages from nuget #
nuget restore %SRC%\mandelbulber2.sln

REM # build_script #
set ZLIBDIR=%SRC%\packages\zlib-msvc14-x64.1.2.11.7795
set PNGDIR=%SRC%\packages\libpng.1.6.28.1
set TIFFDIR=%SRC%\packages\libtiff.4.0.6.2
set JPEGDIR=%SRC%\packages\libjpeg.9.2.0.1
set GSLDIR=%SRC%\packages\gsl-msvc14-x64.2.3.0.2779
set PNGDIST=%SRC%\packages\libpng.redist.1.6.28.1\build\native\bin\x64\v140\dynamic\Release
set TIFFDIST=%SRC%\packages\libtiff.redist.4.0.6.2\build\native\bin\Release\x64\v140"
cd %BUILDTREE%
cmake -G %cmake_platform% ^
-DCMAKE_C_COMPILER=icl.exe ^
-DCMAKE_C_LINK_EXECUTABLE=icl.exe ^
-DCMAKE_CXX_COMPILER=icl.exe ^
-DCMAKE_CXX_LINK_EXECUTABLE=icl.exe ^
-DCMAKE_CXX_STANDARD=11 ^
-DCMAKE_MAKE_PROGRAM=%JomDir%\jom.exe ^
-DCMAKE_VERBOSE_MAKEFILE=TRUE ^
-DUSE_GAMEPAD=1 ^
-DCMAKE_CXX_FLAGS="/Qstd=c++11 /Qopenmp /EHsc" ^
-DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_INSTALL_PREFIX=%OutDir% ^
-DZLIB_LIBRARY=%ZLIBDIR%\build\native\lib_release\zlibstatic.lib ^
-DZLIB_INCLUDE_DIR=%ZLIBDIR%\build\native\include ^
-DPNG_LIBRARY=%PNGDIR%\build\native\lib\x64\v140\dynamic\Release\libpng16.lib ^
-DPNG_PNG_INCLUDE_DIR=%PNGDIR%\build\native\include ^
-DTIFF_LIBRARY=%TIFFDIR%\build\native\lib\Release\x64\v140\libtiff.lib ^
-DTIFF_INCLUDE_DIR=%TIFFDIR%\build\native\include ^
-DJPEG_LIBRARY=%JPEGDIR%\build\native\lib\v140\x64\Release\static\cdecl\jpeg.lib ^
-DJPEG_INCLUDE_DIR=%JPEGDIR%\build\native\include ^
-DGSL_CBLAS_LIBRARY=%GSLDIR%\build\native\static\gslcblas.lib ^
-DGSL_LIBRARY=%GSLDIR%\build\native\static\gsl.lib ^
-DGSL_INCLUDE_DIR=%GSLDIR%\build\native\ ^
%SRC%\mandelbulber2\cmake\

%JomDir%\jom.exe -j12

:copy_files
set BINDIR=%OutDir%
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
xcopy %PNGDIST%\*.dll %BINDIR%
xcopy %TIFFDIST%\*.dll %BINDIR%
del /F /Q %BINDIR%language
del /F /Q %BINDIR%qt_data
del /F /Q %BINDIR%data
xcopy /I %SRC%\mandelbulber2\language %BINDIR%language
xcopy /I %SRC%\mandelbulber2\qt_data %BINDIR%qt_data
xcopy /I %SRC%\mandelbulber2\data %BINDIR%data

REM --- exit ----
GOTO:eof
