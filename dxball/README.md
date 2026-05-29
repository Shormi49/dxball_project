# DX Ball — CSE 426 Computer Graphics Lab
## Spring 2025 | OpenGL + FreeGLUT | C++17

---

## ✅ Features Implemented

### Basic Requirements (30 marks)
| Feature | Detail |
|---|---|
| Base gameplay | Playable paddle-and-ball game |
| Paddle control | Mouse (smooth follow) + Arrow keys + A/D keys |
| 3 Lives | Displayed in HUD at all times |
| Score | +10 per brick, +5 damage, +20 fireball |
| Timer | Live elapsed time shown in HUD |
| 3 Levels | Increasing difficulty, auto-advance |
| Menu screen | New Game, Level Select, High Scores, Help, Exit |
| Pause | Press P any time during gameplay |
| Exit | ESC returns to menu; Q exits |

### Bonus Features (15 marks)
| Feature | Detail |
|---|---|
| Wall (indestructible) bricks | Grey bricks in Level 3+, ball bounces off |
| Perk drops | 6 different perks (see below) |
| Help screen | Full controls + perk + scoring reference |
| Sound | Windows: PlaySound .wav; Linux: stub |
| Multi-level progression | 3 levels, Level Clear screen, then WIN screen |
| High scores | Persisted to `highscores.dat`, top 10, sorted |
| Particle effects | Brick explosion particles |
| Floating score text | +10 / +20 pop-up on each hit |
| Perk status timers | Bar showing remaining fireball/wide/shrink time |
| Animated menu | Bouncing ball mini-preview, pulsing title |
| Gradient bricks | Lit with gradient + crack marks for multi-HP |

---

## 🎮 Perks
| Capsule | Name | Effect |
|---|---|---|
| +LIFE | Extra Life | Grants +1 life (max 9) |
| WIDE | Wide Paddle | Wider paddle for 10 sec |
| FIRE | Fireball | Ball passes through bricks for 5 sec (+20 pts each) |
| FAST | Fast Ball | Increases ball speed |
| SLOW | Slow Ball | Decreases ball speed |
| SHRK | Shrink | Narrows paddle (damage perk) |

---

## 🕹️ Controls
| Key | Action |
|---|---|
| Mouse move | Move paddle (smooth) |
| Arrow Left / Right | Move paddle |
| A / D | Move paddle |
| Space / Left Click | Launch ball |
| P | Pause / Resume |
| ESC | Return to menu |
| N | New Game (from menu) |
| L | Level Select (from menu) |
| H | Help (from menu) |
| 1 / 2 / 3 | Start level directly |
| Q | Quit |

---

## 🔨 Build Instructions

### Linux (Ubuntu / Debian)
```bash
# 1. Install dependencies
sudo apt install freeglut3-dev libglu1-mesa-dev libgl1-mesa-dev g++

# 2. Build
make

# 3. Run
./dxball
```

### Windows (MinGW + FreeGLUT)
1. Install **MinGW-w64**: https://www.mingw-w64.org/
2. Download **FreeGLUT for MinGW**: https://www.transmissionzero.co.uk/software/freeglut-devel/
3. Extract to `C:\freeglut\`
4. Open MinGW terminal in this folder:
```bash
make
dxball.exe
```

Or compile manually:
```bash
g++ -std=c++17 -O2 -I"C:/freeglut/include" ^
    main.cpp globals.cpp ui.cpp levels.cpp game.cpp ^
    physics.cpp menu.cpp input.cpp sound.cpp ^
    -L"C:/freeglut/lib/x64" -lfreeglut -lopengl32 -lglu32 -lwinmm ^
    -mwindows -o dxball.exe
```

### Sound (Windows only)
Place these `.wav` files next to `dxball.exe`:
- `hit.wav` — brick hit
- `wall.wav` — wall hit
- `paddle.wav` — paddle hit
- `perk.wav` — perk collected
- `lose.wav` — life lost
- `gameover.wav` — game over
- `levelup.wav` — level complete
- `launch.wav` — ball launch

The game runs without them (sounds silently skipped).

---

## 📁 File Structure
```
dxball/
├── main.cpp        — Entry point, GLUT setup, display/timer loop
├── globals.h/cpp   — All global variables and struct definitions
├── ui.h/cpp        — Drawing utilities, stars, particles, HUD, buttons
├── levels.h/cpp    — Level layouts, brick drawing, colour themes
├── game.h/cpp      — Ball/paddle/perk drawing + game update loop
├── physics.h/cpp   — Collision detection, perk spawning, high scores
├── menu.h/cpp      — All non-gameplay screens (menu, help, scores...)
├── input.h/cpp     — Keyboard, special keys, mouse handling
├── sound.h/cpp     — Audio (Windows PlaySound / Linux stub)
└── Makefile
```
