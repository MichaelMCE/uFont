

CC=gcc


x64 := 0
ifeq ($(x64),1)
MDIR=x64
BITMODE=-m64
else
MDIR=x32
BITMODE=-m32
endif

# Graphite loop optimizations
#GLOP = -ftree-vectorize -floop-interchange -floop-strip-mine -floop-block
GLOP =

CFLAGS = -flto -Wno-pointer-sign -DNO_DUMMY_DECL $(BITMODE) -I./include -Wall -march=i686 -mtune=i686 -O2 -DSTBI_FAILURE_USERMSG -D_WIN32 -D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -D__WIN32__=1 -std=gnu99 -funroll-loops -finline-functions -fomit-frame-pointer -pipe -s -DUSE_MMX -DHAVE_MMX -DHAVE_MMX1 -DUSE_MMX1 -mmmx -msse -mfpmath=sse,387 -fgcse-las -fgcse-sm -fgcse-lm -fmodulo-sched-allow-regmoves -fmodulo-sched -ftree-vectorizer-verbose=0 $(GLOP) -Wno-strict-aliasing 
#CFLAGS = $(BITMODE) -static-libgcc -I./include  -D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -D__WIN32__=1 -std=gnu99  -Wall -march=i686 -mtune=i686 -O2

LIBS = $(BITMODE) -flto -static-libgcc -L./libs/ -L"lib" -lm -lmylcddll -lole32 -luuid

PNG = libs/libpng/png.o \
	libs/libpng/pngerror.o \
	libs/libpng/pngget.o \
	libs/libpng/pngmem.o \
	libs/libpng/pngpread.o \
	libs/libpng/pngread.o \
	libs/libpng/pngrio.o \
	libs/libpng/pngrtran.o \
	libs/libpng/pngrutil.o \
	libs/libpng/pngset.o \
	libs/libpng/pngtrans.o \
	libs/libpng/pngwio.o \
	libs/libpng/pngwrite.o \
	libs/libpng/pngwtran.o \
	libs/libpng/pngwutil.o

ZLIB = libs/libpng/zutil.o \
	libs/libpng/inftrees.o \
	libs/libpng/inflate.o \
	libs/libpng/inffast.o \
	libs/libpng/crc32.o \
	libs/libpng/adler32.o


UFONTOBJECTS = libs/fio/fileio.o \
	libs/libuf.o \
	libs/libpng.o \
	libs/ufont_primitives.o

LIBOBJECTS = $(UFONTOBJECTS) $(ZLIB) $(PNG) 

EXAMPLES=glyph.exe clock.exe mystify.exe ball.exe cube.exe palettes.exe helloworld.exe uflookup.exe \
		charrefs.exe utf8demo.exe bdf2ufont.exe multifont.exe stars.exe fontfun.exe anidemo.exe shapes.exe \
		dump.exe utf8combining.exe spritetext.exe wrap.exe

all : $(EXAMPLES)


multifont.exe: $(LIBOBJECTS)
ufdemo.exe: $(LIBOBJECTS)
utf8demo.exe: $(LIBOBJECTS)
shapes.exe: $(LIBOBJECTS)
anidemo.exe: $(LIBOBJECTS)
stars.exe: $(LIBOBJECTS)
spritetext.exe: $(LIBOBJECTS)
fontfun.exe: $(LIBOBJECTS)
utf8combining.exe: $(LIBOBJECTS)
charrefs.exe: $(LIBOBJECTS)
uflookup.exe: $(LIBOBJECTS)
helloworld.exe: $(LIBOBJECTS)
palettes.exe: $(LIBOBJECTS)
cube.exe: $(LIBOBJECTS)
ball.exe: $(LIBOBJECTS)
mystify.exe: $(LIBOBJECTS)
clock.exe: $(LIBOBJECTS)
dump.exe: $(LIBOBJECTS)
glyph.exe: $(LIBOBJECTS)
wrap.exe: $(LIBOBJECTS)


# -static-libgcc
%.exe: %.o 
	$(CC)  -o $@ $^ $(LIBS) 
	strip.exe $@

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f *.exe *.o *.bak


### PHONY define
.PHONY: all all-before all-after clean clean-custom



