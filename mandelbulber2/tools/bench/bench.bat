@echo off
Echo Mandelbulber2 Windoze Bench Bot

REM # Timestamp #
for /f "tokens=1-7 delims=:/-, " %%i in ('echo exit^|cmd /q /k"prompt $d $t"') do (
   for /f "tokens=2-4 delims=/-,() skip=1" %%a in ('echo.^|date') do (
      set dow=%%i
      set %%a=%%j
      set %%b=%%k
      set %%c=%%l
      set hh=%%m
      set min=%%n
      set ss=%%o
   )
)

@echo on
REM # Vars #
set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0
set timestamp=%yy%%mm%%dd%%hh%%min%%ss%_%dow%
set SRC=%_SCRIPT_FOLDER%\..\..\..\

REM # eval relative path #
@echo off
set REL_PATH=%SRC%
set ABS_PATH=
pushd %REL_PATH%
set ABS_PATH=%CD%
popd
set SRC=%ABS_PATH%
@echo on

set OutDir=%SRC%\build-win\Release\x64
set IntDir=%SRC%\build-win\Bench\%timestamp%
set bulb=%OutDir%\mandelbulber2.exe

REM # Clean IntDir #
rd /s /q %IntDir%
mkdir %IntDir%
mkdir %IntDir%\ocl

REM # Bench #
cd %OutDir%
%bulb% --nogui --version
%bulb% -O 'opencl_enabled=0' --benchmark 1 -o %IntDir%
%bulb% -O 'opencl_enabled=1' --benchmark 1 -o %IntDir%\ocl

REM --- exit ----
cd %_SCRIPT_FOLDER%
GOTO:eof
