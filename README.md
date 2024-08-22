# Castlevania: Circle of the Moon Randomizer 1.4.1

Reverse engineering and game program: DevAnj
Optional and/or quality-of-life patches: Fusecavator and Liquid Cat
Randomizer logic and patch program: spooky
Reworked item logic and additional features: Malaert64
Emotional support: Circle of the Moon Speedrunning Discord @ https://discord.gg/Ae7Qjd5xDu

Try the randomizer online: https://rando.circleofthemoon.com/ 
Note: Seeds are not shared between the website and the application releases.

# Easy visual guide:

Malaert64 has prepared a great guide for the randomizer. Read it here: https://docs.google.com/document/d/1uot4BD9XW7A--A8ecgoY8mLK_vSoQRpY5XCkzgas87c/view?usp=sharing

It is a very useful resource for beginners to the game, and importantly, it contains a map that shows the area
boundaries used by the countdown meter (they are not always intuitive: for example, the outdoor hallway with Obervation
Tower music, the outdoor armory with all subweapons near Machine Tower with Audience Room music, and the Battle Arena entrance
outdoor area with Chapel Tower music are all actually in the Outer Wall area).

# What it does:

- This will patch a US Castlevania: Circle of the Moon ROM to have a different key item layout. Critically,
  the ability to freeze or petrify enemies with the combinations Mercury/Serpent, Mercury/Cockatrice, Mars/Serpent, 
  or Mars/Cockatrice is considered a progress item potentially requiring you to check a pedestal only accessible by
  using enemies as platforms. These include all pedestals accessible with Brain Floats, but also three miscellaneous 
  locations that may not be obvious (screenshots of these rooms are in the program folder as well):
    - Triumph Hallway's leftmost room, the entrance to the castle, contains a pedestal at the very top accessible with
      Double, Kick Boots, and Ice/Stone if you lure the Flame Armor to the left and kick off the gatehouse tower.
    - Chapel Tower's rightmost room, parallel with the Eternal Corridor, contains a pedestal accessible with Double, Kick
      Boots, and Ice/Stone if you lure the Flame Armor to the right to use as a platform and then wall kick to the top.
    - Underground Gallery's Harpy room has two pedestals accessible by freezing or petrifying the Harpy and then jumping
      to the first one, and then either freezing or petrifying the Harpy again or kicking off the wall to reach the higher
      pedestal guarded by one Lizard Man.
    - If you choose to solve the puzzle in UW14 before checking the spot in the ceiling, it will be inaccessible without ice
      or stone. In logic, ice or stone is required for this pedestal.

- Additionally, DSS cards will be randomly distributed on item pedestals instead of obtained via enemy drops. Enemy drops 
  are replaced with different drops, and drop rate is increased. Trivially easy enemies (worms, zombie variants, other low 
  HP/respawning enemies) will drop poor equipment and consumables. Other enemies will have a chance at any item that can be 
  dropped by enemies in the base game except DSS cards. Boss enemies also are guaranteed to drop items that  are better than average, 
  and no chance of dropping poor items. The three "Candle" enemies will also drop powerful items.

- There are also optional patches and rules with descriptions available in the optional patch menu and in the patch list below.




# What it does not do:

- You will not be expected to check the hidden room on the left side
  of the long vertical room where you fall in the introductory cutscene until you have obtained Roc Wing.

- The pedestal at the end of Battle Arena will not be randomized. It will have Shinning Armor.




# How to use it:

0. Either use the default seed in "seed.txt" or edit this file and specify an unsigned integer of your choice to use as the seed.

1. Put your Castlevania: Circle of the Moon ROM into the same folder as the rando.exe executable. This is mandatory!

2. Drag your Castlevania: Circle of the Moon ROM onto rando.exe or run it with the following syntax: .\rando.exe <ROM Name>.gba
   The original ROM will be copied to "cotm_randomized.gba" in the program folder, and this copy will be patched by the program.

