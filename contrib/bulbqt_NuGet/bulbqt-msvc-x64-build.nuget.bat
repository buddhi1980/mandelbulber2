REM @echo off

Echo LIB Windows Build NuGet

REM # Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set INITDIR=%_SCRIPT_FOLDER%
set BUILDTREE=%INITDIR%\build-win\
SET tbs_arch=x64
SET vcvar_arg=x86_amd64

REM # VC Vars #
SET VCVAR="%programfiles(x86)%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%
SET VCVAR="%programfiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%

REM # Clean Build Tree #
rd /s /q %BUILDTREE%
mkdir %BUILDTREE%

REM # Change to Build Tree drive #
%_SCRIPT_DRIVE%
REM # Change to Build Tree directory #
cd %BUILDTREE%

:copy_files
set BINDIR=%INITDIR%\build-nuget
rd /s /q %BINDIR%
mkdir %BINDIR%
echo %BINDIR%
REM # Change to Binary directory #
cd %BINDIR%

REM copy QT files to binary directory
xcopy "C:\Qt\5.15.2\msvc2019_64" %BINDIR%\build\native\ /s /e

:nuget_req
cd %BINDIR%
REM # make nuget packages from binaries #
copy %INITDIR%\bulbqt-msvc-%tbs_arch%.targets %BINDIR%\build\native\bulbqt-msvc-%tbs_arch%.targets
copy %INITDIR%\bulbqt-msvc-%tbs_arch%.nuspec %BINDIR%\bulbqt-msvc-%tbs_arch%.nuspec
copy "C:\Qt\Licenses\LICENSE" %BINDIR%\LICENSE
copy "C:\Qt\Licenses\LICENSE.FDL" %BINDIR%\LICENSE.FDL
copy "C:\Qt\Licenses\LICENSE" %BINDIR%\build\native\LICENSE
copy "C:\Qt\Licenses\LICENSE.FDL" %BINDIR%\build\native\LICENSE.FDL
REM # Change to Build Tree directory #
cd %BUILDTREE%
REM # Create the NuGet #
nuget pack %BINDIR%\bulbqt-msvc-%tbs_arch%.nuspec
cd %INITDIR%
REM --- exit ----
GOTO:eof
