#ifndef GAME_STRUCTURE_H_INCLUDED
#define GAME_STRUCTURE_H_INCLUDED

#define AREA_LIST_START 0xD9A40
#define AREA_LIST_END 0xD9A80
#define NUM_AREAS ((AREA_LIST_END - AREA_LIST_START) / 4)

#define ROOM_ENTRY_SIZE 0x1c
#define ENTITY_ENTRY_SIZE 0xC

#define ENTITY_TPE_CANDLECLOCKTOWER 0x1DE
#define ENTITY_TPE_CANDLE 0x1DF
#define ENTITY_TPE_MAXUP 0x1E4
#define ENTITY_TPE_CARD 0x1E6
#define ENTITY_TPE_MOVEMENT 0x1E8

#define ENTITY_CNTRL_LIST_END 0x80

#define ENTITY_CANDLE_HOLY_WATER 0x0
#define ENTITY_CANDLE_AXE 0x1
#define ENTITY_CANDLE_KNIFE 0x2
#define ENTITY_CANDLE_CROSS 0x3
#define ENTITY_CANDLE_STOPWATCH 0x4
#define ENTITY_CANDLE_SMALL_HEART 0x5
#define ENTITY_CANDLE_BIG_HEART 0x6

struct room_def { // 0x1C bytes
  int entity_list_offset;
  short enemyA;
  short enemyB;
  short enemyC;
  short enemyD;
  short altenemyA;
  short altenemyB;
  short altenemyC;
  short altenemyD;
  short room_switch_flag; // If set, will swap to alternate enemy list
  short unkn; // 0A 00 for key item pedestal room
  int unkn2; // 04 00 00 00 for regular room
};

struct entity_entry { // 0xC bytes
  short x;
  short y;
  short entity_type;
  short param;
  short flag;
  char unkn;
  char cntrl;
};

#endif