3. Select any optional patches or rules from the list using the corresponding numbers. Selected patches or rules will be marked with 
   an asterisk. When you have made your decision, type 0 and then hit enter to proceed. 
   Warning: Changes to randomizer rules will change item placements even with the same seed, so be sure to remember your choices when saving or sharing seeds.

You now have a randomized ROM. Also, the spoiler log for item placements and enemy drops will be in the folder as "spoilerlog.txt". To use this, you may
need to use the included full map of the game which contains the room IDs used in the log.

Note that Randomizer.ips includes only the randomizer item placements, and if you want to share this patch instead of your seed, you must also
use all the other patches (and omit any optional patches you did not use). Be warned that the MultiLastKey optional rule cannot be shared by IPSes alone,
since the numbers of keys required are patched dynamically by the program instead of via IPS. Additionally, the seed display patch obviously will not display
the seed since it also relies on writing to the ROM at program execution time. It is simpler to share seeds and use the program or the website over distributing sets of IPSes.




Additional instructions for Castlevania Advance Collection:

The Castlevania Advance Collection uses a proprietary emulator with compressed ROMs. The US Circle of the Moon ROM can be patched by the program, but
must be decompressed and extracted. The original ROM must then be replaced, and all game files recompressed and repackaged.

If you wish to use this randomizer with the Advance Collection, follow my guide below:

Video tutorial: https://www.youtube.com/watch?v=azQY-fIwizY
Steam guide article: https://steamcommunity.com/sharedfiles/filedetails/?id=2612394352




# Optional rules:
1. "Ignore Cleansing": Ignore Cleansing when placing magic items. You may be required to traverse Underground Waterway without Cleansing.
2. "Auto Run": Enable permanent dash. You will always have the effect of Dash Boots applied without needing to double tap direction inputs
3. "DSS Patch": Disable switching to DSS effects from unobtained cards. You will not be able to use the DSS glitch.
4. "Break Iron Maidens": Break Iron Maidens from the beginning of the game. You will not be required to press the button.
5. "Required Last Keys": Set number of Last Keys required to open the door to the Ceremonial Room.
6. "Available Last Keys": Set number of Last Keys to be placed on pedestals.
7. "Do Not Randomize Items": Disable item randomization. Enemies will drop their default items.
8. "Tiered Item Drops": Item drops are split into three tiers based on enemy HP: low-tier items are dropped by enemies under 144 HP, low and mid-tier items are dropped 144 - 369 HP, and mid and high-tier items are dropped by enemies with 370+ HP. Any rare items assigned to bosses are exclusive to them.
9. Optional balance changes:
   a. "Buff Ranged Familiars": Doubles the damage dealt by projectiles fired by familiars.
   b. "Buff SubWeapons": Increase regular and Shooter mode damage for some subweapons and corresponding item crush attacks (see BuffSubweapons.ips below for exact changes).
   c. "Buff Shooter Strength": Increase the Shooter mode base strength and strength per level to match Vampirekiller.
10. "Always Allow Speed Dash": Allow activating Pluto + Griffin (increased speed) even when the cards are not obtained.
11. "Halve DSS Cards Placed": Halve the number of placed DSS cards. You will not be able to obtain all DSS cards. Cards could randomly be skewed to action or attribute.
12. "Countdown": Display a counter on the HUD showing the number of magic items and cards remaining in the current area.
13. "Subweapon Shuffle": Randomize which subweapon is in which subweapon location. The numbers of subweapons present in the original game are preserved. Subweapons are only placed in locations that already had a subweapon.
14. "No MP Drain":  Disable the Battle Arena's MP drain effect. You will be able to use DSS in the Battle Arena without MP restoring items.
15. "All Bosses Required": A Last Key will be placed behind every boss except Dracula. All eight Last Keys will be required. The other Last Key settings will be ignored.
16. "All Bosses and Battle Arena Required": Same as previous, except a ninth key will also be placed at the end of the Battle Arena. The other Last Key settings (and normal All Bosses logic) will be ignored.
17. "DSS Run Speed": The Pluto and Griffin DSS card speed increase will apply even when jumping.
18. "Skip Cutscenes": Cutscenes will proceed without dialogue.
19. "Skip Magic Item Tutorials": Magic Items will no longer provide guidance on item use when obtained.
20. "Nerf Roc Wing": Roc Wing will be less effective while Double or Kick Boots have not yet been obtained. Read the guide below for details.

