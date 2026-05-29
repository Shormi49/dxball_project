#include <GL/glut.h>
#include <cstdio>
#include <cmath>
#include "menu.h"
#include "ui.h"
#include "game.h"
#include "levels.h"
#include "input.h"
#include "physics.h"

static float pvBallX = 640, pvBallY = 350;
static float pvDX = 2.1f, pvDY = -1.7f;
static bool  pvBrickAlive[12];
static bool  pvInit = false;

static void initPreview() {
    for (int i = 0; i < 12; i++) pvBrickAlive[i] = true;
    pvInit = true;
}

static void drawMenuPreview() {
    if (!pvInit) initPreview();

    float px = 570.0f, py = 200.0f, pw = 190.0f, ph = 220.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.05f, 0.15f, 0.88f);
    drawRoundedRect(px, py, pw, ph, 6.0f, true);
    glColor4f(0.0f, 0.8f, 0.9f, 0.5f);
    drawRoundedRect(px, py, pw, ph, 6.0f, false);

    glColor4f(0.0f, 1.0f, 1.0f, 0.5f);
    drawTextCentered(px + pw/2.0f, py + ph - 14.0f, "PREVIEW", GLUT_BITMAP_HELVETICA_10);
    glDisable(GL_BLEND);

    static const Color3 pvCols[] = {
        {1.0f,0.3f,0.3f},{1.0f,0.7f,0.0f},{0.3f,1.0f,0.5f},
        {0.0f,0.8f,1.0f},{0.7f,0.0f,1.0f},{1.0f,0.0f,0.8f}
    };
    int idx = 0;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++, idx++) {
            if (!pvBrickAlive[idx]) continue;
            float bx = px + 14.0f + c * 40.0f;
            float by = py + ph - 50.0f - r * 24.0f;
            Color3 col = pvCols[(r*4+c) % 6];
            glColor3f(col.r, col.g, col.b);
            glRectf(bx, by, bx + 32.0f, by + 16.0f);
        }
    }

    pvBallX += pvDX; pvBallY += pvDY;
    if (pvBallX < px + 8)   { pvBallX = px + 8;    pvDX =  fabsf(pvDX); }
    if (pvBallX > px+pw-8)  { pvBallX = px+pw-8;   pvDX = -fabsf(pvDX); }
    if (pvBallY < py + 8)   { pvBallY = py + 8;    pvDY =  fabsf(pvDY); }
    if (pvBallY > py+ph-30) { pvBallY = py+ph-30;  pvDY = -fabsf(pvDY); }

    idx = 0;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++, idx++) {
            if (!pvBrickAlive[idx]) continue;
            float bx = px + 14.0f + c * 40.0f, by = py + ph - 50.0f - r * 24.0f;
            if (pvBallX>=bx && pvBallX<=bx+32 && pvBallY>=by && pvBallY<=by+16) {
                pvBrickAlive[idx] = false; pvDY = -pvDY;
            }
        }
    }

    bool any = false;
    for (int i = 0; i < 12; i++) if (pvBrickAlive[i]) { any=true; break; }
    if (!any) initPreview();

    glColor3f(0.0f, 1.0f, 0.9f);
    glRectf(px + pw/2.0f - 30.0f, py + 18.0f, px + pw/2.0f + 30.0f, py + 28.0f);

    glColor3f(1.0f, 0.3f, 0.4f);
    drawCircle(pvBallX, pvBallY, 5.0f, true, 16);
}

