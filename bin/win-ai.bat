@echo off
setlocal enabledelayedexpansion

:: TODO later - make this the bootstrap file like the *nix ai; support mingw

echo # this script has been created for Windows build
echo # NOTE: WIN32 BUILDS ARE IN AN EXPERIMENTAL STATE

:: Change to the root directory (assuming the script is called from there)
cd %~dp0\..

:: Find VS installation, set up environment
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set VS_PATH=%%i
)
if not defined VS_PATH (
    echo Error: Visual Studio not found
    exit /b 1
)
call "!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
echo Visual Studio environment set up successfully.

mkdir build 2>nul

if not exist .ffroot (
    echo Missing .ffroot. Wrong directory?
    exit /b 1
)

echo # Setting up directory links
call :create_link build\release build\Windows-msvc.release-x86_64
call :create_link build\coverage build\Windows-msvc.coverage-x86_64
call :create_link build\debug build\Windows-msvc.debug-x86_64
call :create_link build\profile build\Windows-msvc.profile-x86_64

mkdir temp 2>nul

:: Using default openssl install location. change if yours is different.
set COMPILE_FLAGS=/EHsc /std:c++17 /O2 /MD /I. /I"C:\Program Files\OpenSSL-Win64\include" /DAOS_SSE42 /W4 /WX /wd4100 /wd4267 /wd4244 /wd4702