# "Nerf Roc Wing" guide:

The optional patch "Nerf Roc Wing" by Liquid Cat changes the way the Roc Wing behaves. The Roc Wing's height will be decreased if Double and/or Kick Boots are not yet obtained according to this list:

Jump Lv. 5 = Roc Wing + Kick Boots + Double

Jump Lv. 4 = Roc Wing + Kick Boots

Jump Lv. 3 = Roc Wing + Double (alternatively solvable with Kick Boots instead of Double)

Jump Lv. 2 = Roc Wing

Jump Lv. 1 = Double (alternatively solvable with just Roc Wing)

Jump Lv. 0 = Nothing

Without Double and Kick Boots, the Roc Wing jump height is nerfed, and you can only jump from the ground.

With Double and no Kick Boots, the Roc Wing jump height is nerfed, but you can jump once from midair (and it will count as your double jump).

Without Double but with Kick Boots, the Roc Wing jump height is normal, but you can only jump from the ground.

With Double and Kick Boots, the Roc Wing jump behaves as it does normally in vanilla (i.e., midair jumps and infinite jumps while still in the air).

Very importantly, the heights in logic take into account that you will jump SLIGHTLY HIGHER while holding the A button during a Roc Wing jump. If you cannot make a jump that is included in the list of locations below with the correct jump level, try holding the A button during the jump.

Liquid Cat also provided this additional explanation of which areas and rooms are affected:

OTHER NOTES:

-Escaping the catacombs and reaching Audience Room can be done with Jump Lv. 1 at minimum. As such, anything beyond the Catacombs doable with just Jump Lv. 1 doesn't have it listed there, as being there with Jump Lv. 0 is impossible.

-Going from Audience Room to top of Machine Tower is doable with Jump Lv. 2 or Kick Boots + Jump Lv. 1.

-Reaching Eternal Corridor and Chapel Tower can be done Jump Lv. 3. Making it with Jump Lv. 2 is possible with Freeze or upper Underground Gallery access. And of course, Jump Lv. 1 can only make it with Kick Boots.

-Observation Tower is ONLY reachable with Jump Lv. 5, the highest jump level of them all.

-Room CT14 (the room in Chapel Tower just after the Thunder Demons) is notably impossible to get past if you make it there with just Jump Lv. 2; to get up the shaft, at minimum you need either Jump Lv. 3 or Kick Boots.

-You need Jump Lv. 3 at minimum to both enter Waterway from Corridor and escape. Making it with Lv. 2 is a softlock, but getting there with Lv. 2 should be impossible to begin with.

-If you end up outside the Battle Arena without Jump Lv. 5, it's not a softlock; you can escape after doing the first three Arena rounds.


LOCATIONS:

-SR3   - Sealed Room: Main shaft left fake wall: Jump Lv. 5

-CC3   - Catacomb: Fleamen brain room - Lower: Jump Lv. 1

-CC3b  - Catacomb: Fleamen brain room - Upper: Jump Lv. 4 OR Jump Lv. 1 + Freeze

-CC8b  - Catacomb: Earth Demon bone pit - Upper: Jump Lv. 3 OR Kick

-CC14b - Catacomb: Muddy Mudman platforms room - Upper:Jump Lv. 1 OR Kick

-CC25  - Catacomb: Mummies' fake wall:Jump Lv. 1

-AS4   - Abyss Staircase: Roc ledge:Jump Lv. 4

-AR14b - Audience Room: Wicked roc chamber - Upper:Jump Lv. 4

-AR17b - Audience Room: Right exterior - Upper:Jump Lv. 2 OR Kick

-AR19  - Audience Room: 100 meter skelly dash hallway:Jump Lv. 2 OR Kick

-AR26  - Audience Room: Below Machine Tower roc ledge:Jump Lv. 5

