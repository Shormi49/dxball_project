#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include "levels.h"
#include "ui.h"

// ─────────────────────────────────────────────
//  Stars initialisation
// ─────────────────────────────────────────────
void initStars() {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x          = (float)(rand() % WIN_W);
        stars[i].y          = (float)(rand() % WIN_H);
        stars[i].speed      = 0.15f + ((float)rand()/RAND_MAX) * 0.5f;
        stars[i].size       = 0.5f  + ((float)rand()/RAND_MAX) * 1.5f;
        stars[i].brightness = 0.3f  + ((float)rand()/RAND_MAX) * 0.7f;
    }
}

// ─────────────────────────────────────────────
//  Colour helper
// ─────────────────────────────────────────────
Color3 brickColor(int level, int row, int hp, int maxHp) {
    // Hue by row, saturation by level
    static const Color3 rowCols[][7] = {
        // level 1 – cool blues/greens
        { {0.0f,1.0f,0.8f}, {0.0f,0.8f,1.0f}, {0.2f,0.5f,1.0f},
          {0.5f,0.2f,1.0f}, {0.8f,0.0f,1.0f} },
        // level 2 – warm oranges/reds
        { {1.0f,0.9f,0.0f}, {1.0f,0.6f,0.0f}, {1.0f,0.3f,0.0f},
          {1.0f,0.0f,0.2f}, {0.8f,0.0f,0.5f}, {0.6f,0.0f,0.8f} },
        // level 3 – mixed vivid
        { {1.0f,0.0f,0.4f}, {1.0f,0.4f,0.0f}, {0.8f,1.0f,0.0f},
          {0.0f,1.0f,0.3f}, {0.0f,0.8f,1.0f}, {0.4f,0.0f,1.0f},
          {1.0f,0.0f,1.0f} },
    };
    int lv = (level < 1) ? 0 : (level > 3 ? 2 : level - 1);
    Color3 base = rowCols[lv][row % 7];

    // Dim based on remaining HP
    if (maxHp > 1) {
        float dmg = 1.0f - 0.35f * (maxHp - hp);
        base.r *= dmg; base.g *= dmg; base.b *= dmg;
    }
    return base;
}

// ─────────────────────────────────────────────
//  Level layouts
// ─────────────────────────────────────────────
static void placeBrick(int& idx, int col, int row, int hp, bool indestr, int level) {
    if (idx >= MAX_BRICKS) return;
    float totalW  = BRICK_COLS * (BRICK_W + BRICK_PAD_X) - BRICK_PAD_X;
    float startX  = (WIN_W - totalW) / 2.0f;
    bricks[idx].x             = startX + col * (BRICK_W + BRICK_PAD_X);
    bricks[idx].y             = BRICK_TOP - row * (BRICK_H + BRICK_PAD_Y);
    bricks[idx].hp            = hp;
    bricks[idx].maxHp         = hp;
    bricks[idx].active        = true;
        bricks[idx].w = BRICK_W;
        bricks[idx].h = BRICK_H;
    bricks[idx].indestructible= indestr;
    bricks[idx].color         = indestr
                                  ? Color3{0.55f, 0.55f, 0.6f}
                                  : brickColor(level, row, hp, hp);
    idx++;
}

