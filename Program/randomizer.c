#include <stdlib.h>
#include "randomizer.h"
#include "addresses.h"
#include "spoilertranslator.h"

// Internal functions
static void findAllItemsReachable(int reachable[], int keyItems[], bool hasIceOrStone, bool ignoreCleansing, bool ironMaiden, int last_keys_count, bool nerfRocWing);
static bool populateKeyItem(int reachable[], int item_assignment[], int keyItems[], bool dssCards[], bool hasIceOrStone, int last_keys_count, struct seed_options *options);
static void populateItems(int reachable[], int item_assignment[], bool dssCards[], struct seed_options *options);
static int getRandomValidPedestal(int reachable[], int item_assignment[]);
static int getRandomHighPriorityValidPedestal(int reachable[], int item_assignment[]);

static int getRandomUnplacedDSSCard(bool dssCards[]);

static void populateEnemyDrops(int regular_drops[], int rare_drops[], int regular_drop_chance[], int rare_drop_chance[], struct seed_options *options);
static int selectDrop(const int dropList[], int dropsPlaced[], int number_drops, bool exclusiveDrop);

static void writeIPS(FILE* randomizer_patch, int item_assignment[], int regular_drops[], int rare_drops[], int regular_drop_chance[], int rare_drop_chance[], struct seed_options *options, FILE* spoiler_log);

/* A better way of handling enemy data - we can now have IDs 
and HP and the like all associated with each other properly.

Had to put this stuff here rather than addresses.h or randomizer.h 
because of "multiple definition" errors on compile - probably something 
screwy with the #include statements in this program. -Malaert64 */
typedef struct {
    int index;
    char name[20];
    int hp;
    int atk;
    int def;
    int exp;
} EnemyData;

/* I omitted Coffin from the end of this, as its presence doesn't
actually impact the randomizer (all stats and drops inherited from Mummy). */
EnemyData enemies_data_table[NUMBER_ENEMIES] = {
    {0, "Medusa Head", 6, 120, 60, 2},
    {1, "Zombie", 48, 70, 20, 2},
    {2, "Ghoul", 100, 190, 79, 3},
    {3, "Wight", 110, 235, 87, 4},
    {4, "Clinking Man", 80, 135, 25, 21},
    {5, "Zombie Thief", 120, 185, 30, 58},
    {6, "Skeleton", 25, 65, 45, 4},
    {7, "Skeleton Bomber", 20, 50, 40, 4},
    {8, "Electric Skeleton", 42, 80, 50, 30},
    {9, "Skeleton Spear", 30, 65, 46, 6},
    {10, "Skeleton Boomerang", 60, 170, 90, 112},
    {11, "Skeleton Soldier", 35, 90, 60, 16},
    {12, "Skeleton Knight", 50, 140, 80, 39},
    {13, "Bone Tower", 84, 201, 280, 160},
    {14, "Fleaman", 60, 142, 45, 29},
    {15, "Poltergeist", 105, 360, 380, 510},
    {16, "Bat", 5, 50, 15, 4},
    {17, "Spirit", 9, 55, 17, 1},
    {18, "Ectoplasm", 12, 165, 51, 2},
    {19, "Specter", 15, 295, 95, 3},
    {20, "Axe Armor", 55, 120, 130, 31},
    {21, "Flame Armor", 160, 320, 300, 280},
    {22, "Flame Demon", 300, 315, 270, 600},
    {23, "Ice Armor", 240, 470, 520, 1500},
    {24, "Thunder Armor", 204, 340, 320, 800},
    {25, "Wind Armor", 320, 500, 460, 1800},
    {26, "Earth Armor", 130, 230, 280, 240},
    {27, "Poison Armor", 260, 382, 310, 822},
    {28, "Forest Armor", 370, 390, 390, 1280},
    {29, "Stone Armor", 90, 220, 320, 222},
    {30, "Ice Demon", 350, 492, 510, 4200},
    {31, "Holy Armor", 350, 420, 450, 1700},
    {32, "Thunder Demon", 180, 270, 230, 450},
    {33, "Dark Armor", 400, 680, 560, 3300},
    {34, "Wind Demon", 400, 540, 490, 3600},
    {35, "Bloody Sword", 30, 220, 500, 200},
    {36, "Golem", 650, 520, 700, 1400},
    {37, "Earth Demon", 150, 90, 85, 25},
    {38, "Were-wolf", 160, 265, 110, 140},
    {39, "Man Eater", 400, 330, 233, 700},
    {40, "Devil Tower", 10, 140, 200, 17},
    {41, "Skeleton Athlete", 100, 100, 50, 25},
    {42, "Harpy", 120, 275, 200, 271},
    {43, "Siren", 160, 443, 300, 880},
    {44, "Imp", 90, 220, 99, 103},
    {45, "Mudman", 25, 79, 30, 2},
    {46, "Gargoyle", 60, 160, 66, 3},
    {47, "Slime", 40, 102, 18, 11},
    {48, "Frozen Shade", 112, 490, 560, 1212},
    {49, "Heat Shade", 80, 240, 200, 136},
    {50, "Poison Worm", 120, 30, 20, 12},
    {51, "Myconid", 50, 250, 114, 25},
    {52, "Will O'Wisp", 11, 110, 16, 9},
    {53, "Spearfish", 40, 360, 450, 280},
    {54, "Merman", 60, 303, 301, 10},
    {55, "Minotaur", 410, 520, 640, 2000},
    {56, "Were-horse", 400, 540, 360, 1970},
    {57, "Marionette", 80, 160, 150, 127},
    {58, "Gremlin", 30, 80, 33, 2},
    {59, "Hopper", 40, 87, 35, 8},
    {60, "Evil Pillar", 20, 460, 800, 480},
    {61, "Were-panther", 200, 300, 130, 270},
    {62, "Were-jaguar", 270, 416, 170, 760},
    {63, "Bone Head", 24, 60, 80, 7},
    {64, "Fox Archer", 75, 130, 59, 53},
    {65, "Fox Hunter", 100, 290, 140, 272},
    {66, "Were-bear", 265, 250, 140, 227},
    {67, "Grizzly", 600, 380, 200, 960},
    {68, "Cerberus", 600, 150, 100, 500},
    {69, "Beast Demon", 150, 330, 250, 260},
    {70, "Arch Demon", 320, 505, 400, 1000},
    {71, "Demon Lord", 460, 660, 500, 1950},
    {72, "Gorgon", 230, 215, 165, 219},
    {73, "Catoblepas", 550, 500, 430, 1800},
    {74, "Succubus", 150, 400, 350, 710},
    {75, "Fallen Angel", 370, 770, 770, 6000},
    {76, "Necromancer", 500, 200, 250, 2500},
    {77, "Hyena", 93, 140, 70, 105},
    {78, "Fishhead", 80, 320, 504, 486},
    {79, "Dryad", 120, 300, 360, 300},
    {80, "Mimic Candle", 990, 600, 600, 6600},
    {81, "Brain Float", 20, 50, 25, 10},
    {82, "Evil Hand", 52, 150, 120, 63},
    {83, "Abiondarg", 88, 388, 188, 388},
    {84, "Iron Golem", 640, 290, 450, 8000},
    {85, "Devil", 1080, 800, 900, 10000},
    {86, "Witch", 144, 330, 290, 600},
    {87, "Mummy", 100, 100, 35, 3},
    {88, "Hipogriff", 300, 500, 210, 740},
    {89, "Adramelech", 1800, 380, 360, 16000},
    {90, "Arachne", 330, 420, 288, 1300},
    {91, "Death Mantis", 200, 318, 240, 400},
    {92, "Alraune", 774, 490, 303, 2500},
    {93, "King Moth", 140, 290, 160, 150},
    {94, "Killer Bee", 8, 308, 108, 88},
    {95, "Dragon Zombie", 1400, 390, 440, 15000},
    {96, "Lizardman", 100, 345, 400, 800},
    {97, "Franken", 1200, 700, 350, 2100},
    {98, "Legion", 420, 610, 375, 1590},
    {99, "Dullahan", 240, 550, 440, 2200},
    {100, "Death", 880, 600, 800, 60000},
    {101, "Camilla", 1500, 650, 700, 80000},
    {102, "Hugh", 1400, 570, 750, 120000},
    {103, "Dracula", 1100, 805, 850, 150000},
    {104, "Dracula", 3000, 1000, 1000, 0},
    {105, "Skeleton Medalist", 250, 100, 100, 1500},
    {106, "Were-jaguar", 320, 518, 260, 1200},
    {107, "Were-wolf", 340, 525, 180, 1100},
    {108, "Catoblepas", 560, 510, 435, 2000},
    {109, "Hipogriff", 500, 620, 280, 1900},
    {110, "Wind Demon", 490, 600, 540, 4000},
    {111, "Witch", 210, 480, 340, 1000},
    {112, "Stone Armor", 260, 585, 750, 3000},
    {113, "Devil Tower", 50, 560, 700, 600},
    {114, "Skeleton", 150, 400, 200, 500},
    {115, "Skeleton Bomber", 150, 400, 200, 550},
    {116, "Electric Skeleton", 150, 400, 200, 700},
    {117, "Skeleton Spear", 150, 400, 200, 580},
    {118, "Flame Demon", 680, 650, 600, 4500},
    {119, "Bone Tower", 120, 500, 650, 800},
    {120, "Fox Hunter", 160, 510, 220, 600},
    {121, "Poison Armor", 380, 680, 634, 3600},
    {122, "Bloody Sword", 55, 600, 1200, 2000},
    {123, "Abiondarg", 188, 588, 288, 588},
    {124, "Legion", 540, 760, 480, 2900},
    {125, "Marionette", 200, 420, 400, 1200},
    {126, "Minotaur", 580, 700, 715, 4100},
    {127, "Arachne", 430, 590, 348, 2400},
    {128, "Succubus", 300, 670, 630, 3100},
    {129, "Demon Lord", 590, 800, 656, 4200},
    {130, "Alraune", 1003, 640, 450, 5000},
    {131, "Hyena", 210, 408, 170, 1000},
    {132, "Devil Armor", 500, 804, 714, 6600},
    {133, "Evil Pillar", 55, 655, 900, 1500},
    {134, "White Armor", 640, 770, 807, 7000},
    {135, "Devil", 1530, 980, 1060, 30000},
    {136, "Scary Candle", 150, 300, 300, 900},
    {137, "Trick Candle", 200, 400, 400, 1400},
    {138, "Nightmare", 250, 550, 550, 2000},
    {139, "Lilim", 400, 800, 800, 8000},
    {140, "Lilith", 660, 960, 960, 20000},
};

