REM @echo off

Echo LIB Windows Build NuGet

REM # Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set INITDIR=%_SCRIPT_FOLDER%
set SRC=%INITDIR%\..\..\..\opus
set BUILDTREE=%SRC%\build-win\
SET tbs_arch=x86
SET vcvar_arg=x86
SET cmake_platform="Visual Studio 16 2019"
SET cmake_arch="Win32"

REM # VC Vars #
SET VCVAR="%programfiles(x86)%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%
SET VCVAR="%programfiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%

REM # Clean Build Tree #
rd /s /q %BUILDTREE%
mkdir %BUILDTREE%
mkdir %BUILDTREE%\deps

REM # Change to Build Tree drive #
%_SCRIPT_DRIVE%
REM # Change to Build Tree directory #
cd %BUILDTREE%

:nuget_Dep
REM # packages from nuget #
mkdir %BUILDTREE%\deps
cd %BUILDTREE%\deps
REM zlib
SET VER=1.2.11.8899
set ZLIBDIR=%BUILDTREE%\deps\zlib-msvc-%tbs_arch%.%VER%\build\native
nuget install zlib-msvc-%tbs_arch% -Version %VER%
REM Ogg (OGG_LIBRARY OGG_INCLUDE_DIR)
SET VER=1.3.2.8787
set OggDIR=%BUILDTREE%\deps\ogg-msvc-%tbs_arch%.%VER%\build\native
nuget install ogg-msvc-%tbs_arch% -Version %VER%
REM Vorbis (Vorbis_Vorbis_LIBRARY Vorbis_Vorbis_INCLUDE_DIR)
SET VER=1.3.5.8787
set VorbisDIR=%BUILDTREE%\deps\vorbis-msvc-%tbs_arch%.%VER%\build\native
nuget install vorbis-msvc-%tbs_arch% -Version %VER%
REM FLAC (FLAC_LIBRARY FLAC_INCLUDE_DIR)
SET VER=1.3.2.8787
set FLACDIR=%BUILDTREE%\deps\flac-msvc-%tbs_arch%.%VER%\build\native
nuget install flac-msvc-%tbs_arch% -Version %VER%

:copy_files
set BINDIR=%SRC%\build-nuget\
rd /s /q %BINDIR%
mkdir %BINDIR%
echo %BINDIR%

:static_LIB
REM # LIB STATIC #
ECHO %cmake_platform% STATIC

rd /s /q %BUILDTREE%\opus
mkdir %BUILDTREE%\opus
cd %BUILDTREE%\opus
cmake -G %cmake_platform% ^
-A %cmake_arch% ^
-DBUILD_BUILD_SHARED_LIBS:BOOL=OFF ^
-DCMAKE_CXX_FLAGS_RELEASE="/MD" ^
-DCMAKE_CXX_FLAGS_DEBUG="/MDd" ^
-DCMAKE_C_FLAGS_RELEASE="/MD" ^
-DCMAKE_C_FLAGS_DEBUG="/MDd" ^
-DZLIB_LIBRARY=%ZLIBDIR%\lib_release\zlibstatic.lib ^
-DZLIB_INCLUDE_DIR=%ZLIBDIR%\include ^
-DOGG_LIBRARY=%OggDIR%\lib_release\ogg.lib ^
-DOGG_INCLUDE_DIR=%OggDIR%\include ^
-DVorbis_Vorbis_LIBRARY=%VorbisDIR%\lib_release\vorbis.lib ^
-DVorbis_Enc_LIBRARY=%VorbisDIR%\lib_release\vorbisenc.lib ^
-DVorbis_File_LIBRARY=%VorbisDIR%\lib_release\vorbisfile.lib ^
-DVorbis_Vorbis_INCLUDE_DIR=%VorbisDIR%\include ^
-DVorbis_Enc_INCLUDE_DIR=%VorbisDIR%\include ^
-DVorbis_File_INCLUDE_DIR=%VorbisDIR%\include ^
-DFLAC_LIBRARY=%FLACDIR%\lib_release\libFLAC.lib ^
-DFLAC_INCLUDE_DIR=%FLACDIR%\include ^
-DCMAKE_INSTALL_PREFIX=%BINDIR%\build\native\ ^
-DCMAKE_BUILD_TYPE="Release" %SRC%
cmake --build . --config Release --target install

move %BINDIR%\build\native\lib %BINDIR%\build\native\lib_release
move %BINDIR%\build\native\bin %BINDIR%\build\native\bin_release

REM # DEBUG #
REM # Clean Build Tree #
rd /s /q %BUILDTREE%\opus
mkdir %BUILDTREE%\opus
cd %BUILDTREE%\opus
cmake -G %cmake_platform% ^
-A %cmake_arch% ^
-DBUILD_BUILD_SHARED_LIBS:BOOL=OFF ^
-DCMAKE_CXX_FLAGS_RELEASE="/MD" ^
-DCMAKE_CXX_FLAGS_DEBUG="/MDd" ^
-DCMAKE_C_FLAGS_RELEASE="/MD" ^
-DCMAKE_C_FLAGS_DEBUG="/MDd" ^
-DZLIB_LIBRARY=%ZLIBDIR%\lib_debug\zlibstaticd.lib ^
-DZLIB_INCLUDE_DIR=%ZLIBDIR%\include ^
-DOGG_LIBRARY=%OggDIR%\lib_debug\ogg.lib ^
-DOGG_INCLUDE_DIR=%OggDIR%\include ^
-DVorbis_Vorbis_LIBRARY=%VorbisDIR%\lib_debug\vorbis.lib ^
-DVorbis_Enc_LIBRARY=%VorbisDIR%\lib_debug\vorbisenc.lib ^
-DVorbis_File_LIBRARY=%VorbisDIR%\lib_debug\vorbisfile.lib ^
-DVorbis_Vorbis_INCLUDE_DIR=%VorbisDIR%\include ^
-DVorbis_Enc_INCLUDE_DIR=%VorbisDIR%\include ^
-DVorbis_File_INCLUDE_DIR=%VorbisDIR%\include ^
-DFLAC_LIBRARY=%FLACDIR%\lib_debug\libFLAC.lib ^
-DFLAC_INCLUDE_DIR=%FLACDIR%\include ^
-DCMAKE_INSTALL_PREFIX=%BINDIR%\build\native\ ^
-DCMAKE_BUILD_TYPE="DEBUG" %SRC%
cmake --build . --config DEBUG --target install

move %BINDIR%\build\native\lib %BINDIR%\build\native\lib_debug
move %BINDIR%\build\native\bin %BINDIR%\build\native\bin_debug

GOTO:nuget_req


:nuget_req
cd %BINDIR%
REM # make nuget packages from binaries #
copy %INITDIR%\opus-msvc-%tbs_arch%.targets %BINDIR%\build\native\opus-msvc-%tbs_arch%.targets
copy %INITDIR%\opus-msvc-%tbs_arch%.nuspec %BINDIR%\opus-msvc-%tbs_arch%.nuspec
copy %SRC%\COPYING %BINDIR%\COPYING
copy %SRC%\README %BINDIR%\README
cd %BUILDTREE%
nuget pack %BINDIR%\opus-msvc-%tbs_arch%.nuspec
cd %INITDIR%
REM --- exit ----
GOTO:eof
