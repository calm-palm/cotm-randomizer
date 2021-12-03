#include <stdlib.h>
#include "randomizer.h"
#include "addresses.h"
#include "spoilertranslator.h"

// Internal functions
static void findAllItemsReachable(int reachable[], int keyItems[], bool hasIceOrStone, bool ignoreCleansing, bool ironMaiden, int last_keys_count);
static bool populateKeyItem(int reachable[], int item_assignment[], int keyItems[], bool dssCards[], bool hasIceOrStone, int last_keys_count, struct seed_options *options);
static void populateItems(int reachable[], int item_assignment[], bool dssCards[], struct seed_options *options);
static int getRandomValidPedestal(int reachable[], int item_assignment[]);
static int getRandomHighPriorityValidPedestal(int reachable[], int item_assignment[]);

static int getRandomUnplacedDSSCard(bool dssCards[]);

static void populateEnemyDrops(int regular_drops[], int rare_drops[], int regular_drop_chance[], int rare_drop_chance[], struct seed_options *options);
static int getPlacedIndexFromID(int itemID, int dropList[], int dropsPlaced[], int number_drops);
static int selectDrop(int dropList[], int dropsPlaced[], int number_drops);

static void writeIPS(FILE* randomizer_patch, int item_assignment[], int regular_drops[], int rare_drops[], int regular_drop_chance[], int rare_drop_chance[], struct seed_options *options, FILE* spoiler_log);

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

    // Also assume that we assign Shinning Armor to the Battle Arena pedestal
    item_assignment[INDEX_BATTLEARENA24] = INDEX_SPECIALITEM_SHINNINGARMOR;

    // If the Last Key optional patch was enabled and keys required set to zero, count the key as already having been obtained

    printf("Initialized arrays and initial items.\n");

    // Iterate for each key item placement
    while (!keyItems[INDEX_KEYITEM_DASHBOOTS] || !keyItems[INDEX_KEYITEM_DOUBLE] || !keyItems[INDEX_KEYITEM_KICKBOOTS] || !keyItems[INDEX_KEYITEM_HEAVYRING] ||
           !keyItems[INDEX_KEYITEM_CLEANSING] || !keyItems[INDEX_KEYITEM_ROCWING] || (keyItems[INDEX_KEYITEM_LASTKEY] < last_keys_count) || !hasIceOrStone)
    {
        // Mark all rooms reachable that we can reach with current key items
        findAllItemsReachable(reachable, keyItems, hasIceOrStone, options->ignoreCleansing, ironMaiden, last_keys_count);

        // Select a random room from those rooms that are now reachable that does not contain a key item to place a key item
        // or a DSS card combo allowing ice or stone
        hasIceOrStone = populateKeyItem(reachable, item_assignment, keyItems, dssCards, hasIceOrStone, last_keys_count, options);
    }

    // Assert that all rooms are now reachable (minus Battle Arena, which we disregard)
    findAllItemsReachable(reachable, keyItems, hasIceOrStone, options->ignoreCleansing, ironMaiden, last_keys_count);
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

