#include <GL/glut.h>
#include <cstdlib>
#include "input.h"
#include "globals.h"
#include "game.h"
#include "levels.h"

// ─────────────────────────────────────────────
//  Helper: clamp paddle
// ─────────────────────────────────────────────
static void clampPaddle() {
    if (paddleX < 0.0f)               paddleX = 0.0f;
    if (paddleX + paddleW > WIN_W)    paddleX = WIN_W - paddleW;
}

// ─────────────────────────────────────────────
//  Mouse regions for button clicks
// ─────────────────────────────────────────────
static void handleMenuClick(int x, int y) {
    // y is already flipped (OpenGL coords)
    float fx = (float)x, fy = (float)y;
    float bx = 60.0f, bw = 290.0f, bh = 44.0f, gap = 10.0f;
    float by0 = 380.0f;

    // 5 menu buttons from top-down in OpenGL Y space
    if (fx >= bx && fx <= bx + bw) {
        for (int i = 0; i < 5; i++) {
            float by = by0 - i * (bh + gap);
            if (fy >= by && fy <= by + bh) {
                switch (i) {
                case 0: // NEW GAME
                    currentLevel = 1;
                    initGame(); initLevel(1);
                    gameState = STATE_PLAYING;
                    break;
                case 1: // LEVEL SELECT
                    gameState = STATE_LEVEL_SELECT;
                    break;
                case 2: // HIGH SCORES
                    gameState = STATE_HIGH_SCORE;
                    break;
                case 3: // HELP
                    gameState = STATE_HELP;
                    break;
                case 4: // EXIT
                    exit(0);
                    break;
                }
                return;
            }
        }
    }
}

static void handleLevelSelectClick(int x, int y) {
    float fx = (float)x, fy = (float)y;
    float bx = 180.0f, bw = 440.0f, bh = 88.0f, gap = 16.0f;
    float by = 410.0f;
    for (int i = 0; i < 3; i++) {
        float byt = by - bh;
        if (fx >= bx && fx <= bx + bw && fy >= byt && fy <= byt + bh) {
            currentLevel = i + 1;
            initGame(); initLevel(i + 1);
            gameState = STATE_PLAYING;
            return;
        }
        by -= bh + gap;
    }
    // Back
    if (fx >= 20 && fx <= 130 && fy >= 15 && fy <= 49)
        gameState = STATE_MENU;
}

static void handleGameOverClick(int x, int y) {
    float fx = (float)x, fy = (float)y;
    // RETRY
    if (fx >= WIN_W/2.0f-220 && fx <= WIN_W/2.0f-50  && fy >= 220 && fy <= 268) {
        currentLevel = 1; initGame(); initLevel(1); gameState = STATE_PLAYING;
    }
    // MENU
    if (fx >= WIN_W/2.0f+50  && fx <= WIN_W/2.0f+220 && fy >= 220 && fy <= 268) {
        gameState = STATE_MENU;
    }
}

static void handleWinClick(int x, int y) {
    float fx = (float)x, fy = (float)y;
    // PLAY AGAIN
    if (fx >= WIN_W/2.0f-220 && fx <= WIN_W/2.0f-40  && fy >= 210 && fy <= 258) {
        currentLevel = 1; initGame(); initLevel(1); gameState = STATE_PLAYING;
    }
    // MENU
    if (fx >= WIN_W/2.0f+40  && fx <= WIN_W/2.0f+220 && fy >= 210 && fy <= 258) {
        gameState = STATE_MENU;
    }
}

