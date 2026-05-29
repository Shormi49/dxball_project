# ────────────────────────────────────────────────────────────────
#  DX Ball — CSE 426 Computer Graphics Lab
#  Makefile  (Linux + Windows MinGW)
# ────────────────────────────────────────────────────────────────

CXX      = g++
TARGET   = dxball

SRCS     = main.cpp globals.cpp ui.cpp levels.cpp game.cpp \
           physics.cpp menu.cpp input.cpp sound.cpp

OBJS     = $(SRCS:.cpp=.o)

# ── Detect OS ────────────────────────────────────────────────────
UNAME := $(shell uname -s 2>/dev/null || echo Windows)

ifeq ($(UNAME), Linux)
    # Linux — needs freeglut3-dev, libglu1-mesa-dev, libgl1-mesa-dev
    CXXFLAGS = -std=c++17 -O2 -Wall -Wno-deprecated-declarations
    LIBS     = -lGL -lGLU -lglut -lm
    EXT      =

else ifeq ($(UNAME), Darwin)
    # macOS
    CXXFLAGS = -std=c++17 -O2 -Wall
    LIBS     = -framework OpenGL -framework GLUT
    EXT      =

else
    # Windows (MinGW) — adjust paths if needed
    # Install:  mingw-w64 + freeglut for MinGW from https://www.transmissionzero.co.uk/software/freeglut-devel/
    CXXFLAGS = -std=c++17 -O2 -Wall -I"C:/freeglut/include"
    LIBS     = -L"C:/freeglut/lib/x64" -lfreeglut -lopengl32 -lglu32 -lwinmm -mwindows
    EXT      = .exe
endif

TARGET_BIN = $(TARGET)$(EXT)

# ── Rules ────────────────────────────────────────────────────────
all: $(TARGET_BIN)

$(TARGET_BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo ""
	@echo "  Build successful!  Run:  ./$(TARGET_BIN)"
	@echo ""

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET_BIN)

.PHONY: all clean