static void drawBigTitle(float cx, float y, const char* txt) {
    glColor3f(0.4f, 0.0f, 0.5f);
    drawTextCentered(cx + 3.0f, y - 3.0f, txt, GLUT_BITMAP_TIMES_ROMAN_24);
    float pulse = 0.7f + 0.3f * sinf(menuAnim * 2.0f);
    glColor3f(0.0f, pulse, pulse);
    drawTextCentered(cx, y, txt, GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawMenuScreen() {
    menuAnim += 0.04f;
    drawBackground();
    drawStars();
    drawMenuPreview();

    glColor3f(0.0f, 1.0f, 1.0f);
    glPushMatrix();
        glTranslatef(WIN_W * 0.28f, 490.0f, 0);
        glScalef(3.8f, 3.8f, 1.0f);
        glColor3f(0.2f, 0.0f, 0.3f);
        drawTextCentered(2.0f, -2.0f, "DX BALL", GLUT_BITMAP_TIMES_ROMAN_24);
        float p = 0.7f + 0.3f*sinf(menuAnim*2.5f);
        glColor3f(0.0f, p, p);
        drawTextCentered(0, 0, "DX BALL", GLUT_BITMAP_TIMES_ROMAN_24);
    glPopMatrix();

    glColor3f(0.3f, 0.4f, 0.6f);
    drawTextCentered(WIN_W * 0.28f, 462.0f,
                     "CSE 426  Computer Graphics Lab",
                     GLUT_BITMAP_HELVETICA_12);

    struct Btn { const char* label; const char* hint; Color3 col; };
    static const Btn btns[] = {
        { "NEW GAME",     "[N]", {0.0f,1.0f,1.0f} },
        { "LEVEL SELECT", "[L]", {1.0f,0.9f,0.0f} },
        { "HIGH SCORES",  "[H]", {1.0f,0.4f,0.8f} },
        { "HELP",         "[?]", {0.3f,1.0f,0.5f} },
        { "EXIT",         "[Q]", {1.0f,0.3f,0.3f} },
    };
    const int N = 5;
    float bx = 60.0f, bw = 290.0f, bh = 44.0f, gap = 10.0f;
    float by0 = 380.0f;
    for (int i = 0; i < N; i++) {
        float by = by0 - i * (bh + gap);
        drawMenuButton(bx, by, bw, bh,
                       btns[i].label, btns[i].hint,
                       btns[i].col, menuSelected == i);
    }

    glColor3f(0.2f, 0.3f, 0.4f);
    drawTextCentered(WIN_W / 2.0f, 12.0f,
                     "Spring 2025  |  v2.0",
                     GLUT_BITMAP_HELVETICA_10);

    drawParticles();
    drawFloatTexts();
}

void drawLevelSelectScreen() {
    drawBackground();
    drawStars();

    glColor3f(1.0f, 0.9f, 0.0f);
    glPushMatrix();
        glTranslatef(WIN_W / 2.0f, 530.0f, 0);
        glScalef(2.5f, 2.5f, 1.0f);
        drawTextCentered(0, 0, "SELECT LEVEL", GLUT_BITMAP_HELVETICA_18);
    glPopMatrix();

    struct LvlInfo { int n; const char* title; const char* desc; const char* hint; Color3 col; };
    static const LvlInfo lvls[] = {
        { 1, "LEVEL 1 — EASY",
          "5 rows x 10 cols  |  1 HP bricks  |  Speed: Normal",
          "[1]", {0.0f,1.0f,0.8f} },
        { 2, "LEVEL 2 — MEDIUM",
          "6 rows  |  Top rows: 2 HP bricks  |  Speed: +25%%",
          "[2]", {1.0f,0.7f,0.0f} },
        { 3, "LEVEL 3 — HARD",
          "7 rows  |  Wall bricks  |  Up to 3 HP  |  Speed: +50%%",
          "[3]", {1.0f,0.2f,0.3f} },
    };
    float bx = 180.0f, bw = 440.0f, bh = 88.0f, gap = 16.0f;
    float by = 410.0f;
    for (int i = 0; i < 3; i++) {
        bool hov = isHovered(bx, by - bh, bw, bh);
        drawMenuButton(bx, by - bh, bw, bh,
                       lvls[i].title, lvls[i].hint, lvls[i].col, hov);
        glColor3f(0.6f, 0.65f, 0.7f);
        drawText(bx + 20.0f, by - bh + 16.0f, lvls[i].desc, GLUT_BITMAP_HELVETICA_12);
        by -= bh + gap;
    }

    bool backHov = isHovered(20.0f, 15.0f, 110.0f, 34.0f);
    drawMenuButton(20.0f, 15.0f, 110.0f, 34.0f,
                   "< BACK", "[ESC]", {0.5f,0.6f,0.7f}, backHov);
}

void drawHelpScreen() {
    drawBackground();
    drawStars();

    glColor3f(0.3f, 1.0f, 0.5f);
    glPushMatrix();
        glTranslatef(WIN_W / 2.0f, 555.0f, 0);
        glScalef(2.2f, 2.2f, 1.0f);
        drawTextCentered(0, 0, "HELP / HOW TO PLAY", GLUT_BITMAP_HELVETICA_18);
    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.05f, 0.15f, 0.75f);
    drawRoundedRect(30.0f, 30.0f, WIN_W - 60.0f, WIN_H - 90.0f, 8.0f, true);
    glDisable(GL_BLEND);

    const char* ctrlKeys[]  = { "Mouse / Arrow Keys", "SPACE / Left Click",
                                  "P", "ESC", "1/2/3 (menu)" };
    const char* ctrlDescs[] = { "Move paddle", "Launch ball",
                                  "Pause / Resume", "Return to menu",
                                  "Select level" };
    float cx = 55.0f, cy = 490.0f;
    glColor3f(0.0f, 1.0f, 0.8f);
    drawText(cx, cy, "CONTROLS", GLUT_BITMAP_HELVETICA_18);
    cy -= 22.0f;
    glColor3f(0.5f, 0.9f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(cx, cy + 3); glVertex2f(cx + 220.0f, cy + 3);
    glEnd();
    cy -= 5.0f;
    for (int i = 0; i < 5; i++, cy -= 22.0f) {
        glColor3f(1.0f, 0.85f, 0.0f);
        drawText(cx, cy, ctrlKeys[i], GLUT_BITMAP_HELVETICA_12);
        glColor3f(0.8f, 0.85f, 0.9f);
        drawText(cx + 140.0f, cy, ctrlDescs[i], GLUT_BITMAP_HELVETICA_12);
    }

    const char* perkNames[] = { "+LIFE", "WIDE", "FIRE", "FAST", "SLOW", "SHRK" };
    const char* perkDescs[] = { "Grants an extra life",
                                  "Widens paddle (10 sec)",
                                  "Ball passes through bricks (5 sec)",
                                  "Increases ball speed",
                                  "Decreases ball speed",
                                  "Shrinks paddle (damage)" };
    float px2 = 420.0f, py2 = 490.0f;
    glColor3f(1.0f, 0.6f, 0.0f);
    drawText(px2, py2, "PERK DROPS", GLUT_BITMAP_HELVETICA_18);
    py2 -= 22.0f;
    glColor3f(1.0f, 0.6f, 0.2f);
    glBegin(GL_LINES);
        glVertex2f(px2, py2 + 3); glVertex2f(px2 + 300.0f, py2 + 3);
    glEnd();
    py2 -= 5.0f;
    for (int i = 0; i < 6; i++, py2 -= 22.0f) {
        glColor3f(1.0f, 0.85f, 0.0f);
        drawText(px2, py2, perkNames[i], GLUT_BITMAP_HELVETICA_12);
        glColor3f(0.8f, 0.85f, 0.9f);
        drawText(px2 + 55.0f, py2, perkDescs[i], GLUT_BITMAP_HELVETICA_12);
    }

    float sx = 55.0f, sy = 195.0f;
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(sx, sy, "SCORING", GLUT_BITMAP_HELVETICA_18);
    sy -= 22.0f;
    glColor3f(1.0f, 1.0f, 0.2f);
    glBegin(GL_LINES);
        glVertex2f(sx, sy+3); glVertex2f(sx + 340.0f, sy+3);
    glEnd();
    sy -= 5.0f;
    const char* scLines[] = {
        "Destroy brick:        +10 pts",
        "Damage multi-HP:      +5 pts",
        "Fireball destroy:     +20 pts",
        "Speed increases every 20 seconds automatically",
        "Win all 3 levels for final score ranking"
    };
    glColor3f(0.8f, 0.85f, 0.9f);
    for (int i = 0; i < 5; i++, sy -= 22.0f)
        drawText(sx, sy, scLines[i], GLUT_BITMAP_HELVETICA_12);

    float tx = 420.0f, ty = 195.0f;
    glColor3f(0.8f, 0.5f, 1.0f);
    drawText(tx, ty, "BRICK TYPES", GLUT_BITMAP_HELVETICA_18);
    ty -= 22.0f;
    glColor3f(0.7f, 0.4f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(tx, ty+3); glVertex2f(tx+280.0f, ty+3);
    glEnd();
    ty -= 5.0f;
    const char* btLines[] = {
        "Cyan/Green  (1 HP)  — Standard",
        "Orange/Red  (2 HP)  — Takes 2 hits",
        "Magenta     (3 HP)  — Takes 3 hits",
        "Grey WALL — Indestructible!",
    };
    glColor3f(0.8f, 0.85f, 0.9f);
    for (int i = 0; i < 4; i++, ty -= 22.0f)
        drawText(tx, ty, btLines[i], GLUT_BITMAP_HELVETICA_12);

    bool bHov = isHovered(20.0f, 15.0f, 110.0f, 34.0f);
    drawMenuButton(20.0f, 15.0f, 110.0f, 34.0f,
                   "< BACK", "[ESC]", {0.5f,0.6f,0.7f}, bHov);
}

void drawHighScoreScreen() {
    static bool loaded = false;
    if (!loaded) {
        FILE* f = fopen("highscores.dat", "rb");
        if (f) { fread(highScores, sizeof(HighScore), 10, f); fclose(f); }
        loaded = true;
    }

    drawBackground();
    drawStars();

    glColor3f(1.0f, 0.4f, 0.8f);
    glPushMatrix();
        glTranslatef(WIN_W / 2.0f, 545.0f, 0);
        glScalef(2.5f, 2.5f, 1.0f);
        drawTextCentered(0, 0, "HIGH SCORES", GLUT_BITMAP_HELVETICA_18);
    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.04f, 0.12f, 0.8f);
    drawRoundedRect(100.0f, 60.0f, WIN_W - 200.0f, WIN_H - 130.0f, 8.0f, true);
    glDisable(GL_BLEND);

    float hdrY = WIN_H - 100.0f;
    glColor3f(0.7f, 0.7f, 0.9f);
    drawText(140.0f, hdrY, "#",     GLUT_BITMAP_HELVETICA_18);
    drawText(195.0f, hdrY, "SCORE", GLUT_BITMAP_HELVETICA_18);
    drawText(360.0f, hdrY, "LEVEL", GLUT_BITMAP_HELVETICA_18);
    drawText(490.0f, hdrY, "TIME",  GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.3f, 0.35f, 0.5f);
    glBegin(GL_LINES);
        glVertex2f(120.0f, hdrY - 5);
        glVertex2f(WIN_W - 120.0f, hdrY - 5);
    glEnd();

    bool any = false;
    float ry = hdrY - 35.0f;
    for (int i = 0; i < 10 && ry > 80.0f; i++, ry -= 34.0f) {
        if (!highScores[i].valid) continue;
        any = true;

        if      (i == 0) glColor3f(1.0f, 0.85f, 0.0f);
        else if (i == 1) glColor3f(0.8f, 0.8f,  0.85f);
        else if (i == 2) glColor3f(0.8f, 0.5f,  0.2f);
        else             glColor3f(0.6f, 0.65f,  0.75f);

        char rankBuf[4], scoreBuf[12], lvlBuf[8], timeBuf[12];
        snprintf(rankBuf,  sizeof(rankBuf),  "%d.", i+1);
        snprintf(scoreBuf, sizeof(scoreBuf), "%d",  highScores[i].score);
        snprintf(lvlBuf,   sizeof(lvlBuf),   "%d",  highScores[i].level);
        formatTime(timeBuf, highScores[i].timeSeconds);

        drawText(140.0f, ry, rankBuf,  GLUT_BITMAP_HELVETICA_18);
        drawText(195.0f, ry, scoreBuf, GLUT_BITMAP_HELVETICA_18);
        drawText(360.0f, ry, lvlBuf,   GLUT_BITMAP_HELVETICA_18);
        drawText(490.0f, ry, timeBuf,  GLUT_BITMAP_HELVETICA_18);
    }
    if (!any) {
        glColor3f(0.4f, 0.45f, 0.55f);
        drawTextCentered(WIN_W / 2.0f, WIN_H / 2.0f,
                         "No scores yet — play a game!",
                         GLUT_BITMAP_HELVETICA_18);
    }

    bool bHov = isHovered(20.0f, 15.0f, 110.0f, 34.0f);
    drawMenuButton(20.0f, 15.0f, 110.0f, 34.0f,
                   "< BACK", "[ESC]", {0.5f,0.6f,0.7f}, bHov);
}

void drawPauseOverlay() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.08f, 0.72f);
    glRectf(0, 0, WIN_W, WIN_H);
    glDisable(GL_BLEND);

    glColor3f(0.0f, 1.0f, 1.0f);
    glPushMatrix();
        glTranslatef(WIN_W / 2.0f, 340.0f, 0);
        glScalef(4.0f, 4.0f, 1.0f);
        drawTextCentered(0, 0, "PAUSED", GLUT_BITMAP_HELVETICA_18);
    glPopMatrix();

    glColor3f(0.6f, 0.65f, 0.75f);
    drawTextCentered(WIN_W / 2.0f, 270.0f,
                     "Press P to resume   |   ESC for menu",
                     GLUT_BITMAP_HELVETICA_18);
}

