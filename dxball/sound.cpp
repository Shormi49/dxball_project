#include "sound.h"
#include <windows.h>
#include <mmsystem.h>

void soundInit() {}

void soundPlay(SoundID id) {
    switch(id) {

    case SND_LAUNCH:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMASTERISK, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    case SND_HIT:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMDEFAULT, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    case SND_PADDLE:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMDEFAULT, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    case SND_WALL:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMHAND, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    case SND_PERK:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMASTERISK, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    case SND_LOSE_LIFE:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMEXCLAMATION, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    case SND_GAME_OVER:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMHAND, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    case SND_LEVEL_UP:
        PlaySound((LPCSTR)SND_ALIAS_SYSTEMASTERISK, NULL, SND_ALIAS_ID | SND_ASYNC);
        break;

    default:
        break;
    }
}