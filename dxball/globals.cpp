#include "globals.h"
#include <cstring>
#include <cstdlib>
#include <cmath>

GameState gameState    = STATE_MENU;
int       score        = 0;
int       lives        = 2;
int       currentLevel = 1;
int       gameTimeSec  = 0;
int       frameCount   = 0;

float ballX  = 400.0f;
float ballY  = PADDLE_Y + PADDLE_H + BALL_RADIUS + 2.0f;
float ballDX = 4.0f;
float ballDY = 4.0f;
bool  ballLaunched = false;

float paddleX = (WIN_W - PADDLE_NORMAL_W) / 2.0f;
float paddleW = PADDLE_NORMAL_W;

Brick    bricks[MAX_BRICKS];
int      brickCount = 0;

Perk     perks[MAX_PERKS];
Particle particles[MAX_PARTICLES];
Star     stars[MAX_STARS];
FloatingText floatTexts[MAX_FLOATING_TEXTS];

int  fireballTimer    = 0;
int  widePaddleTimer  = 0;
int  narrowPaddleTimer= 0;

float baseBallSpeed   = 4.0f;

float menuAnim      = 0.0f;
int   menuSelected  = 0;

int   levelClearCountdown = 0;

int mouseX = WIN_W / 2;
int mouseY = WIN_H / 2;

HighScore highScores[10];
