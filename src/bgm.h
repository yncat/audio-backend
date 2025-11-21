#ifndef BGM_H
#define BGM_H

int globalSetBgmVolume(float volume);
int bgmLoad(const void* address, int size);
int bgmPause(int slot);
int bgmResume(int slot);
int bgmStop(int slot);
int bgmFadeout(int slot, int ms);
int bgmFadein(int slot, int ms);
int bgmCrossfade(int slot1, int slot2, int ms);
int bgmSetLoopPoint(int slot, int ms);
int bgmPlay(int slot);
int bgmFree(int slot);

#endif // BGM_H
