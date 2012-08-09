#
# invoke "make BUILD=DEBUG" or "make BUILD=RELEASE"
# 
# You can override this from the command line,
# e.g. make BUILD=DEBUG
BUILD=DEBUG
#BUILD=RELEASE
#BUILD=STATIC
#BUILD=STATIC_DEBUG

CXX = g++
LD = g++
CFLAGS = -Iinclude -Ishared/include -W -Wall -Wno-unused -DUSE_ALFONT -DUSE_OGG -DUSE_TEGEL
LFLAGS = 
LIBS = -lalfont -ltegel
NAME = tins12

ifeq ($(BUILD),RELEASE)
	CFLAGS += -O3
	LFLAGS += -s
endif
ifeq ($(BUILD),DEBUG)
	CFLAGS += -g -DDEBUG
endif
ifeq ($(BUILD),STATIC)
	CFLAGS += -O3 -DALLEGRO_STATICLINK
	LFLAGS += -s
endif
ifeq ($(BUILD),STATIC_DEBUG)
	CFLAGS += -g -DDEBUG -DALLEGRO_STATICLINK
endif

ifndef WINDOWS
ifdef MINGDIR
	WINDOWS = 1
endif
endif

ifdef WINDOWS
	OBJDIR = obj/win
	CFLAGS += -D__GTHREAD_HIDE_WIN32API
	LFLAGS += -Wl,--subsystem,windows
	ifeq ($(BUILD),RELEASE)
		LIBS += -llogg -logg -lvorbis -lvorbisfile -lalleg44
	endif
	ifeq ($(BUILD),DEBUG)
		LIBS += -llogg -logg -lvorbis -lvorbisfile -lalleg44-debug
	endif
	ifeq ($(BUILD),STATIC_DEBUG)
		LIBS += -llogg -logg -lvorbis -lvorbisfile -lalleg44-debug-static -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound
	endif
	ifeq ($(BUILD),STATIC)
		LIBS += -llogg -logg -lvorbis -lvorbisfile -lalleg44-static -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound
	endif
	BIN = $(NAME).exe
	ICONOBJ = $(OBJDIR)/icon.o	
else
	OBJDIR = obj/unx
	ifeq ($(BUILD),RELEASE)
		LIBS += `pkg-config allegro --libs logg`
	endif
	ifeq ($(BUILD),DEBUG)
		LIBS += `pkg-config allegro-debug --libs logg`
	endif
	ifeq ($(BUILD),STATIC)
		LIBS += `pkg-config allegro --libs logg --static`
	endif
	ifeq ($(BUILD),STATIC_DEBUG)
		LIBS += `pkg-config allegro-debug --libs logg --static`
	endif
	BIN = $(NAME)
endif

SRC = $(wildcard src/*.cpp)
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o, $(basename $(notdir $(SRC)))))
DEP = $(addprefix $(OBJDIR)/,$(addsuffix .d, $(basename $(notdir $(SRC)))))

# shared objects of the twist engine

SRC_SH = $(wildcard shared/src/*.cpp)
OBJ_SH = $(addprefix $(OBJDIR)/,$(addsuffix .o, $(basename $(notdir $(SRC_SH)))))
DEP_SH = $(addprefix $(OBJDIR)/,$(addsuffix .d, $(basename $(notdir $(SRC_SH)))))

.PHONY: game
game: $(OBJ) $(ICONOBJ) $(OBJ_SH)
	$(LD) -o $(BIN) $^ $(LIBS) $(LFLAGS)

.PHONY: all
all: game

$(OBJ): $(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -MMD -o $@ -c $<

$(OBJ_SH): $(OBJDIR)/%.o: shared/src/%.cpp
	$(CXX) $(CFLAGS) -MMD -o $@ -c $<

$(ICONOBJ) : icon.rc icon.ico
	windres -I rc -O coff -i icon.rc -o $(ICONOBJ)

# include the .d files
-include $(DEP) $(DEP_SH)

RMFILES = $(OBJ) $(DEP) $(OBJ_SH) $(DEP_SH)

.PHONY: clean
clean:
ifdef WINDOWS
	del /q $(subst /,\,$(RMFILES))
else
	rm -f $(RMFILES)
endif

.PHONY:distclean
distclean: clean
ifdef WINDOWS
	del /q $(subst /,\,$(BIN))
else
	rm -f $(BIN)
endif

