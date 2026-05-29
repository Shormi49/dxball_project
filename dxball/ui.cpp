#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "ui.h"

// ─────────────────────────────────────────────
//  Text
// ─────────────────────────────────────────────
void drawText(float x, float y, const char* text, void* font) {
    if (!font) font = GLUT_BITMAP_HELVETICA_18;
    glRasterPos2f(x, y);
    for (int i = 0; text[i]; i++)
        glutBitmapCharacter(font, text[i]);
}

void drawTextCentered(float cx, float y, const char* text, void* font) {
    if (!font) font = GLUT_BITMAP_HELVETICA_18;
    int w = 0;
    for (int i = 0; text[i]; i++)
        w += glutBitmapWidth(font, text[i]);
    drawText(cx - w * 0.5f, y, text, font);
}

void drawTextRight(float rx, float y, const char* text, void* font) {
    if (!font) font = GLUT_BITMAP_HELVETICA_18;
    int w = 0;
    for (int i = 0; text[i]; i++)
        w += glutBitmapWidth(font, text[i]);
    drawText(rx - w, y, text, font);
}

// ─────────────────────────────────────────────
//  Primitives
// ─────────────────────────────────────────────
void drawFilledRect(float x, float y, float w, float h) {
    glRectf(x, y, x + w, y + h);
}

void drawRect(float x, float y, float w, float h, float lineW) {
    glLineWidth(lineW);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x,     y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x,     y + h);
    glEnd();
    glLineWidth(1.0f);
}

void drawRoundedRect(float x, float y, float w, float h, float r, bool filled) {
    int segs = 8;
    float cx[4] = { x+r,     x+w-r,   x+w-r,   x+r     };
    float cy[4] = { y+r,     y+r,     y+h-r,   y+h-r   };
    float a0[4] = { 180.0f,  270.0f,  0.0f,    90.0f   };

    if (filled) {
        glBegin(GL_POLYGON);
        for (int c = 0; c < 4; c++) {
            for (int s = 0; s <= segs; s++) {
                float a = (a0[c] + s * 90.0f / segs) * PI / 180.0f;
                glVertex2f(cx[c] + cosf(a)*r, cy[c] + sinf(a)*r);
            }
        }
        glEnd();
    } else {
        glBegin(GL_LINE_LOOP);
        for (int c = 0; c < 4; c++) {
            for (int s = 0; s <= segs; s++) {
                float a = (a0[c] + s * 90.0f / segs) * PI / 180.0f;
                glVertex2f(cx[c] + cosf(a)*r, cy[c] + sinf(a)*r);
            }
        }
        glEnd();
    }
}

void drawCircle(float cx, float cy, float radius, bool filled, int segs) {
    glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
    for (int i = 0; i < segs; i++) {
        float a = i * 2.0f * PI / segs;
        glVertex2f(cx + cosf(a)*radius, cy + sinf(a)*radius);
    }
    glEnd();
}

void drawGradientRectV(float x, float y, float w, float h, Color3 top, Color3 bot) {
    glBegin(GL_QUADS);
        glColor3f(bot.r, bot.g, bot.b);
        glVertex2f(x,     y);
        glVertex2f(x + w, y);
        glColor3f(top.r, top.g, top.b);
        glVertex2f(x + w, y + h);
        glVertex2f(x,     y + h);
    glEnd();
}

void drawGlowRect(float x, float y, float w, float h, Color3 col, float intensity) {
    // draw several semi-transparent expanding rects to fake glow
    for (int i = 3; i >= 1; i--) {
        float expand = i * 3.0f;
        float alpha  = intensity * 0.12f / i;
        glColor4f(col.r, col.g, col.b, alpha);
        drawRoundedRect(x - expand, y - expand,
                        w + expand*2, h + expand*2, 4.0f, true);
    }
}