void generateRandomizerPatch(FILE* randomizer_patch, unsigned int randomizer_seed, struct seed_options *options, FILE* spoiler_log)
{
    // Pedestal randomizer
    int reachable[NUMBER_PEDESTALS];
    int item_assignment[NUMBER_PEDESTALS];
    int keyItems[NUMBER_KEYITEMS] = { false };
    bool dssCards[NUMBER_DSSCARDS] = { false };
    bool hasIceOrStone = false;

    // Pedestal randomizer options
    bool ironMaiden = options->breakIronMaidens;
    int last_keys_count = options->lastKeyAvailable;

    // Enemy drop randomizer
    int regular_drops[NUMBER_ENEMIES];
    int rare_drops[NUMBER_ENEMIES];
    int regular_drop_chance[NUMBER_ENEMIES];
    int rare_drop_chance[NUMBER_ENEMIES];

    int i;

    // Seed random number generator
    srand(randomizer_seed);

    printf("Seeded RNG.\n");

    for (i = 0; i < NUMBER_PEDESTALS; i++)
    {
        // Set all values in reachability array to -1 to indicate unreachable status
        reachable[i] = UNREACHABLE;
        // Set all values in item assignment array to -1 to indicate unassigned status
        item_assignment[i] = UNASSIGNED;
    }

    // Assume that we have obtained Dash Boots already (via AutoDashBoots.ips)
    keyItems[INDEX_KEYITEM_DASHBOOTS] = true;

    // If All Bosses Required is enabled, manually set the Last Key locations and fix the Last Key running total before placing any other items
    if (options->allBossesRequired)
    {
        item_assignment[INDEX_CATACOMB24] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_AUDIENCE_ROOM25] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_MACHINE_TOWER19] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_CHAPEL_TOWER22] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_UNDERGROUND_GALLERY20] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_UNDERGROUND_WAREHOUSE23] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_UNDERGROUND_WATERWAY17] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_OBSERVATION_TOWER20] = INDEX_KEYITEM_LASTKEY;

        keyItems[INDEX_KEYITEM_LASTKEY] = last_keys_count;
    }

    /* If All Bosses + Battle Arena Required is enabled, manually 
    set the Last Key locations and fix the Last Key running total 
    before placing any other items. */
    if (options->allBossesAndBattleArenaRequired) {
        item_assignment[INDEX_CATACOMB24] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_AUDIENCE_ROOM25] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_MACHINE_TOWER19] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_CHAPEL_TOWER22] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_UNDERGROUND_GALLERY20] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_UNDERGROUND_WAREHOUSE23] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_UNDERGROUND_WATERWAY17] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_OBSERVATION_TOWER20] = INDEX_KEYITEM_LASTKEY;
        item_assignment[INDEX_BATTLEARENA24] = INDEX_KEYITEM_LASTKEY; /* :) */

        keyItems[INDEX_KEYITEM_LASTKEY] = last_keys_count;
    } else {
        /* Otherwise, assign Shinning Armor to end of Battle Arena. */
        item_assignment[INDEX_BATTLEARENA24] = INDEX_SPECIALITEM_SHINNINGARMOR;
    }

    printf("Initialized arrays and initial items.\n");

    // Iterate for each key item placement
    while (!keyItems[INDEX_KEYITEM_DASHBOOTS] || !keyItems[INDEX_KEYITEM_DOUBLE] || !keyItems[INDEX_KEYITEM_KICKBOOTS] || !keyItems[INDEX_KEYITEM_HEAVYRING] ||
           !keyItems[INDEX_KEYITEM_CLEANSING] || !keyItems[INDEX_KEYITEM_ROCWING] || (keyItems[INDEX_KEYITEM_LASTKEY] < last_keys_count) || !hasIceOrStone)
    {
        // Mark all rooms reachable that we can reach with current key items
        findAllItemsReachable(reachable, keyItems, hasIceOrStone, options->ignoreCleansing, ironMaiden, last_keys_count, options->nerfRocWing);

        // Select a random room from those rooms that are now reachable that does not contain a key item to place a key item
        // or a DSS card combo allowing ice or stone
        hasIceOrStone = populateKeyItem(reachable, item_assignment, keyItems, dssCards, hasIceOrStone, last_keys_count, options);
    }

    // Assert that all rooms are now reachable (minus Battle Arena, which we disregard)
    findAllItemsReachable(reachable, keyItems, hasIceOrStone, options->ignoreCleansing, ironMaiden, last_keys_count, options->nerfRocWing);
    for (i = 0; i < NUMBER_PEDESTALS; i++)
    {
        if (reachable[i] == UNREACHABLE)
        {
            printf("UNREACHABLE PEDESTAL: %i\n", i);
            exit(1);
        }    
    }

    // Fill remaining pedestals with Max Ups
    populateItems(reachable, item_assignment, dssCards, options);

    // Enemy drop randomizer selection
    populateEnemyDrops(regular_drops, rare_drops, regular_drop_chance, rare_drop_chance, options);

    // Write item placements to IPS
    writeIPS(randomizer_patch, item_assignment, regular_drops, rare_drops, regular_drop_chance, rare_drop_chance, options, spoiler_log);

    return;
}

