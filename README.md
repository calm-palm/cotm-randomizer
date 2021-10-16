# Castlevania: Circle of the Moon Randomizer Beta 1.1

Reverse engineering and game program: DevAnj
Optional patches: fusecavator
Randomizer logic and patch program: spooky
Emotional support: Circle of the Moon Speedrunning Discord @ https://discord.gg/Ae7Qjd5xDu

Try the randomizer online: https://rando.circleofthemoon.com/ 
Note: Seeds are not shared between the website and the application releases.


# What it does:

- This will usually patch a US Castlevania: Circle of the Moon ROM to have a different key item layout. Critically,
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

1. Drag your Castlevania: Circle of the Moon ROM onto rando.exe or run it with the following syntax: .\rando.exe <ROM Name>.gba
   The original ROM will be copied to "cotm_randomized.gba" in the program folder, and this copy will be patched by the program.

2. Select any optional patches or rules from the list using the corresponding numbers. Selected patches or rules will be marked with 
   an asterisk. When you have made your decision, type 0 and then hit enter to proceed. 
   Warning: Changes to randomizer rules will change item placements even with the same seed, so be sure to remember your choices when saving or sharing seeds.

You now have a randomized ROM. Also, the spoiler log for item placements and enemy drops will be in the folder as "spoilerlog.txt". To use this, you may
need to use the included full map of the game which contains the room IDs used in the log.

Note that Randomizer.ips includes only the randomizer item placements, and if you want to share this patch instead of your seed, you must also
use all the other patches (and omit any optional patches you did not use). Be warned that the MultiLastKey optional rule cannot be shared by IPSes alone,
since the numbers of keys required are patched dynamically by the program instead of via IPS. Additionally, the seed display patch obviously will not display
the seed since it also relies on writing to the ROM at program execution time. It is simpler to share seeds and use the program over distributing sets of IPSes.




Additional instructions for Castlevania Advance Collection:

The Castlevania Advance Collection uses a proprietary emulator with compressed ROMs. The US Circle of the Moon ROM can be patched by the program, but
must be decompressed and extracted. The original ROM must then be replaced, and all game files recompressed and repackaged.

If you wish to use this randomizer with the Advance Collection, follow my guide below:

Video tutorial: https://www.youtube.com/watch?v=azQY-fIwizY
Steam guide article: https://steamcommunity.com/sharedfiles/filedetails/?id=2612394352




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
activation. In addition, you will not be able to increase damage and/or change the element of a summon attack unless you possess the cards you swap to.

MultiLastKey.ips
Optional patch created by Fusecavator. Changes game behavior to add instead of set Last Key values, and check for a specific value of Last Keys on the 
door to the ceremonial room, allowing multiple keys to be required to complete the game. Relies on the program to set required key values.

PermanentDash.ips
Optional patch created by Fusecavator. Permanent dash effect without double tapping.

ShooterStrength.ips
Optional patch created by Fusecavator. Increases the Shooter gamemode base strength and strength per level to match Vampirekiller.




# Changes:

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
