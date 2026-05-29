#ifndef SOUND_H
#define SOUND_H

enum SoundID {
    SND_HIT = 0,
    SND_WALL,
    SND_PADDLE,
    SND_PERK,
    SND_LOSE_LIFE,
    SND_GAME_OVER,
    SND_LEVEL_UP,
    SND_LAUNCH,
    SND_COUNT
};

void soundPlay(SoundID id);
void soundInit();

#endif