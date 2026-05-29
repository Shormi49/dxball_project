#ifndef PHYSICS_H
#define PHYSICS_H
#include "globals.h"

void checkBrickCollision();
void checkPaddleCollision();
void checkLevelComplete();
void normalizeSpeed(float targetSpeed);
void spawnPerk(float x, float y);

// High-score save
void saveHighScore();

#endif