static void findAllItemsReachable(int reachable[], int keyItems[], bool hasIceOrStone, bool ignoreCleansing, bool ironMaiden, int last_keys_count, bool nerfRocWing)
{
    // Assume switch is in original location accessible with Double and Kick Boots and set to true if it is false and those are obtained
    // unless the optional rule breaking them without pressing the button has been enabled
    if (!ironMaiden)
    {
        ironMaiden = keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS];
    }

    bool rocOrDouble = keyItems[INDEX_KEYITEM_DOUBLE] || keyItems[INDEX_KEYITEM_ROCWING];
    bool rocOrKickBoots = keyItems[INDEX_KEYITEM_KICKBOOTS] || keyItems[INDEX_KEYITEM_ROCWING];

    // Jump power notes from Liquid Cat:
    // Jump Lv. 5 = Roc Wing + Kick Boots + Double
    // Jump Lv. 4 = Roc Wing + Kick Boots
    // Jump Lv. 3 = Roc Wing + Double (alternatively solvable with Kick Boots instead of Double)
    // Jump Lv. 2 = Roc Wing
    // Jump Lv. 1 = Double (alternatively solvable with just Roc Wing)
    // Jump Lv. 0 = Nothing
    //
    int jumpLevel = 0;

    if (nerfRocWing)
    {
        if (keyItems[INDEX_KEYITEM_DOUBLE])
            jumpLevel = 1;

        if (keyItems[INDEX_KEYITEM_ROCWING])
            jumpLevel = 2;

        if (keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_ROCWING])
            jumpLevel = 3;

        if (keyItems[INDEX_KEYITEM_KICKBOOTS] && keyItems[INDEX_KEYITEM_ROCWING])
            jumpLevel = 4;

        if (keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && keyItems[INDEX_KEYITEM_ROCWING])
            jumpLevel = 5;
    }
    else
    {
        if (keyItems[INDEX_KEYITEM_DOUBLE])
            jumpLevel = 1;

        if (keyItems[INDEX_KEYITEM_ROCWING])
            jumpLevel = 5;
    }


    // No requirements
    reachable[INDEX_CATACOMB4]++;
    reachable[INDEX_CATACOMB8]++;
    reachable[INDEX_CATACOMB9]++;
    reachable[INDEX_CATACOMB10]++;
    reachable[INDEX_CATACOMB13]++;
    reachable[INDEX_CATACOMB14]++;
    reachable[INDEX_CATACOMB16]++;
    reachable[INDEX_CATACOMB20]++;
    reachable[INDEX_CATACOMB22]++;
    reachable[INDEX_CATACOMB24]++;

    // Double
    if (rocOrDouble)
    {
        reachable[INDEX_CATACOMB3]++;
        reachable[INDEX_CATACOMB25]++;
        reachable[INDEX_ABYSS_STAIRWAY9]++;
        reachable[INDEX_AUDIENCE_ROOM4]++;
        reachable[INDEX_AUDIENCE_ROOM7]++;
        reachable[INDEX_AUDIENCE_ROOM8]++;
        reachable[INDEX_AUDIENCE_ROOM10]++;
        reachable[INDEX_AUDIENCE_ROOM14]++;
        reachable[INDEX_AUDIENCE_ROOM16]++;
        reachable[INDEX_AUDIENCE_ROOM17]++;
        reachable[INDEX_AUDIENCE_ROOM18]++;
        reachable[INDEX_AUDIENCE_ROOM21]++;
        reachable[INDEX_AUDIENCE_ROOM25]++;
        reachable[INDEX_OUTER_WALL2]++;
        reachable[INDEX_TRIUMPH_HALLWAY1]++;
    }

    // Tackle
    if (keyItems[INDEX_KEYITEM_TACKLE])
    {
        reachable[INDEX_CATACOMB5]++;
    }

    // Kick Boots
    if (rocOrKickBoots)
    {
        if (!nerfRocWing || jumpLevel >= 3)
        {
            reachable[INDEX_CATACOMB8B]++;
        }
    }

    // Heavy Ring
    if (keyItems[INDEX_KEYITEM_HEAVYRING])
    {
        reachable[INDEX_CATACOMB1]++;
    }



    // Roc Wing
    if (keyItems[INDEX_KEYITEM_ROCWING])
    {
        // Jump Level 5, or if nerfRocWing not in use
        if (!nerfRocWing || jumpLevel >= 5)
        {
            reachable[INDEX_SEALED_ROOM3]++;

            reachable[INDEX_OBSERVATION_TOWER1]++;
            reachable[INDEX_OBSERVATION_TOWER2]++;
            reachable[INDEX_OBSERVATION_TOWER3]++;
            reachable[INDEX_OBSERVATION_TOWER5]++;
            reachable[INDEX_OBSERVATION_TOWER8]++;
            reachable[INDEX_OBSERVATION_TOWER9]++;
            reachable[INDEX_OBSERVATION_TOWER12]++;
            reachable[INDEX_OBSERVATION_TOWER13]++;
            reachable[INDEX_OBSERVATION_TOWER16]++;
            reachable[INDEX_OBSERVATION_TOWER20]++;
            reachable[INDEX_BATTLEARENA24]++; // This is not strictly correct, but we're never going to place an item here
        } 

        // Jump Level 4, or if nerfRocWing not in use
        if (!nerfRocWing || jumpLevel >= 4)
        {
            reachable[INDEX_ABYSS_STAIRWAY4]++;
            reachable[INDEX_AUDIENCE_ROOM14B]++;
            reachable[INDEX_OUTER_WALL0]++;
        }

        // Jump Level 4 or Jump Level 3 with Ice/Stone, or if nerfRocWing not in use
        if (!nerfRocWing || jumpLevel >= 4 || (jumpLevel >= 3 && hasIceOrStone))
        {
            reachable[INDEX_AUDIENCE_ROOM30]++;
            reachable[INDEX_AUDIENCE_ROOM30B]++;
            reachable[INDEX_CHAPEL_TOWER26]++;
            reachable[INDEX_CHAPEL_TOWER26B]++;
        }

        // Jump Level 3, or if nerfRocWing not in use
        if (!nerfRocWing || jumpLevel >= 3)
        {
            reachable[INDEX_UNDERGROUND_WATERWAY13]++;
            reachable[INDEX_UNDERGROUND_WATERWAY18]++;
        }
    }



    // Double and Tackle
    if (rocOrDouble && keyItems[INDEX_KEYITEM_TACKLE])
    {
        reachable[INDEX_AUDIENCE_ROOM11]++;
        reachable[INDEX_MACHINE_TOWER0]++;
        reachable[INDEX_MACHINE_TOWER2]++;
        reachable[INDEX_MACHINE_TOWER4]++;
        reachable[INDEX_MACHINE_TOWER8]++;
        reachable[INDEX_MACHINE_TOWER10]++;
        reachable[INDEX_MACHINE_TOWER11]++;
        reachable[INDEX_MACHINE_TOWER14]++;
    }

    // Double and Kick Boots
    if (rocOrDouble && rocOrKickBoots)
    {
        if (!nerfRocWing || jumpLevel >= 2)
        {
            reachable[INDEX_AUDIENCE_ROOM17B]++;
            reachable[INDEX_AUDIENCE_ROOM19]++;
        }

        reachable[INDEX_ETERNAL_CORRIDOR7]++;
        reachable[INDEX_ETERNAL_CORRIDOR9]++;
        reachable[INDEX_CHAPEL_TOWER5]++;
        reachable[INDEX_CHAPEL_TOWER6]++;
        reachable[INDEX_CHAPEL_TOWER6B]++;
        reachable[INDEX_CHAPEL_TOWER8]++;
        reachable[INDEX_CHAPEL_TOWER15]++;
        reachable[INDEX_CHAPEL_TOWER16]++;
        reachable[INDEX_CHAPEL_TOWER18]++;
    }

    // Double or Kick Boots
    if (rocOrDouble || rocOrKickBoots)
    {
        if (!nerfRocWing || jumpLevel >= 1 || keyItems[INDEX_KEYITEM_KICKBOOTS])
        {
            reachable[INDEX_CATACOMB14B]++;
        }

        reachable[INDEX_ABYSS_STAIRWAY2]++;
        reachable[INDEX_ABYSS_STAIRWAY3]++;
    }

    // Double and Heavy Ring
    if (rocOrDouble && keyItems[INDEX_KEYITEM_HEAVYRING])
    {
        reachable[INDEX_AUDIENCE_ROOM9]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE1]++;
    }

    // Tackle and Roc Wing
    if (keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_ROCWING])
    {
        if (!nerfRocWing || jumpLevel >= 5)
        {
            reachable[INDEX_AUDIENCE_ROOM26]++;
        }
    }

    // Iron Maiden and Roc Wing OR Double, Kick Boots, Iron Maiden, and Ice/Stone
    if ((ironMaiden && keyItems[INDEX_KEYITEM_ROCWING]) || (ironMaiden && keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && hasIceOrStone))
    {
        if (!nerfRocWing || (jumpLevel >= 2 && ironMaiden) || (ironMaiden && keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && hasIceOrStone))
        {
            reachable[INDEX_UNDERGROUND_GALLERY3]++;
        }


        if (!nerfRocWing || (jumpLevel >= 4 && ironMaiden) || (ironMaiden && keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && hasIceOrStone))
        {
            reachable[INDEX_UNDERGROUND_GALLERY3B]++;
        }

    }

    // Iron Maiden, Roc Wing, Cleansing
    if (ironMaiden && keyItems[INDEX_KEYITEM_ROCWING] && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing))
    {
        if (!nerfRocWing || ((jumpLevel >= 2 && ironMaiden && keyItems[INDEX_KEYITEM_CLEANSING]) || ignoreCleansing))
        {
            reachable[INDEX_UNDERGROUND_WATERWAY8]++;
        }
    }
    
    // Tackle, Heavy Ring, and Roc Wing
    if (keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING] && keyItems[INDEX_KEYITEM_ROCWING])
    {
        if (!nerfRocWing || (jumpLevel >= 5 && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING]) || (jumpLevel >= 4 && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING] && hasIceOrStone))
        {
            reachable[INDEX_UNDERGROUND_WAREHOUSE10]++;
        }

        if (!nerfRocWing || (jumpLevel >= 3 && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING]) || (jumpLevel >= 2 && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING] && hasIceOrStone))
        {
            reachable[INDEX_UNDERGROUND_WAREHOUSE16B]++;
        }
    }

    // Double, Tackle, and Kick Boots
    if (rocOrDouble && keyItems[INDEX_KEYITEM_TACKLE] && rocOrKickBoots)
    {
        if (!nerfRocWing || jumpLevel >= 2 || (keyItems[INDEX_KEYITEM_DOUBLE && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_KICKBOOTS]]))
        {
            reachable[INDEX_MACHINE_TOWER3]++;
            reachable[INDEX_MACHINE_TOWER6]++;
        }
    }

    // Double, Tackle, and Heavy Ring
    if (rocOrDouble && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING])
    {
        reachable[INDEX_AUDIENCE_ROOM27]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE6]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE8]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE9]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE11]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE16]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE19]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE23]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE24]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE25]++;
    }

    if (rocOrDouble && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING] && (keyItems[INDEX_KEYITEM_ROCWING] || hasIceOrStone))
    {
        if (!nerfRocWing || (keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING] && (jumpLevel >= 2 || (keyItems[INDEX_KEYITEM_DOUBLE] && hasIceOrStone))))
        {
            reachable[INDEX_UNDERGROUND_WAREHOUSE14]++; // If you solved the puzzle before checking this spot, it is no longer reachable without ice or stone or Roc Wing, so we require these in logic despite an early check being possible
        }
        
    }

    // Double and Iron Maiden
    if (rocOrDouble && ironMaiden)
    {
        reachable[INDEX_UNDERGROUND_WATERWAY1]++;
    }

    // Double, Kick Boots, and Iron Maiden
    if (rocOrDouble && rocOrKickBoots && ironMaiden)
    {
        reachable[INDEX_CHAPEL_TOWER22]++;
    }

    // Double, Kick Boots, and Heavy Ring
    if (rocOrDouble && rocOrKickBoots && keyItems[INDEX_KEYITEM_HEAVYRING])
    {
        reachable[INDEX_CHAPEL_TOWER4]++;
        reachable[INDEX_CHAPEL_TOWER10]++;
    }

    // Double, Iron Maiden, and Cleansing
    if (rocOrDouble && ironMaiden && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing))
    {
        reachable[INDEX_UNDERGROUND_WATERWAY3]++;
        reachable[INDEX_UNDERGROUND_WATERWAY3B]++;
        reachable[INDEX_UNDERGROUND_WATERWAY4]++;
        reachable[INDEX_UNDERGROUND_WATERWAY7]++;
        reachable[INDEX_UNDERGROUND_WATERWAY9]++;
        reachable[INDEX_UNDERGROUND_WATERWAY9B]++;
        reachable[INDEX_UNDERGROUND_WATERWAY12B]++;
    }

    // Double and Kick Boots
    // OR Double, Heavy Ring, Iron Maiden, and Tackle
    // =
    // Double && (Kick Boots || (Heavy Ring, Iron Maiden, and Tackle))
    if (rocOrDouble && (keyItems[INDEX_KEYITEM_KICKBOOTS] || (keyItems[INDEX_KEYITEM_HEAVYRING] && ironMaiden && keyItems[INDEX_KEYITEM_TACKLE])))
    {
        reachable[INDEX_ETERNAL_CORRIDOR5]++;
    }
    
    // Double, Kick Boots, and Iron Maiden
    // OR Double, Heavy Ring, Iron Maiden, and Tackle
    // = 
    // Double && Iron Maiden && (Kick Boots || (Heavy Ring && Tackle))
    if (rocOrDouble && ironMaiden && (keyItems[INDEX_KEYITEM_KICKBOOTS] || (keyItems[INDEX_KEYITEM_HEAVYRING] && keyItems[INDEX_KEYITEM_TACKLE])))
    {
        reachable[INDEX_UNDERGROUND_GALLERY0]++;
    }
            
    // Double, Heavy Ring, Iron Maiden and Kick Boots
    // OR Double, Heavy Ring, Iron Maiden, and Tackle
    // =
    // Double && Heavy Ring && Iron Maiden && (Kick Boots || Tackle)
    if (rocOrDouble && keyItems[INDEX_KEYITEM_HEAVYRING] && ironMaiden && (keyItems[INDEX_KEYITEM_KICKBOOTS] || keyItems[INDEX_KEYITEM_TACKLE]))
    {
        reachable[INDEX_UNDERGROUND_GALLERY1]++;
        reachable[INDEX_UNDERGROUND_GALLERY2]++;
    }


    // Double and Kick Boots OR Double and Tackle
    if ((rocOrDouble && rocOrKickBoots) || (rocOrDouble && keyItems[INDEX_KEYITEM_TACKLE]))
    {
        reachable[INDEX_MACHINE_TOWER13]++;
        reachable[INDEX_MACHINE_TOWER17]++;
        reachable[INDEX_MACHINE_TOWER19]++;
    }
    
    // Double and Last Key and (Kick Boots OR Roc Wing))
    // Note that "last_keys_count" is used instead of the amount required so that we don't run into a situation where a Last Key
    // is behind the door, which would not be nice
    if (rocOrDouble && (keyItems[INDEX_KEYITEM_LASTKEY] >= last_keys_count) && (rocOrKickBoots || keyItems[INDEX_KEYITEM_ROCWING]))
    {
        if (!nerfRocWing || (rocOrDouble && (keyItems[INDEX_KEYITEM_LASTKEY] >= last_keys_count) && (rocOrKickBoots || jumpLevel >= 3)))
        {
            reachable[INDEX_CEREMONIAL_ROOM1]++;
        }
    }

    // Double, Iron Maiden, and Cleansing OR Roc Wing
    if ((rocOrDouble && ironMaiden && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing)) || keyItems[INDEX_KEYITEM_ROCWING])
    {
        reachable[INDEX_UNDERGROUND_WATERWAY12]++;
        reachable[INDEX_UNDERGROUND_WATERWAY17]++;
    }

    // Double, Kick Boots, and Ice/Stone OR Roc Wing
    if ((rocOrDouble && rocOrKickBoots && hasIceOrStone) || keyItems[INDEX_KEYITEM_ROCWING])
    {
        if (!nerfRocWing || jumpLevel >= 2 || (keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && hasIceOrStone))
        {
            reachable[INDEX_TRIUMPH_HALLWAY3]++;
            reachable[INDEX_CHAPEL_TOWER1]++;
            reachable[INDEX_CHAPEL_TOWER13]++;
        }
    }

    // Double, Ice/Stone, and Iron Maiden OR Roc Wing and Iron Maiden
    if ((rocOrDouble && hasIceOrStone && ironMaiden && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing)) || (keyItems[INDEX_KEYITEM_ROCWING] && ironMaiden && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing)))
    {
        if (!nerfRocWing || (((keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing) && ironMaiden) && ((keyItems[INDEX_KEYITEM_DOUBLE] && hasIceOrStone) || jumpLevel >= 3)) )
        {
            reachable[INDEX_UNDERGROUND_WATERWAY5]++;
        }
        
    }

    // Double and Ice/Stone OR Roc Wing
    if ((rocOrDouble && hasIceOrStone) || keyItems[INDEX_KEYITEM_ROCWING])
    {
        // Jump Level 3, Jump Level 1 and Ice/Stone, or if nerfRocWing not in use
        if (!nerfRocWing || jumpLevel >= 4 || (jumpLevel >= 1 && hasIceOrStone))
        {
            reachable[INDEX_CATACOMB3B]++;
        }

        // Jump Level 5, Double and Ice/Stone, or if nerfRocWing not in use
        if (!nerfRocWing || jumpLevel >= 5 || (jumpLevel >= 1 && hasIceOrStone))
        {
            reachable[INDEX_OUTER_WALL1]++;
        }
    }

    // Double, Kick Boots, Iron Maiden, and Tackle
    // OR Double, Heavy Ring, Iron Maiden, and Tackle
    // =
    // Double && Iron Maiden && Tackle (Heavy Ring || (Kick Boots))
    if ((rocOrDouble && ironMaiden && keyItems[INDEX_KEYITEM_TACKLE] && (keyItems[INDEX_KEYITEM_HEAVYRING] || (keyItems[INDEX_KEYITEM_KICKBOOTS]))))
    {

        reachable[INDEX_UNDERGROUND_GALLERY8]++;
    }

    // Double, Kick Boots, Iron Maiden, and Tackle
    // OR Double, Heavy Ring, and Iron Maiden
    // =
    // Double && Iron Maiden && (Heavy Ring || (Kick Boots && Tackle))
    if ((rocOrDouble && ironMaiden && (keyItems[INDEX_KEYITEM_HEAVYRING] || (keyItems[INDEX_KEYITEM_KICKBOOTS] && keyItems[INDEX_KEYITEM_TACKLE]))))
    {
        reachable[INDEX_UNDERGROUND_GALLERY10]++;
        reachable[INDEX_UNDERGROUND_GALLERY13]++;
        reachable[INDEX_UNDERGROUND_GALLERY15]++;
        reachable[INDEX_UNDERGROUND_GALLERY20]++;
    }

    //printf("Checked all reachable items.\n");

    return;
}