echo # Compiling source files
call :compile_if_changed cpp\abt\build.cpp build\release\cpp.abt.build.obj
call :compile_if_changed cpp\abt\disas.cpp build\release\cpp.abt.disas.obj
call :compile_if_changed cpp\abt\main.cpp build\release\cpp.abt.main.obj
call :compile_if_changed cpp\abt\opt.cpp build\release\cpp.abt.opt.obj
call :compile_if_changed cpp\gen\abt_gen.cpp build\release\cpp.gen.abt_gen.obj
call :compile_if_changed cpp\gcache\gcache.cpp build\release\cpp.gcache.gcache.obj
call :compile_if_changed cpp\gen\gcache_gen.cpp build\release\cpp.gen.gcache_gen.obj
call :compile_if_changed cpp\gen\src_func_gen.cpp build\release\cpp.gen.src_func_gen.obj
call :compile_if_changed cpp\src_func\edit.cpp build\release\cpp.src_func.edit.obj
call :compile_if_changed cpp\src_func\fileloc.cpp build\release\cpp.src_func.fileloc.obj
call :compile_if_changed cpp\src_func\header.cpp build\release\cpp.src_func.header.obj
call :compile_if_changed cpp\src_func\iffy.cpp build\release\cpp.src_func.iffy.obj
call :compile_if_changed cpp\src_func\list.cpp build\release\cpp.src_func.list.obj
call :compile_if_changed cpp\src_func\main.cpp build\release\cpp.src_func.main.obj
call :compile_if_changed cpp\src_func\nextfile.cpp build\release\cpp.src_func.nextfile.obj
call :compile_if_changed cpp\src_func\scan.cpp build\release\cpp.src_func.scan.obj
call :compile_if_changed cpp\gen\src_hdr_gen.cpp build\release\cpp.gen.src_hdr_gen.obj
call :compile_if_changed cpp\src_hdr\hdr.cpp build\release\cpp.src_hdr.hdr.obj
call :compile_if_changed cpp\gen\_gen.cpp build\release\cpp.gen._gen.obj
call :compile_if_changed cpp\gen\algo_gen.cpp build\release\cpp.gen.algo_gen.obj
call :compile_if_changed cpp\gen\algo_lib_gen.cpp build\release\cpp.gen.algo_lib_gen.obj
call :compile_if_changed cpp\gen\command_gen.cpp build\release\cpp.gen.command_gen.obj
call :compile_if_changed cpp\gen\dev_gen.cpp build\release\cpp.gen.dev_gen.obj
call :compile_if_changed cpp\gen\dmmeta_gen.cpp build\release\cpp.gen.dmmeta_gen.obj
call :compile_if_changed cpp\gen\report_gen.cpp build\release\cpp.gen.report_gen.obj
call :compile_if_changed cpp\lib\algo\arg.cpp build\release\cpp.lib.algo.arg.obj
call :compile_if_changed cpp\lib\algo\backtrace.cpp build\release\cpp.lib.algo.backtrace.obj
call :compile_if_changed cpp\lib\algo\base64.cpp build\release\cpp.lib.algo.base64.obj
call :compile_if_changed cpp\lib\algo\cpu_hz.cpp build\release\cpp.lib.algo.cpu_hz.obj
call :compile_if_changed cpp\lib\algo\crc32.cpp build\release\cpp.lib.algo.crc32.obj
call :compile_if_changed cpp\lib\algo\decimal.cpp build\release\cpp.lib.algo.decimal.obj
call :compile_if_changed cpp\lib\algo\errtext.cpp build\release\cpp.lib.algo.errtext.obj
call :compile_if_changed cpp\lib\algo\file.cpp build\release\cpp.lib.algo.file.obj
call :compile_if_changed cpp\lib\algo\fmt.cpp build\release\cpp.lib.algo.fmt.obj
call :compile_if_changed cpp\lib\algo\iohook.cpp build\release\cpp.lib.algo.iohook.obj
call :compile_if_changed cpp\lib\algo\lib.cpp build\release\cpp.lib.algo.lib.obj
call :compile_if_changed cpp\lib\algo\line.cpp build\release\cpp.lib.algo.line.obj
call :compile_if_changed cpp\lib\algo\lockfile.cpp build\release\cpp.lib.algo.lockfile.obj
call :compile_if_changed cpp\lib\algo\mmap.cpp build\release\cpp.lib.algo.mmap.obj
call :compile_if_changed cpp\lib\algo\prlog.cpp build\release\cpp.lib.algo.prlog.obj
call :compile_if_changed cpp\lib\algo\regx.cpp build\release\cpp.lib.algo.regx.obj
call :compile_if_changed cpp\lib\algo\string.cpp build\release\cpp.lib.algo.string.obj
call :compile_if_changed cpp\lib\algo\time.cpp build\release\cpp.lib.algo.time.obj
call :compile_if_changed cpp\lib\algo\timehook.cpp build\release\cpp.lib.algo.timehook.obj
call :compile_if_changed cpp\lib\algo\tstamp.cpp build\release\cpp.lib.algo.tstamp.obj
call :compile_if_changed cpp\lib\algo\txttbl.cpp build\release\cpp.lib.algo.txttbl.obj
call :compile_if_changed cpp\lib\algo\u128.cpp build\release\cpp.lib.algo.u128.obj
call :compile_if_changed cpp\lib\algo\win32.cpp build\release\cpp.lib.algo.win32.obj
call :compile_if_changed cpp\gen\lib_git_gen.cpp build\release\cpp.gen.lib_git_gen.obj
call :compile_if_changed cpp\lib\lib_git.cpp build\release\cpp.lib.lib_git.obj
call :compile_if_changed cpp\gen\lib_json_gen.cpp build\release\cpp.gen.lib_json_gen.obj
call :compile_if_changed cpp\lib\lib_json.cpp build\release\cpp.lib.lib_json.obj

echo # Creating libraries 
lib.exe /OUT:build\release\lib_json-x86_64.lib build\release\cpp.gen.lib_json_gen.obj build\release\cpp.lib.lib_json.obj  | findstr /v /c:"Microsoft (R) C/C++ Optimizing Compiler" /c:"Copyright (C) Microsoft Corporation"
lib.exe /OUT:build\release\algo_lib-x86_64.lib build\release\cpp.gen.*.obj build\release\cpp.lib.algo.*.obj build\release\cpp.lib.algo.*.obj | findstr /v /c:"Microsoft (R) C/C++ Optimizing Compiler" /c:"Copyright (C) Microsoft Corporation"
lib.exe /OUT:build\release\lib_git-x86_64.lib build\release\cpp.gen.lib_git_gen.obj build\release\cpp.lib.lib_git.obj | findstr /v /c:"Microsoft (R) C/C++ Optimizing Compiler" /c:"Copyright (C) Microsoft Corporation"

