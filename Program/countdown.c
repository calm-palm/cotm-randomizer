#include "countdown.h"
#include "game_structure.h"

const short flag_list_termination = 0xFFFF;

void patch_countdown(FILE* rom) {
  int next_pos = COUNTDOWN_TABLE_ADDR + (NUM_AREAS * 4);

  for(int area_idx = 0; area_idx < NUM_AREAS; area_idx++) {
    // write table entry for pointer to flag list
    int area_flags_start = next_pos | 0x08000000;
    fseek(rom, COUNTDOWN_TABLE_ADDR + (area_idx * 4), SEEK_SET);
    fwrite(&area_flags_start, 4, 1, rom);

    // get address of room def start/end
    fseek(rom, AREA_LIST_START + (area_idx * 4), SEEK_SET);
    int room_start;
    int room_end;
    fread(&room_start, 4, 1, rom);
    if(area_idx == (NUM_AREAS - 1)) {
      room_end = AREA_LIST_START | 0x08000000;
    }
    else {
      fread(&room_end, 4, 1, rom);
    }

    // loop over rooms
    for(int cur_room = room_start; cur_room < room_end; cur_room += sizeof(struct room_def)) {
      struct room_def cur_def;
      fseek(rom, cur_room & 0xFFFFFF, SEEK_SET);
      fread(&cur_def, sizeof(struct room_def), 1, rom);

      int next_entity_addr = cur_def.entity_list_offset & 0xFFFFFF;
      struct entity_entry cur_ent;
      fseek(rom, next_entity_addr, SEEK_SET);
      fread(&cur_ent, sizeof(struct entity_entry), 1, rom);

      // loop over entities within the room
      while(cur_ent.cntrl != (char)ENTITY_CNTRL_LIST_END) {
        if(cur_ent.entity_type == ENTITY_TPE_MOVEMENT) {
          fseek(rom, next_pos, SEEK_SET);
          fwrite(&(cur_ent.flag), 2, 1, rom);
          next_pos += 2;
        }
        else if(cur_ent.entity_type == ENTITY_TPE_CARD && cur_ent.param >= 0 && cur_ent.param < 0x14) {
          short card_flag = cur_ent.param | 0x8000; // using high bit as a flag that it's a card
          fseek(rom, next_pos, SEEK_SET);
          fwrite(&card_flag, 2, 1, rom);
          next_pos += 2;
        }

        next_entity_addr += sizeof(struct entity_entry);
        fseek(rom, next_entity_addr, SEEK_SET);
        fread(&cur_ent, sizeof(struct entity_entry), 1, rom);
      }
    }

    // write end flag list marker for the area
    fseek(rom, next_pos, SEEK_SET);
    fwrite(&flag_list_termination, 2, 1, rom);
    next_pos += 2;
  }
}
