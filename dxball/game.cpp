#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "game.h"
#include "levels.h"
#include "physics.h"
#include "ui.h"
#include "sound.h"

void initGame() {
    score        = 0;
    lives        = 3;
    gameTimeSec  = 0;
    frameCount   = 0;
    fireballTimer     = 0;
    widePaddleTimer   = 0;
    narrowPaddleTimer = 0;
    paddleW = PADDLE_NORMAL_W;

    for (int i = 0; i < MAX_PARTICLES;      i++) particles[i].active  = false;
    for (int i = 0; i < MAX_FLOATING_TEXTS; i++) floatTexts[i].active = false;
    for (int i = 0; i < MAX_PERKS;          i++) perks[i].active      = false;
}

void resetGame() {
    initGame();
    initLevel(currentLevel);
}

void resetBallToPaddle() {
    paddleW           = PADDLE_NORMAL_W;
    fireballTimer     = 0;
    widePaddleTimer   = 0;
    narrowPaddleTimer = 0;
    float cx = paddleX + paddleW / 2.0f;
    ballX        = cx;
    ballY        = PADDLE_Y + PADDLE_H + BALL_RADIUS + 2.0f;
    ballDX       = baseBallSpeed * (rand() % 2 ? 1.0f : -1.0f);
    ballDY       = baseBallSpeed;
    ballLaunched = false;
    for (int i = 0; i < MAX_PERKS; i++) perks[i].active = false;
}

void launchBall() {
    if (!ballLaunched) {
        ballLaunched = true;
        soundPlay(SND_LAUNCH);
    }
}

void addScore(int pts, float x, float y) {
    score += pts;
    char buf[16];
    snprintf(buf, sizeof(buf), "+%d", pts);
    Color3 col = (pts >= 20) ? COL_ORANGE : COL_YELLOW;
    spawnFloatText(x, y, buf, col);
}

void formatTime(char* buf, int secs) {
    snprintf(buf, 16, "%d:%02d", secs / 60, secs % 60);
}

void applyPerk(PerkType type) {
    soundPlay(SND_PERK);
    switch (type) {
    case PERK_EXTRA_LIFE:
        if (lives < 9) lives++;
        spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "+1 LIFE!", COL_RED);
        break;
    case PERK_WIDE_PADDLE:
        paddleW           = PADDLE_WIDE_W;
        widePaddleTimer   = 600;
        narrowPaddleTimer = 0;
        if (paddleX + paddleW > WIN_W) paddleX = WIN_W - paddleW;
        spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "WIDE PADDLE!", COL_CYAN);
        break;
    case PERK_NARROW_PADDLE:
        paddleW           = PADDLE_NARROW_W;
        narrowPaddleTimer = 400;
        widePaddleTimer   = 0;
        spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "SHRINK!", COL_MAGENTA);
        break;
    case PERK_FAST_BALL:
        baseBallSpeed *= 1.25f;
        if (baseBallSpeed > 13.0f) baseBallSpeed = 13.0f;
        normalizeSpeed(baseBallSpeed);
        spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "SPEED UP!", COL_ORANGE);
        break;
    case PERK_SLOW_BALL:
        baseBallSpeed *= 0.78f;
        if (baseBallSpeed < 2.5f) baseBallSpeed = 2.5f;
        normalizeSpeed(baseBallSpeed);
        spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "SLOW BALL!", MakeColor(0.7f,0.7f,1.0f));
        break;
    case PERK_FIREBALL:
        fireballTimer = 300;
        spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "FIREBALL!", COL_ORANGE);
        break;
    default: break;
    }
}

void drawPaddle() {
    float px = paddleX, py = PADDLE_Y;
    float pw = paddleW, ph = PADDLE_H;

    Color3 glowCol = COL_CYAN;
    if (fireballTimer    > 0) glowCol = COL_ORANGE;
    if (widePaddleTimer  > 0) glowCol = COL_GREEN;
    if (narrowPaddleTimer> 0) glowCol = COL_MAGENTA;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawGlowRect(px, py, pw, ph, glowCol, 1.5f);

    Color3 top = glowCol;
    Color3 bot = { top.r*0.3f, top.g*0.3f, top.b*0.3f };
    drawGradientRectV(px, py, pw, ph, top, bot);

    glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
    glRectf(px+2, py+ph-4, px+pw-2, py+ph);

    glColor3f(glowCol.r, glowCol.g, glowCol.b);
    glLineWidth(2.0f);
    drawRect(px, py, pw, ph, 2.0f);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
}

void drawBall() {
    bool fb = (fireballTimer > 0);
    Color3 ballCol = fb ? COL_ORANGE : Color3{1.0f, 0.25f, 0.35f};

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 3; i >= 1; i--) {
        float alpha = 0.1f / i;
        glColor4f(ballCol.r, ballCol.g, ballCol.b, alpha);
        drawCircle(ballX, ballY, BALL_RADIUS + i*4.0f, true, 24);
    }

    if (fb && frameCount % 2 == 0)
        spawnParticles(ballX, ballY, COL_ORANGE, 2);

    int steps = 6;
    for (int s = steps; s >= 1; s--) {
        float t     = (float)s / steps;
        float r     = BALL_RADIUS * t;
        float blend = 1.0f - t;
        glColor3f(ballCol.r + blend*(1.0f-ballCol.r),
                  ballCol.g + blend*(1.0f-ballCol.g),
                  ballCol.b + blend*(1.0f-ballCol.b));
        drawCircle(ballX, ballY, r, true, 24);
    }
    glColor4f(1.0f, 1.0f, 1.0f, 0.55f);
    drawCircle(ballX - BALL_RADIUS*0.3f,
               ballY + BALL_RADIUS*0.3f,
               BALL_RADIUS*0.3f, true, 12);
    glDisable(GL_BLEND);

    if (!ballLaunched) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
        drawTextCentered(WIN_W/2.0f, PADDLE_Y+PADDLE_H+30.0f,
                         "Click or SPACE to launch",
                         GLUT_BITMAP_HELVETICA_12);
        glDisable(GL_BLEND);
    }
}

