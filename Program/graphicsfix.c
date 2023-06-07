#include <stdbool.h>
#include "graphicsfix.h"
#include "entities.h"

static bool isVanillaMagicItemPedestal(int area_index, int room_index, int entity_index);

void graphicsFix(FILE* rom)
{
    //struct room_def current_room;
    struct entity_entry current_entity;

    int area_index;
    int room_index;
    int entity_index;

    //bool roomContainsMagicItem;

    for (area_index = 0; area_index < getAreaCount(); area_index++)
    {
        for (room_index = 0; room_index < getAreaRoomCount(rom, area_index); room_index++)
        {
            //roomContainsMagicItem = false;
            //current_room = getRoomFromIndices(rom, area_index, room_index);

            // Search for magic items in the room
            for (entity_index = 0; entity_index < getRoomEntityCount(rom, area_index, room_index); entity_index++)
            {
                current_entity = getEntityFromIndices(rom, area_index, room_index, entity_index);

                if (current_entity.entity_type == ENTITY_TPE_MOVEMENT)
                {
                    //roomContainsMagicItem = true;

                    //printf("DEBUG: %i %i %i has item %x.\n", area_index, room_index, entity_index, current_entity.param);

                    if (!isVanillaMagicItemPedestal(area_index, room_index, entity_index))
                    {
                        // Fix Y position of a magic item placed onto a non-magic item pedestal
                        current_entity.y = current_entity.y - 8;
                        setEntityFromIndices(rom, area_index, room_index, entity_index, current_entity);
                    }
                }
                else if (isVanillaMagicItemPedestal(area_index, room_index, entity_index))
                {
                    // Fix Y position of a non-magic item placed into a magic item pedestal
                    current_entity.y = current_entity.y + 8;
                    setEntityFromIndices(rom, area_index, room_index, entity_index, current_entity);
                }
            }

            //if (roomContainsMagicItem)
            //{
                // If a magic item was found, the room UNKNOWN must be changed to 0xA or the magic item's graphics will display incorrectly
                // Note: Unfortunately this does not work properly for rooms that require displaying a different item that never appears in key
                // item rooms like push blocks, so this is disabled for now and only the coordinates must be set.
                //printf("DEBUG: Changing UNKNOWN for room %i %i from %x to 0xA.\n", area_index, room_index, current_room.unkn & 0xFFFF);
                //current_room.unkn = ROOM_TYPE_MAGICITEM;
                //setRoomFromIndices(rom, area_index, room_index, current_room);
            //}
        }
    }
}

static bool isVanillaMagicItemPedestal(int area_index, int room_index, int entity_index)
{
    if (
        (area_index == DASHBOOTS_PEDESTAL_AREA && room_index == DASHBOOTS_PEDESTAL_ROOM && entity_index == DASHBOOTS_PEDESTAL_ENTITY) ||
        (area_index == DOUBLE_PEDESTAL_AREA && room_index == DOUBLE_PEDESTAL_ROOM && entity_index == DOUBLE_PEDESTAL_ENTITY) ||
        (area_index == TACKLE_PEDESTAL_AREA && room_index == TACKLE_PEDESTAL_ROOM && entity_index == TACKLE_PEDESTAL_ENTITY) ||
        (area_index == KICKBOOTS_PEDESTAL_AREA && room_index == KICKBOOTS_PEDESTAL_ROOM && entity_index == KICKBOOTS_PEDESTAL_ENTITY) ||
        (area_index == HEAVYRING_PEDESTAL_AREA && room_index == HEAVYRING_PEDESTAL_ROOM && entity_index == HEAVYRING_PEDESTAL_ENTITY) ||
        (area_index == CLEANSING_PEDESTAL_AREA && room_index == CLEANSING_PEDESTAL_ROOM && entity_index == CLEANSING_PEDESTAL_ENTITY) ||
        (area_index == ROCWING_PEDESTAL_AREA && room_index == ROCWING_PEDESTAL_ROOM && entity_index == ROCWING_PEDESTAL_ENTITY) ||
        (area_index == LASTKEY_PEDESTAL_AREA && room_index == LASTKEY_PEDESTAL_ROOM && entity_index == LASTKEY_PEDESTAL_ENTITY)
    )
        return true;
    else
        return false;
}