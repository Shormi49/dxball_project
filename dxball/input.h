// input.h
#ifndef INPUT_H
#define INPUT_H

void handleKeyboard(unsigned char key, int x, int y);
void handleSpecial(int key, int x, int y);
void handleMouseClick(int button, int state, int x, int y);
void handleMouseMove(int x, int y);

#endif