void drawLevelClearScreen() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.02f, 0.1f, 0.78f);
    glRectf(0, 0, WIN_W, WIN_H);
    glDisable(GL_BLEND);

    glColor3f(1.0f, 0.9f, 0.0f);
    glPushMatrix();
        glTranslatef(WIN_W / 2.0f, 360.0f, 0);
        glScalef(3.5f, 3.5f, 1.0f);
        char buf[32];
        snprintf(buf, sizeof(buf), "LEVEL %d CLEAR!", currentLevel);
        drawTextCentered(0, 0, buf, GLUT_BITMAP_HELVETICA_18);
    glPopMatrix();

    char scoreBuf[32], timeBuf[24];
    snprintf(scoreBuf, sizeof(scoreBuf), "Score: %d", score);
    formatTime(timeBuf, gameTimeSec);
    glColor3f(0.0f, 1.0f, 0.8f);
    drawTextCentered(WIN_W / 2.0f, 290.0f, scoreBuf, GLUT_BITMAP_HELVETICA_18);
    char tbuf2[32]; snprintf(tbuf2, sizeof(tbuf2), "Time: %s", timeBuf);
    drawTextCentered(WIN_W / 2.0f, 260.0f, tbuf2, GLUT_BITMAP_HELVETICA_18);

    float ratio = (float)levelClearCountdown / 180.0f;
    glColor3f(0.15f, 0.15f, 0.25f);
    glRectf(200.0f, 220.0f, 600.0f, 235.0f);
    glColor3f(0.0f, 1.0f, 0.5f);
    glRectf(200.0f, 220.0f, 200.0f + 400.0f * ratio, 235.0f);

    glColor3f(0.5f, 0.55f, 0.6f);
    drawTextCentered(WIN_W / 2.0f, 200.0f,
                     "Next level loading...",
                     GLUT_BITMAP_HELVETICA_12);

    drawParticles();
}

