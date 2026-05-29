#ifndef GAME_H
#define GAME_H
#include "globals.h"

void initGame();
void resetGame();
void resetBallToPaddle();
void launchBall();
void updateGame();

void drawPaddle();
void drawBall();
void drawPerks();

void applyPerk(PerkType type);

// Score helpers
void addScore(int pts, float x, float y);

// Time string helper
void formatTime(char* buf, int seconds);

#endif
