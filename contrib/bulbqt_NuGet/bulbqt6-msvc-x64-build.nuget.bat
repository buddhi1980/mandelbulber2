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
mkdir %BINDIR%\build\native\plugins

REM copy QT files to binary directory
xcopy "C:\Qt\6.11.1\msvc2022_64\include" %BINDIR%\build\native\include /s /e
xcopy "C:\Qt\6.11.1\msvc2022_64\plugins" %BINDIR%\build\native\plugins /s /e

xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\moc.exe" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\uic.exe" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\rcc.exe" %BINDIR%\build\native\bin

xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Concurrent.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Concurrentd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Core.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Cored.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Gui.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Guid.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Multimedia.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Multimediad.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6MultimediaQuick.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6MultimediaQuickd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6MultimediaWidgets.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6MultimediaWidgetsd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Network.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Networkd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6OpenGL.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6OpenGLd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6OpenGLWidgets.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6OpenGLWidgetsd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Qml.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Qmld.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Svg.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Svgd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Test.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Testd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6UiTools.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6UiToolsd.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Widgets.dll" %BINDIR%\build\native\bin
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\bin\Qt6Widgetsd.dll" %BINDIR%\build\native\bin

xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Concurrent.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Concurrentd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Core.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Cored.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6EntryPoint.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6EntryPointd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Gui.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Guid.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Multimedia.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Multimediad.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6MultimediaQuick.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6MultimediaQuickd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6MultimediaWidgets.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6MultimediaWidgetsd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Network.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Networkd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6OpenGL.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6OpenGLd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6OpenGLWidgets.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6OpenGLWidgetsd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Qml.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Qmld.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Svg.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Svgd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Test.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Testd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6UiTools.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6UiToolsd.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Widgets.lib" %BINDIR%\build\native\lib
xcopy /Y /Q "C:\Qt\6.11.1\msvc2022_64\lib\Qt6Widgetsd.lib" %BINDIR%\build\native\lib

:nuget_req
cd %BINDIR%
REM # make nuget packages from binaries #
copy %INITDIR%\bulbqt6-msvc-%tbs_arch%.targets %BINDIR%\build\native\bulbqt6-msvc-%tbs_arch%.targets
copy %INITDIR%\bulbqt6-msvc-%tbs_arch%.nuspec %BINDIR%\bulbqt6-msvc-%tbs_arch%.nuspec
copy "C:\Qt\Licenses\LICENSE" %BINDIR%\LICENSE
copy "C:\Qt\Licenses\LICENSE.FDL" %BINDIR%\LICENSE.FDL
copy "C:\Qt\Licenses\LICENSE" %BINDIR%\build\native\LICENSE
copy "C:\Qt\Licenses\LICENSE.FDL" %BINDIR%\build\native\LICENSE.FDL
REM # Change to Build Tree directory #
cd %BUILDTREE%
REM # Create the NuGet #
nuget pack %BINDIR%\bulbqt6-msvc-%tbs_arch%.nuspec
cd %INITDIR%
REM --- exit ----
GOTO:eof
