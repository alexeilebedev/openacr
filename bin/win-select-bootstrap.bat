@echo off
setlocal enabledelayedexpansion

set BOOTSTRAP_FILE=bin\bootstrap\Windows-msvc.release-x86_64.bat

if not exist %BOOTSTRAP_FILE% (
    echo No available bootstrap file matched your configuration
    echo The list of all available bootstraps is in bin\bootstrap
    echo New ones can be created either manually or from an existing installation
    exit /b 1
)

echo Running %BOOTSTRAP_FILE%
call %BOOTSTRAP_FILE%
if errorlevel 1 (
    echo Bootstrap failed
    exit /b 1
)

endlocal