// ─────────────────────────────────────────────
//  Background
// ─────────────────────────────────────────────
void drawBackground() {
    // Deep space gradient
    glBegin(GL_QUADS);
        glColor3f(0.02f, 0.01f, 0.06f); glVertex2f(0,     0);
        glColor3f(0.03f, 0.01f, 0.08f); glVertex2f(WIN_W, 0);
        glColor3f(0.01f, 0.01f, 0.04f); glVertex2f(WIN_W, WIN_H);
        glColor3f(0.02f, 0.01f, 0.05f); glVertex2f(0,     WIN_H);
    glEnd();

    // Subtle grid lines
    glColor4f(0.08f, 0.04f, 0.18f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (int x = 0; x <= WIN_W; x += 80) {
        glVertex2f((float)x, 0);
        glVertex2f((float)x, WIN_H);
    }
    for (int y = 0; y <= WIN_H; y += 60) {
        glVertex2f(0,     (float)y);
        glVertex2f(WIN_W, (float)y);
    }
    glEnd();

    // Cyber corner brackets
    glColor3f(0.0f, 0.9f, 0.9f);
    glLineWidth(2.5f);
    float blen = 55.0f;
    // bottom-left
    glBegin(GL_LINES);
        glVertex2f(15.0f, 15.0f); glVertex2f(15.0f + blen, 15.0f);
        glVertex2f(15.0f, 15.0f); glVertex2f(15.0f, 15.0f + blen);
    glEnd();
    // bottom-right
    glBegin(GL_LINES);
        glVertex2f(WIN_W-15.0f, 15.0f); glVertex2f(WIN_W-15.0f - blen, 15.0f);
        glVertex2f(WIN_W-15.0f, 15.0f); glVertex2f(WIN_W-15.0f, 15.0f + blen);
    glEnd();
    // top-left
    glBegin(GL_LINES);
        glVertex2f(15.0f, WIN_H-15.0f); glVertex2f(15.0f + blen, WIN_H-15.0f);
        glVertex2f(15.0f, WIN_H-15.0f); glVertex2f(15.0f, WIN_H-15.0f - blen);
    glEnd();
    // top-right
    glBegin(GL_LINES);
        glVertex2f(WIN_W-15.0f, WIN_H-15.0f); glVertex2f(WIN_W-15.0f-blen, WIN_H-15.0f);
        glVertex2f(WIN_W-15.0f, WIN_H-15.0f); glVertex2f(WIN_W-15.0f, WIN_H-15.0f-blen);
    glEnd();
    glLineWidth(1.0f);
}

// ─────────────────────────────────────────────
//  Stars
// ─────────────────────────────────────────────
void updateStars() {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].y -= stars[i].speed;
        if (stars[i].y < 0.0f) {
            stars[i].y = (float)WIN_H;
            stars[i].x = (float)(rand() % WIN_W);
        }
    }
}

void drawStars() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < MAX_STARS; i++) {
        float b = stars[i].brightness;
        glColor3f(0.6f*b, 0.7f*b, 1.0f*b);
        glVertex2f(stars[i].x, stars[i].y);
    }
    glEnd();
    glPointSize(1.0f);
}

// ─────────────────────────────────────────────
//  Particles
// ─────────────────────────────────────────────
void spawnParticles(float x, float y, Color3 col, int n) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) continue;
        if (n-- <= 0) break;
        float ang = ((float)rand() / RAND_MAX) * 2.0f * PI;
        float spd = 1.5f + ((float)rand() / RAND_MAX) * 3.0f;
        particles[i].x       = x;
        particles[i].y       = y;
        particles[i].dx      = cosf(ang) * spd;
        particles[i].dy      = sinf(ang) * spd;
        particles[i].maxLife = 30.0f + (float)(rand() % 20);
        particles[i].life    = particles[i].maxLife;
        particles[i].size    = 2.0f + ((float)rand() / RAND_MAX) * 3.0f;
        particles[i].color   = col;
        particles[i].active  = true;
    }
}

void updateParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        particles[i].x    += particles[i].dx;
        particles[i].y    += particles[i].dy;
        particles[i].dy   -= 0.05f;   // gravity
        particles[i].life -= 1.0f;
        if (particles[i].life <= 0) particles[i].active = false;
    }
}

void drawParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) continue;
        float alpha = particles[i].life / particles[i].maxLife;
        Color3& c = particles[i].color;
        glColor4f(c.r, c.g, c.b, alpha);
        drawCircle(particles[i].x, particles[i].y, particles[i].size * alpha, true, 6);
    }
}

// ─────────────────────────────────────────────
//  Floating texts
// ─────────────────────────────────────────────
void spawnFloatText(float x, float y, const char* txt, Color3 col) {
    for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
        if (floatTexts[i].active) continue;
        floatTexts[i].x       = x;
        floatTexts[i].y       = y;
        floatTexts[i].dy      = 1.2f;
        floatTexts[i].maxLife = 70.0f;
        floatTexts[i].life    = 70.0f;
        floatTexts[i].color   = col;
        strncpy(floatTexts[i].text, txt, 31);
        floatTexts[i].text[31] = '\0';
        floatTexts[i].active  = true;
        return;
    }
}

void updateFloatTexts() {
    for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
        if (!floatTexts[i].active) continue;
        floatTexts[i].y    += floatTexts[i].dy;
        floatTexts[i].life -= 1.0f;
        if (floatTexts[i].life <= 0) floatTexts[i].active = false;
    }
}

void drawFloatTexts() {
    for (int i = 0; i < MAX_FLOATING_TEXTS; i++) {
        if (!floatTexts[i].active) continue;
        float alpha = floatTexts[i].life / floatTexts[i].maxLife;
        Color3& c   = floatTexts[i].color;
        glColor4f(c.r, c.g, c.b, alpha);
        drawTextCentered(floatTexts[i].x, floatTexts[i].y,
                         floatTexts[i].text, GLUT_BITMAP_HELVETICA_18);
    }
}

