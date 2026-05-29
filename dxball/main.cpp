#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "globals.h"
#include "ui.h"
#include "game.h"
#include "levels.h"
#include "physics.h"
#include "menu.h"
#include "input.h"
#include "sound.h"
#include <cstdio>

// ─────────────────────────────────────────────
//  Display callback
// ─────────────────────────────────────────────
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (gameState) {

    case STATE_MENU:
        drawMenuScreen();
        break;

    case STATE_LEVEL_SELECT:
        drawLevelSelectScreen();
        break;

    case STATE_PLAYING:
        drawBackground();
        drawStars();
        drawBricks();
        drawPerks();
        drawPaddle();
        drawBall();
        drawParticles();
        drawFloatTexts();
        drawHUD();
        drawPerkStatusBar();
        break;

    case STATE_PAUSED:
        drawBackground();
        drawStars();
        drawBricks();
        drawPerks();
        drawPaddle();
        drawBall();
        drawParticles();
        drawFloatTexts();
        drawHUD();
        drawPerkStatusBar();
        drawPauseOverlay();
        break;

    case STATE_LEVEL_CLEAR:
        drawBackground();
        drawStars();
        drawBricks();
        drawParticles();
        drawLevelClearScreen();
        break;

    case STATE_GAME_OVER:
        drawGameOverScreen();
        break;

    case STATE_WIN:
        menuAnim += 0.04f;
        drawWinScreen();
        break;

    case STATE_HELP:
        drawHelpScreen();
        break;

    case STATE_HIGH_SCORE:
        drawHighScoreScreen();
        break;
    }

    glutSwapBuffers();
}

// ─────────────────────────────────────────────
//  Timer callback — ~60 fps
// ─────────────────────────────────────────────
static int timerSecCounter = 0;

void timerCallback(int v) {
    (void)v;

    if (gameState == STATE_PLAYING) {
        updateGame();

        // Count real seconds (60 ticks ≈ 1 second)
        timerSecCounter++;
        if (timerSecCounter >= 60) {
            timerSecCounter = 0;
            if (ballLaunched) gameTimeSec++;
        }
    }
    else if (gameState == STATE_LEVEL_CLEAR) {
        // Animate particles during level clear
        updateParticles();
        updateFloatTexts();
        updateStars();

        levelClearCountdown--;
        if (levelClearCountdown <= 0) {
            int nextLvl = currentLevel + 1;
            initLevel(nextLvl);
            gameState = STATE_PLAYING;
        }
    }
    else if (gameState == STATE_MENU  || gameState == STATE_WIN ||
             gameState == STATE_GAME_OVER) {
        updateStars();
        updateParticles();
        menuAnim += 0.04f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, timerCallback, 0);   // ~62.5 fps
}

// ─────────────────────────────────────────────
//  Init
// ─────────────────────────────────────────────
void init() {
    srand((unsigned)time(nullptr));

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Enable alpha blending globally (disabled per-call where needed)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    soundInit();
    initStars();
    initGame();

    // Load high scores
    memset(highScores, 0, sizeof(highScores));
    FILE* f = fopen("highscores.dat", "rb");
    if (f) { fread(highScores, sizeof(HighScore), 10, f); fclose(f); }
}

// ─────────────────────────────────────────────
//  Main
// ─────────────────────────────────────────────
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("DX Ball  —  CSE 426 Computer Graphics Lab");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeyboard);
    glutSpecialFunc(handleSpecial);
    glutMouseFunc(handleMouseClick);
    glutPassiveMotionFunc(handleMouseMove);
    glutMotionFunc(handleMouseMove);   // also track while button held

    glutTimerFunc(0, timerCallback, 0);
    glutMainLoop();
    return 0;
}
