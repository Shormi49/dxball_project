#ifndef LEVELS_H
#define LEVELS_H
#include "globals.h"

void initStars();
void initLevel(int lvl);
Color3 brickColor(int level, int row, int hp, int maxHp);
void drawBricks();

// Per-level brick color
Color3 brickColor(int level, int row, int hp, int maxHp);

#endif
