CD:=cd

ifneq ($(SystemRoot),)
   WIN=true
   MKDIR=cmd /c mkdir
   RM=cmd /c del /S /F /Q
   PATH+=;..\Deps\bin
   ifeq ($(MSVCDIR),)
     REALMAKE=mingw32-make
     CMAKEARGS=-G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=$(REALMAKE)
   else
     REALMAKE=nmake
     MSVC=true
     ifeq ($(IDE),)
	CMAKEARGS=-G "NMake Makefiles"
     else
	CMAKEARGS=-DIDE=true
	REALMAKE=echo "Done"
     endif
   endif
else
   REALMAKE=make
   MKDIR=mkdir -p
   RM=rm -fr
   PATH+=:../Deps/bin
   ifeq ($(ostype),darwin)
     ifeq ($(IDE),)
     else
	CC=
	CXX=
	LD=
	CMAKEARGS=-G "Xcode" -DIDE=true
	REALMAKE=echo "Done"
     endif
   endif
endif
ifeq ($(IPHONE),true)
   CMAKEARGS+=-DIPHONE=true
endif

all: cmake realmake

cmake: .dirs
	$(CD) Build && cmake $(CMAKEARGS) ..

realmake:
	$(CD) Build && $(REALMAKE) ${ARGS}
	
.dirs: Build Bin

Build:
	$(MKDIR) Build

Bin:
	$(MKDIR) Bin

clean:
	$(RM) Build
	$(RM) Bin
