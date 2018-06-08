make clean
#mingw32-env
CROSS_COMPILE=i686-w64-mingw32- mingw32-make $* -f Makefile.mingw
cp display display-win32/display.exe
echo
echo Result is in display-win32/display.exe
echo
