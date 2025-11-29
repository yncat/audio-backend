#ifndef VRROOM_H
#define VRROOM_H

#include "vrstructs.h"

int vrRoomAdd(Position3D centerPosition, Size3D roomSize, WallMaterials* materials);
int vrRoomChange(int index);
int vrRoomClear();

#endif // VRROOM_H