static void findAllItemsReachable(int reachable[], int keyItems[], bool hasIceOrStone, bool ignoreCleansing, bool ironMaiden, int last_keys_count)
{
    // Assume switch is in original location accessible with Double and Kick Boots and set to true if it is false and those are obtained
    // unless the optional rule breaking them without pressing the button has been enabled
    if (!ironMaiden)
    {
        ironMaiden = keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS];
    }
    bool rocOrDouble = keyItems[INDEX_KEYITEM_DOUBLE] || keyItems[INDEX_KEYITEM_ROCWING];
    bool rocOrKickBoots = keyItems[INDEX_KEYITEM_KICKBOOTS] || keyItems[INDEX_KEYITEM_ROCWING];

    // No requirements
    reachable[INDEX_CATACOMB4]++;
    reachable[INDEX_CATACOMB8]++;
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
        reachable[INDEX_CATACOMB8B]++;
        reachable[INDEX_CATACOMB9]++;
    }

    // Heavy Ring
    if (keyItems[INDEX_KEYITEM_HEAVYRING])
    {
        reachable[INDEX_CATACOMB1]++;
    }

    // Roc Wing OR Double, Kick Boots, and Ice/Stone
    if (keyItems[INDEX_KEYITEM_ROCWING] || (keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && hasIceOrStone))
    {
        reachable[INDEX_TRIUMPH_HALLWAY3]++;
        reachable[INDEX_CHAPEL_TOWER1]++;
    }

    // Roc Wing
    if (keyItems[INDEX_KEYITEM_ROCWING])
    {
        reachable[INDEX_SEALED_ROOM3]++;
        reachable[INDEX_ABYSS_STAIRWAY4]++;
        reachable[INDEX_AUDIENCE_ROOM14B]++;
        reachable[INDEX_AUDIENCE_ROOM30]++;
        reachable[INDEX_AUDIENCE_ROOM30B]++;
        reachable[INDEX_OUTER_WALL0]++;

        reachable[INDEX_CHAPEL_TOWER26]++;
        reachable[INDEX_CHAPEL_TOWER26B]++;
        reachable[INDEX_UNDERGROUND_WATERWAY13]++;
        reachable[INDEX_UNDERGROUND_WATERWAY18]++;
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
        reachable[INDEX_BATTLEARENA24]++;
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
        reachable[INDEX_AUDIENCE_ROOM17B]++;
        reachable[INDEX_AUDIENCE_ROOM19]++;
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
        reachable[INDEX_CATACOMB14B]++;
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
        reachable[INDEX_AUDIENCE_ROOM26]++;
    }

    // Iron Maiden and Roc Wing OR Double, Kick Boots, Iron Maiden, and Ice/Stone
    if ((ironMaiden && keyItems[INDEX_KEYITEM_ROCWING]) || (ironMaiden && keyItems[INDEX_KEYITEM_DOUBLE] && keyItems[INDEX_KEYITEM_KICKBOOTS] && hasIceOrStone))
    {
        reachable[INDEX_UNDERGROUND_GALLERY3]++;
        reachable[INDEX_UNDERGROUND_GALLERY3B]++;
    }

    // Iron Maiden, Roc Wing, Cleansing
    if (ironMaiden && keyItems[INDEX_KEYITEM_ROCWING] && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing))
    {
            reachable[INDEX_UNDERGROUND_WATERWAY8]++;
    }
    
    // Tackle, Heavy Ring, and Roc Wing
    if (keyItems[INDEX_KEYITEM_TACKLE] && keyItems[INDEX_KEYITEM_HEAVYRING] && keyItems[INDEX_KEYITEM_ROCWING])
    {
        reachable[INDEX_UNDERGROUND_WAREHOUSE10]++;
        reachable[INDEX_UNDERGROUND_WAREHOUSE16B]++;
    }

    // Double, Tackle, and Kick Boots
    if (rocOrDouble && keyItems[INDEX_KEYITEM_TACKLE] && rocOrKickBoots)
    {
        reachable[INDEX_MACHINE_TOWER3]++;
        reachable[INDEX_MACHINE_TOWER6]++;
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

    // Double, NOT Kick Boots (can make it whout Double), Heavy Ring, and Tackle
    if (rocOrDouble && keyItems[INDEX_KEYITEM_HEAVYRING] && keyItems[INDEX_KEYITEM_TACKLE])
    {
        reachable[INDEX_UNDERGROUND_WAREHOUSE14]++;
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
        reachable[INDEX_CEREMONIAL_ROOM1]++;
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
        reachable[INDEX_CHAPEL_TOWER13]++;
    }

    // Double, Ice/Stone, and Iron Maiden OR Roc Wing and Iron Maiden
    if ((rocOrDouble && hasIceOrStone && ironMaiden && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing)) || (keyItems[INDEX_KEYITEM_ROCWING] && ironMaiden && (keyItems[INDEX_KEYITEM_CLEANSING] || ignoreCleansing)))
    {
        reachable[INDEX_UNDERGROUND_WATERWAY5]++;
    }

    // Double and Ice/Stone OR Roc Wing
    if ((rocOrDouble && hasIceOrStone) || keyItems[INDEX_KEYITEM_ROCWING])
    {
        reachable[INDEX_CATACOMB3B]++;
        reachable[INDEX_OUTER_WALL1]++;
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
    int placed_easy_items[NUMBER_EASY_ITEMS] = { 0 };
    int placed_common_items[NUMBER_COMMON_ITEMS] = { 0 };
    int placed_rare_items[NUMBER_COMMON_ITEMS + NUMBER_RARE_ITEMS] = { 0 };

    int easy_items[NUMBER_EASY_ITEMS] =
    {
        INDEX_ITEM_BODY_LEATHERARMOR,
        INDEX_ITEM_BODY_COTTONROBE,
        INDEX_ITEM_BODY_COTTONCLOTHES,
        INDEX_ITEM_ARM_WRISTBAND,
        INDEX_ITEM_USE_POTION,
        INDEX_ITEM_USE_ANTIDOTE,
        INDEX_ITEM_USE_CURECURSE,
        INDEX_ITEM_USE_MINDRESTORE,
        INDEX_ITEM_USE_HEART
    };

    int common_items[NUMBER_COMMON_ITEMS] = 
    {
        // Armor
        INDEX_ITEM_BODY_LEATHERARMOR,
        INDEX_ITEM_BODY_BRONZEARMOR,
        INDEX_ITEM_BODY_GOLDARMOR,
        INDEX_ITEM_BODY_CHAINMAIL,
        INDEX_ITEM_BODY_STEELARMOR,

        INDEX_ITEM_BODY_COTTONROBE,
        INDEX_ITEM_BODY_SILKROBE,
        INDEX_ITEM_BODY_RAINBOWROBE,

        INDEX_ITEM_BODY_COTTONCLOTHES,
        INDEX_ITEM_BODY_PRISONGARB,
        INDEX_ITEM_BODY_STYLISHSUIT,

        INDEX_ITEM_ARM_DOUBLEGRIPS,
        INDEX_ITEM_ARM_STARBRACELET,
        INDEX_ITEM_ARM_STRENGTHRING,
        INDEX_ITEM_ARM_HARDRING,
        INDEX_ITEM_ARM_INTELLIGENCERING,
        INDEX_ITEM_ARM_LUCKRING,
        INDEX_ITEM_ARM_CURSEDRING,

        INDEX_ITEM_ARM_WRISTBAND,
        INDEX_ITEM_ARM_GAUNTLET,
        INDEX_ITEM_ARM_ARMGUARD,
        INDEX_ITEM_ARM_MAGICGAUNTLET,
        INDEX_ITEM_ARM_MIRACLEARMBAND,

        INDEX_ITEM_ARM_TOYRING,

        // Consumables
        INDEX_ITEM_USE_POTION,
        INDEX_ITEM_USE_MEAT,
        INDEX_ITEM_USE_SPICEDMEAT,

        INDEX_ITEM_USE_ANTIDOTE,
        INDEX_ITEM_USE_CURECURSE,
        INDEX_ITEM_USE_MINDRESTORE,

        INDEX_ITEM_USE_HEART,
        INDEX_ITEM_USE_HEARTHIGH,
    };

    int rare_items[NUMBER_COMMON_ITEMS + NUMBER_RARE_ITEMS] =
    {
        // Armor
        INDEX_ITEM_BODY_LEATHERARMOR,
        INDEX_ITEM_BODY_BRONZEARMOR,
        INDEX_ITEM_BODY_GOLDARMOR,
        INDEX_ITEM_BODY_CHAINMAIL,
        INDEX_ITEM_BODY_STEELARMOR,

        INDEX_ITEM_BODY_COTTONROBE,
        INDEX_ITEM_BODY_SILKROBE,
        INDEX_ITEM_BODY_RAINBOWROBE,

        INDEX_ITEM_BODY_COTTONCLOTHES,
        INDEX_ITEM_BODY_PRISONGARB,
        INDEX_ITEM_BODY_STYLISHSUIT,

        INDEX_ITEM_ARM_DOUBLEGRIPS,
        INDEX_ITEM_ARM_STARBRACELET,
        INDEX_ITEM_ARM_STRENGTHRING,
        INDEX_ITEM_ARM_HARDRING,
        INDEX_ITEM_ARM_INTELLIGENCERING,
        INDEX_ITEM_ARM_LUCKRING,
        INDEX_ITEM_ARM_CURSEDRING,

        INDEX_ITEM_ARM_WRISTBAND,
        INDEX_ITEM_ARM_GAUNTLET,
        INDEX_ITEM_ARM_ARMGUARD,
        INDEX_ITEM_ARM_MAGICGAUNTLET,
        INDEX_ITEM_ARM_MIRACLEARMBAND,

        INDEX_ITEM_ARM_TOYRING,

        // Consumables
        INDEX_ITEM_USE_POTION,
        INDEX_ITEM_USE_MEAT,
        INDEX_ITEM_USE_SPICEDMEAT,

        INDEX_ITEM_USE_ANTIDOTE,
        INDEX_ITEM_USE_CURECURSE,
        INDEX_ITEM_USE_MINDRESTORE,

        INDEX_ITEM_USE_HEART,
        INDEX_ITEM_USE_HEARTHIGH,

        // Nice Items
        // Armor
        INDEX_ITEM_BODY_PLATINUMARMOR,
        INDEX_ITEM_BODY_DIAMONDARMOR,
        INDEX_ITEM_BODY_MIRRORARMOR,
        INDEX_ITEM_BODY_NEEDLEARMOR,
        INDEX_ITEM_BODY_DARKARMOR,

        INDEX_ITEM_BODY_MAGICROBE,
        INDEX_ITEM_BODY_SAGEROBE,

        INDEX_ITEM_BODY_NIGHTSUIT,
        INDEX_ITEM_BODY_NINJAGARB,
        INDEX_ITEM_BODY_SOLDIERFATIGUES,

        INDEX_ITEM_ARM_STRENGTHARMBAND,
        INDEX_ITEM_ARM_DEFENSEARMBAND,
        INDEX_ITEM_ARM_SAGEARMBAND,
        INDEX_ITEM_ARM_GAMBLERARMBAND,

        INDEX_ITEM_ARM_BEARRING,

        // Consumables
        INDEX_ITEM_USE_POTIONHIGH,
        INDEX_ITEM_USE_POTIONEX,

        INDEX_ITEM_USE_MINDHIGH,
        INDEX_ITEM_USE_MINDEX,

        INDEX_ITEM_USE_HEARTEX,
        INDEX_ITEM_USE_HEARTMEGA,
    };

    int i;

    for (i = 0; i < NUMBER_ENEMIES; i++)
    {
        // Give Dracula II shinning armor occasionally as a joke
        if (i == INDEX_ENEMY_DRACULAII)
        {
            regular_drops[i] = rare_drops[i] = INDEX_ITEM_BODY_SHINNINGARMOR;
            regular_drop_chance[i] = rare_drop_chance[i] = 5000;
        }   
        // Bosses
        else if (i == INDEX_ENEMY_CERBERUS || i == INDEX_ENEMY_NECROMANCER || i == INDEX_ENEMY_IRONGOLEM || i == INDEX_ENEMY_ADRAMELECH || i == INDEX_ENEMY_ZOMBIEDRAGON || i == INDEX_ENEMY_DEATH || i == INDEX_ENEMY_CAMILLA || i == INDEX_ENEMY_HUGH || i == INDEX_ENEMY_DRACULAI)
        {
            // Select a random exclusive rare for every boss enemy for common but not rare since we always drop the common
            regular_drops[i] = selectDrop(&rare_items[NUMBER_COMMON_ITEMS], &placed_rare_items[NUMBER_COMMON_ITEMS], NUMBER_RARE_ITEMS);
            //rare_drops[i] = selectDrop(&rare_items[NUMBER_COMMON_ITEMS], &placed_rare_items[NUMBER_COMMON_ITEMS], NUMBER_RARE_ITEMS);
            rare_drops[i] = 0;

            // Max out rare boss drops (normally, drops are hard capped to 50% and 25% respectively regardless of drop rate, but
            // fusecavator's patch AllowAlwaysDrop.ips allows setting the regular item drop chance to 10000 to force a drop always)
            regular_drop_chance[i] = 10000;
            rare_drop_chance[i] = 0;

            // Note: We increment the placed item counters since bosses are now guaranteed to drop an item
            placed_rare_items[getPlacedIndexFromID(regular_drops[i], &rare_items[NUMBER_COMMON_ITEMS], &placed_rare_items[NUMBER_COMMON_ITEMS], NUMBER_RARE_ITEMS)]++;
        }
        // Trivially easy enemies that can be easily farmed AND we are NOT using the hard mode option
        // OR
        // We ARE using the hard mode option and the enemy is below 150 HP
        else if 
        (
            (!options->RandomItemHardMode && 
                (i == INDEX_ENEMY_MEDUSAHEAD || i == INDEX_ENEMY_ZOMBIE || i == INDEX_ENEMY_GHOUL || i == INDEX_ENEMY_WIGHT || i == INDEX_ENEMY_SKELETONBOMBER || i == INDEX_ENEMY_FLEAMAN || i == INDEX_ENEMY_BAT || i == INDEX_ENEMY_SPIRIT || i == INDEX_ENEMY_ECTOPLASM || i == INDEX_ENEMY_SPECTER 
                || i == INDEX_ENEMY_DEVILTOWER || i == INDEX_ENEMY_GARGOYLE || i == INDEX_ENEMY_POISONWORM || i == INDEX_ENEMY_MYCONID || i == INDEX_ENEMY_MERMAN || i == INDEX_ENEMY_GREMLIN || i == INDEX_ENEMY_HOPPER || i == INDEX_ENEMY_EVILHAND || i == INDEX_ENEMY_MUMMY))
            || (options->RandomItemHardMode &&
            (   i == INDEX_ENEMY_MEDUSAHEAD ||
                i == INDEX_ENEMY_ZOMBIE ||
                i == INDEX_ENEMY_GHOUL ||
                i == INDEX_ENEMY_WIGHT ||
                i == INDEX_ENEMY_CLINKINGMAN ||
                i == INDEX_ENEMY_ZOMBIETHIEF ||
                i == INDEX_ENEMY_SKELETON ||
                i == INDEX_ENEMY_SKELETONBOMBER ||
                i == INDEX_ENEMY_ELECTRICSKELETON ||
                i == INDEX_ENEMY_SKELETONSPEAR ||
                i == INDEX_ENEMY_SKELETONBOOMERANG ||
                i == INDEX_ENEMY_SKELETONSOLDIER ||
                i == INDEX_ENEMY_SKELETONKNIGHT ||
                i == INDEX_ENEMY_BONETOWER ||
                i == INDEX_ENEMY_FLEAMAN ||
                i == INDEX_ENEMY_POLTERGEIST ||
                i == INDEX_ENEMY_BAT ||
                i == INDEX_ENEMY_SPIRIT ||
                i == INDEX_ENEMY_ECTOPLASM ||
                i == INDEX_ENEMY_SPECTER ||
                i == INDEX_ENEMY_AXEARMOR ||
                i == INDEX_ENEMY_EARTHARMOR ||
                i == INDEX_ENEMY_STONEARMOR ||
                i == INDEX_ENEMY_BLOODYSWORD ||
                i == INDEX_ENEMY_DEVILTOWER ||
                i == INDEX_ENEMY_SKELETONATHLETE ||
                i == INDEX_ENEMY_HARPY ||
                i == INDEX_ENEMY_IMP ||
                i == INDEX_ENEMY_MUDMAN ||
                i == INDEX_ENEMY_GARGOYLE ||
                i == INDEX_ENEMY_SLIME ||
                i == INDEX_ENEMY_FROZENSHADE ||
                i == INDEX_ENEMY_HEATSHADE ||
                i == INDEX_ENEMY_POISONWORM ||
                i == INDEX_ENEMY_MYCONID ||
                i == INDEX_ENEMY_WILLOWISP ||
                i == INDEX_ENEMY_SPEARFISH ||
                i == INDEX_ENEMY_MERMAN ||
                i == INDEX_ENEMY_MARIONETTE ||
                i == INDEX_ENEMY_GREMLIN ||
                i == INDEX_ENEMY_HOPPER ||
                i == INDEX_ENEMY_EVILPILLAR ||
                i == INDEX_ENEMY_BONEHEAD ||
                i == INDEX_ENEMY_FOXARCHER ||
                i == INDEX_ENEMY_FOXHUNTER ||
                i == INDEX_ENEMY_HYENA ||
                i == INDEX_ENEMY_FISHHEAD ||
                i == INDEX_ENEMY_DRYAD ||
                i == INDEX_ENEMY_BRAINFLOAT ||
                i == INDEX_ENEMY_EVILHAND ||
                i == INDEX_ENEMY_ABIONDARG ||
                i == INDEX_ENEMY_WITCH ||
                i == INDEX_ENEMY_MUMMY ||
                i == INDEX_ENEMY_KINGMOTH ||
                i == INDEX_ENEMY_KILLERBEE ||
                i == INDEX_ENEMY_LIZARDMAN ||
                i == INDEX_ENEMY_BATTLEARENADEVILTOWER ||
                i == INDEX_ENEMY_BATTLEARENABONETOWER ||
                i == INDEX_ENEMY_BATTLEARENABLOODYSWORD ||
                i == INDEX_ENEMY_BATTLEARENAEVILPILLAR)))
        {
            regular_drops[i] = selectDrop(easy_items, placed_easy_items, NUMBER_EASY_ITEMS);
            placed_easy_items[getPlacedIndexFromID(regular_drops[i], easy_items, placed_easy_items, NUMBER_EASY_ITEMS)]++;
            rare_drops[i] = selectDrop(easy_items, placed_easy_items, NUMBER_EASY_ITEMS);
            placed_easy_items[getPlacedIndexFromID(rare_drops[i], easy_items, placed_easy_items, NUMBER_EASY_ITEMS)]++;
            
            // Level 1 rate between 5-10% and rare between 3-8%
            regular_drop_chance[i] =  500 + (rand() % 501);
            rare_drop_chance[i] = 300 + (rand() % 501);
        }
        // It is a "Candle" enemy
        else if (i == INDEX_ENEMY_MIMICCANDLE || i == INDEX_ENEMY_SCARYCANDLE || i == INDEX_ENEMY_TRICKCANDLE)
        {
            // Select a random exclusive rare for every candle enemy for both common and rare drops
            regular_drops[i] = selectDrop(&rare_items[NUMBER_COMMON_ITEMS], &placed_rare_items[NUMBER_COMMON_ITEMS], NUMBER_RARE_ITEMS);
            rare_drops[i] = selectDrop(&rare_items[NUMBER_COMMON_ITEMS], &placed_rare_items[NUMBER_COMMON_ITEMS], NUMBER_RARE_ITEMS);

            // Otherwise, set a regular drop chance between 5-10% and a rare drop chance between 3-5%
            regular_drop_chance[i] = 500 + (rand() % 501);
            rare_drop_chance[i] = 300 + (rand() % 201);
            
            // Increment our counter for how many have been placed
            placed_rare_items[getPlacedIndexFromID(regular_drops[i], &rare_items[NUMBER_COMMON_ITEMS], &placed_rare_items[NUMBER_COMMON_ITEMS], NUMBER_RARE_ITEMS)]++;
            placed_rare_items[getPlacedIndexFromID(rare_drops[i], &rare_items[NUMBER_COMMON_ITEMS], &placed_rare_items[NUMBER_COMMON_ITEMS], NUMBER_RARE_ITEMS)]++;
        }
        // Regular enemies
        else
        {
            // Select a random regular and rare drop for every enemy from their respective lists
            regular_drops[i] = selectDrop(common_items, placed_common_items, NUMBER_COMMON_ITEMS);
            rare_drops[i] = selectDrop(rare_items, placed_rare_items, NUMBER_COMMON_ITEMS + NUMBER_RARE_ITEMS);

            // Otherwise, set a regular drop chance between 5-10% and a rare drop chance between 3-5%
            regular_drop_chance[i] = 500 + (rand() % 501);
            rare_drop_chance[i] = 300 + (rand() % 201);

            // Increment our counter for how many have been placed
            placed_common_items[getPlacedIndexFromID(regular_drops[i], common_items, placed_common_items, NUMBER_COMMON_ITEMS)]++;
            placed_rare_items[getPlacedIndexFromID(rare_drops[i], rare_items, placed_rare_items, NUMBER_COMMON_ITEMS + NUMBER_RARE_ITEMS)]++;
        }
    }
}

