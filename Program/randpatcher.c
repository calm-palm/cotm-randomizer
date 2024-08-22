#include <stdio.h>
#include <stdbool.h>
#include "ips.h"
#include "options.h"
#include "randpatcher.h"
#include "randomizer.h"
#include "countdown.h"
#include "subweapon.h"
#include "graphicsfix.h"

int randompatch(FILE* rom, unsigned int randomizer_seed, struct seed_options *options) {
    FILE *randomizer_patch;
    FILE *spoiler_log;

    // Open a file "Randomizer.IPS" to dump the Randomizer changes
    randomizer_patch = fopen(IPS_RANDOMIZER, "wb");
    if (randomizer_patch == NULL)
    {
        printf("Error opening file %s.\nPress enter to close.\n", IPS_RANDOMIZER);
        return 1;
    }

    // Open a file "spoilerlog.txt" to print pedestal contents
    spoiler_log = fopen(SPOILER_FILENAME, "w");
    if (spoiler_log == NULL)
    {
        printf("Error opening file %s.\nPress enter to close.\n", SPOILER_FILENAME);
        return 1;
    }

    /* If All Bosses Required is enabled, fix Last Key counts. */
    if (options->allBossesRequired) {
        options->lastKeyAvailable = 8;
        options->lastKeyRequired = 8;
    }

    /* If All Bosses + Battle Arena Required is enabled, fix Last Key counts. */
    if (options->allBossesAndBattleArenaRequired) {
        options->lastKeyAvailable = 9;
        options->lastKeyRequired = 9;
    }

    // Call special module for randomizer logic to create patch
    generateRandomizerPatch(randomizer_patch, randomizer_seed, options, spoiler_log);
    fclose(randomizer_patch);
    fclose(spoiler_log);

    // Apply IPS
    if(applyIPS(rom, IPS_AUTODASHBOOTS) || applyIPS(rom, IPS_CARDMODE) 
    || applyIPS(rom, IPS_NODSSDROPS) || applyIPS(rom, IPS_CARDCOMBOSREVEALED) 
    || applyIPS(rom, IPS_CANDLEFIX) || applyIPS(rom, IPS_BLOCKFIX)
    || applyIPS(rom, IPS_MPCOMBOFIX) || applyIPS(rom, IPS_GAMECLEARBYPASS) 
    || applyIPS(rom, IPS_MAPEDITS) || applyIPS(rom, IPS_RANDOMIZER) == 1 
    || applyIPS(rom, IPS_DEMO_FORCE_FIRST) == 1 || applyIPS(rom, IPS_ALLOW_ALWAYS_DROP) == 1
    || applyIPS(rom, IPS_BATTLEARENAMUSIC))
    {
        printf("Failed to apply IPS.\n");
        return 1;
    }

    // Apply optional IPSes
    if (options->applyAutoRunPatch)
        if (applyIPS(rom, IPS_PERMANENTDASH) == 1)
        {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->applyNoDSSGlitchPatch)
        if (applyIPS(rom, IPS_DSSGLITCHFIX) == 1)
        {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->breakIronMaidens)
        if (applyIPS(rom, IPS_IRONMAIDEN) == 1)
        {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->lastKeyRequired != 1) {
        if(applyIPS(rom, IPS_MULTILASTKEY) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }
        fseek(rom, 0x96C1E, SEEK_SET);
        fwrite(&(options->lastKeyRequired), 1, 1, rom);
        fseek(rom, 0xDFB4, SEEK_SET);
        fwrite(&(options->lastKeyRequired), 1, 1, rom);
        fseek(rom, 0xCB84, SEEK_SET);
        fwrite(&(options->lastKeyRequired), 1, 1, rom);
    }

    if (applyIPS(rom, IPS_SEED_DISPLAY) == 1) {
        printf("Failed to apply IPS.\n");
        return 1;
    }
    else {
        unsigned int remaining_seed = randomizer_seed;
        int cur_offset = 0x67213E;
        do {
            int digit = remaining_seed % 10;
            remaining_seed /= 10;
            digit += 0x511C;
            fseek(rom, cur_offset, SEEK_SET);
            fwrite(&digit, 2, 1, rom);
            cur_offset -= 2;
        } while(remaining_seed);
    }

    if (options->applyBuffFamiliars)
        if(applyIPS(rom, IPS_BUFF_FAMILIARS) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->applyBuffSubweapons)
        if(applyIPS(rom, IPS_BUFF_SUBWEAPONS) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->applyShooterStrength)
        if(applyIPS(rom, IPS_SHOOTERSTRENGTH) == 1)
        {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->applyAllowSpeedDash)
        if(applyIPS(rom, IPS_ALLOW_SPEED_DASH) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->countdown) {
        if (applyIPS(rom, IPS_COUNTDOWN) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }
        patch_countdown(rom);
    }

    if (options->subweaponShuffle) {
      subweapon_shuffle(rom, randomizer_seed);
    }

    if (options->noMPDrain)
        if (applyIPS(rom, IPS_NOMPDRAIN) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->dssRunSpeed)
        if (applyIPS(rom, IPS_DSSRUNSPEED) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->skipCutscenes)
        if (applyIPS(rom, IPS_SKIPCUTSCENES) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->skipMagicItemTutorials)
        if (applyIPS(rom, IPS_SKIPMAGICITEMTUTORIALS) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    if (options->nerfRocWing)
        if (applyIPS(rom, IPS_NERFROCWING) == 1) {
            printf("Failed to apply IPS.\n");
            return 1;
        }

    // Fix Y-coordinates for magic items and max ups
    graphicsFix(rom);

    // Fix graphics via drop rework patch
    if (applyIPS(rom, IPS_DROPREWORK) == 1)
    {
        printf("Failed to apply IPS.\n");
        return 1;
    }

    return 0;
}