// ─────────────────────────────────────────────
//  Keyboard
// ─────────────────────────────────────────────
void handleKeyboard(unsigned char key, int x, int y) {
    (void)x; (void)y;

    // Global: ESC
    if (key == 27) { // ESC
        if (gameState == STATE_PLAYING || gameState == STATE_PAUSED) {
            gameState = STATE_MENU;
            gameTimeSec = 0;
        } else if (gameState != STATE_MENU) {
            gameState = STATE_MENU;
        }
        return;
    }

    // Global pause P
    if ((key == 'p' || key == 'P') && gameState == STATE_PLAYING) {
        gameState = STATE_PAUSED;
        return;
    }
    if ((key == 'p' || key == 'P') && gameState == STATE_PAUSED) {
        gameState = STATE_PLAYING;
        return;
    }

    switch (gameState) {
    // ── MENU ────────────────────────────────
    case STATE_MENU:
        switch (key) {
        case 'n': case 'N':
            currentLevel = 1; initGame(); initLevel(1); gameState = STATE_PLAYING; break;
        case 'l': case 'L':
            gameState = STATE_LEVEL_SELECT; break;
        case 'h': case 'H': case '?':
            gameState = STATE_HELP; break;
        case 's': case 'S':
            gameState = STATE_HIGH_SCORE; break;
        case 'q': case 'Q':
            exit(0);
        case '1':
            currentLevel=1; initGame(); initLevel(1); gameState=STATE_PLAYING; break;
        case '2':
            currentLevel=2; initGame(); initLevel(2); gameState=STATE_PLAYING; break;
        case '3':
            currentLevel=3; initGame(); initLevel(3); gameState=STATE_PLAYING; break;
        // Arrow keys for menu selection
        case 13: // Enter
            switch (menuSelected) {
            case 0: currentLevel=1; initGame(); initLevel(1); gameState=STATE_PLAYING; break;
            case 1: gameState=STATE_LEVEL_SELECT; break;
            case 2: gameState=STATE_HIGH_SCORE;   break;
            case 3: gameState=STATE_HELP;          break;
            case 4: exit(0);
            }
            break;
        }
        break;

    // ── LEVEL SELECT ─────────────────────────
    case STATE_LEVEL_SELECT:
        if (key >= '1' && key <= '3') {
            int lv = key - '0';
            currentLevel = lv; initGame(); initLevel(lv); gameState = STATE_PLAYING;
        }
        break;

    // ── PLAYING ──────────────────────────────
    case STATE_PLAYING:
        switch (key) {
        case ' ':
            launchBall(); break;
        case 'a': case 'A':
            paddleX -= 20.0f; clampPaddle(); break;
        case 'd': case 'D':
            paddleX += 20.0f; clampPaddle(); break;
        }
        break;

    // ── GAME OVER ────────────────────────────
    case STATE_GAME_OVER:
        if (key == 'r' || key == 'R') {
            currentLevel = 1; initGame(); initLevel(1); gameState = STATE_PLAYING;
        }
        if (key == 'm' || key == 'M') gameState = STATE_MENU;
        break;

    // ── WIN ──────────────────────────────────
    case STATE_WIN:
        if (key == 'r' || key == 'R') {
            currentLevel = 1; initGame(); initLevel(1); gameState = STATE_PLAYING;
        }
        if (key == 'm' || key == 'M') gameState = STATE_MENU;
        break;

    default: break;
    }

    glutPostRedisplay();
}

// ─────────────────────────────────────────────
//  Special (arrow) keys
// ─────────────────────────────────────────────
void handleSpecial(int key, int x, int y) {
    (void)x; (void)y;

    if (gameState == STATE_MENU) {
        if (key == GLUT_KEY_UP) {
            menuSelected = (menuSelected + 1) % 5;
        } else if (key == GLUT_KEY_DOWN) {
            menuSelected = (menuSelected + 4) % 5;
        }
    }

    if (gameState == STATE_PLAYING) {
        if (key == GLUT_KEY_LEFT) {
            paddleX -= 22.0f; clampPaddle();
        }
        if (key == GLUT_KEY_RIGHT) {
            paddleX += 22.0f; clampPaddle();
        }
    }
    glutPostRedisplay();
}

// ─────────────────────────────────────────────
//  Mouse click
// ─────────────────────────────────────────────
void handleMouseClick(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    int oy = WIN_H - y;   // flip to OpenGL Y

    switch (gameState) {
    case STATE_MENU:
        handleMenuClick(x, oy); break;
    case STATE_LEVEL_SELECT:
        handleLevelSelectClick(x, oy); break;
    case STATE_PLAYING:
        launchBall(); break;
    case STATE_GAME_OVER:
        handleGameOverClick(x, oy); break;
    case STATE_WIN:
        handleWinClick(x, oy); break;
    case STATE_HELP:
    case STATE_HIGH_SCORE:
        // Back button region
        if (x >= 20 && x <= 130 && oy >= 15 && oy <= 49)
            gameState = STATE_MENU;
        break;
    default: break;
    }
    glutPostRedisplay();
}

// ─────────────────────────────────────────────
//  Mouse passive motion → paddle follows cursor
// ─────────────────────────────────────────────
void handleMouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;

    if (gameState == STATE_PLAYING) {
        paddleX = (float)x - paddleW / 2.0f;
        if (paddleX < 0.0f)            paddleX = 0.0f;
        if (paddleX + paddleW > WIN_W) paddleX = WIN_W - paddleW;
    }
    glutPostRedisplay();
}
