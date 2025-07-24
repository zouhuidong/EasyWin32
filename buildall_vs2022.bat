rem x86
mkdir build_vs2022_x86 && cd build_vs2022_x86
cmake -G "Visual Studio 17 2022" -A Win32 ..
cmake --build . --config Release
cd ..

rem x64
mkdir build_vs2022_x64 && cd build_vs2022_x64
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
cd ..