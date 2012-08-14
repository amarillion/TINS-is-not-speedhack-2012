#!/bin/bash

export libs=" \
	-lallegro-5.0.7-monolith-static-mt \
	-ldumb-0.9.3-static-mt \
	-lFLAC-1.2.1-static-mt \
	-lfreetype-2.4.8-static-mt \
	-logg-1.2.1-static-mt \
	-lzlib-1.2.5-static-mt \
	-lgdiplus \
	-luuid \
	-lkernel32 \
	-lwinmm \
	-lpsapi \
	-lopengl32 \
	-lglu32 \
	-luser32 \
	-lcomdlg32 \
	-lgdi32 \
	-lshell32 \
	-lole32 \
	-ladvapi32 \
	-lws2_32 \
	-lshlwapi"

g++ -O2 -Wall src/*.cpp -o bin/drakes.exe -DALLEGRO_STATICLINK $libs