static bool populateKeyItem(int reachable[], int item_assignment[], int keyItems[], bool dssCards[], bool hasIceOrStone, int last_keys_count, struct seed_options *options)
{
    int number_eligible_pedestals = 0;

    int eligible_keyitems[NUMBER_KEYITEMS+1]; // One is added for the ice or stone "progress item" case
    int number_eligible_keyitems = 0;

    int random_chance_to_make_eligible = 9;
    int random_chance_to_use_priority;

    int random_eligible_keyitem_choice;
    int random_pedestal_choice;
    int random_pedestal_secondary_choice;
    int random_action_card_choice;
    int random_attribute_card_choice;

    int i;

    // Get the count of eligible pedestals
    for (i = 0; i < NUMBER_PEDESTALS; i++)
        if (reachable[i] != UNREACHABLE && item_assignment[i] == UNASSIGNED)
            number_eligible_pedestals++;

    // Place index of each eligible key item into our list of eligible key items
    for (i = 0; i < NUMBER_KEYITEMS; i++)
    {
        // If the key item has not yet been granted, it is eligible unless it is the Last Key, which requires at least Double && Kick Boots or Roc Wing to be
        // obtained to avoid a softlock in the hidden room in Ceremonial Room
        if (!keyItems[i] && i != INDEX_KEYITEM_LASTKEY)
        {
            // While Double is not obtained, skew heavily in its favor
            // Implemented by only adding items other than Double to the eligible key items 10% of the time
            if (!keyItems[INDEX_KEYITEM_DOUBLE])
                random_chance_to_make_eligible = rand() % 10;

            if (i == INDEX_KEYITEM_DOUBLE)
            {
                eligible_keyitems[number_eligible_keyitems] = i;
                number_eligible_keyitems++;
            }
            // Roc Wing
            else if (i == INDEX_KEYITEM_ROCWING)
            {
                if (random_chance_to_make_eligible == 9)
                {
                    // If Double, Kick Boots, Tackle, Heavy Ring, Cleansing, and ice/stone have been obtained, make eligible. Otherwise, 20% chance to make eligible.
                    if (keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && hasIceOrStone && keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING] && keyItems[INDEX_KEYITEM_CLEANSING])
                    {
                        random_chance_to_make_eligible = 4;
                    }
                    else
                    {
                        random_chance_to_make_eligible = rand() % 5;
                    }

                    if (random_chance_to_make_eligible == 4)
                    {
                        eligible_keyitems[number_eligible_keyitems] = i;
                        number_eligible_keyitems++;
                    }
                }
            }
            // All other key items
            else
            {
                if (random_chance_to_make_eligible == 9)
                {
                    eligible_keyitems[number_eligible_keyitems] = i;
                    number_eligible_keyitems++;
                }
            }
        }
        // Alternate case for Last Key to take into account optional patch for multiple Last Keys
        // Last Key (which requires Double and Kick Boots or Roc Wing to escape from the hidden room in Ceremonial Room)
        else if (keyItems[INDEX_KEYITEM_LASTKEY] < last_keys_count && (i == INDEX_KEYITEM_LASTKEY && ((keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS]) || keyItems[INDEX_KEYITEM_ROCWING])))
        {
            if (random_chance_to_make_eligible == 9)
            {
                // If Double, Kick Boots, and at least one other item or ice/stone have been obtained, make eligible. Otherwise, halve the chance.
                if (keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && (hasIceOrStone || keyItems[INDEX_KEYITEM_TACKLE] || keyItems[INDEX_KEYITEM_HEAVYRING] || keyItems[INDEX_KEYITEM_CLEANSING]))
                {
                    random_chance_to_make_eligible = 1;
                }
                else
                {
                    random_chance_to_make_eligible = rand() % 2;
                }

                if (random_chance_to_make_eligible)
                {
                    eligible_keyitems[number_eligible_keyitems] = i;
                    number_eligible_keyitems++;
                }
            }
        }
    }
        
    // Determine whether placing an ice or stone DSS combo is acceptable
    // Note: In order to place a DSS combo allowing ice or stone, at least two accessible pedestals must be unassigned
    // and a DSS combo allowing freezing or petrifying must not already have been placed.
    if (number_eligible_pedestals >= 2 && !hasIceOrStone)
    {
        eligible_keyitems[number_eligible_keyitems] = i;
        number_eligible_keyitems++;
    }

    // Pick one key item (or ice/stone combo) from the list of eligible indices
    random_eligible_keyitem_choice = rand() % number_eligible_keyitems;

    // Place ice/stone combo into two eligible pedestals if we hit that last index (which would be NUMBER_KEYITEMS)
    if (eligible_keyitems[random_eligible_keyitem_choice] == NUMBER_KEYITEMS)
    {
        // Find a random index within the list of eligible pedestals 10% of the time. Otherwise, prefer priority pedestals.
        random_chance_to_use_priority = rand() % 10;
        if (random_chance_to_use_priority == 9)
        {
            random_pedestal_choice = getRandomValidPedestal(reachable, item_assignment);
        }
        else
        {
            random_pedestal_choice = getRandomHighPriorityValidPedestal(reachable, item_assignment);
        }

        // Find a second random index that does not match the first index within the list of eligible pedestals
        do
        {
            random_pedestal_secondary_choice = getRandomValidPedestal(reachable, item_assignment);
        } while (random_pedestal_secondary_choice == random_pedestal_choice);

        // Place a random combination of two eligible cards:
        // Valid combinations that would allow you to petrify or freeze enemies reasonably:
        // Serpent + Mercury 
        // Serpent + Mars
        // Cockatrice + Mercury
        // Cockatrice + Mars

        // Assign either Mercury or Mars to the first pedestal
        random_action_card_choice = rand() % 2;
        if (random_action_card_choice == 0)
        {
            // Mercury
            item_assignment[random_pedestal_choice] = CONSTANT_DSS_VALUE + INDEX_DSSCARD_MERCURY;
            dssCards[INDEX_DSSCARD_MERCURY] = true;
        }
        else
        {
            // Mars
            item_assignment[random_pedestal_choice] = CONSTANT_DSS_VALUE + INDEX_DSSCARD_MARS;
            dssCards[INDEX_DSSCARD_MARS] = true;
        }

        // Assign either Serpent or Cockatrice to the second pedestal
        random_attribute_card_choice = rand() % 2;
        if (random_attribute_card_choice == 0)
        {
            // Serpent
            item_assignment[random_pedestal_secondary_choice] = CONSTANT_DSS_VALUE + INDEX_DSSCARD_SERPENT;
            dssCards[INDEX_DSSCARD_SERPENT] = true;
        }
        else
        {
            // Cockatrice
            item_assignment[random_pedestal_secondary_choice] = CONSTANT_DSS_VALUE + INDEX_DSSCARD_COCKATRICE;
            dssCards[INDEX_DSSCARD_COCKATRICE] = true;
        }

        // Enable flag confirming we can access items accessible by freezing or petrifying enemies
        hasIceOrStone = true;
    }
    // Otherwise, place the chosen key item
    else
    {
        // Find a random index within the list of eligible pedestals 20% of the time. Otherwise, prefer priority pedestals.
        random_chance_to_use_priority = rand() % 10;

        // Exception for when the item is a Last Key being placed in a multiple Last Key scenario, in which case we want the placement
        // to be truly random always
        if (random_chance_to_use_priority >= 8 || (eligible_keyitems[random_eligible_keyitem_choice] == INDEX_KEYITEM_LASTKEY && options->lastKeyAvailable > 1))
        {
            random_pedestal_choice = getRandomValidPedestal(reachable, item_assignment);
        }
        else
        {
            random_pedestal_choice = getRandomHighPriorityValidPedestal(reachable, item_assignment);
        }

        // Assign the pedestal to the chosen key item
        item_assignment[random_pedestal_choice] = eligible_keyitems[random_eligible_keyitem_choice];

        // Mark chosen key item as obtained (++ instead of = true to account for multiple Last Key case)
        keyItems[eligible_keyitems[random_eligible_keyitem_choice]]++;
    }

    return hasIceOrStone;
}

