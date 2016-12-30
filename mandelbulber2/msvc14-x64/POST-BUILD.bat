REM @echo off

Echo Mandelbulber2 Visual Studio Post-Build

REM # x64 Build Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set SRC=%CD%\..\..\
set BUILDTREE=%SRC%build-win
SET tbs_arch=x64
SET vcvar_arg=x86_amd64
SET qt_version=Qt5.7.0\5.7\msvc2015_64
set PACKAGESDIR=%SRC%\packages
set QTDIR=%PACKAGESDIR%\bulbqt-msvc14-x64.5.7.0.10377\build\native

:copy_files
set BINDIR=%BUILDTREE%\Release\
echo %BINDIR%	
xcopy /Y /I /S %SRC%\mandelbulber2\deploy\share\mandelbulber2 %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Core.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Gamepad.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Gui.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Multimedia.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5MultimediaQuick_p.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5MultimediaWidgets.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Network.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Svg.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Test.dll %BINDIR%
xcopy /Y %QTDIR%\bin\Qt5Widgets.dll %BINDIR%
xcopy /Y /I %QTDIR%\plugins\gamepads %BINDIR%gamepads
xcopy /Y /I %QTDIR%\plugins\iconengines %BINDIR%iconengines
xcopy /Y /I %QTDIR%\plugins\imageformats %BINDIR%imageformats
xcopy /Y /I %QTDIR%\plugins\platforms %BINDIR%platforms
xcopy /Y /I %QTDIR%\plugins\audio %BINDIR%audio
xcopy /Y /I %QTDIR%\plugins\mediaservice %BINDIR%mediaservice
del /F /Q %BINDIR%language
del /F /Q %BINDIR%qt_data
del /F /Q %BINDIR%data
xcopy /Y /I %SRC%\mandelbulber2\language %BINDIR%language
xcopy /Y /I %SRC%\mandelbulber2\qt_data %BINDIR%qt_data
xcopy /Y /I %SRC%\mandelbulber2\data %BINDIR%data
copy /Y %SRC%\mandelbulber2\deploy\README-win32.txt %BINDIR%README.txt
copy /Y %SRC%\mandelbulber2\deploy\NEWS %BINDIR%NEWS.txt
copy /Y %SRC%\mandelbulber2\deploy\COPYING %BINDIR%COPYING.txt
copy /Y %SRC%\mandelbulber2\deploy\win32\mandelbulber2.ico %BINDIR%mandelbulber2.ico
mkdir %BINDIR%source
xcopy /Y /I  %SRC%\mandelbulber2\src %BINDIR%source\src
xcopy /Y /I  %SRC%\mandelbulber2\qt %BINDIR%source\qt
mkdir %BINDIR%makefiles
copy /Y %SRC%\mandelbulber2\Release\mandelbulber.pro %BINDIR%makefiles\mandelbulber.pro

REM --- exit ----
GOTO:eof
