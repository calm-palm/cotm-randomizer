//////////////////////////////////////////////////////////////////////////////
// This does not need to be included in a standard native binary build.     //
// Similarly, main should not be compiled in with a js build.               //
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "options.h"
#include "randpatcher.h"

const char* cotmr_core_version() {
    return "1-1";
}

// opt,id,type,shortName,Description,default
// opt,id,slider,shortName,Description,low,high,default

const char* cotmr_option_list() {
    return "opt,0,bool,ignoreCleanse,Ignore Cleansing,0\n"
           "opt,1,bool,autoRun,Auto Run,0\n"
           "opt,2,bool,noDss,DSS Patch,0\n"
           "opt,3,bool,allowSpeedDash,Always Allow Speed Dash,0\n"
           "opt,4,bool,breakMaiden,Break Iron Maidens,0\n"
           "opt,5,slider,reqKeys,Required Last Keys,0,9,1\n"
           "opt,6,slider,availKeys,Available Last Keys,0,9,1\n"
           "opt,7,bool,buffFamiliar,Buff Ranged Familiars,0\n"
           "opt,8,bool,buffSubweapon,Buff SubWeapons,0\n"
           "opt,9,bool,buffShooter,Buff Shooter Strength,0\n"
           "opt,10,bool,doNotRandomizeItems,Do Not Randomize Items,0\n"
           "opt,11,bool,randomItemHardMode,Random Item Hardmode,0\n"
           "opt,12,bool,halveDSSCards,Halve DSS Cards Placed,0\n"
           "opt,13,bool,countdown,Countdown,0\n"
           "opt,14,bool,subShuffle,Subweapon Shuffle,0\n"
           "opt,15,bool,noMPDrain,Disable Battle Arena MP Drain,0\n";
           "opt,16,bool,allBosses,Require All Bosses,0\n";
}

static void set_option_val(struct seed_options *options, int opt, int val) {
    switch(opt) {
        case 0:
            options->ignoreCleansing = val;
            break;
        case 1:
            options->applyAutoRunPatch = val;
            break;
        case 2:
            options->applyNoDSSGlitchPatch = val;
            break;
        case 3:
            options->applyAllowSpeedDash = val;
            break;
        case 4:
            options->breakIronMaidens = val;
            break;
        case 5:
            options->lastKeyRequired = val;
            break;
        case 6:
            options->lastKeyAvailable = val;
            break;
        case 7:
            options->applyBuffFamiliars = val;
            break;
        case 8:
            options->applyBuffSubweapons = val;
            break;
        case 9:
            options->applyShooterStrength = val;
            break;
        case 10:
            options->doNotRandomizeItems = val;
            break;
        case 11:
            options->RandomItemHardMode = val;
            break;
        case 12:
            options->halveDSSCards = val;
            break;
        case 13:
            options->countdown = val;
            break;
        case 14:
            options->subweaponShuffle = val;
            break;
        case 15:
            options->noMPDrain = val;
            break;
        case 16:
            options->allBossesRequired = val;
            break;
    }
}

int cotmr_randomize(int seed, char* option_str) {
    FILE *rom;
    char* param_pos;

    struct seed_options options = {
        false,
        false,
        false,
        false,
        false,
        1,
        1,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false
    };

    param_pos = strtok(option_str,",");
    while(param_pos != NULL) {
        int param_name, param_val;
        sscanf(param_pos, "%i=%i", &param_name, &param_val);

        set_option_val(&options, param_name, param_val);

        param_pos = strtok(NULL, ",");
    }

    if(options.lastKeyRequired > options.lastKeyAvailable) {
        options.lastKeyAvailable = options.lastKeyRequired;
    }

    rom = fopen("rom.gba", "r+b");
    if (rom == NULL)
    {
        printf("Error opening rom.\nPress enter to close.\n");
        return 1;
    }

    int patch_success = randompatch(rom, seed, &options);

    fclose(rom);

    return patch_success;
}