static void populateItems(int reachable[], int item_assignment[], bool dssCards[], struct seed_options *options)
{
    int i;
    int chosen_dss_pedestal;
    int random_max_up_index;

    int number_cards_placed = 0;
    int number_cards_to_place = options->halveDSSCards ? NUMBER_DSSCARDS / 2 : NUMBER_DSSCARDS;
    int chosen_card;

    // Count placed DSS cards (will be nonzero if we placed ice/stone, which is likely)
    for (i = 0; i < NUMBER_DSSCARDS; i++)
        if (dssCards[i])
            number_cards_placed++;

    // Distribute remaining DSS cards
    for (i = number_cards_placed; i < number_cards_to_place; i++)
    {
            chosen_dss_pedestal = getRandomValidPedestal(reachable, item_assignment);
            chosen_card = getRandomUnplacedDSSCard(dssCards);
            item_assignment[chosen_dss_pedestal] = CONSTANT_DSS_VALUE + chosen_card;
            dssCards[chosen_card] = true;
    }

    // Fill remaining unassigned pedestals with Max Ups
    for (i = 0; i < NUMBER_PEDESTALS; i++)
    {
        if (item_assignment[i] == UNASSIGNED)
        {
            // Obtain index for Heart Max, HP Max, or MP Max
            random_max_up_index = INDEX_MAXUP_HEARTMAXUP + (rand() % 3);
            item_assignment[i] = random_max_up_index;
        }
    }

    return;
}

static int getRandomValidPedestal(int reachable[], int item_assignment[])
{
    int eligible_pedestals[NUMBER_PEDESTALS];
    int number_eligible_pedestals = 0;

    int i;

    // Place index of each eligible pedestal into our list of eligible indices
    for (i = 0; i < NUMBER_PEDESTALS; i++)
    {
        if (reachable[i] != UNREACHABLE && item_assignment[i] == UNASSIGNED)
        {
            eligible_pedestals[number_eligible_pedestals] = i;
            number_eligible_pedestals++;
        }
    }

    return eligible_pedestals[rand() % number_eligible_pedestals];
}

static int getRandomHighPriorityValidPedestal(int reachable[], int item_assignment[])
{
    int eligible_pedestals[NUMBER_PEDESTALS];
    int number_high_priority_eligible_pedestals = 0;
    int highest_priority = 1;

    int i;

    for (i = 0; i < NUMBER_PEDESTALS; i++)
    {
        if (reachable[i] == highest_priority && item_assignment[i] == UNASSIGNED)
        {
            printf("High priority pedestal: %i\n", i);
            eligible_pedestals[number_high_priority_eligible_pedestals] = i;
            number_high_priority_eligible_pedestals++;
        }

        // If we have no available pedestals of current priority, lower priority and start over
        if (i == NUMBER_PEDESTALS - 1 && number_high_priority_eligible_pedestals == 0)
        {
            i = 0;
            highest_priority++;
            printf("Dropped down to lower priority: %i\n", highest_priority);
        }
    }

    return eligible_pedestals[rand() % number_high_priority_eligible_pedestals];
}

static int getRandomUnplacedDSSCard(bool dssCards[])
{
    int eligible_cards[NUMBER_DSSCARDS];
    int number_eligible_cards = 0;

    int i;

    for (i = 0; i < NUMBER_DSSCARDS; i++)
    {
        if (!dssCards[i])
        {
            eligible_cards[number_eligible_cards] = i;
            number_eligible_cards++;
        }
    }

    return eligible_cards[rand() % number_eligible_cards];
}

