#ifndef BGM_H
#define BGM_H

void globalSetBgmVolume(float volume);
int bgmLoad(const void* address, int size);
void bgmPause(int slot);
void bgmResume(int slot);
void bgmStop(int slot);
void bgmFadeout(int slot, int ms);
void bgmFadein(int slot, int ms);
void bgmCrossfade(int slot1, int slot2, int ms);
void bgmSetLoopPoint(int slot, int ms);
void bgmFree(int slot);

#endif // BGM_H
