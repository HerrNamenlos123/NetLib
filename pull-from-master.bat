@echo off

cd "%~dp0"

:: Call the script in all submodules recursively
for /D %%i in (modules/*) do (
    if exist "%~dp0modules/%%i/pull-from-master.bat" (
        call "%~dp0modules/%%i/pull-from-master.bat" nopause
    )
)

cd "%~dp0"
for %%a in ("%~dp0\.") do set "parentfolder=%%~nxa"

echo.
echo Pulling %parentfolder% from master...
git pull origin master
echo Done
echo.

if [%~1] == [] (
    Pause
)