-AR30  - Audience Room: Roc horse jaguar armory - Left:Jump Lv. 4 OR Jump Lv. 3 + Freeze

-AR30b - Audience Room: Roc horse jaguar armory - Right:Jump Lv. 4 OR Jump Lv. 3 + Freeze

-OW0   - Outer Wall: Left roc ledge:Jump Lv. 4

-OW1   - Outer Wall: Right-brained ledge:Jump Lv. 5 OR Freeze

-TH3   - Triumph Hallway: Entrance Flame Armor climb:Jump Lv. 2 OR Kick + Freeze

-MT3   - Machine Tower: Boneheaded argument wall kicks room:Jump Lv. 2 OR Kick

-MT6   - Machine Tower: Skelly-rang wall kicks room:Jump Lv. 2 OR Kick

-CT1   - Chapel Tower: Flame Armor climb room:Jump Lv. 2 OR Freeze

-CT13  - Chapel Tower: Sharp mind climb room:Jump Lv. 2 OR Freeze

-CT26  - Chapel Tower: Outside Battle Arena - UpperJump Lv. 4 OR Jump Lv. 3 + Freeze

-CT26b - Chapel Tower: Outside Battle Arena - LowerJump Lv. 4 OR Jump Lv. 3 + Freeze

-UG3   - Underground Gallery: Harpy climb room - Lower:Jump Lv. 2 OR Freeze

-UG3B  - Underground Gallery: Harpy climb room - Upper:Jump Lv. 4 OR Freeze

-UW10  - Underground Warehouse: Succubus shaft roc ledge:Jump Lv. 5 OR Jump Lv. 4 + Freeze

-UW14  - Underground Warehouse: Optional puzzle ceiling fake wall:Jump Lv. 2 OR Freeze

-UW16B - Underground Warehouse: Holy fox hideout - Right roc ledge:     Jump Lv. 3 OR Jump Lv. 2 + Freeze

-UY5   - Underground Waterway: Brain freeze room:Jump Lv. 3 OR Freeze

-UY8   - Underground Waterway: Roc fake ceiling:Jump Lv. 2

-UY13  - Underground Waterway: Roc exit shaft:Jump Lv. 3

-UY18  - Underground Waterway: Roc exit shaft fake wall:Jump Lv. 3

-CR1   - Ceremonial Room: Fake floor:Jump Lv. 2 OR Kick

# Patches:

AllowAlwaysDrop.ips
Used internally in the item randomizer to allow setting drop rate to 10000 (100%) and actually
drop the item 100% of the time. Normally, it is hard capped at 50% for common drops and 25% for rare drops.
Created by Fusecavator.

AutoDashBoots.ips
This patch grants Dash Boots on game initialization, effectively giving you Dash Boots from
the beginning of the game without needing to pick them up. Created by DevAnj.

CardUp_v3_Custom.ips
This patch allows placing DSS cards on pedestals, prevents them from timing out, and removes them from
enemy drop tables. Created by DevAnj but drop and pedestal item replacements have been stripped out.

CardCombosRevealed.ips
This patch reveals card combination descriptions instead of showing "???" until the combination
is used. Created by DevAnj.

CandleFix.ips
In lategame, the Trick Candle and Scary Candle load in the Cerberus and Iron Golem boss rooms after defeating
Camilla and Twin Dragon Zombies respectively. If the former bosses have not yet been cleared (i.e., we have 
sequence broken the game and returned to the earlier boss rooms to fight them), the candle enemies will cause 
the bosses to fail to load and soft lock the game. This patches the candles to appear after the early
boss is completed instead. Created by DevAnj.

Countdown.ips
Optional patch created by fuse. Displays a counter on the HUD showing the number of magic items and cards remaining in the current area. Requires
a lookup table generated by the randomizer to function.

DemoForceFirst.ips
Prevents demos on the main title screen after the first one from being displayed to avoid pedestal item reconnaissance from the menu.
Created by Fusecavator.

