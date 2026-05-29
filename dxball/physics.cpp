#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "physics.h"
#include "game.h"
#include "levels.h"
#include "ui.h"
#include "sound.h"

// ─────────────────────────────────────────────
//  Speed normalisation
// ─────────────────────────────────────────────
void normalizeSpeed(float targetSpeed) {
    float cur = sqrtf(ballDX * ballDX + ballDY * ballDY);
    if (cur < 0.001f) { ballDX = targetSpeed; ballDY = targetSpeed; return; }
    ballDX = (ballDX / cur) * targetSpeed;
    ballDY = (ballDY / cur) * targetSpeed;
}

// ─────────────────────────────────────────────
//  Perk spawning
// ─────────────────────────────────────────────
void spawnPerk(float x, float y) {
    for (int i = 0; i < MAX_PERKS; i++) {
        if (perks[i].active) continue;
        // Random perk type (exclude PERK_NONE)
        PerkType pt = (PerkType)(1 + rand() % (PERK_COUNT - 1));
        perks[i].x      = x;
        perks[i].y      = y;
        perks[i].type   = pt;
        perks[i].active = true;
        perks[i].animT  = 0.0f;
        return;
    }
}

// ─────────────────────────────────────────────
//  AABB helper: circle vs rectangle
// ─────────────────────────────────────────────
static bool circleRect(float cx, float cy, float r,
                        float rx, float ry, float rw, float rh)
{
    float nearX = cx < rx ? rx : (cx > rx+rw ? rx+rw : cx);
    float nearY = cy < ry ? ry : (cy > ry+rh ? ry+rh : cy);
    float dx = cx - nearX, dy = cy - nearY;
    return dx*dx + dy*dy <= r*r;
}

// Returns 'H' (horizontal bounce) or 'V' (vertical bounce)
static char collisionSide(float cx, float cy, float r,
                           float rx, float ry, float rw, float rh)
{
    // Overlap on each axis
    float overlapLeft   = (cx + r) - rx;
    float overlapRight  = (rx + rw) - (cx - r);
    float overlapBottom = (cy + r) - ry;
    float overlapTop    = (ry + rh) - (cy - r);

    float minH = overlapLeft  < overlapRight  ? overlapLeft  : overlapRight;
    float minV = overlapBottom < overlapTop   ? overlapBottom : overlapTop;

    return (minH < minV) ? 'H' : 'V';
}

// ─────────────────────────────────────────────
//  Paddle collision
// ─────────────────────────────────────────────
void checkPaddleCollision() {
    float py = PADDLE_Y;
    if (!circleRect(ballX, ballY, BALL_RADIUS,
                    paddleX, py, paddleW, PADDLE_H)) return;
    if (ballDY > 0) return;  // only when ball moving downward

    // Reflect vertical
    ballDY = fabsf(ballDY);

    // Angle based on hit position relative to paddle centre
    float rel = (ballX - (paddleX + paddleW / 2.0f)) / (paddleW / 2.0f);
    if (rel < -1.0f) rel = -1.0f;
    if (rel >  1.0f) rel =  1.0f;

    float spd = sqrtf(ballDX*ballDX + ballDY*ballDY);
    float ang = rel * 60.0f * (PI / 180.0f);   // max ±60°
    ballDX = spd * sinf(ang);
    ballDY = spd * cosf(ang);
    if (ballDY < 0) ballDY = -ballDY;           // always upward

    // Minimum horizontal speed to prevent near-vertical bouncing
    if (fabsf(ballDX) < 1.0f)
        ballDX = (ballDX < 0) ? -1.0f : 1.0f;

    soundPlay(SND_PADDLE);
    spawnParticles(ballX, PADDLE_Y + PADDLE_H,
                   {0.0f, 1.0f, 1.0f}, 4);
}