static Color3 perkColor(PerkType t) {
    switch(t) {
    case PERK_EXTRA_LIFE:    return COL_RED;
    case PERK_WIDE_PADDLE:   return COL_CYAN;
    case PERK_NARROW_PADDLE: return COL_MAGENTA;
    case PERK_FAST_BALL:     return COL_ORANGE;
    case PERK_SLOW_BALL:     return {0.6f,0.6f,1.0f};
    case PERK_FIREBALL:      return {1.0f,0.5f,0.0f};
    default:                 return COL_WHITE;
    }
}

static const char* perkLabel(PerkType t) {
    switch(t) {
    case PERK_EXTRA_LIFE:    return "+LIFE";
    case PERK_WIDE_PADDLE:   return "WIDE";
    case PERK_NARROW_PADDLE: return "SHRK";
    case PERK_FAST_BALL:     return "FAST";
    case PERK_SLOW_BALL:     return "SLOW";
    case PERK_FIREBALL:      return "FIRE";
    default:                 return "???";
    }
}

void drawPerks() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < MAX_PERKS; i++) {
        if (!perks[i].active) continue;
        Perk& p = perks[i];
        p.animT += 0.08f;
        Color3 col   = perkColor(p.type);
        float  pulse = 0.7f + 0.3f*sinf(p.animT*3.0f);
        drawGlowRect(p.x-PERK_W/2.0f, p.y-PERK_H/2.0f, PERK_W, PERK_H, col, pulse);
        glColor3f(col.r*0.6f, col.g*0.6f, col.b*0.6f);
        drawRoundedRect(p.x-PERK_W/2.0f, p.y-PERK_H/2.0f, PERK_W, PERK_H, 4.0f, true);
        glColor3f(col.r, col.g, col.b);
        glLineWidth(1.5f);
        drawRoundedRect(p.x-PERK_W/2.0f, p.y-PERK_H/2.0f, PERK_W, PERK_H, 4.0f, false);
        glLineWidth(1.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawTextCentered(p.x, p.y-4.0f, perkLabel(p.type), GLUT_BITMAP_HELVETICA_10);
    }
    glDisable(GL_BLEND);
}

void updateGame() {
    frameCount++;
    updateStars();
    updateParticles();
    updateFloatTexts();

    if (!ballLaunched) return;

    ballX += ballDX;
    ballY += ballDY;

    // Wall collision
    if (ballX - BALL_RADIUS <= 0.0f) {
        ballX  = BALL_RADIUS;
        ballDX = fabsf(ballDX);
        soundPlay(SND_WALL);
        spawnParticles(ballX, ballY, COL_CYAN, 3);
    }
    if (ballX + BALL_RADIUS >= WIN_W) {
        ballX  = WIN_W - BALL_RADIUS;
        ballDX = -fabsf(ballDX);
        soundPlay(SND_WALL);
        spawnParticles(ballX, ballY, COL_CYAN, 3);
    }
    if (ballY + BALL_RADIUS >= WIN_H) {
        ballY  = WIN_H - BALL_RADIUS;
        ballDY = -fabsf(ballDY);
        soundPlay(SND_WALL);
    }

    // Paddle collision
    checkPaddleCollision();

    // Brick collision
    checkBrickCollision();

    // Bottom out — life lost
    if (ballY - BALL_RADIUS <= 0.0f) {
        lives--;
        soundPlay(SND_LOSE_LIFE);
        spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "LIFE LOST!", COL_RED);
        if (lives <= 0) {
            gameState = STATE_GAME_OVER;
            soundPlay(SND_GAME_OVER);
            saveHighScore();
        } else {
            resetBallToPaddle();
        }
    }

    // Perk fall and catch
    for (int i = 0; i < MAX_PERKS; i++) {
        if (!perks[i].active) continue;
        perks[i].y -= PERK_FALL_SPEED;
        float px = paddleX, pw = paddleW, py = PADDLE_Y;
        if (perks[i].y - PERK_H/2.0f <= py + PADDLE_H &&
            perks[i].y + PERK_H/2.0f >= py &&
            perks[i].x >= px &&
            perks[i].x <= px + pw) {
            applyPerk(perks[i].type);
            perks[i].active = false;
        }
        if (perks[i].y + PERK_H/2.0f < 0.0f)
            perks[i].active = false;
    }

    // Perk timers
    if (fireballTimer > 0) {
        fireballTimer--;
        if (fireballTimer == 0)
            spawnFloatText(WIN_W/2.0f, WIN_H/2.0f, "FIREBALL ENDED", COL_ORANGE);
    }
    if (widePaddleTimer > 0) {
        widePaddleTimer--;
        if (widePaddleTimer == 0) {
            paddleW = PADDLE_NORMAL_W;
            if (paddleX + paddleW > WIN_W) paddleX = WIN_W - paddleW;
        }
    }
    if (narrowPaddleTimer > 0) {
        narrowPaddleTimer--;
        if (narrowPaddleTimer == 0)
            paddleW = PADDLE_NORMAL_W;
    }

    // Speed escalation every 20 seconds
    if (ballLaunched && frameCount % 1200 == 0) {
        baseBallSpeed += 0.25f;
        if (baseBallSpeed > 13.0f) baseBallSpeed = 13.0f;
        normalizeSpeed(baseBallSpeed);
        spawnFloatText(WIN_W/2.0f, WIN_H-60.0f, "SPEED UP!", COL_YELLOW);
    }

    checkLevelComplete();
}