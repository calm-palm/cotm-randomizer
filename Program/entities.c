#include <stdlib.h>
#include "entities.h"

static int areaIndexCheck(int area_index);
static int roomIndexCheck(FILE* rom, int area_index, int room_index);

int getAreaCount()
{
    return (AREA_LIST_END - AREA_LIST_START) / 4;
}

int getAreaRoomCount(FILE* rom, int area_index)
{
    int room_start;
    int room_end;

    //if (areaIndexCheck(area_index)) return -1;
    if (areaIndexCheck(area_index))
    {
        printf("Bad bounds\n.");
        exit(1);
    }

    // The 4 bytes of the area entry is the offset of the first room in the area
    fseek(rom, AREA_LIST_START + (area_index * 4), SEEK_SET);
    fread(&room_start, 4, 1, rom);
    
    if (area_index != getAreaCount() - 1 )
    {
        // The next area's starting room
        fread(&room_end, 4, 1, rom);
    }
    else
    {
        // The address of the area table, which is after the final room in the final area
        room_end = AREA_LIST_START | 0x08000000;
    }

    // Divide difference by size of room entry to get number of rooms in area
    return (room_end - room_start) / ROOM_ENTRY_SIZE;
}

int getRoomAddressFromIndices(FILE* rom, int area_index, int room_index)
{
    int room_start; 

    // The 4 bytes of the area entry is the offset of the first room in the area
    fseek(rom, AREA_LIST_START + (area_index * 4), SEEK_SET);
    fread(&room_start, 4, 1, rom);

    return (room_start & 0xFFFFFF) + ROOM_ENTRY_SIZE * room_index;
}

int getRoomEntityCount(FILE* rom, int area_index, int room_index)
{
    struct room_def chosen_room;
    struct entity_entry current_entity;
    int entity_count = 0;

    //if (areaIndexCheck(area_index) || roomIndexCheck(rom, area_index, room_index)) return -1;
    if (areaIndexCheck(area_index) || roomIndexCheck(rom, area_index, room_index))
    {
        printf("Bad bounds\n.");
        exit(1);
    }
    
    fseek(rom, getRoomAddressFromIndices(rom, area_index, room_index), SEEK_SET);
    fread(&chosen_room, ROOM_ENTRY_SIZE, 1, rom);

    fseek(rom, (chosen_room.entity_list_offset & 0xFFFFFF), SEEK_SET);
    fread(&current_entity, sizeof(struct entity_entry), 1, rom);

    // Loop over room's entities
    while (current_entity.cntrl != (char)ENTITY_CNTRL_LIST_END)
    {
        fread(&current_entity, sizeof(struct entity_entry), 1, rom);
        entity_count++;
    }

    return entity_count;
}

struct room_def getRoomFromIndices(FILE* rom, int area_index, int room_index)
{
    struct room_def chosen_room;

    if (areaIndexCheck(area_index))
    {
        printf("Invalid room,area index %i,%i.\n", area_index, room_index);
        getchar();
        chosen_room.entity_list_offset = -1;
        return chosen_room;
    }

    fseek(rom, getRoomAddressFromIndices(rom, area_index, room_index), SEEK_SET);
    fread(&chosen_room, ROOM_ENTRY_SIZE, 1, rom);

    return chosen_room;
}

struct entity_entry getEntityFromIndices(FILE* rom, int area_index, int room_index, int entity_index)
{
    struct room_def chosen_room;
    struct entity_entry chosen_entity;

    if (areaIndexCheck(area_index) || roomIndexCheck(rom, area_index, room_index))
    {
        printf("Invalid room,area,entity index %i,%i,%i.\n", area_index, room_index, entity_index);
        getchar();
        chosen_entity.x = -1;
        return chosen_entity;
    }

    chosen_room = getRoomFromIndices(rom, area_index, room_index);

    fseek(rom, (chosen_room.entity_list_offset & 0xFFFFFF) + ENTITY_ENTRY_SIZE * entity_index, SEEK_SET);
    fread(&chosen_entity, ENTITY_ENTRY_SIZE, 1, rom);

    return chosen_entity;
}

void setRoomFromIndices(FILE* rom, int area_index, int room_index, struct room_def room_settings)
{
    if (areaIndexCheck(area_index))
    {
        printf("Invalid room,area index %i,%i.\n", area_index, room_index);
        getchar();
        return;
    }

    fseek(rom, getRoomAddressFromIndices(rom, area_index, room_index), SEEK_SET);
    fwrite(&room_settings, ROOM_ENTRY_SIZE, 1, rom);
}

void setEntityFromIndices(FILE* rom, int area_index, int room_index, int entity_index, struct entity_entry entity_settings)
{
    struct room_def chosen_room;

    if (areaIndexCheck(area_index) || roomIndexCheck(rom, area_index, room_index))
    {
        printf("Invalid room,area,entity index %i,%i,%i.\n", area_index, room_index, entity_index);
        getchar();
        return;
    }

    chosen_room = getRoomFromIndices(rom, area_index, room_index);

    fseek(rom, (chosen_room.entity_list_offset & 0xFFFFFF) + ENTITY_ENTRY_SIZE * entity_index, SEEK_SET);
    fwrite(&entity_settings, ENTITY_ENTRY_SIZE, 1, rom);
}

static int areaIndexCheck(int area_index)
{
    // Invalid index check
    if (area_index < 0 || area_index >= getAreaCount())
    {
        return 1;
    }
    else
    {
        return 0;
    }   
}

static int roomIndexCheck(FILE* rom, int area_index, int room_index)
{
    // Invalid index check
    if (room_index < 0 || room_index >= getAreaRoomCount(rom, area_index))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}