// ─────────────────────────────────────────────
//  Brick collision
// ─────────────────────────────────────────────
void checkBrickCollision() {
    bool fireball = (fireballTimer > 0);
    bool hitSound = false;

    for (int i = 0; i < brickCount; i++) {
        if (!bricks[i].active) continue;
        Brick& b = bricks[i];

        if (!circleRect(ballX, ballY, BALL_RADIUS,
                        b.x, b.y, b.w, b.h)) continue;

        // Wall bricks bounce ball but are indestructible
        if (!fireball) {
            char side = collisionSide(ballX, ballY, BALL_RADIUS,
                                      b.x, b.y, b.w, b.h);
            if (side == 'H') ballDX = -ballDX;
            else             ballDY = -ballDY;
        }

        if (b.indestructible) {
            soundPlay(SND_WALL);
            spawnParticles(ballX, ballY, {0.6f,0.6f,0.7f}, 3);
            continue;
        }

        // Damage the brick
        b.hp--;
        hitSound = true;

        // Update colour for remaining HP
        b.color = brickColor(currentLevel,
                  (int)((BRICK_TOP - b.y) / (BRICK_H + BRICK_PAD_Y)),
                  b.hp, b.maxHp);

        int pts = fireball ? 20 : (b.hp == 0 ? 10 : 5);
        addScore(pts, b.x + b.w / 2.0f, b.y + b.h / 2.0f);

        spawnParticles(b.x + b.w / 2.0f, b.y + b.h / 2.0f, b.color, 6);

        if (b.hp <= 0) {
            b.active = false;
            // Perk drop (22% chance)
            if (rand() % 100 < 22)
                spawnPerk(b.x + b.w / 2.0f, b.y + b.h / 2.0f);
        }

        if (fireball) continue;   // fireball ignores multi-brick, but still bounces once
        break;                    // only process one brick per frame for stable physics
    }

    if (hitSound) soundPlay(SND_HIT);
}

// ─────────────────────────────────────────────
//  Level complete
// ─────────────────────────────────────────────
void checkLevelComplete() {
    for (int i = 0; i < brickCount; i++)
        if (bricks[i].active && !bricks[i].indestructible) return;

    // All destructible bricks gone
    soundPlay(SND_LEVEL_UP);

    if (currentLevel >= 3) {
        gameState = STATE_WIN;
        saveHighScore();
    } else {
        gameState            = STATE_LEVEL_CLEAR;
        levelClearCountdown  = 180;  // ~3 seconds at 60 fps
    }
}

// ─────────────────────────────────────────────
//  High score (simple file-based persistence)
// ─────────────────────────────────────────────
void saveHighScore() {
    // Load existing
    FILE* f = fopen("highscores.dat", "rb");
    if (f) {
        fread(highScores, sizeof(HighScore), 10, f);
        fclose(f);
    }

    // Find a slot (lowest score or invalid)
    int slot = -1;
    for (int i = 0; i < 10; i++) {
        if (!highScores[i].valid) { slot = i; break; }
    }
    if (slot == -1) {
        // Find lowest
        int minS = highScores[0].score, minI = 0;
        for (int i = 1; i < 10; i++)
            if (highScores[i].score < minS) { minS = highScores[i].score; minI = i; }
        if (score > minS) slot = minI;
    }

    if (slot >= 0) {
        highScores[slot].score       = score;
        highScores[slot].level       = currentLevel;
        highScores[slot].timeSeconds = gameTimeSec;
        highScores[slot].valid       = true;

        // Sort descending by score (bubble sort – small array)
        for (int i = 0; i < 9; i++)
            for (int j = i+1; j < 10; j++)
                if (highScores[j].valid && highScores[j].score > highScores[i].score) {
                    HighScore tmp = highScores[i];
                    highScores[i] = highScores[j];
                    highScores[j] = tmp;
                }
    }

    f = fopen("highscores.dat", "wb");
    if (f) { fwrite(highScores, sizeof(HighScore), 10, f); fclose(f); }
}