void initLevel(int lvl) {
    currentLevel = lvl;
    brickCount   = 0;

    // Clear
    for (int i = 0; i < MAX_BRICKS; i++) bricks[i].active = false;
    for (int i = 0; i < MAX_PERKS;  i++) perks[i].active  = false;

    // Ball speed
    baseBallSpeed = 3.8f + (lvl - 1) * 0.9f;
    ballLaunched  = false;
    paddleX       = (WIN_W - paddleW) / 2.0f;

    int idx = 0;

    if (lvl == 1) {
        // 5 rows × 10 cols, 1 HP each
        for (int r = 0; r < 5; r++)
            for (int c = 0; c < 10; c++)
                placeBrick(idx, c, r, 1, false, lvl);
    }
    else if (lvl == 2) {
        // 6 rows: top 2 have 2 HP, rest 1 HP
        for (int r = 0; r < 6; r++) {
            int hp = (r < 2) ? 2 : 1;
            for (int c = 0; c < 10; c++)
                placeBrick(idx, c, r, hp, false, lvl);
        }
    }
    else if (lvl == 3) {
        // 7 rows; top row 3 HP, rows 1-2: 2 HP;
        // add 4 indestructible wall bricks in middle
        for (int r = 0; r < 7; r++) {
            int hp = (r == 0) ? 3 : (r < 3) ? 2 : 1;
            for (int c = 0; c < 10; c++) {
                // gaps for wall bricks at specific positions
                bool wall = (r == 3 && (c == 2 || c == 4 || c == 6 || c == 8));
                placeBrick(idx, c, r, wall ? 999 : hp, wall, lvl);
            }
        }
    }
    else {
        // Bonus level 4+: random higher HP, more wall bricks
        int rows = 7;
        int maxHp = 2 + (lvl - 3);
        if (maxHp > 5) maxHp = 5;
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < 10; c++) {
                bool wall = (rand() % 10 == 0);
                int  hp   = wall ? 999 : (1 + rand() % maxHp);
                placeBrick(idx, c, r, hp, wall, 3);
            }
        }
    }
    brickCount = idx;

    // Reset ball position to paddle
    float px = paddleX + paddleW / 2.0f;
    ballX  = px;
    ballY  = PADDLE_Y + PADDLE_H + BALL_RADIUS + 2.0f;
    ballDX = baseBallSpeed * (rand() % 2 ? 1.0f : -1.0f);
    ballDY = baseBallSpeed;
    ballLaunched = false;
}

// ─────────────────────────────────────────────
//  Draw bricks
// ─────────────────────────────────────────────
void drawBricks() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < brickCount; i++) {
        if (!bricks[i].active) continue;
        Brick& b = bricks[i];

        Color3 c = b.indestructible
                   ? Color3{0.55f, 0.55f, 0.6f}
                   : brickColor(currentLevel, (int)((BRICK_TOP - b.y) / (BRICK_H + BRICK_PAD_Y)),
                                b.hp, b.maxHp);

        // Glow behind
        drawGlowRect(b.x, b.y, b.w, b.h, c, 0.7f);

        // Gradient fill
        Color3 dark = { c.r * 0.4f, c.g * 0.4f, c.b * 0.4f };
        drawGradientRectV(b.x + 1, b.y + 1, b.w - 2, b.h - 2, c, dark);

        // Highlight stripe at top
        glColor4f(1.0f, 1.0f, 1.0f, 0.18f);
        glRectf(b.x + 2, b.y + b.h - 5, b.x + b.w - 2, b.y + b.h - 1);

        // Border glow
        glColor3f(c.r, c.g, c.b);
        glLineWidth(1.5f);
        drawRect(b.x, b.y, b.w, b.h, 1.5f);

        // HP cracks on multi-HP bricks
        if (!b.indestructible && b.maxHp > 1 && b.hp < b.maxHp) {
            glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
            glLineWidth(1.0f);
            for (int k = 0; k < b.maxHp - b.hp; k++) {
                float cx = b.x + 16.0f + k * 18.0f;
                glBegin(GL_LINE_STRIP);
                    glVertex2f(cx,        b.y + 4.0f);
                    glVertex2f(cx + 3.0f, b.y + b.h * 0.5f);
                    glVertex2f(cx - 2.0f, b.y + b.h - 4.0f);
                glEnd();
            }
        }

        // Wall brick label
        if (b.indestructible) {
            glColor3f(0.85f, 0.85f, 0.9f);
            drawTextCentered(b.x + b.w / 2.0f, b.y + 6.0f, "WALL",
                             GLUT_BITMAP_HELVETICA_10);
        }
    }
    glDisable(GL_BLEND);
    glLineWidth(1.0f);
}