echo # Linking executables
call :link_if_changed build\release\abt.exe build\release\cpp.abt.*.obj build\release\cpp.gen.abt_gen.obj build\release\algo_lib-x86_64.lib build\release\lib_json-x86_64.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib
call :link_if_changed build\release\gcache.exe build\release\cpp.gcache.*.obj build\release\cpp.gen.gcache_gen.obj build\release\algo_lib-x86_64.lib build\release\lib_json-x86_64.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib
call :link_if_changed build\release\src_func.exe build\release\cpp.gen.src_func_gen.obj build\release\cpp.src_func.*.obj build\release\algo_lib-x86_64.lib build\release\lib_json-x86_64.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib
call :link_if_changed build\release\src_hdr.exe build\release\cpp.gen.src_hdr_gen.obj build\release\cpp.src_hdr.*.obj build\release\algo_lib-x86_64.lib build\release\lib_json-x86_64.lib build\release\lib_git-x86_64.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib

echo # Creating symbolic links for executables
mklink bin\abt.exe ..\build\release\abt.exe
mklink bin\gcache.exe ..\build\release\gcache.exe
mklink bin\src_func.exe ..\build\release\src_func.exe
mklink bin\src_hdr.exe ..\build\release\src_hdr.exe

echo "#" 
echo "# Build completed successfully!"
exit /b 0

:: ------------------------

:create_link
if not exist %2 mkdir %2
if exist %1 (
    pushd %1
    if "!CD!"=="%~f2" (
        popd
        echo Link for %1 already exists and is correct.
    ) else (
        popd
        echo Removing existing link or directory %1
        if exist %1\NUL (
            rmdir %1
        ) else (
            del %1
        )
        mklink /J %1 %2
        if errorlevel 1 goto :error
    )
) else (
    mklink /D %1 %2
    if errorlevel 1 goto :error
)
goto :eof

:compile_if_changed
set source=%1
set object=%2
if exist !object! (
    for %%F in (!source!) do set source_time=%%~tF
    for %%F in (!object!) do set object_time=%%~tF
    if "!source_time!" leq "!object_time!" (
        :: echo !object! is up to date
        goto :eof
    )
)
echo Compiling !source!...
cl.exe %COMPILE_FLAGS% /c !source! /Fo:!object! 2>&1 | findstr /v /c:"Microsoft (R) C/C++ Optimizing Compiler" /c:"Copyright (C) Microsoft Corporation"
if errorlevel 1 goto :error
goto :eof

:link_if_changed
set exe=%1
shift
set objs=
set libs=
:parse_args
if "%1"=="" goto done_parsing
if "%~x1"==".lib" (
    set libs=!libs! %1
) else (
    set objs=!objs! %1
)
shift
goto parse_args
:done_parsing

if exist !exe! (
    set rebuild=0
    for %%F in (!objs!) do (
        if exist %%F (
            for %%G in (%%F) do if %%~tG gtr %%~t!exe! set rebuild=1
        )
    )
    if !rebuild! equ 0 (
        echo !exe! is up to date
        goto :eof
    )
)
echo Linking !exe!...
link.exe !objs! !libs! /OUT:!exe! /SUBSYSTEM:CONSOLE /FORCE:UNRESOLVED /LIBPATH:"%VCTOOLSINSTALLDIR%lib\x64" /LIBPATH:"%WindowsSdkDir%lib\%WindowsSDKVersion%um\x64" /LIBPATH:"%WindowsSdkDir%lib\%WindowsSDKVersion%ucrt\x64" 2>&1 | findstr /v /c:"Microsoft (R) C/C++ Optimizing Compiler" /c:"Copyright (C) Microsoft Corporation"
if errorlevel 1 goto :error
goto :eof

:error
echo Build failed!
exit /b 1