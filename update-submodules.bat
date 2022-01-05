:: update-submodules.bat v1.0
@echo off

cd "%~dp0"

git submodule update --remote --merge --recursive

if [%~1] == [] (
    Pause
)