GameClearBypass.ips
Normally, you must clear the game with each mode to unlock subsequent modes, and complete the game at least once to be able to skip the introductory text crawl.
This allows all game modes to be selected and the introduction to be skipped even without game/mode completion. Created by DevAnj.

MapEdits.ips
This patch adds custom mapping in Underground Gallery and Underground Waterway to avoid softlocking/Kick Boots requirements. Created by DevAnj.

MPComboFix.ips
Normally, the MP boosting card combination is useless since it depletes more MP than it gains.
This patch makes it consume zero MP. Created by DevAnj.

NoDSSDrops.ips
This patch replaces enemy drops that included DSS cards. Created by DevAnj as part of the Card Up patch
but modified for different replacement drops (Lowered rate, Potion instead of Meat, and no Shinning Armor change on Devil).

NoMPDrain.ips
This patch disables the MP drain effect in the Battle Arena. Created by Fusecavator.

SeedDisplay.ips
Displays the seed on the pause menu. Created by Fusecavator.

SoftlockBlockFix.ips
A Tackle block in Machine Tower will cause a softlock if you access the Machine Tower from the Abyss Stairway using the stone tower route with Kick Boots and not Double.
This is a small level edit that moves that block slightly, removing the potential for a softlock. Created by DevAnj.

Randomizer.ips
This patch is generated by the randomizer logic and remains in the folder.

AllowSpeedDash.ips
Optional patch created by Fusecavator. Allows using the Pluto + Griffin combination for the speed boost with or without the cards being obtained.

BrokenMaidens.ips
Optional patch created by DevAnj. Breaks the iron maidens blocking access to the Underground Waterway, Underground Gallery, and the room beyond the
Adramelech boss room from the beginning of the game.

BuffFamiliars.ips
Optional patch created by Fusecavator. Doubles damage of projectiles fired by ranged familiars.

BuffSubweapons.ips
Optional patch created by Fusecavator. Increases the base damage of some subweapons. Changes below (normal multiplier on left, shooter on right):
  Original: 
    Dagger: 45 / 141
    Dagger crush: 32 / 45
    Axe: 89 / 158
    Axe crush: 89 / 126
    Holy water: 63 / 100
    Holy water crush: 45 / 63
    Cross: 110 / 173
    Cross crush: 100 / 141

  Changed:
    Dagger: 100 / 141 (Non-Shooter buffed)
    Dagger crush: 100 / 141 (Both buffed to match non-crush values)
    Axe: 125 / 158 (Non-Shooter somewhat buffed)
    Axe crush: 125 / 158 (Both buffed to match non-crush values)
    Holy water: 63 / 100 (Unchanged)
    Holy water crush: 63 / 100 (Large buff to Shooter, non-Shooter buffed)
    Cross: 110 / 173 (Unchanged)
    Cross crush: 110 / 173 (Slightly buffed to match non-crush values)

DSSGlitchFix.ips
Optional patch created by Fusecavator. Prohibits the DSS glitch.
You will not be able to update the active effect unless the card combination switched to is obtained. For example, if you switch to another DSS combination
that you have not obtained during DSS startup, you will still have the effect of the original combination you had selected when you started the DSS
activation. In addition, you will not be able to increase damage and/or change the element of a summon attack unless you possess the cards.

MultiLastKey.ips
Optional patch created by Fusecavator. Changes game behavior to add instead of set Last Key values, and check for a specific value of Last Keys on the 
door to the ceremonial room, allowing multiple keys to be required to complete the game. Relies on the program to set required key values.

PermanentDash.ips
Optional patch created by Fusecavator. Permanent dash effect without double tapping.

ShooterStrength.ips
Optional patch created by Fusecavator. Increases the Shooter gamemode base strength and strength per level to match Vampirekiller.

DSSRunSpeed.ips
Optional patch created by Liquid Cat. The Pluto/Griffin DSS card combination will increase speed even during a jump.

SkipCutscenes.ips
Optional patch created by Liquid Cat. The text printed during cutscenes will not appear.

SkipMagicItemTutorials.ips
Optional patch created by Liquid Cat. The text printed after Magic Items are obtained will not appear.

