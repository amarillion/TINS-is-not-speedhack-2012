CXX = g++
LD = g++
CFLAGS = -Isrc -W -Wall
LFLAGS = -g -DDEBUG
LIBS = 
NAME = tins12

OBJDIR = obj
LIBS += `pkg-config --libs allegro-debug-5.0 allegro_main-debug-5.0 allegro_image-debug-5.0 allegro_font-debug-5.0 allegro_audio-debug-5.0 allegro_acodec-debug-5.0 allegro_primitives-debug-5.0 allegro_font-debug-5.0 allegro_ttf-debug-5.0`
BIN = $(NAME)

SRC = $(wildcard src/*.cpp)
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o, $(basename $(notdir $(SRC)))))

.PHONY: game
game: $(OBJ)
	$(LD) -o $(BIN) $^ $(LIBS) $(LFLAGS)

.PHONY: all
all: game

$(OBJ): $(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -MMD -o $@ -c $<

RMFILES = $(OBJ)

.PHONY: clean
clean:
	rm -f $(RMFILES)

.PHONY:distclean
distclean: clean
	rm -f $(BIN)

