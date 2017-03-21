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
SET VERBOSE_MAKEFILE=FALSE
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
set ZLIBDIR=%SRC%\packages\zlib-msvc14-x64.1.2.11.7795\build\native
set ZLIBDIST=%ZLIBDIR%\bin_release
set PNGDIR=%SRC%\packages\libpng-msvc14-x64.1.6.29.7800\build\native
set PNGDIST=%PNGDIR%\bin_release
set TIFFDIR=%SRC%\packages\libtiff-msvc14-x64.4.0.7.7799\build\native
set JPEGDIR=%SRC%\packages\libjpeg-msvc14-x64.9.1.0.7796\build\native
set GSLDIR=%SRC%\packages\gsl-msvc14-x64.2.3.0.2779\build\native

cd %BUILDTREE%
cmake -G %cmake_platform% ^
-DCMAKE_C_COMPILER=icl.exe ^
-DCMAKE_C_LINK_EXECUTABLE=icl.exe ^
-DCMAKE_CXX_COMPILER=icl.exe ^
-DCMAKE_CXX_LINK_EXECUTABLE=icl.exe ^
-DCMAKE_CXX_STANDARD=11 ^
-DCMAKE_MAKE_PROGRAM=%JomDir%\jom.exe ^
-DCMAKE_VERBOSE_MAKEFILE=%VERBOSE_MAKEFILE% ^
-DUSE_GAMEPAD=1 ^
-DUSE_OFFLOAD=1 ^
-DCMAKE_CXX_FLAGS="/Qstd=c++11 /Qopenmp /EHsc /nologo" ^
-DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_INSTALL_PREFIX=%OutDir% ^
-DZLIB_LIBRARY=%ZLIBDIR%\lib_release\zlibstatic.lib ^
-DZLIB_INCLUDE_DIR=%ZLIBDIR%\include ^
-DPNG_LIBRARY=%PNGDIR%\lib_release\libpng16_static.lib ^
-DPNG_PNG_INCLUDE_DIR=%PNGDIR%\include ^
-DTIFF_LIBRARY=%TIFFDIR%\lib_release\tiff.lib ^
-DTIFF_INCLUDE_DIR=%TIFFDIR%\include ^
-DJPEG_LIBRARY=%JPEGDIR%\lib_release\jpeg.lib ^
-DJPEG_INCLUDE_DIR=%JPEGDIR%\include ^
-DGSL_CBLAS_LIBRARY=%GSLDIR%\static\gslcblas.lib ^
-DGSL_LIBRARY=%GSLDIR%\static\gsl.lib ^
-DGSL_INCLUDE_DIR=%GSLDIR%\ ^
%SRC%\mandelbulber2\cmake\

cmake --build . --config Release --target install

:copy_files
set BINDIR=%OutDir%
echo %BINDIR%	
xcopy /Q /C /Y /B /S %SRC%\mandelbulber2\deploy\share\mandelbulber2 %BINDIR%
xcopy /Q /C /Y /I %QTDIR%\plugins\gamepads %BINDIR%gamepads
xcopy /Q /C /Y /I %QTDIR%\plugins\iconengines %BINDIR%iconengines
xcopy /Q /C /Y /I %QTDIR%\plugins\imageformats %BINDIR%imageformats
xcopy /Q /C /Y /I %QTDIR%\plugins\platforms %BINDIR%platforms
xcopy /Q /C /Y %QTDIR%\bin\Qt5Gamepad*.dll %BINDIR%
xcopy /Q /C /Y %QTDIR%\bin\Qt5Gui*.dll %BINDIR%
xcopy /Q /C /Y %QTDIR%\bin\Qt5Network*.dll %BINDIR%
xcopy /Q /C /Y %QTDIR%\bin\Qt5Svg*.dll %BINDIR%
xcopy /Q /C /Y %QTDIR%\bin\Qt5Test*.dll %BINDIR%
xcopy /Q /C /Y %QTDIR%\bin\Qt5Widgets*.dll %BINDIR%
xcopy /Q /C /Y %QTDIR%\bin\Qt5Core*.dll %BINDIR%
xcopy /Q /C /Y %QTDIR%\bin\Qt5Multimedia*.dll %BINDIR%
del /F /Q %BINDIR%language
del /F /Q %BINDIR%qt_data
del /F /Q %BINDIR%data
xcopy /Q /C /Y /I %SRC%\mandelbulber2\language %BINDIR%language
xcopy /Q /C /Y /I %SRC%\mandelbulber2\qt_data %BINDIR%qt_data
xcopy /Q /C /Y /I %SRC%\mandelbulber2\data %BINDIR%data
xcopy /Q /C /Y %PNGDIST%\*.dll %BINDIR%
xcopy /Q /C /Y %ZLIBDIST%\*.dll %BINDIR%

REM --- exit ----
GOTO:eof