DropRework.ips
Patch created by Fusecavator. Makes changes to dropped item graphics to avoid garbled Magic Items and allow displaying arbitrary items.

NerfRocWing.ips
Optional patch created by Liquid Cat. Changes the behavior of the Roc Wing depending on which other Magic Items have been obtained.

BattleArenaMusic.ips
Patch created by Liquid Cat. Makes Vampire Killer the background music of the Battle Arena, instead of just pure silence.

# Changes:

1.4.1:
  - Malaert64 added Liquid Cat's BattleArenaMusic.ips patch.
  - Malaert64 made another alteration to item logic: Battle Arena Devil will always drop Shinning Armor when All Bosses and Battle Arena Required is enabled, to compensate for Shinning Armor being removed from the end of Battle Arena.

1.4:
  - Malaert64 edited the randomizer drop tables and distribution logic to support the new Tiered Item Drops optional rule which replaces Random Item Hardmode.
  - Malaert64 added the All Bosses and Battle Arena Required optional rule which requires that not only all bosses, but also the Battle Arena be completed in its entirety to obtain an additional required Last Key.

1.3:
  - Added a patch by Fusecavator to fix the garbled Magic Item issue.
  - Added an optional patch by Liquid Cat and randomizer logic changes to support the nerfed Roc Wing optional rule.

1.2:
  - Added an optional patch by Liquid Cat to apply the Pluto/Griffin speed increase while jumping.
  - Added an optional patch by Liquid Cat to skip text in cutscenes.
  - Added an optional patch by Liquid Cat to skip text after Magic Items are obtained.

1.1:
  - Added an optional rule to require that all bosses be defeated. When enabled, the other Last Key settings are ignored, and a Last Key is placed behind every boss except Dracula.
  - Added functionality to fix room settings to properly display magic items in some cases. Nonstandard rooms (e.g. containing Heavy Ring or Tackle blocks and other unknown conflicting graphics -- around half of rooms) will still have garbled graphics for magic items.
  - Fixed the duplicate "OT8" entry on the map and an address error that could have affected maintainability. Discovered by Liquid Cat.

1.0:
  - Altered the Y-coordinates of magic items placed on non-magic item pedestals and non-magic items placed on magic item pedestals to make them now appear to be at the right height.
  - Fixed logic error that previously made Kick Boots required for the pedestal in Catacomb 9.
  - The Bear Ring no longer appears in the boss drop pool.
  - An option has been added to apply the NoMPDrain patch (created by fusecavator) which disables the MP drain in the Battle Arena.
  - The presets on the randomizer website have been updated.

Beta 1.4:
  - Added optional rule to enable the countdown feature, showing the number of magic items and cards remaining in the current area.
  - Added optional rule to enable subweapon shuffle mode. This randomizes which subweapon is in which subweapon location. The numbers of subweapons
    present in the original game are preserved (16 holy waters, 19 axes, 23 knives, 5 crosses, and 7 stopwatches). Subweapons are only placed in 
    locations that already had a subweapon. Also, fusecavator fixed a bug (identified by Malaert64 and Amadeus) which had prevented candles' (as opposed to torches) subweapons from being randomized.

Beta 1.3:
  - Minor logic change added to set boss drops first, preventing bosses from having the same drop.
  - Minor increase to "candle" type enemy drop rate.
  - If random item hard mode is enabled, bosses now have one exclusive item from the rare items list and "candle" type enemies have two.

Beta 1.2:
  - Minor logic change added to make duplicate successive boss drops less likely. 

Beta 1.1:
  - Added patch and changed item randomizer logic to force bosses to drop a good item 100% of the time instead of being hard capped at 50%.

Beta 1:
  - Added optional rule to halve the number of DSS cards placed.
  - Changed randomizer logic to place Last Keys in truly random accessible rooms when the multiple Last Key rule has been enabled to avoid placing most Last 
    Keys in the most recently unlocked area.
  - Added Linux binary to release.
  - Added map with room codes for use with spoiler log.
  - Changed spoiler log behavior to print readable names for rooms (to use with the provided map), items, and enemies.
  - Minor internal change to avoid clearing input buffer in balance menu to allow pasting an option string including balance changes. Recommended by tn.

