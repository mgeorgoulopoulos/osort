if not exist build mkdir build
cd build
cmake ..
cmake --build .

if exist osort del /F/Q/S osort\*
if not exist osort mkdir osort
copy Debug\osort.exe osort
copy ..\TestList.txt osort
copy ..\README.md osort
if exist ..\osort.zip del ..\osort.zip
tar.exe -a -c -f ..\osort.zip osort
cd ..
pause