void drawGameOverScreen() {
    drawBackground();
    drawStars();
    drawParticles();

    glColor3f(1.0f, 0.1f, 0.1f);
    glPushMatrix();
        glTranslatef(WIN_W / 2.0f, 410.0f, 0);
        glScalef(4.5f, 4.5f, 1.0f);
        drawTextCentered(0, 0, "GAME OVER", GLUT_BITMAP_HELVETICA_18);
    glPopMatrix();

    char sb[32], tb[20];
    snprintf(sb, sizeof(sb), "Final Score: %d", score);
    formatTime(tb, gameTimeSec);
    char lb[32]; snprintf(lb, sizeof(lb), "Reached Level %d  |  Time: %s", currentLevel, tb);
    glColor3f(1.0f, 0.85f, 0.0f);
    drawTextCentered(WIN_W / 2.0f, 320.0f, sb, GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.55f, 0.6f, 0.7f);
    drawTextCentered(WIN_W / 2.0f, 290.0f, lb, GLUT_BITMAP_HELVETICA_18);

    bool rHov = isHovered(WIN_W/2.0f-220.0f, 220.0f, 170.0f, 48.0f);
    bool mHov = isHovered(WIN_W/2.0f+50.0f,  220.0f, 170.0f, 48.0f);
    drawMenuButton(WIN_W/2.0f-220.0f, 220.0f, 170.0f, 48.0f,
                   "RETRY",     "[R]", {1.0f,0.3f,0.3f}, rHov);
    drawMenuButton(WIN_W/2.0f+50.0f,  220.0f, 170.0f, 48.0f,
                   "MAIN MENU", "[M]", {0.0f,1.0f,0.8f}, mHov);

    glColor3f(0.3f, 0.35f, 0.45f);
    drawTextCentered(WIN_W / 2.0f, 185.0f,
                     "[R] Retry   [M] Main Menu",
                     GLUT_BITMAP_HELVETICA_12);
}

