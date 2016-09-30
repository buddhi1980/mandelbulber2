@echo off

rem Define a timestamp function
set startDate=%date%
set startTime=%time%
set /a time_year=%startDate:~10%
set time_year=00000%time_year%
set time_year=%time_year:~-4%
set /a time_month=1%startDate:~4,2% - 100
set time_month=00000%time_month%
set time_month=%time_month:~-2%
set /a time_day=1%startDate:~7,2% - 100
set time_day=00000%time_day%
set time_day=%time_day:~-2%
set /a time_hour=%startTime:~0,2%
set time_hour=00000%time_hour%
set time_hour=%time_hour:~-2%
set /a time_minute=1%startTime:~3,2% - 100
set time_minute=00000%time_minute%
set time_minute=%time_minute:~-2%
set /a time_second=1%startTime:~6,2% - 100
set time_second=00000%time_second%
set time_second=%time_second:~-2%
set SAVESTAMP=%time_year%%time_month%%time_day%%time_hour%%time_minute%%time_second%

set _SCRIPT_DRIVE=%~d0
set _SCRIPT_FOLDER=%~dp0

ECHO Change Directory
%_SCRIPT_DRIVE%
cd %_SCRIPT_FOLDER%
set SRC=%CD%

set build_logs=C:\System\build_logs\
set filename=%build_logs%mandelbulber2-wind-build_bot-%SAVESTAMP%.txt


ECHO Building
cd %SRC% && %SRC%\build.batch.bat 2>&1 > %filename%
