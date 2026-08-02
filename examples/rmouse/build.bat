@echo off
setlocal

:: ===================================================================
::  rmouse examples — build & run  (MinGW-w64 + raylib 5.5)
:: ===================================================================

set "RAYLIB_DIR=..\..\..\raylib-5.5_win64_mingw-w64"
set "INCLUDE=-I..\..\src\rmouse -I%RAYLIB_DIR%\include"
set "LIB=-L%RAYLIB_DIR%\lib"
set "LINK=-lraylib -lopengl32 -lgdi32 -lwinmm"

:: ===================================================================
::  C++ example
:: ===================================================================
set "SRC=example_rmouse.cpp"
set "OUT=rmouse_cpp_example.exe"
set "FLAGS=-std=c++17 -O2 -Wall"

echo.
echo === [C++] Cleaning previous build ===
if exist "%OUT%" del "%OUT%"

echo.
echo === [C++] Building %SRC% ===
g++ %FLAGS% %INCLUDE% %LIB% %SRC% -o "%OUT%" %LINK%

if %ERRORLEVEL% neq 0 (
    echo.
    echo === [C++] BUILD FAILED ===
    pause
    exit /b %ERRORLEVEL%
)

echo === [C++] Build OK: %OUT% ===
echo.
echo === [C++] Running... ===
echo.

"%OUT%"

:: ===================================================================
::  C example
:: ===================================================================
set "SRC=example_rmouse.c"
set "OUT=rmouse_c_example.exe"
set "FLAGS=-std=c11 -O2 -Wall"

echo.
echo === [C] Cleaning previous build ===
if exist "%OUT%" del "%OUT%"

echo.
echo === [C] Building %SRC% ===
gcc %FLAGS% %INCLUDE% %LIB% %SRC% -o "%OUT%" %LINK%

if %ERRORLEVEL% neq 0 (
    echo.
    echo === [C] BUILD FAILED ===
    pause
    exit /b %ERRORLEVEL%
)

echo === [C] Build OK: %OUT% ===
echo.
echo === [C] Running... ===
echo.

"%OUT%"

endlocal