static void populateEnemyDrops(int regular_drops[], int rare_drops[], int regular_drop_chance[], int rare_drop_chance[], struct seed_options *options)
{
    /* Item arrays and tables revised by Malaert64: 
    low, mid, and high are for Tiered Item Mode (TIM) logic,
    while common and rare are used for normal (anything goes) logic. */
    int placed_low_items[NUMBER_LOW_ITEMS] = { 0 };
    int placed_mid_items[NUMBER_MID_ITEMS] = { 0 };
    int placed_high_items[NUMBER_HIGH_ITEMS] = { 0 };

    int placed_common_items[NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS] = { 0 };
    int placed_rare_items[NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS + NUMBER_HIGH_ITEMS] = { 0 };

    const int low_items[NUMBER_LOW_ITEMS] =
    {
        INDEX_ITEM_USE_POTION,
        INDEX_ITEM_USE_MEAT,
        INDEX_ITEM_USE_MINDRESTORE,
        INDEX_ITEM_USE_HEART,
        INDEX_ITEM_USE_ANTIDOTE,
        INDEX_ITEM_USE_CURECURSE,

        INDEX_ITEM_BODY_COTTONCLOTHES,
        INDEX_ITEM_BODY_PRISONGARB,
        INDEX_ITEM_BODY_COTTONROBE,
        INDEX_ITEM_BODY_LEATHERARMOR,
        INDEX_ITEM_BODY_BRONZEARMOR,
        INDEX_ITEM_BODY_GOLDARMOR,

        INDEX_ITEM_ARM_TOYRING,
        INDEX_ITEM_ARM_BEARRING,
        INDEX_ITEM_ARM_WRISTBAND,
        INDEX_ITEM_ARM_ARMGUARD,
        INDEX_ITEM_ARM_MAGICGAUNTLET,
        INDEX_ITEM_ARM_MIRACLEARMBAND,
        INDEX_ITEM_ARM_GAUNTLET
    };

    const int mid_items[NUMBER_MID_ITEMS] = 
    {
        INDEX_ITEM_USE_SPICEDMEAT,
        INDEX_ITEM_USE_MINDHIGH,
        INDEX_ITEM_USE_HEARTHIGH,

        INDEX_ITEM_BODY_STYLISHSUIT,
        INDEX_ITEM_BODY_NIGHTSUIT,
        INDEX_ITEM_BODY_SILKROBE,
        INDEX_ITEM_BODY_RAINBOWROBE,
        INDEX_ITEM_BODY_CHAINMAIL,
        INDEX_ITEM_BODY_STEELARMOR,
        INDEX_ITEM_BODY_PLATINUMARMOR,

        INDEX_ITEM_ARM_STARBRACELET,
        INDEX_ITEM_ARM_CURSEDRING,
        INDEX_ITEM_ARM_STRENGTHRING,
        INDEX_ITEM_ARM_HARDRING,
        INDEX_ITEM_ARM_INTELLIGENCERING,
        INDEX_ITEM_ARM_LUCKRING,
        INDEX_ITEM_ARM_DOUBLEGRIPS
    };

    const int high_items[NUMBER_HIGH_ITEMS] =
    {
        INDEX_ITEM_USE_POTIONHIGH,
        INDEX_ITEM_USE_POTIONEX,
        INDEX_ITEM_USE_MINDEX,
        INDEX_ITEM_USE_HEARTEX,
        INDEX_ITEM_USE_HEARTMEGA,

        INDEX_ITEM_BODY_NINJAGARB,
        INDEX_ITEM_BODY_SOLDIERFATIGUES,
        INDEX_ITEM_BODY_MAGICROBE,
        INDEX_ITEM_BODY_SAGEROBE,
        INDEX_ITEM_BODY_DIAMONDARMOR,
        INDEX_ITEM_BODY_MIRRORARMOR,
        INDEX_ITEM_BODY_NEEDLEARMOR,
        INDEX_ITEM_BODY_DARKARMOR,

        INDEX_ITEM_ARM_STRENGTHARMBAND,
        INDEX_ITEM_ARM_DEFENSEARMBAND,
        INDEX_ITEM_ARM_SAGEARMBAND,
        INDEX_ITEM_ARM_GAMBLERARMBAND        
    };

    const int common_items[NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS] = 
    {
        /* Low Tier Items */
        INDEX_ITEM_USE_POTION,
        INDEX_ITEM_USE_MEAT,
        INDEX_ITEM_USE_MINDRESTORE,
        INDEX_ITEM_USE_HEART,
        INDEX_ITEM_USE_ANTIDOTE,
        INDEX_ITEM_USE_CURECURSE,

        INDEX_ITEM_BODY_COTTONCLOTHES,
        INDEX_ITEM_BODY_PRISONGARB,
        INDEX_ITEM_BODY_COTTONROBE,
        INDEX_ITEM_BODY_LEATHERARMOR,
        INDEX_ITEM_BODY_BRONZEARMOR,
        INDEX_ITEM_BODY_GOLDARMOR,

        INDEX_ITEM_ARM_TOYRING,
        INDEX_ITEM_ARM_BEARRING,
        INDEX_ITEM_ARM_WRISTBAND,
        INDEX_ITEM_ARM_ARMGUARD,
        INDEX_ITEM_ARM_MAGICGAUNTLET,
        INDEX_ITEM_ARM_MIRACLEARMBAND,
        INDEX_ITEM_ARM_GAUNTLET,

        /* Mid Tier Items */
        INDEX_ITEM_USE_SPICEDMEAT,
        INDEX_ITEM_USE_MINDHIGH,
        INDEX_ITEM_USE_HEARTHIGH,

        INDEX_ITEM_BODY_STYLISHSUIT,
        INDEX_ITEM_BODY_NIGHTSUIT,
        INDEX_ITEM_BODY_SILKROBE,
        INDEX_ITEM_BODY_RAINBOWROBE,
        INDEX_ITEM_BODY_CHAINMAIL,
        INDEX_ITEM_BODY_STEELARMOR,
        INDEX_ITEM_BODY_PLATINUMARMOR,

        INDEX_ITEM_ARM_STARBRACELET,
        INDEX_ITEM_ARM_CURSEDRING,
        INDEX_ITEM_ARM_STRENGTHRING,
        INDEX_ITEM_ARM_HARDRING,
        INDEX_ITEM_ARM_INTELLIGENCERING,
        INDEX_ITEM_ARM_LUCKRING,
        INDEX_ITEM_ARM_DOUBLEGRIPS
    };

    const int rare_items[NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS + NUMBER_HIGH_ITEMS] =
    {
        /* Low Tier Items */
        INDEX_ITEM_USE_POTION,
        INDEX_ITEM_USE_MEAT,
        INDEX_ITEM_USE_MINDRESTORE,
        INDEX_ITEM_USE_HEART,
        INDEX_ITEM_USE_ANTIDOTE,
        INDEX_ITEM_USE_CURECURSE,

        INDEX_ITEM_BODY_COTTONCLOTHES,
        INDEX_ITEM_BODY_PRISONGARB,
        INDEX_ITEM_BODY_COTTONROBE,
        INDEX_ITEM_BODY_LEATHERARMOR,
        INDEX_ITEM_BODY_BRONZEARMOR,
        INDEX_ITEM_BODY_GOLDARMOR,

        INDEX_ITEM_ARM_TOYRING,
        INDEX_ITEM_ARM_BEARRING,
        INDEX_ITEM_ARM_WRISTBAND,
        INDEX_ITEM_ARM_ARMGUARD,
        INDEX_ITEM_ARM_MAGICGAUNTLET,
        INDEX_ITEM_ARM_MIRACLEARMBAND,
        INDEX_ITEM_ARM_GAUNTLET,

        /* Mid Tier Items */
        INDEX_ITEM_USE_SPICEDMEAT,
        INDEX_ITEM_USE_MINDHIGH,
        INDEX_ITEM_USE_HEARTHIGH,

        INDEX_ITEM_BODY_STYLISHSUIT,
        INDEX_ITEM_BODY_NIGHTSUIT,
        INDEX_ITEM_BODY_SILKROBE,
        INDEX_ITEM_BODY_RAINBOWROBE,
        INDEX_ITEM_BODY_CHAINMAIL,
        INDEX_ITEM_BODY_STEELARMOR,
        INDEX_ITEM_BODY_PLATINUMARMOR,

        INDEX_ITEM_ARM_STARBRACELET,
        INDEX_ITEM_ARM_CURSEDRING,
        INDEX_ITEM_ARM_STRENGTHRING,
        INDEX_ITEM_ARM_HARDRING,
        INDEX_ITEM_ARM_INTELLIGENCERING,
        INDEX_ITEM_ARM_LUCKRING,
        INDEX_ITEM_ARM_DOUBLEGRIPS,

        /* High Tier Items */
        INDEX_ITEM_USE_POTIONHIGH,
        INDEX_ITEM_USE_POTIONEX,
        INDEX_ITEM_USE_MINDEX,
        INDEX_ITEM_USE_HEARTEX,
        INDEX_ITEM_USE_HEARTMEGA,

        INDEX_ITEM_BODY_NINJAGARB,
        INDEX_ITEM_BODY_SOLDIERFATIGUES,
        INDEX_ITEM_BODY_MAGICROBE,
        INDEX_ITEM_BODY_SAGEROBE,
        INDEX_ITEM_BODY_DIAMONDARMOR,
        INDEX_ITEM_BODY_MIRRORARMOR,
        INDEX_ITEM_BODY_NEEDLEARMOR,
        INDEX_ITEM_BODY_DARKARMOR,

        INDEX_ITEM_ARM_STRENGTHARMBAND,
        INDEX_ITEM_ARM_DEFENSEARMBAND,
        INDEX_ITEM_ARM_SAGEARMBAND,
        INDEX_ITEM_ARM_GAMBLERARMBAND
    };
    
    int i;

    /* ADDED IN 1.3: Set boss and candle items first to prevent boss drop duplicates. */

    /* I had to make this logic worse unfortunately, given I have 
    different item placement arrays for normal and TIM logic. - Malaert64 */

    if (options->tieredItemsMode) {
        /* Note: If item hard mode is enabled, make boss drops exclusive. */
        regular_drops[INDEX_ENEMY_CERBERUS] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_NECROMANCER] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_IRONGOLEM] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_ADRAMELECH] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_ZOMBIEDRAGON] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_DEATH] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_CAMILLA] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_HUGH] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
        regular_drops[INDEX_ENEMY_DRACULAI] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, true);
    } else {
        regular_drops[INDEX_ENEMY_CERBERUS] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_NECROMANCER] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_IRONGOLEM] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_ADRAMELECH] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_ZOMBIEDRAGON] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_DEATH] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_CAMILLA] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_HUGH] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
        regular_drops[INDEX_ENEMY_DRACULAI] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
    }

    /* Setting drop logic for all enemies. */
    for (i = 0; i < NUMBER_ENEMIES; i++)
    {
        /* Give Dracula II Shinning Armor occasionally as a joke. */
        if (i == INDEX_ENEMY_DRACULAII) {
            regular_drops[i] = rare_drops[i] = INDEX_ITEM_BODY_SHINNINGARMOR;
            regular_drop_chance[i] = rare_drop_chance[i] = 5000;
        }  

        /* Set bosses' secondary item to none since 
        we already set the primary item earlier. */
        else if (i == INDEX_ENEMY_CERBERUS || i == INDEX_ENEMY_NECROMANCER || 
                 i == INDEX_ENEMY_IRONGOLEM || i == INDEX_ENEMY_ADRAMELECH || 
                 i == INDEX_ENEMY_ZOMBIEDRAGON || i == INDEX_ENEMY_DEATH || 
                 i == INDEX_ENEMY_CAMILLA || i == INDEX_ENEMY_HUGH || 
                 i == INDEX_ENEMY_DRACULAI) {

            /* Set rare drop to none. */
            rare_drops[i] = 0;

            /* Max out rare boss drops (normally, drops are capped to 
            50% and 25% for common and rare respectively, but Fuse's 
            patch AllowAlwaysDrop.ips allows setting the regular item 
            drop chance to 10000 to force a drop always). */
            regular_drop_chance[i] = 10000;
            rare_drop_chance[i] = 0;
        }

        /* Candle enemies are special cases, and I've restructured their 
        logic to fit with the new system of placement arrays. -Malaert64 */
        else if (i == INDEX_ENEMY_SCARYCANDLE || i == INDEX_ENEMY_TRICKCANDLE || i == INDEX_ENEMY_MIMICCANDLE) {
            if (options->tieredItemsMode){
                regular_drops[i] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, false);
                rare_drops[i] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, false);
            } else {
                regular_drops[i] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
                rare_drops[i] = selectDrop(&rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], &placed_rare_items[(NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS)], NUMBER_HIGH_ITEMS, false);
            }

            /* Set base drop chances at 20-30% for common and 15-20% for rare. */
            regular_drop_chance[i] = 2000 + (rand() % 1001);
            rare_drop_chance[i] = 1500 + (rand() % 501);
        }

        /* On All Bosses and Battle Arena Required, the Shinning Armor at 
        the end of Battle Arena is removed. We compensate for this by giving 
        Battle Arena Devil a 100% chance to drop Shinning Armor. */
        else if (i == INDEX_ENEMY_BATTLEARENADEVIL && options->allBossesAndBattleArenaRequired) {
            regular_drops[i] = INDEX_ITEM_BODY_SHINNINGARMOR;
            rare_drops[i] = 0;

            regular_drop_chance[i] = 10000;
            rare_drop_chance[i] = 0;
        }

        /* Low-tier items drop from enemies that are trivial to farm (60 HP 
        or less) on normal drop logic, or enemies under 144 HP on TIM logic. */
        
        /* Logic revised by Malaert64 to remove 
        massive if block checking enemy IDs. */
        else if ((!options->tieredItemsMode && enemies_data_table[i].hp <= 60) || (options->tieredItemsMode && enemies_data_table[i].hp <= 143)) {
            /* Low-tier enemy drops. */
            regular_drops[i] = selectDrop(low_items, placed_low_items, NUMBER_LOW_ITEMS, false);
            rare_drops[i] = selectDrop(low_items, placed_low_items, NUMBER_LOW_ITEMS, false);
            
            /* Set base drop chances at 6-10% for common and 3-6% for rare. */
            regular_drop_chance[i] =  600 + (rand() % 401);
            rare_drop_chance[i] = 300 + (rand() % 301);
        }

        /* Rest of TIM Logic, by Malaert64. */
        else if (options->tieredItemsMode) {
            /* If under 370 HP, mid-tier enemy. */
            if (enemies_data_table[i].hp <= 369) {
                regular_drops[i] = selectDrop(low_items, placed_low_items, NUMBER_LOW_ITEMS, false);
                rare_drops[i] = selectDrop(mid_items, placed_mid_items, NUMBER_MID_ITEMS, false);
            /* Otherwise, enemy HP is 370+, thus high-tier enemy. */
            } else {
                regular_drops[i] = selectDrop(mid_items, placed_mid_items, NUMBER_MID_ITEMS, false);
                rare_drops[i] = selectDrop(high_items, placed_high_items, NUMBER_HIGH_ITEMS, false);
            }
            
            /* Set base drop chances at 6-10% for common and 3-6% for rare. */
            regular_drop_chance[i] =  600 + (rand() % 401);
            rare_drop_chance[i] = 300 + (rand() % 301);
        }

        /* Regular enemies outside of TIM. */
        else {
            /* Select a random regular and rare drop for 
            every enemy from their respective lists */
            regular_drops[i] = selectDrop(common_items, placed_common_items, (NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS), false);
            rare_drops[i] = selectDrop(rare_items, placed_rare_items, (NUMBER_LOW_ITEMS + NUMBER_MID_ITEMS + NUMBER_HIGH_ITEMS), false);

            /* Set base drop chances at 6-10% for common and 3-6% for rare. */
            regular_drop_chance[i] = 600 + (rand() % 401);
            rare_drop_chance[i] = 300 + (rand() % 301);
        }
    }
}

