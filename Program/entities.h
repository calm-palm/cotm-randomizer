#ifndef ENTITIES_H_INCLUDED
#define ENTITIES_H_INCLUDED

#include <stdio.h>
#include "game_structure.h"

int getAreaCount();
int getAreaRoomCount(FILE* rom, int area_index);
int getRoomEntityCount(FILE* rom, int area_index, int room_index);

int getRoomAddressFromIndices(FILE* rom, int area_index, int room_index);

struct room_def getRoomFromIndices(FILE* rom, int area_index, int room_index);
struct entity_entry getEntityFromIndices(FILE* rom, int area_index, int room_index, int entity_index);

void setRoomFromIndices(FILE* rom, int area_index, int room_index, struct room_def room_settings);
void setEntityFromIndices(FILE* rom, int area_index, int room_index, int entity_index, struct entity_entry entity_settings);

#endif
