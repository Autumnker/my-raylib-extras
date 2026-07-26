@echo off
setlocal

:: ===================================================================
::  rmouse example — build & run  (MinGW-w64 + raylib 5.5)
:: ===================================================================

set "RAYLIB_DIR=..\..\..\raylib-5.5_win64_mingw-w64"
set "SRC=main.cpp"
set "OUT=rmouse_example.exe"

set "INCLUDE=-I..\..\src\rmouse -I%RAYLIB_DIR%\include"
set "LIB=-L%RAYLIB_DIR%\lib"
set "LINK=-lraylib -lopengl32 -lgdi32 -lwinmm"
set "CXXFLAGS=-std=c++17 -O2 -Wall"

echo.
echo === Cleaning previous build ===
if exist "%OUT%" del "%OUT%"

echo.
echo === Building %SRC% ===
g++ %CXXFLAGS% %INCLUDE% %LIB% %SRC% -o "%OUT%" %LINK%

if %ERRORLEVEL% neq 0 (
    echo.
    echo === BUILD FAILED ===
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo === Build OK: %OUT% ===
echo.
echo === Running... ===
echo.

"%OUT%"

endlocal