static int selectDrop(const int dropList[], int dropsPlaced[], int number_drops, bool exclusiveDrop) {
    int lowest_number;
    int number_valid_drops = 0;
    int i;

    int random_result;

    int eligible_items[NUMBER_ITEMS];

    lowest_number = dropsPlaced[0];
    /* Only make eligible drops which we have placed the least. */
    for (i = 0; i < number_drops; i++)
    {
        /* A drop with the priority we are expecting 
        is available to add as a candidate. */
        if (dropsPlaced[i] == lowest_number)
        {
            eligible_items[number_valid_drops] = i;
            number_valid_drops++;
        }

        /* If this criteria is met, there is at least one item 
        that hasn't been placed as many times as the others.
        We have to lower the lowest number and start from the
        beginning of the loop to capture all the valid indices. */
        else if (dropsPlaced[i] < lowest_number)
        {
            lowest_number = dropsPlaced[i];
            number_valid_drops = i = 0;
        }
    }
    /* Postcondition: Our array eligible_items has number_valid_drops 
    many valid item indices as its elements. */

    /* Select a random valid item from the index of valid choices. */
    random_result = rand() % number_valid_drops;

    /* Increment the number of this item placed, unless it should 
    be exclusive to the boss/candle, in which case set it to an 
    arbitrarily large number to make it exclusive. */
    if (exclusiveDrop) {
        dropsPlaced[eligible_items[random_result]] += 999;
    } else {
        dropsPlaced[eligible_items[random_result]]++;
    }

    /* Return the item ID. */
    return dropList[eligible_items[random_result]];
}

