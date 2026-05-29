#ifndef UI_H
#define UI_H

#include "globals.h"

// ── Text helpers ──────────────────────────────────────────────
void drawText(float x, float y, const char* text, void* font = nullptr);
void drawTextCentered(float cx, float y, const char* text, void* font = nullptr);
void drawTextRight(float rx, float y, const char* text, void* font = nullptr);

// ── Primitives ────────────────────────────────────────────────
void drawFilledRect(float x, float y, float w, float h);
void drawRect(float x, float y, float w, float h, float lineW = 1.0f);
void drawRoundedRect(float x, float y, float w, float h, float r, bool filled = true);
void drawCircle(float cx, float cy, float radius, bool filled = true, int segs = 36);
void drawGradientRectV(float x, float y, float w, float h,
                       Color3 top, Color3 bot);
void drawGlowRect(float x, float y, float w, float h,
                  Color3 col, float intensity = 1.0f);

// ── Background ────────────────────────────────────────────────
void drawBackground();
void updateStars();
void drawStars();

// ── Particles ─────────────────────────────────────────────────
void spawnParticles(float x, float y, Color3 col, int n = 8);
void updateParticles();
void drawParticles();

// ── Floating texts ────────────────────────────────────────────
void spawnFloatText(float x, float y, const char* txt, Color3 col);
void updateFloatTexts();
void drawFloatTexts();

// ── HUD ───────────────────────────────────────────────────────
void drawHUD();
void drawPerkStatusBar();

// ── Button helper ─────────────────────────────────────────────
// Returns true if mouse is inside rect
bool isHovered(float x, float y, float w, float h);
void drawMenuButton(float x, float y, float w, float h,
                    const char* label, const char* hint,
                    Color3 col, bool selected = false);

// ── Colour constants ──────────────────────────────────────────
inline Color3 MakeColor(float r, float g, float b){ return {r,g,b}; }
const Color3 COL_CYAN    = {0.0f, 1.0f, 1.0f};
const Color3 COL_WHITE   = {1.0f, 1.0f, 1.0f};
const Color3 COL_YELLOW  = {1.0f, 0.9f, 0.0f};
const Color3 COL_RED     = {1.0f, 0.2f, 0.2f};
const Color3 COL_GREEN   = {0.0f, 1.0f, 0.5f};
const Color3 COL_ORANGE  = {1.0f, 0.55f, 0.0f};
const Color3 COL_MAGENTA = {1.0f, 0.0f, 1.0f};
const Color3 COL_PURPLE  = {0.6f, 0.0f, 1.0f};
const Color3 COL_DARK    = {0.05f,0.05f,0.15f};

#endif // UI_H