Alpha 8:
  - Fixed logic error not correctly marking Underground Gallery 8 as a room only accessible with Tackle from the Abyss Stairway entrance. Found by JupiterClimb
    and DevAnj.
  - Added instructions to readme.txt for patching the Circle of the Moon ROM in the Castlevania Advance Collection.

Alpha 7:
  - Added multiple Last Key optional rule using patch made by Fusecavator. Multiple Last Keys may be placed, and an equal or lesser number may 
    be required to open the door. This information is visible on the pause menu.
  - Added patch made by Fusecavator to display seed number on the pause menu.
  - Added patch made by Fusecavator to prevent rando information leakage by only displaying the first demo on the title screen (which shows no pedestals).
  - Fixed logic error causing it to be possible for not all enemy drop items to be placed.
  - Changed enemy drop randomization to force the "Candle" enemies to drop items from the boss drop table. 
  - Added additional freeze spots to logic (leftmost Triumph Hallway room and rightmost Chapel Tower room Flame Armor freeze spots, Underground Gallery Harpy
    freeze spots) and added pictures and explanation to release.
  - Added optional patches BuffFamiliars.ips, BuffSubweapons.ips, AllowSpeedDash.ips, and ShooterStrength.ips.
  - Added optional rule to not apply drop randomization (that is, enemy drops will be vanilla minus DSS cards).
  - Added optional rule for drop randomization hard mode (all enemies below 150 HP drop items from the poor drop pool).

Alpha 6:
  - Added enemy drop randomization. Enemies will have random drops and more common drop rates. The items are in the spoiler log (see spreadsheet
    to decode items and enemies).
  - Added map edits provided by DevAnj to allow traversing Underground Waterway without Kick Boots and avoid a softlock in Underground Gallery.
  - The patcher now copies your provided ROM and patches the copy instead. You will no longer need to provide a fresh ROM to the program. It will
    output a patched ROM in the program folder called "cotm_randomized.gba".
  - Fixed bug causing items to be placed on some pedestals in Underground Waterway without Cleansing being available even without the Cleansing optional rule
    being enabled. Added the Cleansing requirement to those pedestals. Reported by Garnitexas.

Alpha 5:
  - Addresses of action/attribute cards were mistakenly swapped in the randomizer code, potentially causing a soft lock with ice/stone cards as required progress items.
    This has been fixed. Bug reported by Garnitexas.
  - Fixed bug introduced in logic causing crash by failing to account for the iron maiden optional patch not being enabled. Reported by Darrenvile.

Alpha 4:
  - Added patch to allow selecting all game modes and skipping the introductory text crawl without the required save data.
  - Logic adjusted to make early Roc Wing and Last Key more unlikely. Additionally, the logic is now weighted toward placing key items on pedestals
    that have most recently been made accessible.
  - Replaced DSS fix with different fix that works differently provided by Fusecavator. Instead of prohibiting pausing during DSS activation,
    it will not update the active effect unless the card combination switched to is obtained. For example, if you switch to another DSS combination
    that you have not obtained during DSS startup, you will still have the effect of the original combination you had selected when you started the DSS
    activation. In addition, you will not be able to increase damage and/or change the element of a summon attack unless you possess the cards you swap to.
  - Added an optional rule and patch to allow iron maidens blocking access to the Underground Waterway, Underground Gallery, and the room beyond the
    Adramelech boss room from the beginning of the game.

Alpha 3:
  - Audience Room 17's pedestal addresses were mistakenly swapped. The addresses were exchanged in this update to prevent a soft lock. Bug located by JupiterClimb.

Alpha 2:
  - Logic adjusted to correctly allow access to all Double and Kick Boots areas with Roc Wing.
  - Logic adjusted to be heavily skewed in favor of Double when Double is not obtained by only considering other key items eligible to place 10% of the time
    to avoid placing many magic items in Catacomb.
