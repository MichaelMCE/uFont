@echo off


del *.bak
del *.o
del *.s

rem del *.a
rem del mylcd.dll
rem del libmylcd.a
rem del libmylcd.a
del *.bmp
rem del *.tga
rem del *.pgm
rem del *.png
rem del *.raw
rem del *.rgb
del *.exe

rem del libs\libpng.o
del libs\fio\fileio.o
rem del libs\libuf.o
rem del libs\ufont_primitives.o

rem del libs\libpng.o
del libs\fio\fileio.bak
rem del libs\libuf.o
rem del libs\ufont_primitives.o


cd libs
del *.bak
del *.o
cd ..