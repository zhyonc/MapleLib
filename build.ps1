if (Test-Path build) {
    Remove-Item -Recurse -Force build
}
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A Win32