static void writeIPS(FILE* randomizer_patch, int item_assignment[], int regular_drops[], int rare_drops[], int regular_drop_chance[], int rare_drop_chance[], struct seed_options *options, FILE* spoiler_log)
{
    char* beginning_string = "PATCH";
    char* ending_string = "EOF";

    char address_byte;
    char size_byte[2];

    static unsigned int itemAddresses[] =
    {
        ADDRESS_SEALED_ROOM3,
        ADDRESS_CATACOMB1,
        ADDRESS_CATACOMB3,
        ADDRESS_CATACOMB3B,
        ADDRESS_CATACOMB4,
        ADDRESS_CATACOMB5,
        ADDRESS_CATACOMB8,
        ADDRESS_CATACOMB8B,
        ADDRESS_CATACOMB9,
        ADDRESS_CATACOMB10,
        ADDRESS_CATACOMB13,
        ADDRESS_CATACOMB14,
        ADDRESS_CATACOMB14B,
        ADDRESS_CATACOMB16,
        ADDRESS_CATACOMB20,
        ADDRESS_CATACOMB22,
        ADDRESS_CATACOMB24,
        ADDRESS_CATACOMB25,
        ADDRESS_ABYSS_STAIRWAY2,
        ADDRESS_ABYSS_STAIRWAY3,
        ADDRESS_ABYSS_STAIRWAY4,
        ADDRESS_ABYSS_STAIRWAY9,
        ADDRESS_AUDIENCE_ROOM4,
        ADDRESS_AUDIENCE_ROOM7,
        ADDRESS_AUDIENCE_ROOM8,
        ADDRESS_AUDIENCE_ROOM9,
        ADDRESS_AUDIENCE_ROOM10,
        ADDRESS_AUDIENCE_ROOM11,
        ADDRESS_AUDIENCE_ROOM14,
        ADDRESS_AUDIENCE_ROOM14B,
        ADDRESS_AUDIENCE_ROOM16,
        ADDRESS_AUDIENCE_ROOM17,
        ADDRESS_AUDIENCE_ROOM17B,
        ADDRESS_AUDIENCE_ROOM18,
        ADDRESS_AUDIENCE_ROOM19,
        ADDRESS_AUDIENCE_ROOM21,
        ADDRESS_AUDIENCE_ROOM25,
        ADDRESS_AUDIENCE_ROOM26,
        ADDRESS_AUDIENCE_ROOM27,
        ADDRESS_AUDIENCE_ROOM30,
        ADDRESS_AUDIENCE_ROOM30B,
        ADDRESS_OUTER_WALL0,
        ADDRESS_OUTER_WALL1,
        ADDRESS_OUTER_WALL2,
        ADDRESS_TRIUMPH_HALLWAY1,
        ADDRESS_TRIUMPH_HALLWAY3,
        ADDRESS_MACHINE_TOWER0,
        ADDRESS_MACHINE_TOWER2,
        ADDRESS_MACHINE_TOWER3,
        ADDRESS_MACHINE_TOWER4,
        ADDRESS_MACHINE_TOWER6,
        ADDRESS_MACHINE_TOWER8,
        ADDRESS_MACHINE_TOWER10,
        ADDRESS_MACHINE_TOWER11,
        ADDRESS_MACHINE_TOWER13,
        ADDRESS_MACHINE_TOWER14,
        ADDRESS_MACHINE_TOWER17,
        ADDRESS_MACHINE_TOWER19,
        ADDRESS_ETERNAL_CORRIDOR5,
        ADDRESS_ETERNAL_CORRIDOR7,
        ADDRESS_ETERNAL_CORRIDOR9,
        ADDRESS_CHAPEL_TOWER1,
        ADDRESS_CHAPEL_TOWER4,
        ADDRESS_CHAPEL_TOWER5,
        ADDRESS_CHAPEL_TOWER6,
        ADDRESS_CHAPEL_TOWER6B,
        ADDRESS_CHAPEL_TOWER8,
        ADDRESS_CHAPEL_TOWER10,
        ADDRESS_CHAPEL_TOWER13,
        ADDRESS_CHAPEL_TOWER15,
        ADDRESS_CHAPEL_TOWER16,
        ADDRESS_CHAPEL_TOWER18,
        ADDRESS_CHAPEL_TOWER22,
        ADDRESS_CHAPEL_TOWER26,
        ADDRESS_CHAPEL_TOWER26B,
        ADDRESS_UNDERGROUND_GALLERY0,
        ADDRESS_UNDERGROUND_GALLERY1,
        ADDRESS_UNDERGROUND_GALLERY2,
        ADDRESS_UNDERGROUND_GALLERY3,
        ADDRESS_UNDERGROUND_GALLERY3B,
        ADDRESS_UNDERGROUND_GALLERY8,
        ADDRESS_UNDERGROUND_GALLERY10,
        ADDRESS_UNDERGROUND_GALLERY13,
        ADDRESS_UNDERGROUND_GALLERY15,
        ADDRESS_UNDERGROUND_GALLERY20,
        ADDRESS_UNDERGROUND_WAREHOUSE1,
        ADDRESS_UNDERGROUND_WAREHOUSE6,
        ADDRESS_UNDERGROUND_WAREHOUSE8,
        ADDRESS_UNDERGROUND_WAREHOUSE9,
        ADDRESS_UNDERGROUND_WAREHOUSE10,
        ADDRESS_UNDERGROUND_WAREHOUSE11,
        ADDRESS_UNDERGROUND_WAREHOUSE14,
        ADDRESS_UNDERGROUND_WAREHOUSE16,
        ADDRESS_UNDERGROUND_WAREHOUSE16B,
        ADDRESS_UNDERGROUND_WAREHOUSE19,
        ADDRESS_UNDERGROUND_WAREHOUSE23,
        ADDRESS_UNDERGROUND_WAREHOUSE24,
        ADDRESS_UNDERGROUND_WAREHOUSE25,
        ADDRESS_UNDERGROUND_WATERWAY1,
        ADDRESS_UNDERGROUND_WATERWAY3,
        ADDRESS_UNDERGROUND_WATERWAY3B,
        ADDRESS_UNDERGROUND_WATERWAY4,
        ADDRESS_UNDERGROUND_WATERWAY5,
        ADDRESS_UNDERGROUND_WATERWAY7,
        ADDRESS_UNDERGROUND_WATERWAY8,
        ADDRESS_UNDERGROUND_WATERWAY9,
        ADDRESS_UNDERGROUND_WATERWAY9B,
        ADDRESS_UNDERGROUND_WATERWAY12,
        ADDRESS_UNDERGROUND_WATERWAY12B,
        ADDRESS_UNDERGROUND_WATERWAY13,
        ADDRESS_UNDERGROUND_WATERWAY17,
        ADDRESS_UNDERGROUND_WATERWAY18,
        ADDRESS_OBSERVATION_TOWER1,
        ADDRESS_OBSERVATION_TOWER2,
        ADDRESS_OBSERVATION_TOWER3,
        ADDRESS_OBSERVATION_TOWER5,
        ADDRESS_OBSERVATION_TOWER8,
        ADDRESS_OBSERVATION_TOWER9,
        ADDRESS_OBSERVATION_TOWER12,
        ADDRESS_OBSERVATION_TOWER13,
        ADDRESS_OBSERVATION_TOWER16,
        ADDRESS_OBSERVATION_TOWER20,
        ADDRESS_CEREMONIAL_ROOM1,
        ADDRESS_BATTLEARENA24
    };

    static unsigned char itemIDs[][ITEMID_WIDTH] =
    {
        KEYITEM_DASHBOOTS,
        KEYITEM_DOUBLE,
        KEYITEM_TACKLE,
        KEYITEM_KICKBOOTS,
        KEYITEM_HEAVYRING,
        KEYITEM_CLEANSING,
        KEYITEM_ROCWING,
        KEYITEM_LASTKEY,
        KEYITEM_MAP,
        MAXUP_HEARTMAXUP,
        MAXUP_HPMAXUP,
        MAXUP_MPMAXUP,
        SPECIALITEM_SHINNINGARMOR
    };

    static unsigned char dssCardIDs[][ITEMID_WIDTH] =
    {
        DSSCARD_SALAMANDER,
        DSSCARD_SERPENT,
        DSSCARD_MANDRAGORA,
        DSSCARD_GOLEM,
        DSSCARD_COCKATRICE,
        DSSCARD_MANTICORE,
        DSSCARD_GRIFFIN,
        DSSCARD_THUNDERBIRD,
        DSSCARD_UNICORN,
        DSSCARD_BLACKDOG,
        DSSCARD_MERCURY,
        DSSCARD_VENUS,
        DSSCARD_JUPITER,
        DSSCARD_MARS,
        DSSCARD_DIANA,
        DSSCARD_APOLLO,
        DSSCARD_NEPTUNE,
        DSSCARD_SATURN,
        DSSCARD_URANUS,
        DSSCARD_PLUTO
    };

    unsigned int enemy_address;
    unsigned int drop_byte;

    int i;

    // Write "PATCH" at beginning of file (subtract one from size; not null-terminated)
    fwrite(beginning_string, 1, 5, randomizer_patch);

    // Write all item changes
    for (i = 0; i < NUMBER_PEDESTALS; i++)
    {
        // Construct and write three address bytes
        address_byte = (itemAddresses[i] & 0x00FF0000) >> 16;
        fwrite(&address_byte, 1, 1, randomizer_patch);
        address_byte = (itemAddresses[i] & 0x0000FF00) >> 8;
        fwrite(&address_byte, 1, 1, randomizer_patch);
        address_byte = (itemAddresses[i] & 0x000000FF);
        fwrite(&address_byte, 1, 1, randomizer_patch);

        // Write size byte (item IDs are four bytes)
        size_byte[0] = 0;
        size_byte[1] = ITEMID_WIDTH;
        fwrite(&size_byte, 2, 1, randomizer_patch);
        
        if (item_assignment[i] < CONSTANT_DSS_VALUE)
        {
            // Item
            fwrite(itemIDs[item_assignment[i]], ITEMID_WIDTH, 1, randomizer_patch);
            writePedestal(spoiler_log, i);
            writePedestalItem(spoiler_log, item_assignment[i]);
        }
        else
        {
            // DSS Card
            fwrite(dssCardIDs[item_assignment[i] - CONSTANT_DSS_VALUE], ITEMID_WIDTH, 1, randomizer_patch);
            writePedestal(spoiler_log, i);
            writeDSSCard(spoiler_log, item_assignment[i] - CONSTANT_DSS_VALUE);
        }
    }

    // Write all drop changes
    if (!options->doNotRandomizeItems)
    {
        enemy_address = ADDRESS_ENEMY_DATA_TABLE + OFFSET_TO_ENEMYDROPS;
        for (i = 0; i < NUMBER_ENEMIES; i++)
        {
            // Address section
            address_byte = (enemy_address & 0x00FF0000) >> 16;
            fwrite(&address_byte, 1, 1, randomizer_patch);
            address_byte = (enemy_address & 0x0000FF00) >> 8;
            fwrite(&address_byte, 1, 1, randomizer_patch);
            address_byte = (enemy_address & 0x000000FF);
            fwrite(&address_byte, 1, 1, randomizer_patch);

            // Write size byte (drop IDs and chances are both two bytes, and there are four entries)
            size_byte[0] = 0;
            size_byte[1] = DROP_WIDTH * 4;
            fwrite(&size_byte, 2, 1, randomizer_patch);

            // Write in order: regular item and its chance and rare item and its chance
            writeEnemy(spoiler_log, i);
            fprintf(spoiler_log, "Regular: ");
            writeDropItem(spoiler_log, regular_drops[i]);
            fprintf(spoiler_log, "Reg.chance: %i\n", regular_drop_chance[i]);
            fprintf(spoiler_log, "Rare: ");
            writeDropItem(spoiler_log, rare_drops[i]);
            fprintf(spoiler_log, "Ra. chance: %i\n", rare_drop_chance[i]);

            drop_byte = (regular_drops[i] & 0x000000FF);
            fwrite(&drop_byte, 1, 1, randomizer_patch);
            drop_byte = (regular_drops[i] & 0x0000FF00) >> 8;
            fwrite(&drop_byte, 1, 1, randomizer_patch);

            drop_byte = (regular_drop_chance[i] & 0x000000FF);
            fwrite(&drop_byte, 1, 1, randomizer_patch);
            drop_byte = (regular_drop_chance[i] & 0x0000FF00) >> 8;
            fwrite(&drop_byte, 1, 1, randomizer_patch);

            drop_byte = (rare_drops[i] & 0x000000FF);
            fwrite(&drop_byte, 1, 1, randomizer_patch);
            drop_byte = (rare_drops[i] & 0x0000FF00) >> 8;
            fwrite(&drop_byte, 1, 1, randomizer_patch);

            drop_byte = (rare_drop_chance[i] & 0x000000FF);
            fwrite(&drop_byte, 1, 1, randomizer_patch);
            drop_byte = (rare_drop_chance[i] & 0x0000FF00) >> 8;
            fwrite(&drop_byte, 1, 1, randomizer_patch);

            // Advance to the next enemy's drop section
            enemy_address += CONSTANT_ENEMY_DATA_LENGTH;
        }
    }
    // Write "EOF" at end of file
    fwrite(ending_string, 1, 3, randomizer_patch);

    return;
}
