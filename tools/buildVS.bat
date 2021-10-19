set CMAKE_EXE=tools\cmake\bin\cmake.exe
set BUILD_DIR=build
set BUILD_CONFIG="RelWithDebInfo"

cd ..
%CMAKE_EXE% -G "Visual Studio 16 2019" -A x64 -S . -B %BUILD_DIR%