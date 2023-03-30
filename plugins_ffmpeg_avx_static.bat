echo off
set PATH=C:/gcc1131/bin;%PATH%
rem echo %PATH%
rem cd "C:/gcc1131/bin"

md lib 


cd libavcodec
cd librsvg/fontconfig
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DLIBXML_STATIC=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/freetype
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -DTT_CONFIG_OPTION_COLOR_LAYERS=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/freetype/autofit
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -DFT_CONFIG_OPTION_USE_HARFBUZZ=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/bdf
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/bzip2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/cache
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/cff
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/cid
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/dlg
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/gxvalid
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/gzip
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/lzw
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/otvalid
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/pcf
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/pfr
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/psaux
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/pshinter
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/psnames
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/raster
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/sdf
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/sfnt
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/smooth
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/svg
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/truetype
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/type1
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/type42
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/freetype/winfonts
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -D__FTERRORS_H__=1 -UFTERRORS_H_ -DFT2_BUILD_LIBRARY=1 -c %%f -o %%~nf.o
cd ../../..
cd librsvg/fribidi
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DFRIBIDI_LIB_STATIC=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGIZE=1 -DDONT_HAVE_FRIBIDI_UNICODE_VERSION_H=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/harfbuzz
for %%f in ("harfbuzz.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DCAIRO_WIN32_STATIC_BUILD=1 -DLIBXML_STATIC=1 -DFRIBIDI_LIB_STATIC=1 -DHAVE_CAIRO=1 -DHAVE_FREETYPE=1 -DNHAVE_GLIB -DNHAVE_GOBJECT -DNDONT_HAVE_FRIBIDI_UNICODE_VERSION_H -c %%f -o %%~nf.o
cd ../..
cd librsvg/libxml2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DLIBXML_STATIC=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/cairo
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DCAIRO_WIN32_STATIC_BUILD=1 -DLIBXML_STATIC=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/pixman
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DPACKAGE=1 -DUSE_X86_MMX=1 -DUSE_SSE2=1 -DUSE_SSSE3=1 -c %%f -o %%~nf.o
cd ../..

cd librsvg
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DRSVG_STATIC=1 -DRSVG_COMPILATION=1 -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DLIBXML_STATIC=1 -c %%f -o %%~nf.o
cd ..
cd librsvg/glib
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DLIBXML_STATIC=1 -DPCRE2_STATIC=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/gio
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DLIBXML_STATIC=1 -DPCRE2_STATIC=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/gobject
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DLIBXML_STATIC=1 -DPCRE2_STATIC=1 -DX86_WIN64=1 -DFFI_EFI64=0 -DFFI_UNIX64=0 -DFFI_STATIC_BUILD=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/gdk-pixbuf
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/gmodule
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/gvdb
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/pango
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DFRIBIDI_LIB_STATIC=1 -DLIBXML_STATIC=1 -DSYSCONFDIR=\"/etc\" -DLIBDIR=\"/pango\" -DDONT_HAVE_FRIBIDI_UNICODE_VERSION_H -c %%f -o %%~nf.o
echo for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DFRIBIDI_LIB_STATIC=1 -DLIBXML_STATIC=1 -DSYSCONFDIR=\"/etc\" -DLIBDIR=\"/pango\" -DDONT_HAVE_FRIBIDI_UNICODE_VERSION_H -c %%f -o %%~nf.o
cd ../..
cd librsvg/pcre2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DPCRE2_STATIC=1 -DPCRE2_CODE_UNIT_WIDTH=8 -DHAVE_CONFIG_H -c %%f -o %%~nf.o
cd ../..
cd librsvg/libcroco
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DG_LIB_STATIC=1 -DGLIB_STATIC_COMPILATION=1 -DLIBXML_STATIC=1 -c %%f -o %%~nf.o
cd ../..

cd librsvg/libffi
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DX86_WIN64=1 -DFFI_EFI64=0 -DFFI_UNIX64=0 -DFFI_STATIC_BUILD=1 -c %%f -o %%~nf.o
echo for %%f in ("%~dp1*.S") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DX86_WIN64=1 -DFFI_EFI64=0 -DFFI_UNIX64=0 -DFFI_STATIC_BUILD=1 -c %%f -o %%~nf.o
cd ../..
cd librsvg/fnmatch
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../..
cd librsvg/libintl
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DNIN_LIBASPRINTF -DNO_XMALLOC -DHAVE_CONFIG_H -DHAVE_STDINT_H_WITH_UINTMAX -DHAVE_GETCWD -DUSE_WINDOWS_THREADS=1 -DHAVE_MEMPCPY=1 -DHAVE_TSEARCH=1 -DNSTATIC="" -DLOCALEDIR=\"/usr/share/locale\" -DLOCALE_ALIAS_PATH=\"/usr/share/locale/locale.alias\" -DLIBDIR=\"/libintl\" -DIN_LIBINTL -DLIBINTL_VERSION=0x001308 -DNWIDE_CHAR_VERSION -c %%f -o %%~nf.o
cd ../..
cd librsvg/libpng
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DPNG_DEBUG=1 -DNZLIB_WINAPI -DPNG_DLL_EXPORT=/"/" -c %%f -o %%~nf.o
cd ../..
cd librsvg/zlib
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DNZLIB_DLL -c %%f -o %%~nf.o
cd ../..

cd libtwolame
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DLIBTWOLAME_STATIC=1 -c %%f -o %%~nf.o
cd ..
cd libogg
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ..
cd libtheora/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DCAIRO_WIN32_STATIC_BUILD=1 -DHAVE_CAIRO=1 -DNOC_DUMP_IMAGES -DNOC_COLLECT_METRICS -DNHAVE_MEMORY_CONSTRAINT -DOC_STATE_USE_VTABLE=1 -DOC_X86_ASM=1 -DOC_X86_64_ASM=1 -c %%f -o %%~nf.o
cd ../..
cd libtheora
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DCAIRO_WIN32_STATIC_BUILD=1 -DHAVE_CAIRO=1 -DNOC_DUMP_IMAGES -DNOC_COLLECT_METRICS -DNHAVE_MEMORY_CONSTRAINT -DOC_STATE_USE_VTABLE=1 -DOC_X86_ASM=1 -DOC_X86_64_ASM=1 -c %%f -o %%~nf.o
cd ..
cd libspeex
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DHAVE_SIMD=1 -c %%f -o %%~nf.o
cd ..
cd libsnappy
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DSNAPPY_HAVE_SSSE3=1 -DNSNAPPY_HAVE_BMI2 -c %%f -o %%~nf.o
cd ..
cd libshine
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ..
cd libmp3lame
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DSTDC_HEADERS=1 -DHAVE_ERRNO_H=1 -DHAVE_FCNTL_H=1 -DHAVE_STDINT_H=1 -DNHAVE_NASM -DHAVE_XMMINTRIN_H=1 -DUSE_FAST_LOG=1 -c %%f -o %%~nf.o
cd ..
cd libgsm
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DNeedFunctionPrototypes=1 -DSASR -DWAV49 -c %%f -o %%~nf.o
cd ..
cd libvorbis
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DHAVE_ERRNO_H=1 -DHAVE_STDINT_H=1 -D__PTHREAD_JUMBO_BUILD__=1 -c %%f -o %%~nf.o
cd ..
cd libcelt2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DNUSE_SIMD -c %%f -o %%~nf.o
cd ..
cd libcelt
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -c %%f -o %%~nf.o
cd ..
cd libcelt/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -c %%f -o %%~nf.o
cd ../..
cd libopus
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -c %%f -o %%~nf.o
cd ..
cd libsilk
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -c %%f -o %%~nf.o
cd ..
cd libsilk/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -c %%f -o %%~nf.o
cd ../..
cd libsilk/fixed
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -c %%f -o %%~nf.o
cd ../..
cd libsilk/fixed/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -c %%f -o %%~nf.o
cd ../../..
cd libkvazaar
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DKVZ_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -DHAVE_ERRNO_H=1 -DHAVE_STDINT_H=1 -D__PTHREAD_JUMBO_BUILD__=1 -c %%f -o %%~nf.o
cd ..
cd libkvazaar/altivec
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DKVZ_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ../..
cd libkvazaar/generic
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DKVZ_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ../..
cd libkvazaar/sse2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DKVZ_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ../..
cd libkvazaar/sse41
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DKVZ_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ../..
echo cd libkvazaar/avx2
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DKVZ_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
echo cd ../..
cd libkvazaar/x86_asm
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DHAVE_ALIGNED_STACK=1 -Xgnu %%f -o %%~nf.o
cd ../..
cd libuvg266
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DUVG_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -DHAVE_ERRNO_H=1 -DHAVE_STDINT_H=1 -D__PTHREAD_JUMBO_BUILD__=1 -c %%f -o %%~nf.o
cd ..
cd libuvg266/altivec
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DUVG_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ../..
cd libuvg266/sse2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DUVG_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ../..
cd libuvg266/sse41
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DHAVE_CONFIG_H=1 -DUVG_STATIC_LIB=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ../..
cd libpthread_win32
for %%f in ("pthread.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_CONFIG_H=1 -DHAVE_ERRNO_H=1 -DHAVE_STDINT_H=1 -D__PTHREAD_JUMBO_BUILD__=1 -DPTW32_STATIC_LIB=1 -c %%f -o %%~nf.o
cd ..
cd libopenjpeg
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -D__SSE__ -D__SSE2__ -DOPJ_STATIC=1 -DUSE_JPIP -c %%f -o %%~nf.o
cd ..
cd liblcms2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DCMS_IS_WINDOWS_ -DCMS_RELY_ON_WINDOWS_STATIC_MUTEX_INIT -D__LITTLE_ENDIAN__ -c %%f -o %%~nf.o
cd ..
cd libwebp/dec
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DWEBP_USE_THREAD -DWEBP_EXTERN=extern -c %%f -o %%~nf.o
cd ../..
cd libwebp/demux
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DWEBP_USE_THREAD -DWEBP_EXTERN=extern -c %%f -o %%~nf.o
cd ../..
cd libwebp/dsp
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DWEBP_USE_THREAD -DWEBP_EXTERN=extern -c %%f -o %%~nf.o
cd ../..
cd libwebp/enc
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DWEBP_USE_THREAD -DWEBP_EXTERN=extern -c %%f -o %%~nf.o
cd ../..
cd libwebp/mux
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DWEBP_USE_THREAD -DWEBP_EXTERN=extern -c %%f -o %%~nf.o
cd ../..
cd libwebp/sharpyuv
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DWEBP_USE_THREAD -DWEBP_EXTERN=extern -c %%f -o %%~nf.o
cd ../..
cd libwebp/utils
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DWEBP_USE_THREAD -DWEBP_EXTERN=extern -c %%f -o %%~nf.o
cd ../..
cd libaribb24
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ..
cd libcodec2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ..
cd libxavs
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DARCH_X86_64=1 -DXAVS_VERSION=2 -DHAVE_STDINT_H=1 -D__WIN32__=1 -c %%f -o %%~nf.o
cd ..
cd libxavs2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DSYS_WINDOWS=1 -DHAVE_THREAD=1 -DHAVE_WIN32THREAD=1 -DHAVE_INTERLACED=1 -DHAVE_STDINT_H=1 -DHAVE_LOG2F=1 -c %%f -o %%~nf.o
cd ..
cd libxavs2/vec
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DSYS_WINDOWS=1 -DHAVE_THREAD=1 -DHAVE_WIN32THREAD=1 -DHAVE_INTERLACED=1 -DHAVE_STDINT_H=1 -DHAVE_LOG2F=1 -c %%f -o %%~nf.o
cd ../..
cd libxavs2/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DHIGH_BIT_DEPTH=0 -Xgnu %%f -o %%~nf.o
cd ../..
cd libdavs2
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=10 -DHIGH_BIT_DEPTH=1 -DHAVE_WIN32THREAD=1 -DSYS_WINDOWS=1 -c %%f -o %%~nf.o
cd ..
cd libdavs2/vec
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=10 -DHIGH_BIT_DEPTH=1 -DHAVE_WIN32THREAD=1 -DSYS_WINDOWS=1 -c %%f -o %%~nf.o
cd ../..
cd libdavs2/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=10 -DHIGH_BIT_DEPTH=1 -Xgnu %%f -o %%~nf.o
cd ../..
cd libuavs3d
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DARCH_X86_64=1 -DCOMPILE_10BIT=1 -DUAVS3D_STATIC=1 -c %%f -o %%~nf.o
cd ..
cd libuavs3d/sse
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DARCH_X86_64=1 -DCOMPILE_10BIT=1 -DUAVS3D_STATIC=1 -c %%f -o %%~nf.o
cd ../..
echo cd libuavs3d/avx2
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DCOMPILE_10BIT=0 -c %%f -o %%~nf.o
echo cd ../..
cd libuavs3e
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DARCH_X86_64=1 -DCOMPILE_10BIT=0 -DUAVS3E_STATIC=1 -c %%f -o %%~nf.o
cd ..
cd libuavs3e/sse
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DARCH_X86_64=1 -DCOMPILE_10BIT=0 -DUAVS3E_STATIC=1 -c %%f -o %%~nf.o
cd ../..
cd libxvid
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DARCH_IS_64BIT=1 -DARCH_IS_X86_64=1 -c %%f -o %%~nf.o
cd ..
cd libxvid/x86_asm
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DWINDOWS=1 -DARCH_IS_X86_64=1 -DNO_PREFIX=1 -Xgnu %%f -o %%~nf.o
cd ../..
cd libx264
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DHIGH_BIT_DEPTH=0 -DSYS_WINDOWS=1 -DHAVE_THREAD=1 -DHAVE_WIN32THREAD=1 -DHAVE_INTERLACED=1 -DHAVE_STDINT_H=1 -DHAVE_LOG2F=1 -DHAVE_X86_INLINE_ASM=1 -c %%f -o %%~nf.o
cd ..
cd libx264/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DHIGH_BIT_DEPTH=0 -DSYS_WINDOWS=1 -DHAVE_THREAD=1 -DHAVE_WIN32THREAD=1 -DHAVE_INTERLACED=1 -DHAVE_STDINT_H=1 -DHAVE_LOG2F=1 -DHAVE_X86_INLINE_ASM=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DHIGH_BIT_DEPTH=0 -Dprivate_prefix=x264_8 -Xgnu %%f -o %%~nf.o
for %%f in ("cpu-a.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DHIGH_BIT_DEPTH=0 -Dprivate_prefix=x264 -Xgnu %%f -o %%~nf.o
cd ../..
cd libx264_10bit
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=10 -DHIGH_BIT_DEPTH=1 -DSYS_WINDOWS=1 -DHAVE_THREAD=1 -DHAVE_WIN32THREAD=1 -DHAVE_INTERLACED=1 -DHAVE_STDINT_H=1 -DHAVE_LOG2F=1 -DHAVE_X86_INLINE_ASM=1 -c %%f -o %%~nf.o
cd ..
cd libx264_10bit/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_MMX=1 -DARCH_X86_64=1 -DBIT_DEPTH=10 -DHIGH_BIT_DEPTH=1 -DSYS_WINDOWS=1 -DHAVE_THREAD=1 -DHAVE_WIN32THREAD=1 -DHAVE_INTERLACED=1 -DHAVE_STDINT_H=1 -DHAVE_LOG2F=1 -DHAVE_X86_INLINE_ASM=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=10 -DHIGH_BIT_DEPTH=1 -Dprivate_prefix=x264_10 -Xgnu %%f -o %%~nf.o
cd ../..
cd libopenh264/processing
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DX86_ASM -DNHAVE_AVX2 -c %%f -o %%~nf.o
cd ../..
cd libopenh264/processing/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DWIN64=1 -DNHAVE_AVX2 -Xgnu %%f -o %%~nf.o
cd ../../..
cd libopenh264/decoder
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DX86_ASM -DNHAVE_AVX2 -c %%f -o %%~nf.o
cd ../..
cd libopenh264/decoder/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DWIN64=1 -DNHAVE_AVX2 -Xgnu %%f -o %%~nf.o
cd ../../..
cd libopenh264/decoder/x86_dec
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DWIN64=1 -DNHAVE_AVX2 -Xgnu %%f -o %%~nf.o
cd ../../..
cd libopenh264/encoder
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DX86_ASM -DNHAVE_AVX2 -c %%f -o %%~nf.o
cd ../..
cd libopenh264/encoder/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DWIN64=1 -DNHAVE_AVX2 -Xgnu %%f -o %%~nf.o
cd ../../..
cd libx265/vec
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=1 -DX265_NS=x265 -DLINKED_10BIT=1 -DLINKED_12BIT=1 -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=0 -DX265_DEPTH=8 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ../..
cd libx265_10bit/vec
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=0 -DX265_NS=x265_10bit -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=1 -DX265_DEPTH=10 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ../..
cd libx265_12bit/vec
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=0 -DX265_NS=x265_12bit -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=1 -DX265_DEPTH=12 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ../..
cd libx265
cd x86
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=1 -DX265_NS=x265 -DLINKED_10BIT=1 -DLINKED_12BIT=1 -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=0 -DX265_DEPTH=8 -DNX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -fwin64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=8 -DHIGH_BIT_DEPTH=0 -DX265_NS=x265 -DPIC=1 -DSUFFIX=o -Xgnu -w-macro-params-legacy %%f -o %%~nf.o
cd ..
cd input
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=1 -DX265_NS=x265 -DLINKED_10BIT=1 -DLINKED_12BIT=1 -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=0 -DX265_DEPTH=8 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ..
cd output
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=1 -DX265_NS=x265 -DLINKED_10BIT=1 -DLINKED_12BIT=1 -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=0 -DX265_DEPTH=8 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ..
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=1 -DX265_NS=x265 -DLINKED_10BIT=1 -DLINKED_12BIT=1 -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=0 -DX265_DEPTH=8 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ..
cd libx265_10bit
cd x86
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=0 -DX265_NS=x265_10bit -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=1 -DX265_DEPTH=10 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -fwin64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=10 -DHIGH_BIT_DEPTH=1 -DX265_NS=x265_10bit -DPIC=1 -DSUFFIX=o -Xgnu -w-macro-params-legacy %%f -o %%~nf.o
cd ..
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=0 -DX265_NS=x265_10bit -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=1 -DX265_DEPTH=10 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ..
cd libx265_12bit
cd x86
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=0 -DX265_NS=x265_12bit -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=1 -DX265_DEPTH=12 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -fwin64 -O3 -DARCH_X86_64=1 -DBIT_DEPTH=12 -DHIGH_BIT_DEPTH=1 -DX265_NS=x265_12bit -DPIC=1 -DSUFFIX=o -Xgnu -w-macro-params-legacy %%f -o %%~nf.o
cd ..
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=0 -DX265_NS=x265_12bit -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=1 -DX265_DEPTH=12 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -DHAVE_STRTOK_R -D_DEBUG=1 -c %%f -o %%~nf.o
cd ..
cd libx265/dynamicHDR10
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -D_WIN32_WINNT_WIN7=0x0601 -DEXPORT_C_API=1 -DX265_NS=x265 -DLINKED_10BIT=1 -DLINKED_12BIT=1 -DX86_64=1 -DX265_VERSION=3.5+86-6da609e -DHIGH_BIT_DEPTH=0 -DX265_DEPTH=8 -DX265_ARCH_X86=1 -DENABLE_HDR10_PLUS=1 -DENABLE_ASSEMBLY=1 -D_DEBUG=1 -c %%f -o %%~nf.o
cd ../..

cd libxevd
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DWINDOWS_MUTEX_SYNC=0 -c %%f -o %%~nf.o
cd ..
echo cd libxevd/x86
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DWINDOWS_MUTEX_SYNC=0 -c %%f -o %%~nf.o
echo cd ../..
cd libxeve
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DWINDOWS_MUTEX_SYNC=0 -DENABLE_IBC_CHROMA_REFINE=0 -c %%f -o %%~nf.o
cd ..
echo cd libxeve/x86
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWIN32 -DWIN64 -DWINDOWS_MUTEX_SYNC=0 -DENABLE_IBC_CHROMA_REFINE=0 -c %%f -o %%~nf.o
echo cd ../..


cd libsvtav1
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNCONFIG_ACCOUNTING -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ..
cd libsvtav1/ASM_SSE2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNCONFIG_ACCOUNTING -DARCH_X86_64=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DCONFIG_PIC=1 -DCONFIG_AV1_ENCODER=1 -DNCONFIG_AV1_DECODER=0 -DNHAVE_AVX2 -Xgnu %%f -o %%~nf.o
cd ../..
cd libsvtav1/ASM_SSSE3
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNCONFIG_ACCOUNTING -DARCH_X86_64=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DCONFIG_PIC=1 -DCONFIG_AV1_ENCODER=1 -DNCONFIG_AV1_DECODER=0 -DNHAVE_AVX2 -Xgnu %%f -o %%~nf.o
cd ../..
cd libsvtav1/ASM_SSE4_1
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNCONFIG_ACCOUNTING -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../..
echo cd libsvtav1/ASM_AVX2
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNCONFIG_ACCOUNTING -DARCH_X86_64=1 -c %%f -o %%~nf.o
echo for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_32=0 -DARCH_X86_64=1 -DWIN64=1 -DPIC=1 -DFORCE_VEX_ENCODING=0 -Dprivate_prefix=svt_dav1d -Dpublic_prefix=svt_dav1d -Xgnu %%f -o %%~nf.o
echo cd ../..
cd libsvtav1/cpuinfo
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNOMINMAX -DNCONFIG_ACCOUNTING -DARCH_X86_64=1 -DCPUINFO_MOCK=1 -c %%f -o %%~nf.o
cd ../..

cd libvpx/vpx_dsp
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../..
cd libvpx/vpx_mem
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../..
cd libvpx/vpx_scale
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../..
cd libvpx/vpx_util
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../..
cd libvpx/vp9
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../..
cd libvpx/vp9/common
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../../..
cd libvpx/vp9/decoder
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../../..
cd libvpx/vp9/encoder
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../../..

cd libaom/aom
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../..
cd libaom/av1c
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../..
cd libaom/av1c/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../../..
echo cd libaom/av1c/x86/avx2
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
echo cd ../../../..
cd libaom/av1dsp
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../..
cd libaom/av1dsp/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_64=1 -DCONFIG_PIC=1 -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DNHAVE_AVX2 -Xgnu %%f -o %%~nf.o
cd ../../..
echo cd libaom/av1dsp/x86/avx2
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
echo cd ../../../..
cd libaom/av1dsp/flow_estimation
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../../..
cd libaom/av1dsp/flow_estimation/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../../../..
echo cd libaom/av1dsp/flow_estimation/x86/avx2
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
echo cd ../../../../..
cd libaom/av1d
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../..
cd libaom/av1e
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../..
cd libaom/av1e/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
cd ../../..
echo cd libaom/av1e/x86/avx2
echo for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DCONFIG_AV1_ENCODER=1 -DCONFIG_AV1_DECODER=1 -DCONFIG_ACCOUNTING=1 -DCONFIG_INSPECTION=1 -DHAVE_SIMD=1 -DARCH_X86_64=1 -c %%f -o %%~nf.o
echo cd ../../../..

cd libdav1d
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -D__USE_MINGW_ANSI_STDIO=1 -DUNICODE=1 -D_UNICODE=1 -D_CRT_DECLARE_NONSTDC_NAMES=1 -DARCH_X86=1 -DARCH_X86_32=0 -DARCH_X86_64=1 -DHAVE_ASM=1 -DCONFIG_8BPC -DCONFIG_16BPC -DBITDEPTH=8 -DHAVE_ALIGNED_MALLOC=1 -c %%f -o %%~nf.o
cd ..
cd libdav1d/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -D__USE_MINGW_ANSI_STDIO=1 -DUNICODE=1 -D_UNICODE=1 -D_CRT_DECLARE_NONSTDC_NAMES=1 -DARCH_X86=1 -DARCH_X86_32=0 -DARCH_X86_64=1 -DHAVE_ASM=1 -DCONFIG_8BPC -DCONFIG_16BPC -DBITDEPTH=8 -DHAVE_ALIGNED_MALLOC=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_32=0 -DARCH_X86_64=1 -DWIN64=1 -DPIC=1 -DFORCE_VEX_ENCODING=0 -Dprivate_prefix=dav1d -Dpublic_prefix=dav1d -Xgnu %%f -o %%~nf.o
cd ../..
cd libdav1d/16bit
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -D__USE_MINGW_ANSI_STDIO=1 -DUNICODE=1 -D_UNICODE=1 -D_CRT_DECLARE_NONSTDC_NAMES=1 -DARCH_X86=1 -DARCH_X86_32=0 -DARCH_X86_64=1 -DHAVE_ASM=1 -DCONFIG_16BPC -DBITDEPTH=16 -DHAVE_ALIGNED_MALLOC=1 -c %%f -o %%~nf.o
cd ../..

cd libjxl/brotli/enc
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DBROTLI_HAVE_LOG2=1 -c %%f -o %%~nf.o
cd ../../..
cd libjxl/brotli/dec
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DBROTLI_HAVE_LOG2=1 -c %%f -o %%~nf.o
cd ../../..
cd libjxl/brotli/common
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DBROTLI_HAVE_LOG2=1 -c %%f -o %%~nf.o
cd ../../..

cd libjxl
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -DJPEGXL_VERSION=\"0.8.0-bc61a82\" -c %%f -o %%~nf.o
cd ..
cd libjxl/base
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../..
cd libjxl/butteraugli
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../..
cd libjxl/jpeg
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../..
cd libjxl/modular
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../..
cd libjxl/modular/encoding
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../../..
cd libjxl/modular/transform
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../../..
cd libjxl/render_pipeline
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../..
cd libjxl/dec
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../..
cd libjxl/enc
for %%f in ("%~dp1*.cc") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -D_WIN32_WINNT=0x0602 -DWIN32_LEAN_AND_MEAN=/"/" -DNJPEGXL_ENABLE_SKCMS -DJXL_DEBUG_WARNING=1 -DJXL_CRASH_ON_ERROR=1 -DJXL_DEBUG_ON_ALL_ERROR=1 -DHWY_COMPILE_ONLY_STATIC -c %%f -o %%~nf.o
cd ../..

cd librsvg/zlib
ar.exe rcs "../../../lib/zlib_x64.a" adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o gzwrite.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
cd ../..
cd librsvg/libpng
ar.exe rcs "../../../lib/libpng_x64.a" png.o pngerror.o pngget.o pngmem.o pngpread.o pngread.o pngrio.o pngrtran.o pngrutil.o pngset.o pngtrans.o pngwio.o pngwrite.o pngwtran.o pngwutil.o
cd ../..
cd librsvg/libxml2
ar.exe rcs "../../../lib/libxml2_x64.a" buf.o catalog.o chvalid.o dict.o encoding.o entities.o error.o globals.o hash.o HTMLtree.o list.o nanohttp.o parser.o parserInternals.o pattern.o SAX2.o threads.o tree.o uri.o valid.o xmlIO.o xmlmemory.o xpath.o xmlregexp.o xmlsave.o xmlstring.o xmlunicode.o debugXML.o nanoftp.o relaxng.o HTMLparser.o SAX.o xmlschemas.o xmlschemastypes.o xmlreader.o xinclude.o xpointer.o 
cd ../..
cd librsvg/fontconfig
ar.exe rcs "../../../lib/fontconfig_x64.a" fcarch.o fcatomic.o fccache.o fccfg.o fccharset.o fccompat.o fcdbg.o fcdefault.o fcdir.o fcformat.o fcfreetype.o fcfs.o fchash.o fcinit.o fclang.o fclist.o fcmatch.o fcmatrix.o fcname.o fcobjs.o fcpat.o fcptrlist.o fcrange.o fcserialize.o fcstat.o fcstr.o fcweight.o fcxml.o ftglue.o 
cd ../..
cd librsvg/freetype
ar.exe rcs "../../../lib/freetype_x64.a" ftadvanc.o ftbbox.o ftbdf.o ftbitmap.o ftcalc.o ftcolor.o ftdbgmem.o ftdebug.o fterrors.o ftfntfmt.o ftfstype.o ftgasp.o ftgloadr.o ftglyph.o ftgxval.o fthash.o ftinit.o ftlcdfil.o ftmac.o ftmm.o ftobjs.o ftotval.o ftoutln.o ftpatent.o ftpfr.o ftpsprop.o ftrfork.o ftsnames.o ftstream.o ftstroke.o ftsynth.o ftsystem.o fttrigon.o fttype1.o ftutil.o ftwinfnt.o md5.o autofit/afblue.o autofit/afcjk.o autofit/afdummy.o autofit/afglobal.o autofit/afhints.o autofit/afindic.o autofit/aflatin.o autofit/afloader.o autofit/afmodule.o autofit/afranges.o autofit/afshaper.o autofit/ft-hb.o bdf/bdfdrivr.o bdf/bdflib.o bzip2/ftbzip2.o cache/ftcbasic.o cache/ftccache.o cache/ftccmap.o cache/ftcglyph.o cache/ftcimage.o cache/ftcmanag.o cache/ftcmru.o cache/ftcsbits.o cff/cffcmap.o cff/cffdrivr.o cff/cffgload.o cff/cffload.o cff/cffobjs.o cff/cffparse.o cid/cidgload.o cid/cidload.o cid/cidobjs.o cid/cidparse.o cid/cidriver.o gxvalid/gxvbsln.o gxvalid/gxvcommn.o gxvalid/gxvfeat.o gxvalid/gxvfgen.o gxvalid/gxvjust.o gxvalid/gxvkern.o gxvalid/gxvlcar.o gxvalid/gxvmod.o gxvalid/gxvmort.o gxvalid/gxvmort0.o gxvalid/gxvmort1.o gxvalid/gxvmort2.o gxvalid/gxvmort4.o gxvalid/gxvmort5.o gxvalid/gxvmorx.o gxvalid/gxvmorx0.o gxvalid/gxvmorx1.o gxvalid/gxvmorx2.o gxvalid/gxvmorx4.o gxvalid/gxvmorx5.o gxvalid/gxvopbd.o gxvalid/gxvprop.o gxvalid/gxvtrak.o gzip/ftgzip.o lzw/ftlzw.o otvalid/otvbase.o otvalid/otvcommn.o otvalid/otvgdef.o otvalid/otvgpos.o otvalid/otvgsub.o otvalid/otvjstf.o otvalid/otvmath.o otvalid/otvmod.o pcf/pcfdrivr.o pcf/pcfread.o pcf/pcfutil.o pfr/pfrcmap.o pfr/pfrdrivr.o pfr/pfrgload.o pfr/pfrload.o pfr/pfrobjs.o pfr/pfrsbit.o psaux/afmparse.o psaux/cffdecode.o psaux/psarrst.o psaux/psauxmod.o psaux/psblues.o psaux/psconv.o psaux/pserror.o psaux/psfont.o psaux/psft.o psaux/pshints.o psaux/psintrp.o psaux/psobjs.o psaux/psread.o psaux/psstack.o psaux/t1cmap.o psaux/t1decode.o pshinter/pshalgo.o pshinter/pshglob.o pshinter/pshmod.o pshinter/pshrec.o psnames/psmodule.o raster/ftraster.o raster/ftrend1.o sdf/ftbsdf.o sdf/ftsdf.o sdf/ftsdfcommon.o sdf/ftsdfrend.o sfnt/sfdriver.o sfnt/sfobjs.o sfnt/sfwoff.o sfnt/sfwoff2.o sfnt/ttbdf.o sfnt/ttcmap.o sfnt/ttcolr.o sfnt/ttcpal.o sfnt/ttkern.o sfnt/ttload.o sfnt/ttmtx.o sfnt/ttpost.o sfnt/ttsbit.o sfnt/ttsvg.o sfnt/woff2tags.o smooth/ftgrays.o smooth/ftsmooth.o svg/ftsvg.o truetype/ttdriver.o truetype/ttgload.o truetype/ttgxvar.o truetype/ttinterp.o truetype/ttobjs.o truetype/ttpload.o truetype/ttsubpix.o type1/t1afm.o type1/t1driver.o type1/t1gload.o type1/t1load.o type1/t1objs.o type1/t1parse.o type42/t42drivr.o type42/t42objs.o type42/t42parse.o winfonts/winfnt.o 
cd ../..
cd librsvg/fribidi
ar.exe rcs "../../../lib/fribidi_x64.a" fribidi.o fribidi-bidi.o fribidi-bidi-types.o fribidi-char-sets.o fribidi-char-sets-cap-rtl.o fribidi-char-sets-cp1255.o fribidi-char-sets-cp1256.o fribidi-char-sets-iso8859-6.o fribidi-char-sets-iso8859-8.o fribidi-char-sets-utf8.o fribidi-deprecated.o fribidi-joining-types.o fribidi-run.o fribidi-brackets.o fribidi-joining.o fribidi-mirroring.o fribidi-shape.o fribidi-arabic.o 
cd ../..
cd librsvg/harfbuzz
ar.exe rcs "../../../lib/harfbuzz_x64.a" harfbuzz.o
cd ../..
cd librsvg
ar.exe rcs "../../lib/libcairo_pixman_x64.a" cairo/cairo.o cairo/cairo-analysis-surface.o cairo/cairo-array.o cairo/cairo-base64-stream.o cairo/cairo-base85-stream.o cairo/cairo-boxes.o cairo/cairo-boxes-intersect.o cairo/cairo-cache.o cairo/cairo-cff-subset.o cairo/cairo-clip.o cairo/cairo-clip-region.o cairo/cairo-clip-boxes.o cairo/cairo-color.o cairo/cairo-composite-rectangles.o cairo/cairo-compositor.o cairo/cairo-damage.o cairo/cairo-debug.o cairo/cairo-default-context.o cairo/cairo-deflate-stream.o cairo/cairo-device.o cairo/cairo-error.o cairo/cairo-font-face.o cairo/cairo-font-face-twin.o cairo/cairo-font-options.o cairo/cairo-freed-pool.o cairo/cairo-gstate.o cairo/cairo-hash.o cairo/cairo-image-info.o cairo/cairo-image-surface.o cairo/cairo-lzw.o cairo/cairo-matrix.o cairo/cairo-misc.o cairo/cairo-mono-scan-converter.o cairo/cairo-mutex.o cairo/cairo-observer.o cairo/cairo-output-stream.o cairo/cairo-paginated-surface.o cairo/cairo-path.o cairo/cairo-path-bounds.o cairo/cairo-path-fill.o cairo/cairo-path-fixed.o cairo/cairo-pattern.o cairo/cairo-pdf-interchange.o cairo/cairo-pdf-operators.o cairo/cairo-pdf-shading.o cairo/cairo-pdf-surface.o cairo/cairo-colr-glyph-render.o cairo/cairo-script-surface.o cairo/cairo-png.o cairo/cairo-polygon.o cairo/cairo-ps-surface.o cairo/cairo-raster-source-pattern.o cairo/cairo-recording-surface.o cairo/cairo-rectangle.o cairo/cairo-region.o cairo/cairo-scaled-font.o cairo/cairo-scaled-font-subsets.o cairo/cairo-spline.o cairo/cairo-stroke-style.o cairo/cairo-surface.o cairo/cairo-surface-clipper.o cairo/cairo-surface-snapshot.o cairo/cairo-surface-wrapper.o cairo/cairo-svg-glyph-render.o cairo/cairo-svg-surface.o cairo/cairo-tag-attributes.o cairo/cairo-tag-stack.o cairo/cairo-toy-font-face.o cairo/cairo-traps.o cairo/cairo-truetype-subset.o cairo/cairo-type1-fallback.o cairo/cairo-type1-glyph-names.o cairo/cairo-type1-subset.o cairo/cairo-type3-glyph-surface.o cairo/cairo-user-font.o cairo/cairo-unicode.o cairo/cairo-version.o cairo/cairo-xml-surface.o cairo/cairo-arc.o cairo/cairo-image-source.o cairo/cairo-image-compositor.o cairo/cairo-ft-font.o cairo/cairo-path-stroke.o cairo/cairo-pen.o cairo/cairo-stroke-dash.o cairo/cairo-slope.o cairo/cairo-mask-compositor.o cairo/cairo-traps-compositor.o cairo/cairo-shape-mask-compositor.o cairo/cairo-clip-surface.o cairo/cairo-surface-offset.o cairo/cairo-path-stroke-traps.o cairo/cairo-path-stroke-polygon.o cairo/cairo-path-stroke-boxes.o cairo/cairo-clip-polygon.o cairo/cairo-polygon-intersect.o cairo/cairo-bentley-ottmann.o cairo/cairo-wideint.o cairo/cairo-freelist.o cairo/cairo-line.o cairo/cairo-contour.o cairo/cairo-bentley-ottmann-rectangular.o cairo/cairo-spans-compositor.o cairo/cairo-mesh-pattern-rasterizer.o cairo/cairo-polygon-reduce.o cairo/cairo-rectangular-scan-converter.o cairo/cairo-tor22-scan-converter.o cairo/cairo-tor-scan-converter.o cairo/cairo-spans.o cairo/cairo-hull.o cairo/cairo-no-compositor.o cairo/cairo-path-in-fill.o cairo/cairo-bentley-ottmann-rectilinear.o cairo/cairo-font-face-twin-data.o cairo/cairo-fallback-compositor.o pixman/pixman.o pixman/pixman-access.o pixman/pixman-access-accessors.o pixman/pixman-arm.o pixman/pixman-bits-image.o pixman/pixman-combine-float.o pixman/pixman-combine32.o pixman/pixman-conical-gradient.o pixman/pixman-edge.o pixman/pixman-fast-path.o pixman/pixman-glyph.o pixman/pixman-image.o pixman/pixman-implementation.o pixman/pixman-general.o pixman/pixman-gradient-walker.o pixman/pixman-linear-gradient.o pixman/pixman-noop.o pixman/pixman-matrix.o pixman/pixman-mips.o pixman/pixman-ppc.o pixman/pixman-radial-gradient.o pixman/pixman-region16.o pixman/pixman-region32.o pixman/pixman-solid-fill.o pixman/pixman-trap.o pixman/pixman-utils.o pixman/pixman-x86.o pixman/pixman-edge-accessors.o pixman/pixman-mmx.o pixman/pixman-sse2.o pixman/pixman-ssse3.o
cd ..

cd librsvg
ar.exe rcs "../../lib/librsvg_x64.a" rsvg.o rsvg-base.o rsvg-base-file-util.o rsvg-cairo-clip.o rsvg-cairo-draw.o rsvg-cairo-render.o rsvg-cond.o rsvg-css.o rsvg-defs.o rsvg-file-util.o rsvg-filter.o rsvg-gobject.o rsvg-image.o rsvg-io.o rsvg-marker.o rsvg-mask.o rsvg-paint-server.o rsvg-path.o rsvg-shapes.o rsvg-size-callback.o rsvg-structure.o rsvg-styles.o rsvg-text.o rsvg-xml.o librsvg-enum-types.o glib\garcbox.o glib\garray.o glib\gasyncqueue.o glib\gbitlock.o glib\gbookmarkfile.o glib\gbytes.o glib\gcharset.o glib\gchecksum.o glib\gconvert.o glib\gdate.o glib\gdatetime.o glib\gdataset.o glib\gdir.o glib\gerror.o glib\genviron.o glib\gfileutils.o glib\ggettext.o glib\ghash.o glib\ghook.o glib\ghostutils.o glib\giochannel.o glib\giowin32.o glib\gkeyfile.o glib\glib-init.o glib\glib-private.o glib\glist.o glib\gmain.o glib\gmappedfile.o glib\gmarkup.o glib\gmem.o glib\gmessages.o glib\gnode.o glib\goption.o glib\gpattern.o glib\gprintf.o glib\gqsort.o glib\gquark.o glib\gqueue.o glib\grand.o glib\grefcount.o glib\gstdio.o glib\gslice.o glib\gslist.o glib\gstrfuncs.o glib\gstring.o glib\gtimezone.o glib\gtimer.o glib\gtestutils.o glib\gthread.o glib\gthreadpool.o glib\gthread-win32.o glib\gtranslit.o glib\gtree.o glib\gunidecomp.o glib\guniprop.o glib\guri.o glib\gutils.o glib\gutf8.o glib\gvariant.o glib\gvariant-core.o glib\gvariant-parser.o glib\gvariant-serialiser.o glib\gvarianttype.o glib\gvarianttypeinfo.o glib\gwakeup.o glib\gwin32.o glib\gsequence.o glib\gregex.o glib\localcharset.o glib\printf.o glib\grcbox.o glib\gshell.o glib\gpoll.o glib\gspawn-win32.o glib\vasnprintf.o glib\gbase64.o glib\gunibreak.o glib\printf-args.o glib\printf-parse.o glib\gunicollate.o gio\gioenumtypes.o gio\gpowerprofilemonitor.o gio\gwin32networkmonitor.o gio\gdummytlsbackend.o gio\gnetworkmonitorbase.o gio\ginetaddressmask.o gio\gtlsfiledatabase.o gio\gdtlsconnection.o gio\gdtlsclientconnection.o gio\gdtlsserverconnection.o gio\gtlsserverconnection.o gio\gfile.o gio\gmemoryinputstream.o gio\ginputstream.o gio\gcancellable.o gio\gbufferedinputstream.o gio\gzlibdecompressor.o gio\gconverterinputstream.o gio\gioerror.o gio\gpollableutils.o gio\gconverter.o gio\gfilterinputstream.o gio\gfileinfo.o gio\gtask.o gio\gasyncresult.o gio\gfileattribute.o gio\gicon.o gio\gemblem.o gio\gemblemedicon.o gio\gbytesicon.o gio\gthemedicon.o gio\gfileicon.o gio\gloadableicon.o gio\gvfs.o gio\gsimpleasyncresult.o gio\gpollableoutputstream.o gio\goutputstream.o gio\glocalfileoutputstream.o gio\gseekable.o gio\gpollableinputstream.o gio\glocalfileinfo.o gio\gfileoutputstream.o gio\gioscheduler.o gio\gresourcefile.o gio\glocalvfs.o gio\giomodule.o gio\gfileinputstream.o gio\glocalfileiostream.o gio\gappinfo.o gio\gcontenttype.o gio\glocalfile.o gio\gresource.o gio\gpollfilemonitor.o gio\gfileenumerator.o gio\gfilemonitor.o gio\thumbnail-verify.o gio\ginitable.o gio\gregistrysettingsbackend.o gio\gsettingsbackend.o gio\glocalfilemonitor.o gio\glocalfileinputstream.o gio\gcontextspecificgroup.o gio\gsocks4proxy.o gio\gsocks4aproxy.o gio\gsocks5proxy.o gio\gmemorymonitor.o gio\gdebugcontroller.o gio\gnotificationbackend.o gio\gtlsbackend.o gio\gproxyresolver.o gio\gnetworkmonitor.o gio\ghttpproxy.o gio\gwinhttpvfs.o gio\gwin32filemonitor.o gio\giostream.o gio\gproxyaddress.o gio\ginetaddress.o gio\gapplication.o gio\gactiongroup.o gio\gactionmap.o gio\gdbusmessage.o gio\gdbuserror.o gio\gdbusinterface.o gio\gdbusutils.o gio\gasyncinitable.o gio\gdbusmethodinvocation.o gio\gdbusintrospection.o gio\gdbusprivate.o gio\gactiongroupexporter.o gio\gmarshal-internal.o gio\gremoteactiongroup.o gio\gsocketconnection.o gio\gsocket.o gio\gmemorymonitorwin32.o gio\gkeyfilesettingsbackend.o gio\gvolumemonitor.o gio\gnativevolumemonitor.o gio\gsocketaddress.o gio\ginetsocketaddress.o gio\gsimplepermission.o gio\gvolume.o gio\gmount.o gio\gdrive.o gio\gdbusactiongroup.o gio\gsimpleactiongroup.o gio\gapplicationcommandline.o gio\gaction.o gio\gwin32inputstream.o gio\gpermission.o gio\gsocketconnectable.o gio\gunixconnection.o gio\gdbusauth.o gio\gdbusconnection.o gio\gapplicationimpl-dbus.o gio\gnativesocketaddress.o gio\gwin32fsmonitorutils.o gio\gnetworking.o gio\gsettings.o gio\gdbusauthobserver.o gio\gsettingsschema.o gio\gproxyaddressenumerator.o gio\gsocketaddressenumerator.o gio\gcredentials.o gio\gtlsconnection.o gio\gdelayedsettingsbackend.o gio\gsettings-mapping.o gio\gresolver.o gio\gproxy.o gio\gnetworkaddress.o gio\gtlsdatabase.o gio\gtlsinteraction.o gio\gtlscertificate.o gio\gsrvtarget.o gio\gthreadedresolver.o gio\gasynchelper.o gio\gdbusproxy.o gio\gwinhttpfile.o gio\gsimpleaction.o gio\gdummyproxyresolver.o gio\gwin32notificationbackend.o gio\gdbusaddress.o gio\gdbusobject.o gio\gtlsclientconnection.o gio\gdbusauthmechanismsha1.o gio\gdatainputstream.o gio\gsocketclient.o gio\gwinhttpfileinputstream.o gio\gunixsocketaddress.o gio\gdbusauthmechanism.o gio\gdataoutputstream.o gio\gtlspassword.o gio\gtcpconnection.o gio\gdatagrambased.o gio\gwinhttpfileoutputstream.o gio\gwin32sid.o gio\gunixcredentialsmessage.o gio\gnullsettingsbackend.o gio\gmemorysettingsbackend.o gio\gpowerprofilemonitordbus.o gio\gwin32volumemonitor.o gio\gnetworkservice.o gio\gtcpwrapperconnection.o gio\gfilteroutputstream.o gio\gdbusauthmechanismexternal.o gio\glistmodel.o gio\gnotification.o gio\gwin32mount.o gio\gdummyfile.o gio\gunionvolumemonitor.o gio\glocalfileenumerator.o gio\gsocketinputstream.o gio\gsocketoutputstream.o gio\gdbusauthmechanismanon.o gio\gsocketcontrolmessage.o gio\gdbusnamewatching.o gio\giomodule-priv.o gio\gwin32appinfo.o gio\gfileiostream.o gio\xdgmime.o gio\gwin32registrykey.o gio\gwin32packageparser.o gio\xdgmimeparent.o gio\xdgmimecache.o gio\xdgmimealias.o gio\xdgmimeicon.o gio\xdgmimeglob.o gio\xdgmimemagic.o gio\xdgmimeint.o gio\gwin32file-sync-stream.o gio\gdbusdaemon.o gio\gdbusserver.o gio\gdbusinterfaceskeleton.o gio\gsocketlistener.o gio\gthreadedsocketservice.o gio\gdbusobjectskeleton.o gio\gsocketservice.o gobject\gatomicarray.o gobject\gboxed.o gobject\gclosure.o gobject\genums.o gobject\gmarshal.o gobject\gobject.o gobject\gparam.o gobject\gparamspecs.o gobject\gsignal.o gobject\gtype.o gobject\gtypemodule.o gobject\gtypeplugin.o gobject\gvalue.o gobject\gvaluearray.o gobject\gvaluetransform.o gobject\gvaluetypes.o gobject\gsourceclosure.o gmodule\gmodule.o gvdb\gvdb-reader.o gdk-pixbuf\gdk-pixbuf.o gdk-pixbuf\gdk-pixbuf-data.o gdk-pixbuf\gdk-pixbuf-loader.o gdk-pixbuf\gdk-pixbuf-animation.o gdk-pixbuf\gdk-pixbuf-enum-types.o gdk-pixbuf\gdk-pixbuf-scaled-anim.o gdk-pixbuf\gdk-pixbuf-simple-anim.o gdk-pixbuf\gdk-pixbuf-io.o gdk-pixbuf\gdk-pixdata.o gdk-pixbuf\gdk-pixbuf-util.o gdk-pixbuf\gdk-pixbuf-scale.o gdk-pixbuf\pixops.o libcroco\cr-additional-sel.o libcroco\cr-attr-sel.o libcroco\cr-doc-handler.o libcroco\cr-enc-handler.o libcroco\cr-input.o libcroco\cr-num.o libcroco\cr-parser.o libcroco\cr-parsing-location.o libcroco\cr-pseudo.o libcroco\cr-rgb.o libcroco\cr-selector.o libcroco\cr-simple-sel.o libcroco\cr-string.o libcroco\cr-term.o libcroco\cr-tknzr.o libcroco\cr-token.o libcroco\cr-utils.o pango\pango-layout.o pango\pango-context.o pango\pango-gravity.o pango\pango-fontmap.o pango\pango-language.o pango\fonts.o pango\pangofc-decoder.o pango\pangofc-font.o pango\pangofc-fontmap.o pango\pangoft2.o pango\pangoft2-fontmap.o pango\pangoft2-render.o pango\pangocairo-fontmap.o pango\pangocairo-fcfont.o pango\pangocairo-fcfontmap.o pango\pangocairo-context.o pango\pango-tabs.o pango\glyphstring.o pango\pango-glyph-item.o pango\pango-enum-types.o pango\pango-attributes.o pango\pango-fontset-simple.o pango\pango-matrix.o pango\pango-utils.o pango\pango-item.o pango\break.o pango\pango-markup.o pango\itemize.o pango\pangocairo-render.o pango\shape.o pango\ellipsize.o pango\pango-renderer.o pango\pango-script.o pango\pango-color.o pango\pango-fontset.o pango\pango-coverage.o pango\pango-emoji.o pango\pangocairo-font.o pango\pango-bidi-type.o pcre2\pcre2_substring.o pcre2\pcre2_context.o pcre2\pcre2_match_data.o pcre2\pcre2_jit_compile.o pcre2\pcre2_pattern_info.o pcre2\pcre2_error.o pcre2\pcre2_compile.o pcre2\pcre2_match.o pcre2\pcre2_dfa_match.o pcre2\pcre2_config.o pcre2\pcre2_string_utils.o pcre2\pcre2_newline.o pcre2\pcre2_extuni.o pcre2\pcre2_xclass.o pcre2\pcre2_valid_utf.o pcre2\pcre2_study.o pcre2\pcre2_ord2utf.o pcre2\pcre2_script_run.o pcre2\pcre2_auto_possess.o pcre2\pcre2_find_bracket.o pcre2\pcre2_tables.o pcre2\pcre2_ucd.o pcre2\pcre2_convert.o pcre2\pcre2_chartables.o libffi\ffiw64.o libffi\win64.o libffi\closures.o libffi\prep_cif.o libffi\tramp.o libffi\types.o libintl\bindtextdom.o libintl\dcgettext.o libintl\dcigettext.o libintl\dcngettext.o libintl\dgettext.o libintl\dngettext.o libintl\explodename.o libintl\finddomain.o libintl\gettext.o libintl\hash-string.o libintl\intl-compat.o libintl\l10nflist.o libintl\langprefs.o libintl\loadmsgcat.o libintl\localealias.o libintl\localename.o libintl\lock.o libintl\log.o libintl\ngettext.o libintl\plural.o libintl\plural-exp.o libintl\relocatable.o libintl\textdomain.o fnmatch\fnmatch.o harfbuzz\harfbuzz.o
cd ..

cd libogg
ar.exe rcs "../../lib/libogg_x64.a" bitwise.o framing.o
cd ..
cd libtheora
ar.exe rcs "../../lib/libtheora_x64.a" analyze.o apiwrapper.o bitpack.o decapiwrapper.o decinfo.o decode.o dequant.o encapiwrapper.o encfrag.o encinfo.o encode.o enquant.o fdct.o fragment.o huffdec.o huffenc.o idct.o info.o internal.o mathops.o mcenc.o quant.o rate.o state.o tokenize.o x86/mmxencfrag.o x86/mmxfdct.o x86/mmxfrag.o x86/mmxidct.o x86/mmxstate.o x86/sse2encfrag.o x86/sse2fdct.o x86/sse2idct.o x86/x86cpu.o x86/x86enc.o x86/x86enquant.o x86/x86state.o
cd ..
cd libspeex
ar.exe rcs "../../lib/libspeex_x64.a" bits.o cb_search.o exc_5_64_table.o exc_5_256_table.o exc_8_128_table.o exc_10_16_table.o exc_10_32_table.o exc_20_32_table.o fftwrap.o filters.o gain_table.o gain_table_lbr.o hexc_10_32_table.o hexc_table.o high_lsp_tables.o kiss_fft.o kiss_fftr.o lpc.o lsp.o lsp_tables_nb.o ltp.o modes.o modes_wb.o nb_celp.o quant_lsp.o sb_celp.o smallft.o speex.o speex_callbacks.o speex_header.o stereo.o vbr.o vorbis_psy.o vq.o window.o
cd ..
cd libsnappy
ar.exe rcs "../../lib/libsnappy_x64.a" snappy.o snappy-c.o snappy-sinksource.o snappy-stubs-internal.o
cd ..
cd libshine
ar.exe rcs "../../lib/libshine_x64.a" bitstream.o huffman.o l3bitstream.o l3loop.o l3mdct.o l3subband.o layer3.o reservoir.o tables.o
cd ..
cd libmp3lame
ar.exe rcs "../../lib/libmp3lame_x64.a" bitstream.o encoder.o fft.o gain_analysis.o id3tag.o lame.o mpglib_interface.o newmdct.o presets.o psymodel.o quantize.o quantize_pvt.o reservoir.o set_get.o tables.o takehiro.o util.o vbrquantize.o VbrTag.o version.o xmm_quantize_sub.o
cd ..
cd libgsm
ar.exe rcs "../../lib/libgsm_x64.a" add.o code.o debug.o decode.o gsm_create.o gsm_decode.o gsm_destroy.o gsm_encode.o gsm_explode.o gsm_implode.o gsm_option.o gsm_print.o long_term.o lpc.o preprocess.o rpe.o short_term.o table.o
cd ..
cd libvorbis
ar.exe rcs "../../lib/libvorbis_x64.a" vorbisenc.o analysis.o block.o bitrate.o codebook.o envelope.o floor0.o floor1.o info.o lpc.o lsp.o mapping0.o mdct.o psy.o registry.o res0.o sharedbook.o smallft.o synthesis.o window.o 
cd ..
cd libtwolame
ar.exe rcs "../../lib/libtwolame_x64.a" twolame.o ath.o availbits.o bitbuffer.o crc.o encode.o energy.o dab.o fft.o get_set.o mem.o psycho_0.o psycho_1.o psycho_2.o psycho_3.o psycho_4.o psycho_n1.o subband.o util.o
cd ..
cd libopenjpeg
ar.exe rcs "../../lib/libopenjpeg_x64.a" bio.o cio.o cidx_manager.o dwt.o event.o function_list.o ht_dec.o image.o invert.o j2k.o jp2.o mct.o mqc.o openjpeg.o opj_clock.o opj_malloc.o phix_manager.o pi.o ppix_manager.o sparse_array.o t1.o t2.o tcd.o tgt.o thix_manager.o thread.o tpix_manager.o
cd ..
cd liblcms2
ar.exe rcs "../../lib/liblcms2_x64.a" cmsalpha.o cmscam02.o cmscgats.o cmscnvrt.o cmserr.o cmsgamma.o cmsgmt.o cmshalf.o cmsintrp.o cmsio0.o cmsio1.o cmslut.o cmsmd5.o cmsmtrx.o cmsnamed.o cmsopt.o cmspack.o cmspcs.o cmsplugin.o cmsps2.o cmssamp.o cmssm.o cmstypes.o cmsvirt.o cmswtpnt.o cmsxform.o
cd ..
cd libwebp
ar.exe rcs "../../lib/libwebp_x64.a" enc\alpha_enc.o enc\analysis_enc.o enc\backward_references_cost_enc.o enc\backward_references_enc.o enc\config_enc.o enc\cost_enc.o enc\filter_enc.o enc\frame_enc.o enc\histogram_enc.o enc\iterator_enc.o enc\near_lossless_enc.o enc\picture_csp_enc.o enc\picture_enc.o enc\picture_psnr_enc.o enc\picture_rescale_enc.o enc\picture_tools_enc.o enc\predictor_enc.o enc\quant_enc.o enc\syntax_enc.o enc\token_enc.o enc\tree_enc.o enc\vp8l_enc.o enc\webp_enc.o sharpyuv\sharpyuv.o sharpyuv\sharpyuv_cpu.o sharpyuv\sharpyuv_csp.o sharpyuv\sharpyuv_dsp.o sharpyuv\sharpyuv_gamma.o sharpyuv\sharpyuv_sse2.o utils\bit_reader_utils.o utils\bit_writer_utils.o utils\color_cache_utils.o utils\filters_utils.o utils\huffman_encode_utils.o utils\huffman_utils.o utils\quant_levels_dec_utils.o utils\quant_levels_utils.o utils\random_utils.o utils\rescaler_utils.o utils\thread_utils.o utils\utils.o dsp\alpha_processing.o dsp\alpha_processing_sse2.o dsp\alpha_processing_sse41.o dsp\cost.o dsp\cost_sse2.o dsp\cpu.o dsp\dec.o dsp\dec_sse2.o dsp\dec_sse41.o dsp\dec_clip_tables.o dsp\enc.o dsp\enc_sse2.o dsp\enc_sse41.o dsp\filters.o dsp\filters_sse2.o dsp\lossless.o dsp\lossless_sse2.o dsp\lossless_sse41.o dsp\lossless_enc.o dsp\lossless_enc_sse2.o dsp\lossless_enc_sse41.o dsp\rescaler.o dsp\rescaler_sse2.o dsp\ssim.o dsp\ssim_sse2.o dsp\upsampling.o dsp\upsampling_sse2.o dsp\upsampling_sse41.o dsp\yuv.o dsp\yuv_sse2.o dsp\yuv_sse41.o dec\alpha_dec.o dec\buffer_dec.o dec\frame_dec.o dec\idec_dec.o dec\io_dec.o dec\quant_dec.o dec\tree_dec.o dec\vp8_dec.o dec\vp8l_dec.o dec\webp_dec.o demux\anim_decode.o demux\demux.o mux\anim_encode.o mux\muxedit.o mux\muxinternal.o mux\muxread.o
cd ..
cd libaribb24
ar.exe rcs "../../lib/libaribb24_x64.a" aribb24.o decoder.o drcs.o md5.o parser.o
cd ..
cd libpthread_win32
ar.exe rcs "../../lib/libpthread_x64.a" pthread.o
cd ..
cd libkvazaar
ar.exe rcs "../../lib/libkvazaar_x64.a" bitstream.o cabac.o cfg.o checkpoint.o cli.o constraint.o context.o cu.o encode_coding_tree.o encoder.o encoder_state-bitstream.o encoder_state-ctors_dtors.o encoder_state-geometry.o encoderstate.o fast_coeff_cost.o filter.o image.o imagelist.o input_frame_buffer.o inter.o intra.o kvazaar.o libmd5.o ml_classifier_intra_depth_pred.o ml_intra_cu_depth_pred.o nal.o rate_control.o rdo.o sao.o scalinglist.o search.o search_inter.o search_intra.o strategies-dct.o strategies-encode.o strategies-intra.o strategies-ipol.o strategies-nal.o strategies-picture.o strategies-quant.o strategies-sao.o strategyselector.o tables.o threadqueue.o transform.o videoframe.o yuv_io.o generic\dct-generic.o generic\encode_coding_tree-generic.o generic\intra-generic.o generic\ipol-generic.o generic\nal-generic.o generic\picture-generic.o generic\quant-generic.o generic\sao-generic.o sse2\picture-sse2.o sse41\picture-sse41.o x86_asm\picture-x86-asm.o x86_asm\picture-x86-asm-sad.o x86_asm\picture-x86-asm-satd.o altivec\picture-altivec.o
cd ..
cd libuvg266
ar.exe rcs "../../lib/libuvg266_x64.a" alf.o alf-generic.o dct-generic.o encode_coding_tree-generic.o intra-generic.o ipol-generic.o nal-generic.o picture-generic.o quant-generic.o sao-generic.o bitstream.o cabac.o cfg.o cli.o constraint.o context.o cu.o encode_coding_tree.o encoder.o encoder_state-bitstream.o encoder_state-ctors_dtors.o encoder_state-geometry.o encoderstate.o fast_coeff_cost.o filter.o image.o imagelist.o input_frame_buffer.o inter.o intra.o libmd5.o ml_intra_cu_depth_pred.o nal.o rate_control.o rdo.o reshape.o sao.o scalinglist.o search.o search_inter.o search_intra.o strategies-alf.o strategies-dct.o strategies-encode.o strategies-intra.o strategies-ipol.o strategies-nal.o strategies-picture.o strategies-quant.o strategies-sao.o strategyselector.o tables.o threadqueue.o transform.o uvg266.o videoframe.o yuv_io.o sse2\picture-sse2.o sse41\alf-sse41.o sse41\picture-sse41.o
cd ..
cd libcelt2
ar.exe rcs "../../lib/libcelt_x64.a" bands.o celt.o cwrs.o dump_modes.o entcode.o entdec.o entenc.o header.o kiss_fft.o laplace.o mathops.o mdct.o modes.o pitch.o plc.o quant_bands.o rate.o vq.o
cd ..

ar.exe rcs "../lib/libcelt_opus_silk_x64.a" libcelt\bands.o libcelt\celt.o libcelt\celt_decoder.o libcelt\celt_encoder.o libcelt\celt_lpc.o libcelt\cwrs.o libcelt\entcode.o libcelt\entdec.o libcelt\entenc.o libcelt\kiss_fft.o libcelt\laplace.o libcelt\mathops.o libcelt\mdct.o libcelt\modes.o libcelt\pitch.o libcelt\quant_bands.o libcelt\rate.o libcelt\vq.o libcelt\x86\celt_lpc_sse4_1.o libcelt\x86\pitch_sse.o libcelt\x86\pitch_sse2.o libcelt\x86\pitch_sse4_1.o libcelt\x86\vq_sse2.o libcelt\x86\x86_celt_map.o libcelt\x86\x86cpu.o libopus\analysis.o libopus\mapping_matrix.o libopus\mlp.o libopus\mlp_data.o libopus\opus.o libopus\opus_decoder.o libopus\opus_encoder.o libopus\opus_multistream.o libopus\opus_multistream_decoder.o libopus\opus_multistream_encoder.o libopus\opus_projection_decoder.o libopus\opus_projection_encoder.o libopus\repacketizer.o libsilk\A2NLSF.o libsilk\ana_filt_bank_1.o libsilk\biquad_alt.o libsilk\bwexpander.o libsilk\bwexpander_32.o libsilk\check_control_input.o libsilk\CNG.o libsilk\code_signs.o libsilk\control_audio_bandwidth.o libsilk\control_codec.o libsilk\control_SNR.o libsilk\debug.o libsilk\dec_API.o libsilk\decode_core.o libsilk\decode_frame.o libsilk\decode_indices.o libsilk\decode_parameters.o libsilk\decode_pitch.o libsilk\decode_pulses.o libsilk\decoder_set_fs.o libsilk\enc_API.o libsilk\encode_indices.o libsilk\encode_pulses.o libsilk\gain_quant.o libsilk\HP_variable_cutoff.o libsilk\init_decoder.o libsilk\init_encoder.o libsilk\inner_prod_aligned.o libsilk\interpolate.o libsilk\lin2log.o libsilk\log2lin.o libsilk\LP_variable_cutoff.o libsilk\LPC_analysis_filter.o libsilk\LPC_fit.o libsilk\LPC_inv_pred_gain.o libsilk\NLSF_decode.o libsilk\NLSF_del_dec_quant.o libsilk\NLSF_encode.o libsilk\NLSF_stabilize.o libsilk\NLSF_unpack.o libsilk\NLSF_VQ.o libsilk\NLSF_VQ_weights_laroia.o libsilk\NLSF2A.o libsilk\NSQ.o libsilk\NSQ_del_dec.o libsilk\pitch_est_tables.o libsilk\PLC.o libsilk\process_NLSFs.o libsilk\quant_LTP_gains.o libsilk\resampler.o libsilk\resampler_down2.o libsilk\resampler_down2_3.o libsilk\resampler_private_AR2.o libsilk\resampler_private_down_FIR.o libsilk\resampler_private_IIR_FIR.o libsilk\resampler_private_up2_HQ.o libsilk\resampler_rom.o libsilk\shell_coder.o libsilk\sigm_Q15.o libsilk\sort.o libsilk\stereo_decode_pred.o libsilk\stereo_encode_pred.o libsilk\stereo_find_predictor.o libsilk\stereo_LR_to_MS.o libsilk\stereo_MS_to_LR.o libsilk\stereo_quant_pred.o libsilk\sum_sqr_shift.o libsilk\table_LSF_cos.o libsilk\tables_gain.o libsilk\tables_LTP.o libsilk\tables_NLSF_CB_NB_MB.o libsilk\tables_NLSF_CB_WB.o libsilk\tables_other.o libsilk\tables_pitch_lag.o libsilk\tables_pulses_per_block.o libsilk\VAD.o libsilk\VQ_WMat_EC.o libsilk\x86\NSQ_del_dec_sse4_1.o libsilk\x86\NSQ_sse4_1.o libsilk\x86\VAD_sse4_1.o libsilk\x86\VQ_WMat_EC_sse4_1.o libsilk\x86\x86_silk_map.o libsilk\fixed\apply_sine_window_FIX.o libsilk\fixed\autocorr_FIX.o libsilk\fixed\burg_modified_FIX.o libsilk\fixed\corrMatrix_FIX.o libsilk\fixed\encode_frame_FIX.o libsilk\fixed\find_LPC_FIX.o libsilk\fixed\find_LTP_FIX.o libsilk\fixed\find_pitch_lags_FIX.o libsilk\fixed\find_pred_coefs_FIX.o libsilk\fixed\k2a_FIX.o libsilk\fixed\k2a_Q16_FIX.o libsilk\fixed\LTP_analysis_filter_FIX.o libsilk\fixed\LTP_scale_ctrl_FIX.o libsilk\fixed\noise_shape_analysis_FIX.o libsilk\fixed\pitch_analysis_core_FIX.o libsilk\fixed\process_gains_FIX.o libsilk\fixed\regularize_correlations_FIX.o libsilk\fixed\residual_energy_FIX.o libsilk\fixed\residual_energy16_FIX.o libsilk\fixed\schur_FIX.o libsilk\fixed\schur64_FIX.o libsilk\fixed\vector_ops_FIX.o libsilk\fixed\warped_autocorrelation_FIX.o libsilk\fixed\x86\burg_modified_FIX_sse4_1.o libsilk\fixed\x86\vector_ops_FIX_sse4_1.o

cd libcodec2
ar.exe rcs "../../lib/libcodec2_x64.a" codec2.o codec2_fft.o interp.o kiss_fft.o kiss_fftr.o lpc.o lsp.o mbest.o newamp1.o newamp2.o nlp.o pack.o phase.o postfilter.o quantise.o sine.o codebook.o codebookd.o codebookdt.o codebookge.o codebookjmv.o codebookjvm.o codebooklspmelvq.o codebookmel.o codebooknewamp1.o codebooknewamp1_energy.o codebooknewamp2.o codebooknewamp2_energy.o codebookres.o codebookvq.o
cd ..
cd libxavs
ar.exe rcs "../../lib/libxavs_x64.a" analyse.o cavlc.o common.o cpu.o csp.o dct.o encoder.o eval.o frame.o lookahead.o macroblock.o macroblock_enc.o mc.o mdate.o me.o pixel.o predict.o quant.o ratecontrol.o set.o set_enc.o
cd ..
cd libxavs2
ar.exe rcs "../../lib/libxavs2_x64.a" aec.o aec_ctx.o aec_fastrdo.o aec_rdo.o aec_vrdo.o alf.o block_info.o cg_scan.o common.o common_arm.o cpu.o cudata.o encoder.o encoder_report.o filter_alf.o filter_deblock.o filter_sao.o frame.o header.o intra.o mc.o md_inter.o md_intra.o me.o parameters.o pixel.o pre_encode.o presets.o primitives.o quant.o ratecontrol.o rdo.o rdoq.o rps.o sao.o slice.o tdrdo.o threadpool.o transform.o win32thread.o wquant.o wrapper.o xavs2.o xavs2_api.o xlist.o yuv_writer.o vec\intrinsic.o vec\intrinsic_alf.o vec\intrinsic_cg_scan.o vec\intrinsic_dct.o vec\intrinsic_deblock.o vec\intrinsic_idct.o vec\intrinsic_inter_pred.o vec\intrinsic_intra-filledge.o vec\intrinsic_intra-pred.o vec\intrinsic_mad.o vec\intrinsic_pixel.o vec\intrinsic_quant.o vec\intrinsic_sao.o x86\blockcopy8.o x86\const-a.o x86\cpu-a.o x86\dct8.o x86\mc-a.o x86\mc-a2.o x86\pixel-32.o x86\pixel-a.o x86\pixeladd8.o x86\pixel-util8.o x86\quant8.o x86\sad-a.o x86\sad-vpp.o x86\satd-a.o x86\ssd-a.o 
cd ..
cd libdavs2
ar.exe rcs "../../lib/libdavs2_x64.a" aec.o alf.o bitstream.o block_info.o common.o cpu.o cu.o davs2.o deblock.o decoder.o frame.o header.o intra.o mc.o memory.o pixel.o predict.o primitives.o quant.o sao.o threadpool.o transform.o win32thread.o vec\intrinsic.o vec\intrinsic_alf.o vec\intrinsic_deblock.o vec\intrinsic_idct.o vec\intrinsic_inter_pred.o vec\intrinsic_intra-filledge.o vec\intrinsic_intra-pred.o vec\intrinsic_pixel.o vec\intrinsic_pixel_avx.o vec\intrinsic_sao.o x86\blockcopy8.o x86\const-a.o x86\cpu-a.o x86\dct8.o x86\mc-a2.o x86\pixeladd8.o x86\quant8.o 
cd ..
cd libuavs3d
ar.exe rcs "../../lib/libuavs3d_x64.a" alf.o bitstream.o com_table.o com_util.o deblock.o dec_util.o inter_pred.o intra_pred.o inv_trans.o parser.o pic_manager.o recon.o sao.o threadpool.o uavs3d.o win32thread.o sse\alf_sse.o sse\deblock_sse.o sse\inter_pred_sse.o sse\intra_pred_sse.o sse\itrans_sse.o sse\pixel_sse.o sse\sao_sse.o sse\sse.o
cd ..
cd libuavs3e
ar.exe rcs "../../lib/libuavs3e_x64.a" alf.o analyze.o bitstream.o com_alf.o com_cost.o com_df.o com_ipred.o com_itdq.o com_mc.o com_refman.o com_sao.o com_tables.o com_thread.o com_thread_win32.o com_util.o entropy.o inter.o intra.o lookahead.o me.o quant.o ratectrl.o sao.o tables.o transform.o uavs3e.o util.o sse\alf_sse.o sse\cost_sse.o sse\deblock_sse.o sse\inter_pred_sse.o sse\intra_pred_sse.o sse\itdq_sse.o sse\pixel_sse.o sse\sao_sse.o sse\sse.o sse\util_sse.o 
cd ..
cd libxvid
ar.exe rcs "../../lib/libxvid_x64.a" xvid.o vop_type_decision.o timer.o simple_idct.o sad.o reduced.o quant_mpeg.o quant_matrix.o quant_h263.o qpel.o postprocessing.o plugin_ssim.o plugin_single.o plugin_psnrhvsm.o plugin_psnr.o plugin_lumimasking.o plugin_dump.o plugin_2pass2.o plugin_2pass1.o motion_comp.o mem_transfer.o mem_align.o mbtransquant.o mbprediction.o mbcoding.o interpolate8x8.o image.o idct.o gmc.o font.o fdct.o estimation_rd_based_bvop.o estimation_rd_based.o estimation_pvop.o estimation_gmc.o estimation_common.o estimation_bvop.o encoder.o emms.o decoder.o colorspace.o cbp.o bitstream.o x86_asm\cbp_mmx.o x86_asm\cbp_sse2.o x86_asm\colorspace_rgb_mmx.o x86_asm\colorspace_yuv_mmx.o x86_asm\colorspace_yuyv_mmx.o x86_asm\cpuid.o x86_asm\deintl_sse.o x86_asm\fdct_mmx_ffmpeg.o x86_asm\fdct_mmx_skal.o x86_asm\fdct_sse2_skal.o x86_asm\gmc_mmx.o x86_asm\idct_3dne.o x86_asm\idct_mmx.o x86_asm\idct_sse2_dmitry.o x86_asm\interlacing_mmx.o x86_asm\interpolate8x8_3dn.o x86_asm\interpolate8x8_3dne.o x86_asm\interpolate8x8_mmx.o x86_asm\interpolate8x8_xmm.o x86_asm\mem_transfer_3dne.o x86_asm\mem_transfer_mmx.o x86_asm\plugin_ssim-a.o x86_asm\postprocessing_mmx.o x86_asm\postprocessing_sse2.o x86_asm\qpel_mmx.o x86_asm\quantize_h263_3dne.o x86_asm\quantize_h263_mmx.o x86_asm\quantize_mpeg_mmx.o x86_asm\quantize_mpeg_xmm.o x86_asm\reduced_mmx.o x86_asm\sad_3dn.o x86_asm\sad_3dne.o x86_asm\sad_mmx.o x86_asm\sad_sse2.o x86_asm\sad_xmm.o
cd ..

ar.exe rcs "../lib/libx264_0810bit_x64.a" libx264\analyse.o libx264\api.o libx264\base.o libx264\bitstream.o libx264\cabac.o libx264\cabac_enc.o libx264\cavlc.o libx264\common.o libx264\cpu.o libx264\dct.o libx264\deblock.o libx264\encoder.o libx264\frame.o libx264\lookahead.o libx264\macroblock.o libx264\macroblock_enc.o libx264\mc.o libx264\me.o libx264\mvpred.o libx264\osdep.o libx264\pixel.o libx264\predict.o libx264\quant.o libx264\ratecontrol.o libx264\rectangle.o libx264\set.o libx264\set_enc.o libx264\slicetype-cl.o libx264\tables.o libx264\threadpool.o libx264\vlc.o libx264\win32thread.o libx264_10bit\analyse.o libx264_10bit\bitstream.o libx264_10bit\cabac.o libx264_10bit\cabac_enc.o libx264_10bit\cavlc.o libx264_10bit\common.o libx264_10bit\dct.o libx264_10bit\deblock.o libx264_10bit\encoder.o libx264_10bit\frame.o libx264_10bit\lookahead.o libx264_10bit\macroblock.o libx264_10bit\macroblock_enc.o libx264_10bit\mc.o libx264_10bit\me.o libx264_10bit\mvpred.o libx264_10bit\pixel.o libx264_10bit\predict.o libx264_10bit\quant.o libx264_10bit\ratecontrol.o libx264_10bit\rectangle.o libx264_10bit\set.o libx264_10bit\set_enc.o libx264_10bit\slicetype-cl.o libx264_10bit\threadpool.o libx264_10bit\vlc.o libx264\x86\bitstream-a.o libx264\x86\cabac-a.o libx264\x86\const-a.o libx264\x86\cpu-a.o libx264\x86\dct-64.o libx264\x86\dct-a.o libx264\x86\deblock-a.o libx264\x86\mc-a.o libx264\x86\mc-a2.o libx264\x86\mc-c.o libx264\x86\pixel-a.o libx264\x86\predict-a.o libx264\x86\predict-c.o libx264\x86\quant-a.o libx264\x86\sad-a.o libx264\x86\trellis-64.o libx264_10bit\x86\bitstream-a.o libx264_10bit\x86\cabac-a.o libx264_10bit\x86\const-a.o libx264_10bit\x86\cpu-a.o libx264_10bit\x86\dct-32.o libx264_10bit\x86\dct-64.o libx264_10bit\x86\dct-a.o libx264_10bit\x86\deblock-a.o libx264_10bit\x86\mc-a.o libx264_10bit\x86\mc-a2.o libx264_10bit\x86\mc-c.o libx264_10bit\x86\pixel-32.o libx264_10bit\x86\pixel-a.o libx264_10bit\x86\predict-a.o libx264_10bit\x86\predict-c.o libx264_10bit\x86\quant-a.o libx264_10bit\x86\sad16-a.o libx264_10bit\x86\sad-a.o libx264_10bit\x86\trellis-64.o 

cd libopenh264
ar.exe rcs "../../lib/libopenh264_x64.a" decoder\au_parser.o decoder\bit_stream.o decoder\cabac_decoder.o decoder\common_tables.o decoder\copy_mb.o decoder\cpu.o decoder\crt_util_safe_x.o decoder\deblocking.o decoder\deblocking_common.o decoder\decode_mb_aux.o decoder\decode_slice.o decoder\decoder.o decoder\decoder_core.o decoder\decoder_data_tables.o decoder\error_concealment.o decoder\expand_pic.o decoder\fmo.o decoder\get_intra_predictor.o decoder\intra_pred_common.o decoder\manage_dec_ref.o decoder\mc.o decoder\memmgr_nal_unit.o decoder\memory_align.o decoder\mv_pred.o decoder\parse_mb_syn_cabac.o decoder\parse_mb_syn_cavlc.o decoder\pic_queue.o decoder\rec_mb.o decoder\sad_common.o decoder\utils.o decoder\wels_decoder_thread.o decoder\welsCodecTrace.o decoder\welsDecoderExt.o decoder\WelsTaskThread.o decoder\WelsThread.o decoder\WelsThreadLib.o decoder\WelsThreadPool.o decoder\x86\cpuid.o decoder\x86\dct.o decoder\x86\deblock.o decoder\x86\expand_picture.o decoder\x86\intra_pred_com.o decoder\x86\mb_copy.o decoder\x86\mc_chroma.o decoder\x86\mc_luma.o decoder\x86\satd_sad.o decoder\x86_dec\dct.o decoder\x86_dec\intra_pred.o decoder\x86\vaa.o encoder\au_set.o encoder\deblocking.o encoder\decode_mb_aux.o encoder\DllEntry.o encoder\encode_mb_aux.o encoder\encoder.o encoder\encoder_data_tables.o encoder\encoder_ext.o encoder\get_intra_predictor.o encoder\md.o encoder\mv_pred.o encoder\nal_encap.o encoder\paraset_strategy.o encoder\picture_handle.o encoder\ratectl.o encoder\ref_list_mgr_svc.o encoder\sample.o encoder\set_mb_syn_cabac.o encoder\set_mb_syn_cavlc.o encoder\slice_multi_threading.o encoder\svc_base_layer_md.o encoder\svc_enc_slice_segment.o encoder\svc_encode_mb.o encoder\svc_encode_slice.o encoder\svc_mode_decision.o encoder\svc_motion_estimate.o encoder\svc_set_mb_syn_cabac.o encoder\svc_set_mb_syn_cavlc.o encoder\wels_preprocess.o encoder\wels_task_base.o encoder\wels_task_encoder.o encoder\wels_task_management.o encoder\welsEncoderExt.o encoder\x86\asm_inc.o encoder\x86\coeff.o encoder\x86\dct.o encoder\x86\intra_pred.o encoder\x86\matrix_transpose.o encoder\x86\memzero.o encoder\x86\quant.o encoder\x86\sample_sc.o encoder\x86\score.o processing\AdaptiveQuantization.o processing\BackgroundDetection.o processing\ComplexityAnalysis.o processing\denoise.o processing\denoise_filter.o processing\downsample.o processing\downsamplefuncs.o processing\imagerotate.o processing\imagerotatefuncs.o processing\memory.o processing\SceneChangeDetection.o processing\ScrollDetection.o processing\ScrollDetectionFuncs.o processing\vaacalcfuncs.o processing\vaacalculation.o processing\WelsFrameWork.o processing\WelsFrameWorkEx.o processing\x86\denoisefilter.o processing\x86\downsample_bilinear.o processing\x86\vaa.o
cd ..

ar.exe rcs "../lib/libx265_081012bit_x64.a" libx265\api.o libx265\dynamicHDR10\metadataFromJson.o libx265\dynamicHDR10\SeiMetadataDictionary.o libx265\json11.o libx265\dynamicHDR10\JsonHelper.o libx265\dynamicHDR10\api.o libx265\common.o libx265\encoder.o libx265\ratecontrol.o libx265\slicetype.o libx265\param.o libx265\motion.o libx265\reference.o libx265\scalinglist.o libx265\nal.o libx265\dpb.o libx265\piclist.o libx265\winxp.o libx265\frame.o libx265\frameencoder.o libx265\picyuv.o libx265\ringmem.o libx265\bitstream.o libx265\entropy.o libx265\cudata.o libx265\wavefront.o libx265\md5.o libx265\sei.o libx265\framefilter.o libx265\framedata.o libx265\lowres.o libx265\temporalfilter.o libx265\threading.o libx265\threadpool.o libx265\yuv.o libx265\sao.o libx265\search.o libx265\weightPrediction.o libx265\analysis.o libx265\pixel.o libx265\quant.o libx265\predict.o libx265\shortyuv.o libx265\level.o libx265\primitives.o libx265\slice.o libx265\bitcost.o libx265\ipfilter.o libx265\constants.o libx265\deblock.o libx265\dct.o libx265\version.o libx265\cpu.o libx265\lowpassdct.o libx265\intrapred.o libx265\loopfilter.o libx265_10bit\api.o libx265_10bit\common.o libx265_10bit\encoder.o libx265_10bit\ratecontrol.o libx265_10bit\slicetype.o libx265_10bit\param.o libx265_10bit\motion.o libx265_10bit\reference.o libx265_10bit\scalinglist.o libx265_10bit\nal.o libx265_10bit\dpb.o libx265_10bit\piclist.o libx265_10bit\winxp.o libx265_10bit\frame.o libx265_10bit\frameencoder.o libx265_10bit\picyuv.o libx265_10bit\ringmem.o libx265_10bit\bitstream.o libx265_10bit\entropy.o libx265_10bit\cudata.o libx265_10bit\wavefront.o libx265_10bit\md5.o libx265_10bit\sei.o libx265_10bit\framefilter.o libx265_10bit\framedata.o libx265_10bit\lowres.o libx265_10bit\temporalfilter.o libx265_10bit\threading.o libx265_10bit\threadpool.o libx265_10bit\yuv.o libx265_10bit\sao.o libx265_10bit\search.o libx265_10bit\weightPrediction.o libx265_10bit\analysis.o libx265_10bit\pixel.o libx265_10bit\quant.o libx265_10bit\predict.o libx265_10bit\shortyuv.o libx265_10bit\level.o libx265_10bit\primitives.o libx265_10bit\slice.o libx265_10bit\bitcost.o libx265_10bit\ipfilter.o libx265_10bit\constants.o libx265_10bit\deblock.o libx265_10bit\dct.o libx265_10bit\version.o libx265_10bit\cpu.o libx265_10bit\lowpassdct.o libx265_10bit\intrapred.o libx265_10bit\loopfilter.o libx265_12bit\api.o libx265_12bit\common.o libx265_12bit\encoder.o libx265_12bit\ratecontrol.o libx265_12bit\slicetype.o libx265_12bit\param.o libx265_12bit\ringmem.o libx265_12bit\motion.o libx265_12bit\reference.o libx265_12bit\scalinglist.o libx265_12bit\nal.o libx265_12bit\dpb.o libx265_12bit\piclist.o libx265_12bit\winxp.o libx265_12bit\frame.o libx265_12bit\frameencoder.o libx265_12bit\picyuv.o libx265_12bit\bitstream.o libx265_12bit\entropy.o libx265_12bit\cudata.o libx265_12bit\wavefront.o libx265_12bit\md5.o libx265_12bit\sei.o libx265_12bit\framefilter.o libx265_12bit\framedata.o libx265_12bit\lowres.o libx265_12bit\temporalfilter.o libx265_12bit\threading.o libx265_12bit\threadpool.o libx265_12bit\yuv.o libx265_12bit\sao.o libx265_12bit\search.o libx265_12bit\weightPrediction.o libx265_12bit\analysis.o libx265_12bit\pixel.o libx265_12bit\quant.o libx265_12bit\predict.o libx265_12bit\shortyuv.o libx265_12bit\level.o libx265_12bit\primitives.o libx265_12bit\slice.o libx265_12bit\bitcost.o libx265_12bit\ipfilter.o libx265_12bit\constants.o libx265_12bit\deblock.o libx265_12bit\dct.o libx265_12bit\version.o libx265_12bit\cpu.o libx265_12bit\lowpassdct.o libx265_12bit\intrapred.o libx265_12bit\loopfilter.o libx265\x86\asm-primitives.o libx265\x86\blockcopy8.o libx265\x86\const-a.o libx265\x86\cpu-a.o libx265\x86\dct8.o libx265\x86\h-ipfilter8.o libx265\x86\intrapred8.o libx265_10bit\x86\intrapred16.o libx265_12bit\x86\intrapred16.o libx265\x86\intrapred8_allangs.o libx265\x86\ipfilter8.o libx265\x86\loopfilter.o libx265\x86\mc-a.o libx265\x86\mc-a2.o libx265\x86\pixeladd8.o libx265\x86\pixel-util8.o libx265\x86\seaintegral.o libx265\x86\v4-ipfilter8.o libx265_10bit\x86\asm-primitives.o libx265_10bit\x86\blockcopy8.o libx265_10bit\x86\const-a.o libx265_10bit\x86\cpu-a.o libx265_10bit\x86\dct8.o libx265_10bit\x86\loopfilter.o libx265_10bit\x86\mc-a.o libx265_10bit\x86\mc-a2.o libx265_10bit\x86\pixeladd8.o libx265_10bit\x86\pixel-util8.o libx265_10bit\x86\seaintegral.o libx265_12bit\x86\asm-primitives.o libx265_12bit\x86\blockcopy8.o libx265_12bit\x86\const-a.o libx265_12bit\x86\cpu-a.o libx265_12bit\x86\dct8.o libx265_12bit\x86\loopfilter.o libx265_12bit\x86\mc-a.o libx265_12bit\x86\mc-a2.o libx265_12bit\x86\pixeladd8.o libx265_12bit\x86\pixel-util8.o libx265_12bit\x86\seaintegral.o libx265\x86\pixel-a.o libx265_10bit\x86\pixel-a.o libx265_12bit\x86\pixel-a.o libx265\x86\ssd-a.o libx265_10bit\x86\ssd-a.o libx265_12bit\x86\ssd-a.o libx265_10bit\x86\v4-ipfilter16.o libx265_12bit\x86\v4-ipfilter16.o libx265_10bit\x86\ipfilter16.o libx265_12bit\x86\ipfilter16.o libx265_10bit\x86\h-ipfilter16.o libx265_12bit\x86\h-ipfilter16.o libx265_10bit\x86\h4-ipfilter16.o libx265_12bit\x86\h4-ipfilter16.o libx265\x86\dct-sse3.o libx265\x86\dct-ssse3.o libx265\x86\dct-sse41.o libx265_10bit\x86\dct-sse3.o libx265_10bit\x86\dct-ssse3.o libx265_10bit\x86\dct-sse41.o libx265_12bit\x86\dct-sse3.o libx265_12bit\x86\dct-ssse3.o libx265_12bit\x86\dct-sse41.o libx265\x86\sad-a.o libx265_10bit\x86\sad16-a.o libx265_12bit\x86\sad16-a.o libx265\vec\vec-primitives.o libx265_10bit\vec\vec-primitives.o libx265_12bit\vec\vec-primitives.o

cd libxevd
ar.exe rcs "../../lib/libxevd_x64.a" xevd_bsr.o xevd_df.o xevd_eco.o xevd_ipred.o xevd_itdq.o xevd_mc.o xevd_picman.o xevd_recon.o xevd_tbl.o xevd_tp.o xevd_util.o xevdm.o xevdm_alf.o xevdm_df.o xevdm_dra.o xevdm_eco.o xevdm_ipred.o xevdm_itdq.o xevdm_mc.o xevdm_picman.o xevdm_recon.o xevdm_tbl.o xevdm_util.o
cd ..
cd libxeve
ar.exe rcs "../../lib/libxeve_x64.a" xeve_bsw.o xeve_df.o xeve_eco.o xeve_enc.o xeve_fcst.o xeve_ipred.o xeve_itdq.o xeve_mc.o xeve_mode.o xeve_param_parse.o xeve_picman.o xeve_pinter.o xeve_pintra.o xeve_port.o xeve_rc.o xeve_recon.o xeve_sad.o xeve_tbl.o xeve_thread_pool.o xeve_tq.o xeve_util.o xevem.o xevem_alf.o xevem_df.o xevem_dra.o xevem_eco.o xevem_ibc_hash.o xevem_ipred.o xevem_itdq.o xevem_mc.o xevem_mode.o xevem_pibc.o xevem_picman.o xevem_pinter.o xevem_pintra.o xevem_recon.o xevem_stat.o xevem_tbl.o xevem_tq.o xevem_util.o
cd ..
cd libsvtav1
ar.exe rcs "../../lib/libsvtav1_x64.a" aom_dsp_rtcd.o av1me.o cfl_c.o common_dsp_rtcd.o convolve.o corner_detect.o corner_match.o dwt.o EbAdaptiveMotionVectorPrediction.o EbBitstreamUnit.o EbBlend_a64_mask.o EbBlend_a64_mask_c.o EbBlockStructures.o EbCabacContextModel.o EbCdef.o EbCdefProcess.o EbCodingLoop.o EbCodingUnit.o EbComputeSAD_c.o EbDeblockingCommon.o EbDeblockingFilter.o EbDlfProcess.o EbEncCdef.o EbEncDecProcess.o EbEncDecResults.o EbEncDecSegments.o EbEncDecTasks.o EbEncHandle.o EbEncInterPrediction.o EbEncIntraPrediction.o EbEncodeContext.o EbEncSettings.o EbEncWarpedMotion.o EbEntropyCoding.o EbEntropyCodingProcess.o EbEntropyCodingResults.o EbFullLoop.o EbGlobalMotionEstimation.o EbGlobalMotionEstimationCost.o EbInitialRateControlProcess.o EbInitialRateControlReorderQueue.o EbInitialRateControlResults.o EbInterPrediction.o EbInterPrediction_c.o EbIntraPrediction.o EbIntraPrediction_c.o EbInvTransforms.o EbLog.o EbMalloc.o EbMcp.o EbMdRateEstimation.o EbMeSadCalculation.o EbMetadataHandle.o EbModeDecision.o EbModeDecisionConfigurationProcess.o EbModeDecisionProcess.o EbMotionEstimation.o EbMotionEstimationContext.o EbMotionEstimationProcess.o EbMotionEstimationResults.o EbNeighborArrays.o EbPacketizationProcess.o EbPacketizationReorderQueue.o EbPackUnPack_c.o EbPictureAnalysisProcess.o EbPictureAnalysisResults.o EbPictureBufferDesc.o EbPictureControlSet.o EbPictureDecisionProcess.o EbPictureDecisionQueue.o EbPictureDecisionReorderQueue.o EbPictureDecisionResults.o EbPictureDemuxResults.o EbPictureManagerProcess.o EbPictureManagerQueue.o EbPictureOperators.o EbPictureOperators_c.o EbPredictionStructure.o EbProductCodingLoop.o EbPsnr.o EbRateControlProcess.o EbRateControlResults.o EbRateControlTables.o EbRateControlTasks.o EbRateDistortionCost.o EbReferenceObject.o EbResize.o EbResourceCoordinationProcess.o EbResourceCoordinationResults.o EbRestoration.o EbRestorationPick.o EbRestProcess.o EbSegmentation.o EbSegmentationParams.o EbSequenceControlSet.o EbSourceBasedOperationsProcess.o EbSuperRes.o EbSystemResourceManager.o EbTemporalFiltering.o EbThreads.o EbTime.o EbTransforms.o EbUtility.o EbWarpedMotion.o EncodeTxbRef_c.o fast.o fast_9.o nonmax.o filterintra_c.o fft.o firstpass.o global_motion.o grainSynthesis.o hash.o hash_motion.o level.o mcomp.o noise_model.o noise_util.o palette.o pass2_strategy.o ransac.o sad_av1.o variance.o vector.o ASM_SSE2\aom_subpixel_8t_sse2.o ASM_SSE2\convolve_2d_sse2.o ASM_SSE2\convolve_sse2.o ASM_SSE2\EbAvcStyleMcp_Intrinsic_SSE2.o ASM_SSE2\EbComputeMean_Intrinsic_SSE2.o ASM_SSE2\EbDeblockingFilter_Intrinsic_SSE2.o ASM_SSE2\EbIntraPrediction_AV1_Intrinsic_SSE2.o ASM_SSE2\EbMeSadCalculation_Intrinsic_SSE2.o ASM_SSE2\EbPackUnPack_Intrinsic_SSE2.o ASM_SSE2\EbPictureOperators_Intrinsic_SSE2.o ASM_SSE2\EbPictureOperators_SSE2.o ASM_SSE2\encodetxb_sse2.o ASM_SSE2\fft_sse2.o ASM_SSE2\intrapred_sse2.o ASM_SSE2\highbd_intrapred_sse2.o ASM_SSE2\highbd_intrapred_sse2_.o ASM_SSE2\highbd_subtract_sse2.o ASM_SSE2\highbd_variance_sse2.o ASM_SSE2\highbd_variance_impl_sse2.o ASM_SSE2\jnt_convolve_2d_sse2.o ASM_SSE2\jnt_convolve_sse2.o ASM_SSE2\subpel_variance_sse2.o ASM_SSE2\subtract_sse2.o ASM_SSE2\variance_sse2.o ASM_SSE2\wiener_convolve_sse2.o ASM_SSE2\x64RegisterUtil.o ASM_SSSE3\aom_subpixel_8t_intrin_ssse3.o ASM_SSSE3\aom_subpixel_8t_ssse3.o ASM_SSSE3\aom_subpixel_bilinear_ssse3.o ASM_SSSE3\av1_inv_txfm_ssse3.o ASM_SSSE3\EbHighbdIntraPrediction_Intrinsic_SSSE3.o ASM_SSSE3\highbd_convolve_2d_ssse3.o ASM_SSSE3\highbd_convolve_ssse3.o ASM_SSSE3\highbd_wiener_convolve_ssse3.o ASM_SSSE3\intrapred_ssse3.o ASM_SSSE3\jnt_convolve_ssse3.o ASM_SSSE3\reconinter_ssse3.o ASM_SSSE3\subpel_variance_ssse3.o ASM_SSSE3\variance_impl_ssse3.o ASM_SSE4_1\av1_convolve_scale_sse4.o ASM_SSE4_1\av1_quantize_sse4_1.o ASM_SSE4_1\cdef_block_sse4_1.o ASM_SSE4_1\corner_match_sse4.o ASM_SSE4_1\EbBlend_a64_mask_sse4.o ASM_SSE4_1\EbCdef_sse4.o ASM_SSE4_1\EbComputeSAD_Intrinsic_SSE4_1.o ASM_SSE4_1\EbIntraPrediction16bit_Intrinsic_SSE4_1.o ASM_SSE4_1\EbPictureOperators_Intrinsic_SSE4_1.o ASM_SSE4_1\EbTemporalFiltering_SSE4_1.o ASM_SSE4_1\encodetxb_sse4.o ASM_SSE4_1\filterintra_sse4.o ASM_SSE4_1\highbd_convolve_2d_sse4.o ASM_SSE4_1\highbd_fwd_txfm_sse4.o ASM_SSE4_1\highbd_inv_txfm_sse4.o ASM_SSE4_1\highbd_jnt_convolve_sse4.o ASM_SSE4_1\obmc_variance_sse4.o ASM_SSE4_1\pickrst_sse4.o ASM_SSE4_1\reconinter_sse4.o ASM_SSE4_1\selfguided_sse4.o ASM_SSE4_1\warp_plane_sse4.o cpuinfo\api.o cpuinfo\cache.o cpuinfo\clog.o cpuinfo\descriptor.o cpuinfo\deterministic.o cpuinfo\info.o cpuinfo\init.o cpuinfo\init_cache.o cpuinfo\init_win.o cpuinfo\init_x86.o cpuinfo\isa.o cpuinfo\mockcpuid.o cpuinfo\name.o cpuinfo\topology.o cpuinfo\uarch.o cpuinfo\vendor.o
cd ..
cd libdav1d
ar.exe rcs "../../lib/libdav1d_x64.a" cdef_apply_tmpl.o cdef_tmpl.o cdf.o cpu.o data.o decode.o dequant_tables.o fg_apply_tmpl.o filmgrain_tmpl.o getbits.o intra_edge.o ipred_prepare_tmpl.o ipred_tmpl.o itx_1d.o itx_tmpl.o lf_apply_tmpl.o lf_mask.o lib.o log.o loopfilter_tmpl.o looprestoration_tmpl.o lr_apply_tmpl.o mc_tmpl.o mem.o msac.o obu.o picture.o qm.o recon_tmpl.o ref.o refmvs.o scan.o tables.o thread.o thread_task.o warpmv.o wedge.o 16bit\cdef_apply_tmpl.o 16bit\cdef_tmpl.o 16bit\fg_apply_tmpl.o 16bit\filmgrain_tmpl.o 16bit\ipred_prepare_tmpl.o 16bit\ipred_tmpl.o 16bit\itx_tmpl.o 16bit\lf_apply_tmpl.o 16bit\loopfilter_tmpl.o 16bit\looprestoration_tmpl.o 16bit\lr_apply_tmpl.o 16bit\mc_tmpl.o 16bit\recon_tmpl.o x86\cdef_sse.o x86\cdef16_sse.o x86\cpuid.o x86\filmgrain_sse.o x86\filmgrain16_sse.o x86\ipred_sse.o x86\ipred16_sse.o x86\itx_sse.o x86\itx16_sse.o x86\loopfilter_sse.o x86\loopfilter16_sse.o x86\looprestoration_sse.o x86\looprestoration16_sse.o x86\mc_sse.o x86\mc16_sse.o x86\msac.o x86\refmvs.o x86\cpu.o x86\cdef_avx2.o x86\cdef_avx512.o x86\cdef16_avx2.o x86\cdef16_avx512.o x86\filmgrain_avx2.o x86\filmgrain16_avx2.o x86\filmgrain16_avx512.o x86\filmgrain_avx512.o x86\ipred_avx2.o x86\ipred_avx512.o x86\ipred16_avx2.o x86\ipred16_avx512.o x86\itx_avx2.o x86\itx_avx512.o x86\itx16_avx2.o x86\itx16_avx512.o x86\loopfilter_avx2.o x86\loopfilter_avx512.o x86\loopfilter16_avx2.o x86\loopfilter16_avx512.o x86\looprestoration_avx2.o x86\looprestoration_avx512.o x86\looprestoration16_avx2.o x86\looprestoration16_avx512.o x86\mc_avx2.o x86\mc_avx512.o x86\mc16_avx2.o x86\mc16_avx512.o 
cd ..
cd libvpx
ar.exe rcs "../../lib/libvpx_x64.a" vpx_dsp/add_noise.o vpx_dsp/avg.o vpx_dsp/bitreader.o vpx_dsp/bitreader_buffer.o vpx_dsp/bitwriter.o vpx_dsp/bitwriter_buffer.o vpx_dsp/deblock.o vpx_dsp/fastssim.o vpx_dsp/fwd_txfm.o vpx_dsp/intrapred.o vpx_dsp/inv_txfm.o vpx_dsp/loopfilter.o vpx_dsp/prob.o vpx_dsp/psnr.o vpx_dsp/psnrhvs.o vpx_dsp/quantize.o vpx_dsp/sad.o vpx_dsp/skin_detection.o vpx_dsp/ssim.o vpx_dsp/subtract.o vpx_dsp/sum_squares.o vpx_dsp/variance.o vpx_dsp/vpx_convolve.o vpx_dsp/vpx_dsp_rtcd.o vpx_mem/vpx_mem.o vpx_scale/gen_scalers.o vpx_scale/vpx_scale.o vpx_scale/vpx_scale_rtcd.o vpx_scale/yv12config.o vpx_scale/yv12extend.o vpx_scale/yv12extend_dspr2.o vpx_util/vpx_thread.o vpx_util/vpx_write_yuv_frame.o vp9/vp9_cx_iface.o vp9/vp9_dx_iface.o vp9/vp9_iface_common.o vp9/vpx_codec.o vp9/vpx_config.o vp9/vpx_decoder.o vp9/vpx_encoder.o vp9/vpx_image.o vp9/common/vp9_alloccommon.o vp9/common/vp9_blockd.o vp9/common/vp9_common_data.o vp9/common/vp9_debugmodes.o vp9/common/vp9_entropy.o vp9/common/vp9_entropymode.o vp9/common/vp9_entropymv.o vp9/common/vp9_filter.o vp9/common/vp9_frame_buffers.o vp9/common/vp9_idct.o vp9/common/vp9_loopfilter.o vp9/common/vp9_mfqe.o vp9/common/vp9_mvref_common.o vp9/common/vp9_postproc.o vp9/common/vp9_pred_common.o vp9/common/vp9_quant_common.o vp9/common/vp9_reconinter.o vp9/common/vp9_reconintra.o vp9/common/vp9_rtcd.o vp9/common/vp9_scale.o vp9/common/vp9_scan.o vp9/common/vp9_seg_common.o vp9/common/vp9_thread_common.o vp9/common/vp9_tile_common.o vp9/decoder/vp9_decodeframe.o vp9/decoder/vp9_decodemv.o vp9/decoder/vp9_decoder.o vp9/decoder/vp9_detokenize.o vp9/decoder/vp9_dsubexp.o vp9/decoder/vp9_job_queue.o vp9/encoder/vp9_alt_ref_aq.o vp9/encoder/vp9_aq_360.o vp9/encoder/vp9_aq_complexity.o vp9/encoder/vp9_aq_cyclicrefresh.o vp9/encoder/vp9_aq_variance.o vp9/encoder/vp9_bitstream.o vp9/encoder/vp9_blockiness.o vp9/encoder/vp9_context_tree.o vp9/encoder/vp9_cost.o vp9/encoder/vp9_dct.o vp9/encoder/vp9_denoiser.o vp9/encoder/vp9_encodeframe.o vp9/encoder/vp9_encodemb.o vp9/encoder/vp9_encodemv.o vp9/encoder/vp9_encoder.o vp9/encoder/vp9_ethread.o vp9/encoder/vp9_ext_ratectrl.o vp9/encoder/vp9_extend.o vp9/encoder/vp9_firstpass.o vp9/encoder/vp9_frame_scale.o vp9/encoder/vp9_lookahead.o vp9/encoder/vp9_mbgraph.o vp9/encoder/vp9_mcomp.o vp9/encoder/vp9_multi_thread.o vp9/encoder/vp9_noise_estimate.o vp9/encoder/vp9_non_greedy_mv.o vp9/encoder/vp9_picklpf.o vp9/encoder/vp9_pickmode.o vp9/encoder/vp9_quantize.o vp9/encoder/vp9_ratectrl.o vp9/encoder/vp9_rd.o vp9/encoder/vp9_rdopt.o vp9/encoder/vp9_resize.o vp9/encoder/vp9_segmentation.o vp9/encoder/vp9_skin_detection.o vp9/encoder/vp9_speed_features.o vp9/encoder/vp9_subexp.o vp9/encoder/vp9_svc_layercontext.o vp9/encoder/vp9_temporal_filter.o vp9/encoder/vp9_tokenize.o vp9/encoder/vp9_tpl_model.o vp9/encoder/vp9_treewriter.o
cd ..
cd libaom
ar.exe rcs "../../lib/libaom_x64.a" aom\arg_defs.o aom\args_helper.o aom\vector.o aom\aom_codec.o aom\aom_config.o aom\aom_decoder.o aom\aom_encoder.o aom\aom_image.o aom\aom_integer.o aom\aom_mem.o aom\aom_scale.o aom\aom_scale_rtcd.o aom\aom_thread.o aom\av1_cx_iface.o aom\av1_dx_iface.o aom\fast.o aom\fast_9.o aom\gen_scalers.o aom\ivfdec.o aom\nonmax.o aom\tools_common.o aom\yv12config.o aom\yv12extend.o av1c\alloccommon.o av1c\av1_inv_txfm1d.o av1c\av1_inv_txfm2d.o av1c\av1_loopfilter.o av1c\av1_txfm.o av1c\blockd.o av1c\cdef.o av1c\cdef_block.o av1c\cfl.o av1c\common_data.o av1c\convolve.o av1c\debugmodes.o av1c\entropy.o av1c\av1_rtcd.o av1c\entropymode.o av1c\entropymv.o av1c\frame_buffers.o av1c\idct.o av1c\mvref_common.o av1c\obu_util.o av1c\pred_common.o av1c\quant_common.o av1c\reconinter.o av1c\reconintra.o av1c\resize.o av1c\restoration.o av1c\scale.o av1c\scan.o av1c\seg_common.o av1c\thread_common.o av1c\tile_common.o av1c\timing.o av1c\txb_common.o av1c\warped_motion.o av1d\accounting.o av1d\decodeframe.o av1d\decodemv.o av1d\decoder.o av1d\decodetxb.o av1d\detokenize.o av1d\grain_synthesis.o av1d\inspection.o av1d\obu.o av1dsp\aom_convolve.o av1dsp\avg.o av1dsp\binary_codes_reader.o av1dsp\binary_codes_writer.o av1dsp\bitreader.o av1dsp\bitreader_buffer.o av1dsp\bitwriter.o av1dsp\bitwriter_buffer.o av1dsp\aom_dsp_rtcd.o av1dsp\blend_a64_hmask.o av1dsp\blend_a64_mask.o av1dsp\blend_a64_vmask.o av1dsp\blk_sse_sum.o av1dsp\entcode.o av1dsp\entdec.o av1dsp\entenc.o av1dsp\fastssim.o av1dsp\fft.o av1dsp\fwd_txfm.o av1dsp\grain_table.o av1dsp\intrapred.o av1dsp\loopfilter.o av1dsp\noise_model.o av1dsp\noise_util.o av1dsp\odintrin.o av1dsp\pyramid.o av1dsp\psnr.o av1dsp\psnrhvs.o av1dsp\quantize.o av1dsp\sad.o av1dsp\sad_av1.o av1dsp\sse.o av1dsp\ssim.o av1dsp\subtract.o av1dsp\sum_squares.o av1dsp\variance.o av1e\allintra_vis.o av1e\aq_complexity.o av1e\aq_cyclicrefresh.o av1e\aq_variance.o av1e\av1_fwd_txfm1d.o av1e\av1_fwd_txfm2d.o av1e\av1_noise_estimate.o av1e\av1_quantize.o av1e\bitstream.o av1e\blockiness.o av1e\cnn.o av1e\compound_type.o av1e\context_tree.o av1dsp\flow_estimation\corner_detect.o av1dsp\flow_estimation\corner_match.o av1dsp\flow_estimation\disflow.o av1dsp\flow_estimation\flow_estimation.o av1dsp\flow_estimation\ransac.o av1e\cost.o av1e\dwt.o av1e\encode_strategy.o av1e\encodeframe.o av1e\encodeframe_utils.o av1e\encodemb.o av1e\encodemv.o av1e\encoder.o av1e\encoder_utils.o av1e\encodetxb.o av1e\ethread.o av1e\extend.o av1e\external_partition.o av1e\firstpass.o av1e\global_motion.o av1e\global_motion_facade.o av1e\gop_structure.o av1e\hash.o av1e\hash_motion.o av1e\hybrid_fwd_txfm.o av1e\interp_search.o av1e\intra_mode_search.o av1e\level.o av1e\lookahead.o av1e\mcomp.o av1e\ml.o av1e\motion_search_facade.o av1e\mv_prec.o av1e\nonrd_opt.o av1e\nonrd_pickmode.o av1e\optical_flow.o av1e\palette.o av1e\partition_search.o av1e\partition_strategy.o av1e\pass2_strategy.o av1e\pickcdef.o av1e\picklpf.o av1e\pickrst.o av1e\ratectrl.o av1e\rd.o av1e\rdopt.o av1e\reconinter_enc.o av1e\saliency_map.o av1e\segmentation.o av1e\sparse_linear_solver.o av1e\speed_features.o av1e\superres_scale.o av1e\svc_layercontext.o av1e\temporal_filter.o av1e\thirdpass.o av1e\tokenize.o av1e\tpl_model.o av1e\tx_search.o av1e\txb_rdopt.o av1e\var_based_part.o av1e\wedge_utils.o av1dsp\x86\adaptive_quantize_sse2.o av1dsp\x86\aom_asm_stubs.o av1dsp\x86\aom_convolve_copy_sse2.o av1dsp\x86\aom_high_subpixel_8t_sse2.o av1dsp\x86\aom_high_subpixel_bilinear_sse2.o av1dsp\x86\aom_subpixel_8t_intrin_sse2.o av1dsp\x86\aom_subpixel_8t_sse2.o av1dsp\x86\aom_subpixel_bilinear_sse2.o av1dsp\x86\avg_intrin_sse2.o av1dsp\x86\blk_sse_sum_sse2.o av1dsp\x86\dct_sse2.o av1dsp\x86\error_sse2.o av1dsp\x86\fft_sse2.o av1dsp\x86\fwd_txfm_sse2.o av1dsp\x86\highbd_adaptive_quantize_sse2.o av1dsp\x86\highbd_convolve_sse2.o av1dsp\x86\highbd_intrapred_asm_sse2.o av1dsp\x86\highbd_intrapred_sse2.o av1dsp\x86\highbd_loopfilter_sse2.o av1dsp\x86\highbd_quantize_intrin_sse2.o av1dsp\x86\highbd_sad_sse2.o av1dsp\x86\highbd_sad4d_sse2.o av1dsp\x86\highbd_subtract_sse2.o av1dsp\x86\highbd_subpel_variance_impl_sse2.o av1dsp\x86\highbd_variance_impl_sse2.o av1dsp\x86\highbd_variance_sse2.o av1dsp\x86\intrapred_asm_sse2.o av1dsp\x86\intrapred_sse2.o av1dsp\x86\loopfilter_sse2.o av1dsp\x86\quantize_sse2.o av1dsp\x86\sad_sse2.o av1dsp\x86\sad4d_sse2.o av1dsp\x86\ssim_sse2_x86_64.o av1dsp\x86\subpel_variance_sse2.o av1dsp\x86\sum_squares_sse2.o av1dsp\x86\subtract_sse2.o av1dsp\x86\variance_sse2.o av1dsp\x86\av1_quantize_ssse3_x86_64.o av1dsp\x86\aom_subpixel_8t_ssse3.o av1dsp\x86\aom_subpixel_8t_intrin_ssse3.o av1dsp\x86\aom_subpixel_bilinear_ssse3.o av1dsp\x86\fwd_txfm_ssse3_x86_64.o av1dsp\x86\highbd_convolve_ssse3.o av1dsp\x86\intrapred_ssse3.o av1dsp\x86\jnt_sad_ssse3.o av1dsp\x86\masked_sad_intrin_ssse3.o av1dsp\x86\masked_sad4d_ssse3.o av1dsp\x86\quantize_ssse3.o av1dsp\x86\quantize_ssse3_x86_64.o av1dsp\x86\variance_impl_ssse3.o av1dsp\x86\avg_intrin_sse4.o av1dsp\x86\blend_a64_hmask_sse4.o av1dsp\x86\blend_a64_mask_sse4.o av1dsp\x86\blend_a64_vmask_sse4.o av1dsp\x86\highbd_variance_sse4.o av1dsp\x86\intrapred_sse4.o av1dsp\x86\obmc_sad_sse4.o av1dsp\x86\obmc_variance_sse4.o av1dsp\x86\sse_sse4.o av1dsp\x86\aom_quantize_avx.o av1c\x86\cdef_block_sse2.o av1c\x86\cfl_sse2.o av1c\x86\convolve_sse2.o av1c\x86\convolve_2d_sse2.o av1c\x86\jnt_convolve_sse2.o av1c\x86\wiener_convolve_sse2.o av1c\x86\warp_plane_sse2.o av1c\x86\av1_inv_txfm_ssse3.o av1c\x86\cdef_block_ssse3.o av1c\x86\cfl_ssse3.o av1c\x86\highbd_convolve_2d_ssse3.o av1c\x86\highbd_wiener_convolve_ssse3.o av1c\x86\jnt_convolve_ssse3.o av1c\x86\reconinter_ssse3.o av1c\x86\resize_ssse3.o av1c\x86\av1_convolve_horiz_rs_sse4.o av1c\x86\av1_convolve_scale_sse4.o av1c\x86\av1_txfm_sse4.o av1c\x86\cdef_block_sse4.o av1c\x86\filterintra_sse4.o av1c\x86\highbd_convolve_2d_sse4.o av1c\x86\highbd_inv_txfm_sse4.o av1c\x86\highbd_jnt_convolve_sse4.o av1c\x86\highbd_warp_plane_sse4.o av1c\x86\intra_edge_sse4.o av1c\x86\reconinter_sse4.o av1c\x86\selfguided_sse4.o av1c\x86\warp_plane_sse4.o av1e\x86\av1_fwd_txfm_sse2.o av1e\x86\av1_k_means_sse2.o av1e\x86\av1_quantize_sse2.o av1e\x86\encodetxb_sse2.o av1e\x86\error_intrin_sse2.o av1e\x86\highbd_block_error_intrin_sse2.o av1e\x86\highbd_temporal_filter_sse2.o av1e\x86\reconinter_enc_sse2.o av1e\x86\temporal_filter_sse2.o av1e\x86\ml_sse3.o av1e\x86\wedge_utils_sse2.o av1e\x86\reconinter_enc_ssse3.o av1dsp\flow_estimation\x86\corner_match_sse4.o av1dsp\flow_estimation\x86\disflow_sse4.o av1e\x86\encodetxb_sse4.o av1e\x86\av1_fwd_txfm1d_sse4.o av1e\x86\av1_highbd_quantize_sse4.o av1e\x86\highbd_fwd_txfm_sse4.o av1e\x86\av1_fwd_txfm2d_sse4.o av1e\x86\pickrst_sse4.o av1e\x86\rdopt_sse4.o av1e\x86\hash_sse42.o
cd ..
cd libjxl
ar.exe rcs "../../lib/libjxl_x64.a" ac_strategy.o aligned_allocator.o alpha.o ans_common.o enc_aux_out.o blending.o box.o box_content_decoder.o butteraugli_wrapper.o chroma_from_luma.o codec.o coeff_order.o color_encoding_internal.o color_management.o compressed_dc.o convolve_separable5.o convolve_separable7.o convolve_slow.o convolve_symmetric3.o convolve_symmetric5.o dct_scales.o dec_ans.o dec_cache.o dec_context_map.o dec_external_image.o dec_frame.o dec_group.o dec_group_border.o dec_huffman.o dec_modular.o dec_noise.o dec_patch_dictionary.o dec_xyb.o decode.o decode_to_jpeg.o enc_ac_strategy.o enc_ar_control_field.o enc_adaptive_quantization.o enc_ans.o enc_bit_writer.o enc_butteraugli_comparator.o enc_butteraugli_pnorm.o enc_cache.o enc_chroma_from_luma.o enc_cluster.o enc_coeff_order.o enc_color_management.o enc_comparator.o enc_context_map.o enc_detect_dots.o enc_dot_dictionary.o enc_entropy_coder.o enc_external_image.o enc_fast_lossless.o enc_fields.o enc_file.o enc_frame.o enc_group.o enc_heuristics.o enc_huffman.o enc_icc_codec.o enc_image_bundle.o enc_patch_dictionary.o enc_photon_noise.o enc_modular.o enc_noise.o enc_quant_weights.o enc_splines.o enc_toc.o enc_transforms.o enc_xyb.o encode.o entropy_coder.o epf.o exif.o fields.o frame_header.o enc_gaborish.o gauss_blur.o headers.o huffman_table.o enc_huffman_tree.o icc_codec.o icc_codec_common.o image.o image_bundle.o image_metadata.o enc_linalg.o loop_filter.o luminance.o memory_manager_internal.o opsin_params.o enc_optimize.o packed_image_convert.o passes_state.o per_target.o enc_progressive_split.o quant_weights.o quantizer.o splines.o targets.o thread_parallel_runner.o thread_parallel_runner_internal.o toc.o tone_mapping.o base\cache_aligned.o base\data_parallel.o base\padded_bytes.o base\random.o butteraugli\butteraugli.o jpeg\dec_jpeg_data.o jpeg\dec_jpeg_data_writer.o jpeg\enc_jpeg_data.o jpeg\enc_jpeg_data_reader.o jpeg\enc_jpeg_huffman_decode.o jpeg\jpeg_data.o modular\modular_image.o modular\encoding\dec_ma.o modular\encoding\enc_debug_tree.o modular\encoding\enc_encoding.o modular\encoding\encoding.o modular\encoding\enc_ma.o modular\transform\enc_palette.o modular\transform\enc_rct.o modular\transform\enc_squeeze.o modular\transform\enc_transform.o modular\transform\palette.o modular\transform\rct.o modular\transform\squeeze.o modular\transform\transform.o brotli\common\constants.o brotli\common\context.o brotli\common\dictionary.o brotli\common\platform.o brotli\common\shared_dictionary.o brotli\common\transform.o brotli\dec\bit_reader.o brotli\dec\decode.o brotli\dec\huffman.o brotli\dec\state.o brotli\enc\backward_references.o brotli\enc\backward_references_hq.o brotli\enc\bit_cost.o brotli\enc\block_splitter.o brotli\enc\brotli_bit_stream.o brotli\enc\cluster.o brotli\enc\command.o brotli\enc\compound_dictionary.o brotli\enc\compress_fragment.o brotli\enc\compress_fragment_two_pass.o brotli\enc\dictionary_hash.o brotli\enc\encode.o brotli\enc\encoder_dict.o brotli\enc\entropy_encode.o brotli\enc\fast_log.o brotli\enc\histogram.o brotli\enc\literal_cost.o brotli\enc\memory.o brotli\enc\metablock.o brotli\enc\static_dict.o brotli\enc\utf8_util.o render_pipeline\low_memory_render_pipeline.o render_pipeline\render_pipeline.o render_pipeline\simple_render_pipeline.o render_pipeline\stage_blending.o render_pipeline\stage_chroma_upsampling.o render_pipeline\stage_epf.o render_pipeline\stage_from_linear.o render_pipeline\stage_gaborish.o render_pipeline\stage_noise.o render_pipeline\stage_patches.o render_pipeline\stage_splines.o render_pipeline\stage_spot.o render_pipeline\stage_to_linear.o render_pipeline\stage_tone_mapping.o render_pipeline\stage_upsampling.o render_pipeline\stage_write.o render_pipeline\stage_xyb.o render_pipeline\stage_ycbcr.o dec\decode.o dec\color_description.o dec\color_hints.o dec\pgx.o dec\pnm.o enc\encode.o enc\npy.o enc\pgx.o enc\pnm.o
cd ..
cd ..

cd libavutil/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_32=0 -DARCH_X86_64=1 -DPIC=1 -DHAVE_ALIGNED_STACK=1 -DHAVE_CPUNOP=0 -DHAVE_MMX_EXTERNAL=1 -DHAVE_MMXEXT_EXTERNAL=1 -DHAVE_SSE2_EXTERNAL=1 -DHAVE_SSSE3_EXTERNAL=1 -DHAVE_AVX_EXTERNAL=1 -DHAVE_FMA3_EXTERNAL=1 -DHAVE_XOP_EXTERNAL=0 -DHAVE_AVX2_EXTERNAL=0 -DHAVE_FMA4_EXTERNAL=0 -DHAVE_AVX512_EXTERNAL=0 -DHAVE_AVX512ICL_EXTERNAL=0 -Xgnu %%f -o %%~nf.o
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
cd ../..
cd libavutil
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libavutil_x64.a" xtea.o xga_font_data.o video_enc_params.o version.o uuid.o utils.o tx_int32.o tx_float.o tx_double.o tx.o twofish.o tree.o timecode.o time.o threadmessage.o tea.o stereo3d.o spherical.o slicethread.o sha512.o sha.o samplefmt.o ripemd.o reverse.o rc4.o rational.o random_seed.o pixelutils.o pixdesc.o pca.o parseutils.o opt.o murmur3.o mem.o md5.o mathematics.o mastering_display_metadata.o lzo.o log2_tab.o log.o lls.o lfg.o intmath.o integer.o imgutils.o hwcontext_dxva2.o hwcontext_d3d11va.o hwcontext.o hmac.o hdr_dynamic_vivid_metadata.o hdr_dynamic_metadata.o hash.o half2float.o frame.o float_dsp.o float2half.o fixed_dsp.o film_grain_params.o file_open.o file.o fifo.o eval.o error.o encryption_info.o downmix_info.o dovi_meta.o display.o dict.o detection_bbox.o des.o csp.o crc.o cpu.o color_utils.o channel_layout.o cast5.o camellia.o buffer.o bprint.o blowfish.o base64.o avstring.o avsscanf.o audio_fifo.o ambient_viewing_environment.o aes_ctr.o aes.o adler32.o x86/cpu.o x86/cpuid.o x86/emms.o x86/fixed_dsp.o x86/fixed_dsp_init.o x86/float_dsp_init.o x86/imgutils.o x86/imgutils_init.o x86/lls_init.o x86/pixelutils.o x86/pixelutils_init.o x86/tx_float.o x86/tx_float_init.o x86/float_dsp.o x86/lls.o
cd ..
cd libswresample/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_32=0 -DARCH_X86_64=1 -DPIC=1 -DHAVE_ALIGNED_STACK=1 -DHAVE_CPUNOP=0 -DHAVE_MMX_EXTERNAL=1 -DHAVE_MMXEXT_EXTERNAL=1 -DHAVE_SSE2_EXTERNAL=1 -DHAVE_SSSE3_EXTERNAL=1 -DHAVE_AVX_EXTERNAL=1 -DHAVE_FMA3_EXTERNAL=1 -DHAVE_XOP_EXTERNAL=0 -DHAVE_AVX2_EXTERNAL=0 -DHAVE_FMA4_EXTERNAL=0 -DHAVE_AVX512_EXTERNAL=0 -DHAVE_AVX512ICL_EXTERNAL=0 -Xgnu %%f -o %%~nf.o
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
cd ../..
cd libswresample/libsoxr
for %%f in ("soxr.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -c %%f -o %%~nf.o
cd ../..
cd libswresample
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libswresample_x64.a" audioconvert.o dither.o options.o rematrix.o resample.o resample_dsp.o soxr_resample.o swresample.o swresample_frame.o version.o libsoxr/soxr.o x86/audio_convert.o x86/rematrix.o x86/resample.o x86/audio_convert_init.o x86/rematrix_init.o x86/resample_init.o
cd ..
cd libswscale/x86
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_32=0 -DARCH_X86_64=1 -DPIC=1 -DHAVE_ALIGNED_STACK=1 -DHAVE_CPUNOP=0 -DHAVE_MMX_EXTERNAL=1 -DHAVE_MMXEXT_EXTERNAL=1 -DHAVE_SSE2_EXTERNAL=1 -DHAVE_SSSE3_EXTERNAL=1 -DHAVE_AVX_EXTERNAL=1 -DHAVE_FMA3_EXTERNAL=1 -DHAVE_XOP_EXTERNAL=0 -DHAVE_AVX2_EXTERNAL=0 -DHAVE_FMA4_EXTERNAL=0 -DHAVE_AVX512_EXTERNAL=0 -DHAVE_AVX512ICL_EXTERNAL=0 -Xgnu %%f -o %%~nf.o
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
cd ../..
cd libswscale
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libswscale_x64.a" alphablend.o gamma.o hscale.o hscale_fast_bilinear.o input.o options.o output.o rgb2rgb.o slice.o swscale.o swscale_unscaled.o utils.o version.o vscale.o yuv2rgb.o x86/hscale_fast_bilinear_simd.o x86/input.o x86/output.o x86/rgb_2_rgb.o x86/rgb2rgb.o x86/scale.o x86/swscale.o x86/yuv_2_rgb.o x86/yuv2rgb.o x86/yuv2yuvX.o
cd ..
cd libavcodec/fixed
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
cd ../..
cd libavcodec/hwcontext
for %%f in ("dxva2*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
cd ../..
cd libavcodec/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_32=0 -DARCH_X86_64=1 -DPIC=1 -DHAVE_ALIGNED_STACK=1 -DHAVE_CPUNOP=0 -DHAVE_MMX_INLINE=0 -DHAVE_SSE2_EXTERNAL=1 -DHAVE_SSSE3_EXTERNAL=1 -DHAVE_AVX_EXTERNAL=1 -DHAVE_FMA3_EXTERNAL=1 -DHAVE_XOP_EXTERNAL=0 -DHAVE_AVX2_EXTERNAL=0 -DHAVE_FMA4_EXTERNAL=0 -DHAVE_AVX512_EXTERNAL=0 -DHAVE_AVX512ICL_EXTERNAL=0 -DUSE_APPROXIMATION=0 -Xgnu %%f -o %%~nf.o
cd ../..
cd libavcodec
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -DRSVG_STATIC=1 -DKVZ_STATIC_LIB=1 -DUVG_STATIC_LIB=1 -DOPJ_STATIC=1 -DUAVS3D_STATIC=1 -DUAVS3E_STATIC=1 -DLIBTWOLAME_STATIC=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libavcodec_x64.a" 4xm.o 8bps.o 8svx.o 012v.o a64multienc.o aac_ac3_parser.o aac_adtstoasc_bsf.o aac_parser.o aaccoder.o aacenc.o aacenc_is.o aacenc_ltp.o aacenc_pred.o aacenc_tns.o aacenctab.o aacps_common.o aacpsy.o aactab.o aandcttab.o aasc.o ac3.o ac3_channel_layout_tab.o ac3_parser.o ac3dec_data.o ac3dsp.o ac3enc.o ac3tab.o acelp_filters.o acelp_pitch_delay.o acelp_vectors.o adpcm.o adpcm_data.o adpcmenc.o adts_header.o adts_parser.o adx.o adx_parser.o adxdec.o adxenc.o agm.o aic.o alac.o alac_data.o alacdsp.o alacenc.o aliaspixdec.o aliaspixenc.o allcodecs.o alsdec.o anm.o ansi.o apac.o apedec.o aptx.o aptxdec.o aptxenc.o arbc.o argo.o ass.o ass_split.o assdec.o assenc.o asv.o asvdec.o asvenc.o atrac.o atrac1.o atrac3.o atrac3plus.o atrac3plusdec.o atrac3plusdsp.o atrac9dec.o atsc_a53.o audio_frame_queue.o audiodsp.o aura.o av1_frame_merge_bsf.o av1_frame_split_bsf.o av1_metadata_bsf.o av1_parse.o av1_parser.o av1dec.o avcodec.o avdct.o avfft.o avpacket.o avrndec.o avs.o avs2.o avs2_parser.o avs3_parser.o avuidec.o avuienc.o bethsoftvideo.o bfi.o bgmc.o bink.o binkaudio.o binkdsp.o bintext.o bitpacked_dec.o bitpacked_enc.o bitstream.o bitstream_filters.o blockdsp.o bmp.o bmp_parser.o bmpenc.o bmvaudio.o bmvvideo.o bonk.o brenderpix.o bsf.o bswapdsp.o c93.o cabac.o canopus.o cavs.o cavs_parser.o cavsdata.o cavsdec.o cavsdsp.o cbs.o cbs_av1.o cbs_bsf.o cbs_h2645.o cbs_jpeg.o cbs_mpeg2.o cbs_sei.o cbs_vp9.o ccaption_dec.o cdgraphics.o cdtoons.o cdxl.o celp_filters.o celp_math.o cfhd.o cfhddata.o cfhddsp.o cfhdenc.o cfhdencdsp.o cga_data.o chomp_bsf.o cinepak.o cinepakenc.o clearvideo.o cljrdec.o cljrenc.o cllc.o cngdec.o cngenc.o codec_desc.o codec_par.o cook.o cook_parser.o cpia.o cri.o cri_parser.o cscd.o cyuv.o d3d11va.o dca.o dca_core.o dca_core_bsf.o dca_exss.o dca_lbr.o dca_parser.o dca_sample_rate_tab.o dca_xll.o dcaadpcm.o dcadata.o dcadct.o dcadec.o dcadsp.o dcaenc.o dcahuff.o dct.o dctref.o dds.o decode.o dfa.o dfpwmdec.o dfpwmenc.o dirac.o dirac_arith.o dirac_dwt.o dirac_parser.o dirac_vlc.o diracdec.o diracdsp.o diractab.o dnxhd_parser.o dnxhddata.o dnxhddec.o dnxhdenc.o dolby_e.o dolby_e_parse.o dolby_e_parser.o dovi_rpu.o dpcm.o dpx.o dpx_parser.o dpxenc.o dsd.o dsddec.o dsicinaudio.o dsicinvideo.o dss_sp.o dstdec.o dts2pts_bsf.o dump_extradata_bsf.o dv.o dv_error_marker_bsf.o dv_profile.o dvaudio_parser.o dvaudiodec.o dvbsub_parser.o dvbsubdec.o dvbsubenc.o dvd_nav_parser.o dvdata.o dvdec.o dvdsub.o dvdsub_parser.o dvdsubdec.o dvdsubenc.o dvenc.o dxa.o dxtory.o dxv.o dynamic_hdr_vivid.o dynamic_hdr10_plus.o eac3_core_bsf.o eac3_data.o eac3enc.o eacmv.o eaidct.o eamad.o eatgq.o eatgv.o eatqi.o elbg.o elsdec.o encode.o error_resilience.o escape124.o escape130.o evc_parser.o evrcdec.o executor.o exif.o exr.o exrdsp.o exrenc.o extract_extradata_bsf.o faandct.o faanidct.o fastaudio.o faxcompr.o fdctdsp.o fflcms2.o fft_init_table.o ffv1.o ffv1dec.o ffv1enc.o ffwavesynth.o fic.o filter_units_bsf.o fits.o fitsdec.o fitsenc.o flac.o flac_parser.o flacdata.o flacdec.o flacdsp.o flacenc.o flacencdsp.o flashsv.o flashsv2enc.o flashsvenc.o flicvideo.o flvdec.o flvenc.o fmtconvert.o fmvc.o frame_thread_encoder.o fraps.o frwu.o ftr.o ftr_parser.o g2meet.o g722.o g722dec.o g722dsp.o g722enc.o g723_1.o g723_1_parser.o g723_1dec.o g723_1enc.o g726.o g729_parser.o g729dec.o g729postfilter.o gdv.o gemdec.o get_buffer.o gif.o gif_parser.o gifdec.o golomb.o gsm_parser.o gsmdec.o gsmdec_data.o h261.o h261_parser.o h261data.o h261dec.o h261enc.o h263.o h263_parser.o h263data.o h263dec.o h263dsp.o h264_cabac.o h264_cavlc.o h264_direct.o h264_levels.o h264_loopfilter.o h264_mb.o h264_metadata_bsf.o h264_mp4toannexb_bsf.o h264_parse.o h264_parser.o h264_picture.o h264_ps.o h264_redundant_pps_bsf.o h264_refs.o h264_sei.o h264_slice.o h264chroma.o h264data.o h264dec.o h264dsp.o h264idct.o h264pred.o h264qpel.o h265_metadata_bsf.o h265_profile_level.o h266_metadata_bsf.o h274.o h2645_parse.o h2645_sei.o h2645_vui.o h2645data.o hap.o hapdec.o hapenc.o hapqa_extract_bsf.o hcadec.o hcom.o hdr_parser.o hdrdec.o hdrenc.o hevc_cabac.o hevc_data.o hevc_filter.o hevc_mp4toannexb_bsf.o hevc_mvs.o hevc_parse.o hevc_parser.o hevc_ps.o hevc_ps_enc.o hevc_refs.o hevc_sei.o hevcdec.o hevcdsp.o hevcpred.o hnm4video.o hpeldsp.o hq_hqa.o hq_hqadata.o hq_hqadsp.o hqx.o hqxdsp.o hqxvlc.o htmlsubtitles.o huffman.o huffyuv.o huffyuvdec.o huffyuvdsp.o huffyuvenc.o huffyuvencdsp.o idcinvideo.o idctdsp.o iff.o iirfilter.o ilbcdec.o imc.o imgconvert.o imm4.o imm5.o imx.o imx_dump_header_bsf.o indeo2.o indeo3.o indeo4.o indeo5.o intelh263dec.o interplayacm.o interplayvideo.o intrax8.o intrax8dsp.o ipu_parser.o ituh263dec.o ituh263enc.o ivi.o ivi_dsp.o j2kenc.o jacosubdec.o jfdctfst.o jfdctint.o jpeg2000.o jpeg2000_parser.o jpeg2000dec.o jpeg2000dsp.o jpeg2000dwt.o jpeg2000htdec.o jpegls.o jpeglsdec.o jpeglsenc.o jpegquanttables.o jpegtables.o jrevdct.o jvdec.o kbdwin.o kgv1dec.o kmvc.o lagarith.o lagarithrac.o latm_parser.o lcldec.o lclenc.o libaom.o libaomdec.o libaomenc.o libaribb24.o libcelt_dec.o libcodec2.o libdav1d.o libdavs2.o libgsmdec.o libgsmenc.o libjxl.o libjxldec.o libjxlenc.o libkvazaar.o libmp3lame.o libopenh264.o libopenh264dec.o libopenh264enc.o libopenjpegdec.o libopenjpegenc.o libopus.o libopusdec.o libopusenc.o libshine.o libspeexdec.o libspeexenc.o libsvtav1.o libtheoraenc.o libtwolame.o libuavs3d.o libuavs3e.o libuvg266.o libvorbisdec.o libvorbisenc.o libvpx.o libvpxdec.o libvpxenc.o libwebpenc.o libwebpenc_animencoder.o libwebpenc_common.o libx264.o libx265.o libxavs.o libxavs2.o libxevd.o libxeve.o libxvid.o ljpegenc.o loco.o lossless_audiodsp.o lossless_videodsp.o lossless_videoencdsp.o lpc.o lscrdec.o lsp.o lzf.o lzw.o lzwenc.o m101.o mace.o magicyuv.o magicyuvenc.o mathtables.o mdec.o me_cmp.o media100.o metasound.o mf_utils.o mfenc.o microdvddec.o midivid.o mimic.o misc4.o misc4_parser.o mjpeg_parser.o mjpeg2jpeg_bsf.o mjpega_dump_header_bsf.o mjpegbdec.o mjpegdec.o mjpegdec_common.o mjpegenc.o mjpegenc_common.o mjpegenc_huffman.o mlp.o mlp_parse.o mlp_parser.o mlpdec.o mlpdsp.o mlpenc.o mlz.o mmvideo.o mobiclip.o motion_est.o motionpixels.o movsub_bsf.o movtextdec.o movtextenc.o mp3_header_decompress_bsf.o mpc.o mpc7.o mpc8.o mpeg_er.o mpeg2_metadata_bsf.o mpeg4_unpack_bframes_bsf.o mpeg4audio.o mpeg4audio_sample_rates.o mpeg4video.o mpeg4video_parser.o mpeg4videodec.o mpeg4videodsp.o mpeg4videoenc.o mpeg12.o mpeg12data.o mpeg12dec.o mpeg12enc.o mpeg12framerate.o mpegaudio.o mpegaudio_parser.o mpegaudiodata.o mpegaudiodec_common.o mpegaudiodec_fixed.o mpegaudiodecheader.o mpegaudiodsp.o mpegaudiodsp_data.o mpegaudiodsp_fixed.o mpegaudioenc_fixed.o mpegaudiotabs.o mpegpicture.o mpegutils.o mpegvideo.o mpegvideo_dec.o mpegvideo_enc.o mpegvideo_motion.o mpegvideo_parser.o mpegvideodata.o mpegvideoencdsp.o mpl2dec.o mqc.o mqcdec.o mqcenc.o mscc.o msgsmdec.o msmpeg4.o msmpeg4_vc1_data.o msmpeg4data.o msmpeg4dec.o msmpeg4enc.o msp2dec.o msrle.o msrledec.o mss1.o mss2.o mss2dsp.o mss3.o mss4.o mss12.o mss34dsp.o msvideo1.o msvideo1enc.o mv30.o mvcdec.o mvha.o mwsc.o mxpegdec.o nellymoser.o nellymoserdec.o nellymoserenc.o noise_bsf.o notchlc.o null_bsf.o nuv.o on2avc.o on2avcdata.o options.o opus_celt.o opus_metadata_bsf.o opus_parse.o opus_parser.o opus_pvq.o opus_rc.o opus_silk.o opusdec.o opusdec_celt.o opusdsp.o opusenc.o opusenc_psy.o opustab.o pafaudio.o pafvideo.o pamenc.o parser.o parsers.o pcm.o pcm_rechunk_bsf.o pcm-bluray.o pcm-blurayenc.o pcm-dvd.o pcm-dvdenc.o pcx.o pcxenc.o pgs_frame_merge_bsf.o pgssubdec.o pgxdec.o photocd.o pictordec.o pixblockdsp.o pixlet.o png.o png_parser.o pngdec.o pngdsp.o pngenc.o pnm.o pnm_parser.o pnmdec.o pnmenc.o profiles.o prores_metadata_bsf.o proresdata.o proresdec2.o proresdsp.o proresenc_anatoliy.o proresenc_kostya.o prosumer.o psd.o psymodel.o pthread.o pthread_frame.o pthread_slice.o ptx.o qcelpdec.o qdm2.o qdmc.o qdrw.o qoi_parser.o qoidec.o qoienc.o qpeg.o qpeldsp.o qtrle.o qtrleenc.o r210dec.o r210enc.o ra144.o ra144dec.o ra144enc.o ra288.o ralf.o rangecoder.o rasc.o ratecontrol.o raw.o rawdec.o rawenc.o rdft.o realtextdec.o remove_extradata_bsf.o rl.o rl2.o rle.o roqaudioenc.o roqvideo.o roqvideodec.o roqvideoenc.o rpza.o rpzaenc.o rscc.o rtjpeg.o rv10.o rv10enc.o rv20enc.o rv30.o rv30dsp.o rv34.o rv34_parser.o rv34dsp.o rv40.o rv40dsp.o s302m.o s302menc.o samidec.o sanm.o sbc.o sbc_parser.o sbcdec.o sbcdec_data.o sbcdsp.o sbcdsp_data.o sbcenc.o scpr.o scpr3.o screenpresso.o setts_bsf.o sga.o sgidec.o sgienc.o sgirledec.o sheervideo.o shorten.o simple_idct.o sipr.o sipr_parser.o sipr16k.o siren.o smacker.o smc.o smcenc.o snappy.o snow.o snow_dwt.o snowdec.o snowenc.o sonic.o sp5xdec.o speedhq.o speedhqdec.o speedhqenc.o speexdec.o srtdec.o srtenc.o startcode.o subviewerdec.o sunrast.o sunrastenc.o svq1.o svq1dec.o svq1enc.o svq3.o synth_filter.o tak.o tak_parser.o takdec.o takdsp.o targa.o targa_y216dec.o targaenc.o tdsc.o textdec.o texturedsp.o texturedspenc.o tiertexseqv.o tiff.o tiff_common.o tiffenc.o tmv.o to_upper4.o tpeldsp.o trace_headers_bsf.o truehd_core_bsf.o truemotion1.o truemotion2.o truemotion2rt.o truespeech.o tscc.o tscc2.o tta.o ttadata.o ttadsp.o ttaenc.o ttaencdsp.o ttmlenc.o twinvq.o twinvqdec.o txd.o ulti.o utils.o utvideodec.o utvideodsp.o utvideoenc.o v210dec.o v210enc.o v210x.o v308dec.o v308enc.o v408dec.o v408enc.o v410dec.o v410enc.o vb.o vble.o vbndec.o vbnenc.o vc1.o vc1_block.o vc1_loopfilter.o vc1_mc.o vc1_parser.o vc1_pred.o vc1data.o vc1dec.o vc1dsp.o vc2enc.o vc2enc_dwt.o vcr1.o version.o videodsp.o vima.o vlc.o vmdaudio.o vmdvideo.o vmnc.o vorbis.o vorbis_data.o vorbis_parser.o vorbisdec.o vorbisdsp.o vorbisenc.o vp3.o vp3_parser.o vp3dsp.o vp5.o vp6.o vp6dsp.o vp8.o vp8_parser.o vp8dsp.o vp9.o vp9_metadata_bsf.o vp9_parser.o vp9_raw_reorder_bsf.o vp9_superframe_bsf.o vp9_superframe_split_bsf.o vp9block.o vp9data.o vp9dsp.o vp9dsp_8bpp.o vp9dsp_10bpp.o vp9dsp_12bpp.o vp9lpf.o vp9mvs.o vp9prob.o vp9recon.o vp56.o vp56data.o vp56dsp.o vpx_rac.o vqavideo.o vqcdec.o vvc_cabac.o vvc_ctu.o vvc_data.o vvc_filter.o vvc_inter.o vvc_itx_1d.o vvc_mp4toannexb_bsf.o vvc_mvs.o vvc_parser.o vvc_ps.o vvc_refs.o vvc_thread.o vvcdec.o vvcdsp.o vvcpred.o wavpack.o wavpackdata.o wavpackenc.o wbmpdec.o wbmpenc.o wcmv.o webp.o webp_parser.o webvttdec.o webvttenc.o wma.o wma_common.o wma_freqs.o wmadec.o wmaenc.o wmalosslessdec.o wmaprodec.o wmavoice.o wmv2.o wmv2data.o wmv2dec.o wmv2dsp.o wmv2enc.o wnv1.o wrapped_avframe.o ws-snd1.o xan.o xbm_parser.o xbmdec.o xbmenc.o xface.o xfacedec.o xfaceenc.o xiph.o xl.o xma_parser.o xpmdec.o xsubdec.o xsubenc.o xvididct.o xwd_parser.o xwddec.o xwdenc.o xxan.o y41pdec.o y41penc.o ylc.o yop.o yuv4dec.o yuv4enc.o zerocodec.o zlib_wrapper.o zmbv.o zmbvenc.o x86\aacencdsp.o x86\aacencdsp_init.o x86\aacpsdsp.o x86\aacpsdsp_init.o x86\ac3dsp.o x86\ac3dsp_downmix.o x86\ac3dsp_init.o x86\alacdsp.o x86\alacdsp_init.o x86\audiodsp.o x86\audiodsp_init.o x86\blockdsp.o x86\blockdsp_init.o x86\bswapdsp.o x86\bswapdsp_init.o x86\cavsdsp.o x86\cavsidct.o x86\celt_pvq_init.o x86\celt_pvq_search.o x86\cfhddsp.o x86\cfhddsp_init.o x86\cfhdencdsp.o x86\cfhdencdsp_init.o x86\constants.o x86\dcadsp.o x86\dcadsp_init.o x86\dct_init.o x86\dct32.o x86\dirac_dwt.o x86\dirac_dwt_init.o x86\diracdsp.o x86\diracdsp_init.o x86\dnxhdenc.o x86\dnxhdenc_init.o x86\exrdsp.o x86\exrdsp_init.o x86\fdct.o x86\fdctdsp_init.o x86\fft.o x86\fft_init.o x86\flac_dsp_gpl.o x86\flacdsp.o x86\flacdsp_init.o x86\flacencdsp_init.o x86\fmtconvert.o x86\fmtconvert_init.o x86\fpel.o x86\g722dsp.o x86\g722dsp_init.o x86\h263_loopfilter.o x86\h263dsp_init.o x86\h264_cabac.o x86\h264_chromamc.o x86\h264_chromamc_10bit.o x86\h264_deblock.o x86\h264_deblock_10bit.o x86\h264_idct.o x86\h264_idct_10bit.o x86\h264_intrapred.o x86\h264_intrapred_10bit.o x86\h264_intrapred_init.o x86\h264_qpel.o x86\h264_qpel_10bit.o x86\h264_qpel_8bit.o x86\h264_weight.o x86\h264_weight_10bit.o x86\h264chroma_init.o x86\h264dsp_init.o x86\hevc_add_res.o x86\hevc_deblock.o x86\hevc_idct.o x86\hevc_mc.o x86\hevc_sao.o x86\hevc_sao_10bit.o x86\hevcdsp_init.o x86\hpeldsp.o x86\hpeldsp_init.o x86\hpeldsp_vp3.o x86\hpeldsp_vp3_init.o x86\huffyuvdsp.o x86\huffyuvdsp_init.o x86\huffyuvencdsp.o x86\huffyuvencdsp_init.o x86\idctdsp.o x86\idctdsp_init.o x86\imdct36.o x86\jpeg2000dsp.o x86\jpeg2000dsp_init.o x86\lossless_audiodsp.o x86\lossless_audiodsp_init.o x86\lossless_videodsp.o x86\lossless_videodsp_init.o x86\lossless_videoencdsp.o x86\lossless_videoencdsp_init.o x86\lpc.o x86\lpc_init.o x86\me_cmp.o x86\me_cmp_init.o x86\mlpdsp.o x86\mlpdsp_init.o x86\mpegaudiodsp.o x86\mpeg4videodsp.o x86\mpegvideo.o x86\mpegvideoenc.o x86\mpegvideoencdsp.o x86\mpegvideoencdsp_init.o x86\opusdsp.o x86\opusdsp_init.o x86\pixblockdsp.o x86\pixblockdsp_init.o x86\pngdsp.o x86\pngdsp_init.o x86\proresdsp.o x86\proresdsp_init.o x86\qpel.o x86\qpeldsp.o x86\qpeldsp_init.o x86\rv34dsp.o x86\rv34dsp_init.o x86\rv40dsp.o x86\rv40dsp_init.o x86\sbcdsp.o x86\sbcdsp_init.o x86\sbrdsp.o x86\sbrdsp_init.o x86\simple_idct10.o x86\snowdsp.o x86\svq1enc.o x86\svq1enc_init.o x86\synth_filter.o x86\synth_filter_init.o x86\takdsp.o x86\takdsp_init.o x86\ttadsp.o x86\ttadsp_init.o x86\ttaencdsp.o x86\ttaencdsp_init.o x86\utvideodsp.o x86\utvideodsp_init.o x86\v210.o x86\v210enc.o x86\v210enc_init.o x86\v210-init.o x86\videodsp.o x86\videodsp_init.o x86\vc1dsp_init.o x86\vc1dsp_loopfilter.o x86\vc1dsp_mc.o x86\vc1dsp_mmx.o x86\vorbisdsp.o x86\vorbisdsp_init.o x86\vp3dsp.o x86\vp3dsp_init.o x86\vp6dsp.o x86\vp6dsp_init.o x86\vp8dsp.o x86\vp8dsp_init.o x86\vp8dsp_loopfilter.o x86\vp9dsp_init.o x86\vp9dsp_init_10bpp.o x86\vp9dsp_init_12bpp.o x86\vp9dsp_init_16bpp.o x86\vp9intrapred.o x86\vp9intrapred_16bpp.o x86\vp9itxfm.o x86\vp9itxfm_16bpp.o x86\vp9lpf.o x86\vp9lpf_16bpp.o x86\vp9mc.o x86\vp9mc_16bpp.o x86\xvididct.o x86\xvididct_init.o x86\vvc_alf.o x86\vvc_sao.o x86\vvc_sao_10bit.o x86\vvcdsp_init.o hwcontext\dxva2.o hwcontext\dxva2_av1.o hwcontext\dxva2_h264.o hwcontext\dxva2_hevc.o hwcontext\dxva2_mpeg2.o hwcontext\dxva2_vc1.o hwcontext\dxva2_vp9.o aacdec.o fixed\aacdec_fixed.o fixed\aacps_fixed.o fixed\aacpsdsp_fixed.o aacsbr.o fixed\aacsbr_fixed.o fixed\ac3dec_fixed.o fixed\ac3enc_fixed.o cbrt_data.o fixed\cbrt_data_fixed.o dct32_float.o fixed\dct32_fixed.o fft_float.o fixed\fft_fixed_32.o mdct_float.o fixed\mdct_fixed_32.o sbrdsp.o fixed\sbrdsp_fixed.o sinewin.o 
cd ..
cd libavformat
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -DLIBXML_STATIC=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libavformat_x64.a" 3dostr.o 4xm.o a64.o aacdec.o aadec.o aaxdec.o ac3_channel_layout_tab.o ac3dec.o acedec.o acm.o act.o adp.o ads.o adtsenc.o adxdec.o aea.o afc.o aiff.o aiffdec.o aiffenc.o aixdec.o allformats.o alp.o amr.o amvenc.o anm.o apac.o apc.o ape.o apetag.o apm.o apngdec.o apngenc.o aptxdec.o aqtitledec.o argo_asf.o argo_brp.o argo_cvg.o asf.o asf_tags.o asfcrypt.o asfdec_f.o asfdec_o.o asfenc.o assdec.o assenc.o ast.o astdec.o astenc.o async.o au.o av1.o av1dec.o avc.o avformat.o avidec.o avienc.o avio.o aviobuf.o avlanguage.o avr.o avs.o avs2dec.o avs3dec.o bethsoftvid.o bfi.o bink.o binka.o bintext.o bit.o bmv.o boadec.o bonk.o brstm.o c93.o cache.o caf.o cafdec.o cafenc.o cavsvideodec.o cdg.o cdxl.o cinedec.o codec2.o concat.o concatdec.o crcenc.o crypto.o dash.o dashdec.o dashenc.o data_uri.o dauddec.o daudenc.o dca_sample_rate_tab.o dcstr.o demux.o demux_utils.o derf.o dfa.o dfpwmdec.o dhav.o diracdec.o dnxhddec.o dovi_isom.o dsfdec.o dsicin.o dss.o dtsdec.o dtshddec.o dump.o dv.o dvbsub.o dvbtxt.o dvenc.o dxa.o eacdata.o electronicarts.o epafdec.o evc.o evcdec.o ffmetadec.o ffmetaenc.o fifo.o fifo_test.o file.o filmstripdec.o filmstripenc.o fitsdec.o fitsenc.o flac_picture.o flacdec.o flacenc.o flacenc_header.o flic.o flvdec.o flvenc.o format.o framecrcenc.o framehash.o frmdec.o fsb.o ftp.o fwse.o g722.o g723_1.o g726.o g729dec.o gdv.o genh.o gif.o gifdec.o gopher.o gsmdec.o gxf.o gxfenc.o h261dec.o h263dec.o h264dec.o hashenc.o hca.o hcom.o hdsenc.o hevc.o hevcdec.o hls.o hls_sample_encryption.o hlsenc.o hlsplaylist.o hlsproto.o hnm.o http.o httpauth.o icecast.o icodec.o icoenc.o id3v1.o id3v2.o id3v2enc.o idcin.o idroqdec.o idroqenc.o iff.o ifv.o ilbc.o imf_cpl.o imfdec.o img2.o img2_alias_pix.o img2_brender_pix.o img2dec.o img2enc.o imx.o ingenientdec.o ip.o ipfsgateway.o ipmovie.o ipudec.o ircam.o ircamdec.o ircamenc.o isom.o isom_tags.o iss.o iv8.o ivfdec.o ivfenc.o jacosubdec.o jacosubenc.o jpegtables.o jpegxl_probe.o jvdec.o kvag.o lafdec.o latmenc.o lmlm4.o loasdec.o lrc.o lrcdec.o lrcenc.o luodatdec.o lvfdec.o lxfdec.o m4vdec.o matroska.o matroskadec.o matroskaenc.o mca.o mccdec.o md5proto.o metadata.o mgsts.o microdvddec.o microdvdenc.o mj2kdec.o mkvtimestamp_v2.o mlpdec.o mlvdec.o mm.o mmf.o mms.o mmsh.o mmst.o mods.o moflex.o mov.o mov_chan.o mov_esds.o movenc.o movenc_ttml.o movenccenc.o movenchint.o mp3dec.o mp3enc.o mpc.o mpc8.o mpeg.o mpeg4audio_sample_rates.o mpegaudiotabs.o mpegenc.o mpegts.o mpegtsenc.o mpegvideodec.o mpjpeg.o mpjpegdec.o mpl2dec.o mpsubdec.o msf.o msnwc_tcp.o mspdec.o mtaf.o mtv.o musx.o mux.o mux_utils.o mvdec.o mvi.o mxf.o mxfdec.o mxfenc.o mxg.o ncdec.o network.o nistspheredec.o nspdec.o nsvdec.o nullenc.o nut.o nutdec.o nutenc.o nuv.o oggdec.o oggenc.o oggparsecelt.o oggparsedirac.o oggparseflac.o oggparseogm.o oggparseopus.o oggparseskeleton.o oggparsespeex.o oggparsetheora.o oggparsevorbis.o oggparsevp8.o oma.o omadec.o omaenc.o options.o os_support.o paf.o pcm.o pcmdec.o pcmenc.o pjsdec.o pmpdec.o pp_bnk.o prompeg.o protocols.o psxstr.o pva.o pvfdec.o qcp.o qtpalette.o r3d.o rawdec.o rawenc.o rawutils.o rawvideodec.o rdt.o realtextdec.o redspark.o replaygain.o riff.o riffdec.o riffenc.o rl2.o rm.o rmdec.o rmenc.o rmsipr.o rpl.o rsd.o rso.o rsodec.o rsoenc.o rtmpdigest.o rtmphttp.o rtmppkt.o rtmpproto.o rtp.o rtpdec.o rtpdec_ac3.o rtpdec_amr.o rtpdec_asf.o rtpdec_dv.o rtpdec_g726.o rtpdec_h261.o rtpdec_h263.o rtpdec_h263_rfc2190.o rtpdec_h264.o rtpdec_hevc.o rtpdec_ilbc.o rtpdec_jpeg.o rtpdec_latm.o rtpdec_mpa_robust.o rtpdec_mpeg4.o rtpdec_mpeg12.o rtpdec_mpegts.o rtpdec_qcelp.o rtpdec_qdm2.o rtpdec_qt.o rtpdec_rfc4175.o rtpdec_svq3.o rtpdec_vc2hq.o rtpdec_vp8.o rtpdec_vp9.o rtpdec_xiph.o rtpenc.o rtpenc_aac.o rtpenc_amr.o rtpenc_chain.o rtpenc_h261.o rtpenc_h263.o rtpenc_h263_rfc2190.o rtpenc_h264_hevc.o rtpenc_jpeg.o rtpenc_latm.o rtpenc_mpegts.o rtpenc_mpv.o rtpenc_rfc4175.o rtpenc_vc2hq.o rtpenc_vp8.o rtpenc_vp9.o rtpenc_xiph.o rtpproto.o rtsp.o rtspdec.o rtspenc.o s337m.o samidec.o sapdec.o sapenc.o sauce.o sbcdec.o sbgdec.o sccdec.o sccenc.o scd.o sdp.o sdr2.o sdsdec.o sdxdec.o seek.o segafilm.o segafilmenc.o segment.o serdec.o sga.o shortendec.o sierravmd.o siff.o smacker.o smjpeg.o smjpegdec.o smjpegenc.o smoothstreamingenc.o smush.o sol.o soxdec.o soxenc.o spdif.o spdifdec.o spdifenc.o srtdec.o srtenc.o srtp.o srtpproto.o stldec.o subfile.o subtitles.o subviewer1dec.o subviewerdec.o supdec.o supenc.o svag.o svs.o swf.o swfdec.o swfenc.o takdec.o tcp.o tedcaptionsdec.o tee.o tee_common.o teeproto.o thp.o tiertexseq.o tls.o tls_schannel.o tmv.o to_upper4.o tta.o ttaenc.o ttmlenc.o tty.o txd.o ty.o udp.o uncodedframecrcenc.o url.o urldecode.o utils.o vag.o vc1dec.o vc1test.o vc1testenc.o version.o vividas.o vivo.o voc.o voc_packet.o vocdec.o vocenc.o vorbiscomment.o vpcc.o vpk.o vplayerdec.o vqf.o vvc.o vvcdec.o w64.o wavdec.o wavenc.o wc3movie.o webm_chunk.o webmdashenc.o webpenc.o webvttdec.o webvttenc.o westwood_aud.o westwood_audenc.o westwood_vqa.o wsddec.o wtv_common.o wtvdec.o wtvenc.o wv.o wvdec.o wvedec.o wvenc.o xa.o xmv.o xvag.o xwma.o yop.o yuv4mpegdec.o yuv4mpegenc.o
cd ..
cd libpostproc
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libpostproc_x64.a" postprocess.o version.o
cd ..
cd libavfilter/dnn
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
cd ../..
cd libavfilter/x86
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
for %%f in ("%~dp1*.asm") do nasm.exe -f win64 -O3 -DARCH_X86_32=0 -DARCH_X86_64=1 -DPIC=1 -DHAVE_ALIGNED_STACK=1 -DHAVE_CPUNOP=0 -DHAVE_MMX_EXTERNAL=1 -DHAVE_SSE2_EXTERNAL=1 -DHAVE_SSSE3_EXTERNAL=1 -DHAVE_AVX_EXTERNAL=1 -DHAVE_FMA3_EXTERNAL=1 -DHAVE_XOP_EXTERNAL=0 -DHAVE_AVX2_EXTERNAL=0 -DHAVE_FMA4_EXTERNAL=0 -DHAVE_AVX512_EXTERNAL=0 -DHAVE_AVX512ICL_EXTERNAL=0 -DUSE_APPROXIMATION=0 -Xgnu %%f -o %%~nf.o
cd ../..
cd libavfilter
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -DFRIBIDI_LIB_STATIC=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libavfilter_x64.a" aeval.o af_acontrast.o af_acopy.o af_acrossover.o af_acrusher.o af_adeclick.o af_adecorrelate.o af_adelay.o af_adenorm.o af_aderivative.o af_adrc.o af_adynamicequalizer.o af_adynamicsmooth.o af_aecho.o af_aemphasis.o af_aexciter.o af_afade.o af_afftdn.o af_afftfilt.o af_afir.o af_aformat.o af_afreqshift.o af_afwtdn.o af_agate.o af_aiir.o af_alimiter.o af_amerge.o af_amix.o af_amultiply.o af_anequalizer.o af_anlmdn.o af_anlms.o af_anull.o af_apad.o af_aphaser.o af_apsyclip.o af_apulsator.o af_aresample.o af_arnndn.o af_asdr.o af_asetnsamples.o af_asetrate.o af_ashowinfo.o af_asoftclip.o af_aspectralstats.o af_astats.o af_asubboost.o af_asupercut.o af_atempo.o af_atilt.o af_axcorrelate.o af_biquads.o af_channelmap.o af_channelsplit.o af_chorus.o af_compand.o af_compensationdelay.o af_crossfeed.o af_crystalizer.o af_dcshift.o af_deesser.o af_dialoguenhance.o af_drmeter.o af_dynaudnorm.o af_earwax.o af_extrastereo.o af_firequalizer.o af_flanger.o af_haas.o af_hdcd.o af_headphone.o af_join.o af_loudnorm.o af_mcompand.o af_pan.o af_replaygain.o af_sidechaincompress.o af_silencedetect.o af_silenceremove.o af_speechnorm.o af_stereotools.o af_stereowiden.o af_superequalizer.o af_surround.o af_tremolo.o af_vibrato.o af_virtualbass.o af_volume.o af_volumedetect.o allfilters.o asink_anullsink.o asrc_afirsrc.o asrc_anoisesrc.o asrc_anullsrc.o asrc_hilbert.o asrc_sinc.o asrc_sine.o audio.o avf_a3dscope.o avf_abitscope.o avf_ahistogram.o avf_aphasemeter.o avf_avectorscope.o avf_concat.o avf_showcqt.o avf_showcwt.o avf_showfreqs.o avf_showspatial.o avf_showspectrum.o avf_showvolume.o avf_showwaves.o avfilter.o avfiltergraph.o bbox.o blend_modes.o boxblur.o buffersink.o buffersrc.o colorspace.o colorspacedsp.o dnn_filter_common.o drawutils.o ebur128.o edge_common.o f_bench.o f_cue.o f_drawgraph.o f_ebur128.o f_graphmonitor.o f_interleave.o f_latency.o f_loop.o f_metadata.o f_perms.o f_realtime.o f_reverse.o f_segment.o f_select.o f_sendcmd.o f_sidedata.o f_streamselect.o fifo.o formats.o framepool.o framequeue.o framesync.o generate_wave_table.o graphdump.o graphparser.o lavfutils.o lswsutils.o motion_estimation.o palette.o pthread.o qp_table.o scale_eval.o scene_sad.o setpts.o settb.o signature_lookup.o split.o src_avsynctest.o src_movie.o transform.o trim.o vaf_spectrumsynth.o version.o vf_addroi.o vf_alphamerge.o vf_amplify.o vf_aspect.o vf_atadenoise.o vf_avgblur.o vf_backgroundkey.o vf_bbox.o vf_bilateral.o vf_bitplanenoise.o vf_blackdetect.o vf_blackframe.o vf_blend.o vf_blockdetect.o vf_blurdetect.o vf_bm3d.o vf_boxblur.o vf_bwdif.o vf_cas.o vf_chromakey.o vf_chromanr.o vf_chromashift.o vf_ciescope.o vf_codecview.o vf_colorbalance.o vf_colorchannelmixer.o vf_colorconstancy.o vf_colorcontrast.o vf_colorcorrect.o vf_colorize.o vf_colorkey.o vf_colorlevels.o vf_colormap.o vf_colormatrix.o vf_colorspace.o vf_colortemperature.o vf_convolution.o vf_convolve.o vf_copy.o vf_corr.o vf_cover_rect.o vf_crop.o vf_cropdetect.o vf_curves.o vf_datascope.o vf_dblur.o vf_dctdnoiz.o vf_deband.o vf_deblock.o vf_decimate.o vf_dedot.o vf_deflicker.o vf_dejudder.o vf_delogo.o vf_derain.o vf_deshake.o vf_despill.o vf_detelecine.o vf_displace.o vf_dnn_classify.o vf_dnn_detect.o vf_dnn_processing.o vf_drawbox.o vf_drawtext.o vf_edgedetect.o vf_elbg.o vf_entropy.o vf_epx.o vf_eq.o vf_estdif.o vf_exposure.o vf_extractplanes.o vf_fade.o vf_feedback.o vf_fftdnoiz.o vf_fftfilt.o vf_field.o vf_fieldhint.o vf_fieldmatch.o vf_fieldorder.o vf_fillborders.o vf_find_rect.o vf_floodfill.o vf_format.o vf_fps.o vf_framepack.o vf_framerate.o vf_framestep.o vf_freezedetect.o vf_freezeframes.o vf_fspp.o vf_gblur.o vf_geq.o vf_gradfun.o vf_grayworld.o vf_guided.o vf_hflip.o vf_histeq.o vf_histogram.o vf_hqdn3d.o vf_hqx.o vf_hsvkey.o vf_hue.o vf_huesaturation.o vf_hwdownload.o vf_hwmap.o vf_hwupload.o vf_hysteresis.o vf_iccdetect.o vf_iccgen.o vf_identity.o vf_idet.o vf_il.o vf_kerndeint.o vf_lagfun.o vf_lenscorrection.o vf_limitdiff.o vf_limiter.o vf_lumakey.o vf_lut.o vf_lut2.o vf_lut3d.o vf_maskedclamp.o vf_maskedmerge.o vf_maskedminmax.o vf_maskedthreshold.o vf_maskfun.o vf_median.o vf_mergeplanes.o vf_mestimate.o vf_midequalizer.o vf_minterpolate.o vf_mix.o vf_monochrome.o vf_morpho.o vf_mpdecimate.o vf_multiply.o vf_negate.o vf_neighbor.o vf_nlmeans.o vf_nnedi.o vf_noise.o vf_normalize.o vf_null.o vf_overlay.o vf_owdenoise.o vf_pad.o vf_palettegen.o vf_paletteuse.o vf_perspective.o vf_phase.o vf_photosensitivity.o vf_pixdesctest.o vf_pixelize.o vf_pp.o vf_pp7.o vf_premultiply.o vf_pseudocolor.o vf_psnr.o vf_pullup.o vf_qp.o vf_random.o vf_readeia608.o vf_readvitc.o vf_remap.o vf_removegrain.o vf_removelogo.o vf_repeatfields.o vf_rotate.o vf_sab.o vf_scale.o vf_scdet.o vf_scroll.o vf_selectivecolor.o vf_separatefields.o vf_setparams.o vf_shear.o vf_showinfo.o vf_showpalette.o vf_shuffleframes.o vf_shufflepixels.o vf_shuffleplanes.o vf_signalstats.o vf_signature.o vf_siti.o vf_smartblur.o vf_spp.o vf_sr.o vf_ssim.o vf_stack.o vf_stereo3d.o vf_super2xsai.o vf_swaprect.o vf_swapuv.o vf_telecine.o vf_threshold.o vf_thumbnail.o vf_tile.o vf_tinterlace.o vf_tmidequalizer.o vf_tonemap.o vf_tpad.o vf_transpose.o vf_unsharp.o vf_untile.o vf_v360.o vf_vaguedenoiser.o vf_varblur.o vf_vectorscope.o vf_vflip.o vf_vfrdet.o vf_vibrance.o vf_vif.o vf_vignette.o vf_vmafmotion.o vf_w3fdif.o vf_waveform.o vf_weave.o vf_xbr.o vf_xfade.o vf_xmedian.o vf_yadif.o vf_yaepblur.o vf_zoompan.o video.o vsink_nullsink.o vsrc_cellauto.o vsrc_ddagrab.o vsrc_gradients.o vsrc_life.o vsrc_mandelbrot.o vsrc_mptestsrc.o vsrc_sierpinski.o vsrc_testsrc.o yadif_common.o x86/af_afir.o x86/af_afir_init.o x86/af_anlmdn.o x86/af_anlmdn_init.o x86/af_volume.o x86/af_volume_init.o x86/avf_showcqt.o x86/avf_showcqt_init.o x86/colorspacedsp.o x86/colorspacedsp_init.o x86/scene_sad.o x86/scene_sad_init.o x86/vf_atadenoise.o x86/vf_atadenoise_init.o x86/vf_blend.o x86/vf_blend_init.o x86/vf_bwdif.o x86/vf_bwdif_init.o x86/vf_convolution.o x86/vf_convolution_init.o x86/vf_eq.o x86/vf_eq_init.o x86/vf_framerate.o x86/vf_framerate_init.o x86/vf_fspp.o x86/vf_fspp_init.o x86/vf_gblur.o x86/vf_gblur_init.o x86/vf_gradfun.o x86/vf_gradfun_init.o x86/vf_hflip.o x86/vf_hflip_init.o x86/vf_hqdn3d.o x86/vf_hqdn3d_init.o x86/vf_idet.o x86/vf_idet_init.o x86/vf_interlace.o x86/vf_limiter.o x86/vf_limiter_init.o x86/vf_lut3d.o x86/vf_lut3d_init.o x86/vf_maskedclamp.o x86/vf_maskedclamp_init.o x86/vf_maskedmerge.o x86/vf_maskedmerge_init.o x86/vf_nlmeans.o x86/vf_nlmeans_init.o x86/vf_noise.o x86/vf_overlay.o x86/vf_overlay_init.o x86/vf_pp7.o x86/vf_pp7_init.o x86/vf_psnr.o x86/vf_psnr_init.o x86/vf_pullup.o x86/vf_pullup_init.o x86/vf_removegrain.o x86/vf_removegrain_init.o x86/vf_spp.o x86/vf_ssim.o x86/vf_ssim_init.o x86/vf_stereo3d.o x86/vf_stereo3d_init.o x86/vf_threshold.o x86/vf_threshold_init.o x86/vf_tinterlace_init.o x86/vf_transpose.o x86/vf_transpose_init.o x86/vf_v360.o x86/vf_v360_init.o x86/vf_w3fdif.o x86/vf_w3fdif_init.o x86/vf_yadif.o x86/vf_yadif_init.o x86/yadif-10.o x86/yadif-16.o dnn/dnn_backend_common.o dnn/dnn_backend_native.o dnn/dnn_backend_native_layer_avgpool.o dnn/dnn_backend_native_layer_conv2d.o dnn/dnn_backend_native_layer_dense.o dnn/dnn_backend_native_layer_depth2space.o dnn/dnn_backend_native_layer_mathbinary.o dnn/dnn_backend_native_layer_mathunary.o dnn/dnn_backend_native_layer_maximum.o dnn/dnn_backend_native_layer_pad.o dnn/dnn_backend_native_layers.o dnn/dnn_interface.o dnn/dnn_io_proc.o dnn/queue.o dnn/safe_queue.o 
cd ..

cd fftools\libsdl2
cd main\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd sensor
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd sensor\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd render\direct3d12
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd hidapi
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd hidapi\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd joystick\hidapi
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd joystick\virtual
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd misc
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd misc\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd locale
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd locale\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd core\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -DNHAVE_XINPUT_GAMEPAD_EX -DNHAVE_XINPUT_STATE_EX -c %%f -o %%~nf.o
cd ..\..
cd cpuinfo
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd audio
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd audio\directsound
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd audio\disk
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd audio\dummy
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd audio\wasapi
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd audio\winmm
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd atomic
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd dynapi
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd events
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd file
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd filesystem\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd haptic
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd haptic\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -DNHAVE_XINPUT_GAMEPAD_EX -DNHAVE_XINPUT_STATE_EX -c %%f -o %%~nf.o
cd ..\..
cd joystick
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd joystick\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -DNHAVE_XINPUT_GAMEPAD_EX -DNHAVE_XINPUT_STATE_EX -c %%f -o %%~nf.o
cd ..\..
cd libm
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd loadso\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd power
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd power\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd render
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd render\direct3d
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd render\direct3d11
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
for %%f in ("%~dp1*.cpp") do g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd render\opengl
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd render\opengles
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd render\opengles2
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd render\software
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd stdlib
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd thread
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd thread\generic
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd thread\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd timer
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd timer\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd video
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..
cd video\dummy
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd video\windows
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
cd video\yuv2rgb
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
cd ..\..
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DWINVER=0x0602 -DHAVE_LIBC -c %%f -o %%~nf.o
ar.exe rcs "../../lib/libsdl2_x64.a" SDL.o SDL_assert.o SDL_dataqueue.o SDL_error.o SDL_guid.o SDL_hints.o SDL_list.o SDL_log.o SDL_utils.o atomic\SDL_atomic.o atomic\SDL_spinlock.o audio\SDL_audio.o audio\SDL_audiocvt.o audio\SDL_audiodev.o audio\SDL_audiotypecvt.o audio\SDL_mixer.o audio\SDL_wave.o audio\directsound\SDL_directsound.o audio\disk\SDL_diskaudio.o audio\dummy\SDL_dummyaudio.o audio\wasapi\SDL_wasapi.o audio\wasapi\SDL_wasapi_win32.o audio\winmm\SDL_winmm.o core\windows\SDL_hid.o core\windows\SDL_immdevice.o core\windows\SDL_windows.o core\windows\SDL_xinput.o cpuinfo\SDL_cpuinfo.o dynapi\SDL_dynapi.o events\SDL_clipboardevents.o events\SDL_displayevents.o events\SDL_dropevents.o events\SDL_events.o events\SDL_gesture.o events\SDL_keyboard.o events\SDL_mouse.o events\SDL_quit.o events\SDL_touch.o events\SDL_windowevents.o file\SDL_rwops.o filesystem\windows\SDL_sysfilesystem.o haptic\SDL_haptic.o haptic\windows\SDL_dinputhaptic.o haptic\windows\SDL_windowshaptic.o haptic\windows\SDL_xinputhaptic.o hidapi\SDL_hidapi.o joystick\controller_type.o joystick\SDL_gamecontroller.o joystick\SDL_joystick.o joystick\virtual\SDL_virtualjoystick.o joystick\windows\SDL_dinputjoystick.o joystick\windows\SDL_mmjoystick.o joystick\windows\SDL_rawinputjoystick.o joystick\windows\SDL_windows_gaming_input.o joystick\windows\SDL_windowsjoystick.o joystick\windows\SDL_xinputjoystick.o joystick\hidapi\SDL_hidapi_combined.o joystick\hidapi\SDL_hidapi_gamecube.o joystick\hidapi\SDL_hidapi_luna.o joystick\hidapi\SDL_hidapi_ps3.o joystick\hidapi\SDL_hidapi_ps4.o joystick\hidapi\SDL_hidapi_ps5.o joystick\hidapi\SDL_hidapi_rumble.o joystick\hidapi\SDL_hidapi_shield.o joystick\hidapi\SDL_hidapi_stadia.o joystick\hidapi\SDL_hidapi_steam.o joystick\hidapi\SDL_hidapi_switch.o joystick\hidapi\SDL_hidapi_wii.o joystick\hidapi\SDL_hidapi_xbox360.o joystick\hidapi\SDL_hidapi_xbox360w.o joystick\hidapi\SDL_hidapi_xboxone.o joystick\hidapi\SDL_hidapijoystick.o libm\e_atan2.o libm\e_fmod.o libm\e_log.o libm\e_pow.o libm\e_rem_pio2.o libm\e_sqrt.o libm\k_cos.o libm\k_rem_pio2.o libm\k_sin.o libm\k_tan.o libm\s_atan.o libm\s_copysign.o libm\s_cos.o libm\s_fabs.o libm\s_floor.o libm\s_scalbn.o libm\s_sin.o libm\s_tan.o loadso\windows\SDL_sysloadso.o locale\SDL_locale.o locale\windows\SDL_syslocale.o misc\SDL_url.o misc\windows\SDL_sysurl.o power\SDL_power.o power\windows\SDL_syspower.o render\SDL_d3dmath.o render\SDL_render.o render\SDL_yuv_sw.o render\direct3d\SDL_render_d3d.o render\direct3d\SDL_shaders_d3d.o render\direct3d11\SDL_render_d3d11.o render\direct3d11\SDL_shaders_d3d11.o render\direct3d12\SDL_render_d3d12.o render\direct3d12\SDL_shaders_d3d12.o render\opengl\SDL_render_gl.o render\opengl\SDL_shaders_gl.o render\opengles\SDL_render_gles.o render\opengles2\SDL_render_gles2.o render\opengles2\SDL_shaders_gles2.o render\software\SDL_render_sw.o render\software\SDL_rotate.o render\software\SDL_blendpoint.o render\software\SDL_blendline.o render\software\SDL_blendfillrect.o render\software\SDL_drawline.o render\software\SDL_drawpoint.o render\software\SDL_triangle.o sensor\SDL_sensor.o sensor\windows\SDL_windowssensor.o stdlib\SDL_crc16.o stdlib\SDL_crc32.o stdlib\SDL_getenv.o stdlib\SDL_iconv.o stdlib\SDL_malloc.o stdlib\SDL_qsort.o stdlib\SDL_stdlib.o stdlib\SDL_string.o stdlib\SDL_strtokr.o thread\SDL_thread.o thread\windows\SDL_syscond_cv.o thread\windows\SDL_sysmutex.o thread\windows\SDL_syssem.o thread\windows\SDL_systhread.o thread\windows\SDL_systls.o thread\generic\SDL_syscond.o timer\SDL_timer.o timer\windows\SDL_systimer.o video\SDL_blit.o video\SDL_blit_0.o video\SDL_blit_1.o video\SDL_blit_A.o video\SDL_blit_auto.o video\SDL_blit_copy.o video\SDL_blit_N.o video\SDL_blit_slow.o video\SDL_bmp.o video\SDL_clipboard.o video\SDL_egl.o video\SDL_fillrect.o video\SDL_pixels.o video\SDL_RLEaccel.o video\SDL_rect.o video\SDL_shape.o video\SDL_stretch.o video\SDl_surface.o video\SDL_video.o video\SDL_vulkan_utils.o video\SDL_yuv.o video\windows\SDL_windowsclipboard.o video\windows\SDL_windowsevents.o video\windows\SDL_windowsframebuffer.o video\windows\SDL_windowskeyboard.o video\windows\SDL_windowsmessagebox.o video\windows\SDL_windowsmodes.o video\windows\SDL_windowsmouse.o video\windows\SDL_windowsopengl.o video\windows\SDL_windowsopengles.o video\windows\SDL_windowsshape.o video\windows\SDL_windowsvideo.o video\windows\SDL_windowswindow.o video\windows\SDL_windowsvulkan.o video\yuv2rgb\yuv_rgb.o video\dummy\SDL_nullevents.o video\dummy\SDL_nullframebuffer.o video\dummy\SDL_nullvideo.o
cd ..\..

cd libavdevice
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libavdevice_x64.a" alldevices.o avdevice.o dshow.o dshow_common.o dshow_crossbar.o dshow_enummediatypes.o dshow_enumpins.o dshow_filter.o dshow_pin.o gdigrab.o lavfi.o reverse.o timefilter.o utils.o version.o vfwcap.o 
cd ..
cd fftools
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -c %%f -o %%~nf.o
cd ..
g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -o "lib/ffmpeg.exe" fftools/ffmpeg.o fftools/ffmpeg_demux.o fftools/ffmpeg_filter.o fftools/ffmpeg_hw.o fftools/ffmpeg_mux.o fftools/ffmpeg_mux_init.o fftools/ffmpeg_opt.o fftools/cmdutils.o fftools/objpool.o fftools/opt_common.o fftools/sync_queue.o fftools/thread_queue.o -Llib -lavdevice_x64 -lavfilter_x64 -lavformat_x64 -lavcodec_x64 -lswresample_x64 -lswscale_x64 -lavutil_x64 -lpostproc_x64 -laom_x64 -laribb24_x64 -lcelt_opus_silk_x64 -lcelt_x64 -lcodec2_x64 -ldav1d_x64 -ljxl_x64 -lkvazaar_x64 -llcms2_x64 -lopenh264_x64 -lopenjpeg_x64 -lsvtav1_x64 -luavs3d_x64 -luavs3e_x64 -lwebp_x64 -lx264_0810bit_x64 -lx265_081012bit_x64 -lxavs_x64 -lxavs2_x64 -lxevd_x64 -lxeve_x64 -lxvid_x64 -lvorbis_x64 -ltwolame_x64 -ltheora_x64 -lspeex_x64 -lsnappy_x64 -lshine_x64 -lmp3lame_x64 -lgsm_x64 -ldavs2_x64 -lvpx_x64 -lcairo_pixman_x64 -luvg266_x64 -lxml2_x64 -logg_x64 -lpthread_x64 -lpng_x64 -l:zlib_x64.a -l:fontconfig_x64.a -l:freetype_x64.a -l:fribidi_x64.a -l:harfbuzz_x64.a -L. -ldnsapi -ldwrite -lbcrypt -liphlpapi -lgdi32 -lole32 -loleaut32 -lshlwapi -lstrmiids -luuid -lvfw32 -lws2_32 
cd libavdevice
for %%f in ("%~dp1*.c") do gcc.exe -std=gnu11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -DHAVE_AV_CONFIG_H=1 -DBUILDING_avutil=1 -DFFPLUS=1 -c %%f -o %%~nf.o
ar.exe rcs "../lib/libavdevice_x64.a" alldevices.o avdevice.o dshow.o dshow_common.o dshow_crossbar.o dshow_enummediatypes.o dshow_enumpins.o dshow_filter.o dshow_pin.o gdigrab.o lavfi.o reverse.o sdl2.o timefilter.o utils.o version.o vfwcap.o 
cd ..
g++.exe -std=gnu++11 -march=x86-64-v2 -ftree-vectorize -g0 -O3 -fPIC -mavx -mxsave -mpclmul -maes -o "lib/ffplay.exe" fftools/ffplay.o fftools/cmdutils.o fftools/opt_common.o -Llib -lsdl2_x64 -lavdevice_x64 -lavfilter_x64 -lavformat_x64 -lavcodec_x64 -lswresample_x64 -lswscale_x64 -lavutil_x64 -lpostproc_x64 -laom_x64 -laribb24_x64 -lcelt_opus_silk_x64 -lcelt_x64 -lcodec2_x64 -ldav1d_x64 -ljxl_x64 -lkvazaar_x64 -llcms2_x64 -lopenh264_x64 -lopenjpeg_x64 -lsvtav1_x64 -luavs3d_x64 -luavs3e_x64 -lwebp_x64 -lx264_0810bit_x64 -lx265_081012bit_x64 -lxavs_x64 -lxavs2_x64 -lxevd_x64 -lxeve_x64 -lxvid_x64 -lvorbis_x64 -ltwolame_x64 -ltheora_x64 -lspeex_x64 -lsnappy_x64 -lshine_x64 -lmp3lame_x64 -lgsm_x64 -ldavs2_x64 -lvpx_x64 -lcairo_pixman_x64 -luvg266_x64 -lxml2_x64 -logg_x64 -lpthread_x64 -lpng_x64 -l:zlib_x64.a -l:fontconfig_x64.a -l:freetype_x64.a -l:fribidi_x64.a -l:harfbuzz_x64.a -L. -lbcrypt -ldnsapi -ldwrite -lgdi32 -liphlpapi -limm32 -lole32 -loleaut32 -lshcore -lsetupapi -lstrmiids -lshlwapi -luuid -lversion -lvfw32 -lwinmm -lws2_32 
pause
