#include "game_structure.h"
#include "subweapon.h"

#include <stdlib.h>

static int roll_subweapon(int *choices) {
  int sum = 0;
  for(int i = 0; i < 5; i++) {
    sum += choices[i];
  }

  int num = rand() % sum;
  int prev_sum = 0;

  for(int i = 0; i < 5; i++) {
    if((choices[i] + prev_sum) > num) {
      return i;
    }
    prev_sum += choices[i];
  }

  // we should not get here with a vanilla source rom
  // but if we do, might as well just pick random
  return (rand() % 5);
}

void subweapon_shuffle(FILE* rom, unsigned int seed) {
  // probably safe to use rand without this do to the seed previously being set
  // but just for safety, force it
  srand(seed);

  int remaining[] = { // these should be the vanilla counts
    22, // holy water
    24, // axe
    32, // knife
    6, // cross
    12 // stopwatch
  };

  for(int area_idx = 0; area_idx < NUM_AREAS; area_idx++) {
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
        if((cur_ent.entity_type == ENTITY_TPE_CANDLE || cur_ent.entity_type == ENTITY_TPE_CANDLECLOCKTOWER) &&
           cur_ent.param >= ENTITY_CANDLE_HOLY_WATER &&
           cur_ent.param <= ENTITY_CANDLE_STOPWATCH) {

          int new_subweapon = roll_subweapon(remaining);
          remaining[new_subweapon]--;
          cur_ent.param = new_subweapon;

          fseek(rom, next_entity_addr, SEEK_SET);
          fwrite(&cur_ent, sizeof(struct entity_entry), 1, rom);
        }

        next_entity_addr += sizeof(struct entity_entry);
        fseek(rom, next_entity_addr, SEEK_SET);
        fread(&cur_ent, sizeof(struct entity_entry), 1, rom);
      }
    }
  }
}
