@echo off
setlocal
if not exist build ( mkdir build )

move Debug build\
move CMakeFiles build\
move phosphor.dir build\
move x64 build\
move ALL_BUILD.vcxproj build\
move ALL_BUILD.vcxproj.filters build\
move cmake_install.cmake build\
move CMakeCache.txt build\
move phosphor.sln build\
move phosphor.vcxproj build\
move phosphor.vcxproj.filters build\
move ZERO_CHECK.vcxproj build\
move ZERO_CHECK.vcxproj.filters build\

endlocal