void drawWinScreen() {
    drawBackground();
    drawStars();
    drawParticles();

    float p = 0.7f + 0.3f * sinf(menuAnim * 3.0f);
    glColor3f(p, p * 0.85f, 0.0f);
    glPushMatrix();
        glTranslatef(WIN_W / 2.0f, 430.0f, 0);
        glScalef(3.8f, 3.8f, 1.0f);
        drawTextCentered(0, 0, "YOU WIN!", GLUT_BITMAP_HELVETICA_18);
    glPopMatrix();

    glColor3f(0.0f, 1.0f, 0.7f);
    drawTextCentered(WIN_W / 2.0f, 340.0f,
                     "All 3 levels cleared!", GLUT_BITMAP_HELVETICA_18);

    char sb[32], tb[20];
    snprintf(sb, sizeof(sb), "Final Score: %d", score);
    formatTime(tb, gameTimeSec);
    char tl[32]; snprintf(tl, sizeof(tl), "Total Time: %s", tb);
    glColor3f(1.0f, 0.9f, 0.0f);
    drawTextCentered(WIN_W / 2.0f, 305.0f, sb, GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.55f, 0.6f, 0.7f);
    drawTextCentered(WIN_W / 2.0f, 274.0f, tl, GLUT_BITMAP_HELVETICA_18);

    bool rHov = isHovered(WIN_W/2.0f-220.0f, 210.0f, 180.0f, 48.0f);
    bool mHov = isHovered(WIN_W/2.0f+40.0f,  210.0f, 180.0f, 48.0f);
    drawMenuButton(WIN_W/2.0f-220.0f, 210.0f, 180.0f, 48.0f,
                   "PLAY AGAIN", "[R]", {1.0f,0.9f,0.0f}, rHov);
    drawMenuButton(WIN_W/2.0f+40.0f,  210.0f, 180.0f, 48.0f,
                   "MAIN MENU",  "[M]", {0.0f,1.0f,0.8f}, mHov);
}