static int getPlacedIndexFromID(int itemID, int dropList[], int dropsPlaced[], int number_drops)
{
    int i;

    for (i = 0; i < number_drops; i++)
    {
        if (itemID == dropList[i])
        {
            return i;
        }
    }

    printf("Error: No index matching ID in drop table array.\n");
    exit(1);
}

static int selectDrop(int dropList[], int dropsPlaced[], int number_drops)
{
    int lowest_number;
    int number_valid_drops = 0;
    int i;

    int random_result;

    int eligible_items[NUMBER_ITEMS];

    lowest_number = dropsPlaced[0];
    // Only make eligible drops which we have placed the least
    for (i = 0; i < number_drops; i++)
    {
        // A drop with the priority we are expecting is available to add as a candidate
        if (dropsPlaced[i] == lowest_number)
        {
            eligible_items[number_valid_drops] = i;
            number_valid_drops++;
        }

        // If this criteria is met, there is at least one item that hasn't been placed as many times as the others.
        // We have to lower the lowest number and start from the beginning of the loop to capture all the valid indices.
        else if (dropsPlaced[i] < lowest_number)
        {
            lowest_number = dropsPlaced[i];
            number_valid_drops = i = 0;
        }
    }
    // Postcondition: Our array eligible_items has number_valid_drops many valid item indices as its elements

    random_result = rand() % number_valid_drops;
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