// ─────────────────────────────────────────────
//  HUD
// ─────────────────────────────────────────────
void drawHUD() {
    char buf[64];

    // Top bar background
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glRectf(0, WIN_H - 28, WIN_W, WIN_H);

    // Score
    glColor3f(0.0f, 1.0f, 1.0f);
    snprintf(buf, sizeof(buf), "SCORE: %d", score);
    drawText(20, WIN_H - 18, buf, GLUT_BITMAP_HELVETICA_18);

    // Level + Time (centre)
    int mm = gameTimeSec / 60, ss = gameTimeSec % 60;
    snprintf(buf, sizeof(buf), "LEVEL %d  |  TIME: %d:%02d", currentLevel, mm, ss);
    drawTextCentered(WIN_W / 2.0f, WIN_H - 18, buf, GLUT_BITMAP_HELVETICA_18);

    // Lives (right) – heart symbols as 'o'
    glColor3f(1.0f, 0.25f, 0.35f);
    snprintf(buf, sizeof(buf), "LIVES: %d", lives);
    drawTextRight(WIN_W - 20, WIN_H - 18, buf, GLUT_BITMAP_HELVETICA_18);

    // Divider line
    glColor3f(0.0f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(0, WIN_H - 30);
        glVertex2f(WIN_W, WIN_H - 30);
    glEnd();
}

// ─────────────────────────────────────────────
//  Perk status bar (bottom)
// ─────────────────────────────────────────────
void drawPerkStatusBar() {
    float bx = 20.0f, by = 6.0f, bh = 12.0f, bw = 90.0f, gap = 8.0f;
    char buf[32];

    if (fireballTimer > 0) {
        float ratio = fireballTimer / 300.0f;
        glColor4f(1.0f, 0.4f, 0.0f, 0.4f);
        drawFilledRect(bx, by, bw, bh);
        glColor4f(1.0f, 0.6f, 0.0f, 0.9f);
        drawFilledRect(bx, by, bw * ratio, bh);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(bx + 4, by + 9, "FIREBALL", GLUT_BITMAP_HELVETICA_10);
        bx += bw + gap;
    }
    if (widePaddleTimer > 0) {
        float ratio = widePaddleTimer / 600.0f;
        glColor4f(0.0f, 0.6f, 0.5f, 0.4f);
        drawFilledRect(bx, by, bw, bh);
        glColor4f(0.0f, 1.0f, 0.8f, 0.9f);
        drawFilledRect(bx, by, bw * ratio, bh);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(bx + 4, by + 9, "WIDE PAD", GLUT_BITMAP_HELVETICA_10);
        bx += bw + gap;
    }
    if (narrowPaddleTimer > 0) {
        float ratio = narrowPaddleTimer / 400.0f;
        glColor4f(0.6f, 0.0f, 0.6f, 0.4f);
        drawFilledRect(bx, by, bw, bh);
        glColor4f(1.0f, 0.0f, 1.0f, 0.9f);
        drawFilledRect(bx, by, bw * ratio, bh);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(bx + 4, by + 9, "SHRUNK  ", GLUT_BITMAP_HELVETICA_10);
    }
}

// ─────────────────────────────────────────────
//  Button helper
// ─────────────────────────────────────────────
bool isHovered(float x, float y, float w, float h) {
    float my = WIN_H - mouseY;
    return (mouseX >= x && mouseX <= x + w && my >= y && my <= y + h);
}

void drawMenuButton(float x, float y, float w, float h,
                    const char* label, const char* hint,
                    Color3 col, bool selected)
{
    bool hov = isHovered(x, y, w, h);
    bool lit = hov || selected;

    // Glow behind
    if (lit) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawGlowRect(x, y, w, h, col, 1.2f);
    }

    // Fill
    if (lit) glColor4f(col.r * 0.25f, col.g * 0.25f, col.b * 0.25f, 0.9f);
    else     glColor4f(0.04f, 0.04f, 0.12f, 0.85f);
    drawRoundedRect(x, y, w, h, 5.0f, true);

    // Border
    if (lit) glColor3f(col.r, col.g, col.b);
    else     glColor4f(col.r*0.5f, col.g*0.5f, col.b*0.5f, 0.7f);
    glLineWidth(lit ? 2.0f : 1.0f);
    drawRoundedRect(x, y, w, h, 5.0f, false);
    glLineWidth(1.0f);

    // Arrow indicator
    if (lit) {
        glColor3f(col.r, col.g, col.b);
        float ay = y + h / 2.0f;
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 18.0f, ay);
            glVertex2f(x - 6.0f,  ay + 7.0f);
            glVertex2f(x - 6.0f,  ay - 7.0f);
        glEnd();
    }

    // Label
    if (lit) glColor3f(1.0f, 1.0f, 1.0f);
    else     glColor3f(col.r * 0.85f, col.g * 0.85f, col.b * 0.85f);
    drawText(x + 18.0f, y + h / 2.0f - 7.0f, label, GLUT_BITMAP_HELVETICA_18);

    // Key hint (right side)
    glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
    drawTextRight(x + w - 10.0f, y + h / 2.0f - 7.0f, hint, GLUT_BITMAP_HELVETICA_12);

    glDisable(GL_BLEND);
}
