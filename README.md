Cinder-OpenAL
=============

Compiling Soft-OpenAL

on OS X:

http://kcat.strangesoft.net/openal.html#download
cmake -D LIBTYPE=STATIC -D CMAKE_OSX_ARCHITECTURES="i386;x86_64"

Compiling project with Soft-OpenAL

on MSW:
add preprocessor define: AL_LIBTYPE_STATIC
add linker input: Winmm.lib
