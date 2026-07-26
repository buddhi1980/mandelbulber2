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
SET VCVAR="C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%
SET VCVAR="C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %VCVAR% call %VCVAR% %vcvar_arg%

REM # Clean Build Tree #
rd /s /q %BUILDTREE%
mkdir %BUILDTREE%

REM # Change to Build Tree drive #
%_SCRIPT_DRIVE%
REM # Change to Build Tree directory #
cd %BUILDTREE%

:copy_files
set BINDIR=%INITDIR%build-nuget
rd /s /q %BINDIR%
mkdir %BINDIR%
echo %BINDIR%
REM # Change to Binary directory #
cd %BINDIR%

mkdir %BINDIR%\build
mkdir %BINDIR%\build\native
mkdir %BINDIR%\build\native\bin
mkdir %BINDIR%\build\native\include
mkdir %BINDIR%\build\native\lib

REM copy QT files to binary directory
xcopy "C:\OpenCL-SDK-v2026.05.29-Win-x64\include" %BINDIR%\build\native\include /s /e
xcopy "C:\OpenCL-SDK-v2026.05.29-Win-x64\bin" %BINDIR%\build\native\bin /s /e
xcopy "C:\OpenCL-SDK-v2026.05.29-Win-x64\lib" %BINDIR%\build\native\lib /s /e

:nuget_req
cd %BINDIR%
REM # make nuget packages from binaries #
copy %INITDIR%\opencl-nuget-msvc-%tbs_arch%.targets %BINDIR%\build\native\opencl-nuget-msvc-%tbs_arch%.targets
copy %INITDIR%\opencl-nuget-msvc-%tbs_arch%.nuspec %BINDIR%\opencl-nuget-msvc-%tbs_arch%.nuspec

set "url=https://raw.githubusercontent.com/KhronosGroup/OpenCL-SDK/e26922bdf54eaa9fcc31fe1f91d21b8d2bd6970f/LICENSE"
set "output=%BINDIR%\LICENSE"
curl -L -o "%output%" "%url%"

REM # Change to Build Tree directory #
cd %BUILDTREE%
REM # Create the NuGet #
nuget pack %BINDIR%\opencl-nuget-msvc-%tbs_arch%.nuspec
cd %INITDIR%
REM --- exit ----
GOTO:eof
