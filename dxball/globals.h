#ifndef GLOBALS_H
#define GLOBALS_H

// ─────────────────────────────────────────────
//  Window & World
// ─────────────────────────────────────────────
#define WIN_W  800
#define WIN_H  600
#define PI     3.14159265358979f

// ─────────────────────────────────────────────
//  Game dimensions
// ─────────────────────────────────────────────
#define PADDLE_NORMAL_W   100.0f
#define PADDLE_WIDE_W     155.0f
#define PADDLE_NARROW_W    60.0f
#define PADDLE_H           14.0f
#define PADDLE_Y           40.0f

#define BALL_RADIUS         9.0f

#define BRICK_COLS         10
#define BRICK_ROWS_MAX      7
#define MAX_BRICKS         80      // 10 * 8 max
#define BRICK_W            72.0f
#define BRICK_H            22.0f
#define BRICK_PAD_X         4.0f
#define BRICK_PAD_Y         5.0f
#define BRICK_TOP         480.0f   // top-most brick row Y (decreasing Y = higher)

#define MAX_PERKS          30
#define PERK_W             28.0f
#define PERK_H             16.0f
#define PERK_FALL_SPEED     2.5f

#define MAX_PARTICLES      200
#define MAX_STARS           80
#define MAX_FLOATING_TEXTS  20

// ─────────────────────────────────────────────
//  Game states
// ─────────────────────────────────────────────
enum GameState {
    STATE_MENU = 0,
    STATE_LEVEL_SELECT,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_LEVEL_CLEAR,
    STATE_GAME_OVER,
    STATE_WIN,
    STATE_HELP,
    STATE_HIGH_SCORE
};

// ─────────────────────────────────────────────
//  Perk types
// ─────────────────────────────────────────────
enum PerkType {
    PERK_NONE = 0,
    PERK_EXTRA_LIFE,     // +1 life
    PERK_WIDE_PADDLE,    // wider paddle (timed)
    PERK_NARROW_PADDLE,  // narrower paddle (damage)
    PERK_FAST_BALL,      // faster ball
    PERK_SLOW_BALL,      // slower ball
    PERK_FIREBALL,       // pass through bricks (timed)
    PERK_MULTI_BALL,     // extra ball (simulated as extra hit power)
    PERK_COUNT
};

// ─────────────────────────────────────────────
//  Structs
// ─────────────────────────────────────────────
struct Color3 {
    float r, g, b;
};

struct Brick {
    float w, h;
    float x, y;
    int   hp, maxHp;
    bool  active;
    bool  indestructible;   // wall brick
    Color3 color;
};

struct Perk {
    float x, y;
    PerkType type;
    bool  active;
    float animT;            // rotation / animation counter
};

struct Particle {
    float x, y;
    float dx, dy;
    float life, maxLife;
    float size;
    Color3 color;
    bool active;
};

struct Star {
    float x, y, speed, size, brightness;
};

struct FloatingText {
    float x, y, dy;
    float life, maxLife;
    char  text[32];
    Color3 color;
    bool  active;
};

struct HighScore {
    int   score;
    int   level;
    int   timeSeconds;
    bool  valid;
};

// ─────────────────────────────────────────────
//  Global variable declarations (defined in globals.cpp)
// ─────────────────────────────────────────────
extern GameState gameState;
extern int       score;
extern int       lives;
extern int       currentLevel;
extern int       gameTimeSec;     // elapsed seconds
extern int       frameCount;

// Ball
extern float ballX, ballY;
extern float ballDX, ballDY;
extern bool  ballLaunched;

// Paddle
extern float paddleX;
extern float paddleW;

// Bricks
extern Brick    bricks[MAX_BRICKS];
extern int      brickCount;

// Perks
extern Perk     perks[MAX_PERKS];

// Particles
extern Particle particles[MAX_PARTICLES];

// Stars
extern Star     stars[MAX_STARS];

// Floating texts
extern FloatingText floatTexts[MAX_FLOATING_TEXTS];

// Perk timers (frames)
extern int  fireballTimer;
extern int  widePaddleTimer;
extern int  narrowPaddleTimer;

// Speed escalation per level
extern float baseBallSpeed;

// Menu animation
extern float menuAnim;
extern int   menuSelected;  // 0-based selected option

// Level clear animation
extern int   levelClearCountdown;

// Mouse
extern int mouseX, mouseY;

// High scores
extern HighScore highScores[10];

#endif // GLOBALS_H
