#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "addresses.h"
#include "options.h"
#include "randpatcher.h"
#include "menu.h"

#define SEED_FILENAME "seed.txt"
#define OUTPUT_ROM_FILENAME "cotm_randomized.gba"
#define BUFFER_SIZE 1024
#define OPTIONS_FILENAME "options.txt"

int main (int argc, char *argv[])
{
    FILE *source_rom;
    FILE *rom;
    FILE *seed;
    FILE *optionsfile;
    bool headless;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    unsigned int randomizer_seed;

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
        false,
        false,
        false,
        false
    };

    // Validate program is being run with an input ROM
    if ((argc != 2) && (argc != 3))
    {
        printf("Incorrect syntax.\nSyntax example: .\\rando.exe <ROM Name>.gba\nAlternatively, drag the ROM onto the executable.\nPress enter to close.\n");
        getchar();
        return 1;
    }
    if (argc == 3){
        if (strcmp(argv[3], "headless") != 0){
            headless = true
        }
    }
    // Print a little credits line
    printf("Castlevania: Circle of the Moon Randomizer\nReverse engineering and game program: DevAnj\nOptional patches: fusecavator\nRandomizer logic and patch program: spooky\nEmotional support: Circle of the Moon Speedrunning Discord @ https://discord.gg/Ae7Qjd5xDu\n");
    if (!headless){
        getchar();
    }
    // First, copy the original ROM
    source_rom = fopen(argv[1], "rb");
    rom = fopen(OUTPUT_ROM_FILENAME, "wb");
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, source_rom)) > 0)
    {
        fwrite(buffer, 1, bytes_read, rom);
    }
    fclose(source_rom);
    fclose(rom);

    // Open ROM
    printf("Patching ROM %s.\nPress enter to continue or Ctrl + C to exit.\n", OUTPUT_ROM_FILENAME);
    if (!headless){
        getchar();
    }
    rom = fopen(OUTPUT_ROM_FILENAME, "r+b");
    if (rom == NULL)
    {
        printf("Error opening file %s.\nPress enter to close.\n", OUTPUT_ROM_FILENAME);
        return 1;
    }

    // Get seed and seed random number generator
    seed = fopen(SEED_FILENAME, "r");
    if (seed == NULL)
    {
        printf("Error opening file %s.\nPress enter to close.\n", SEED_FILENAME);
        return 1;
    }
    fscanf(seed, "%u", &randomizer_seed);
    printf("Using seed %u from %s. The seed must be an unsigned integer. If you specified a seed but it is being\nreported as zero, you may have included invalid characters like letters or symbols.\nPress enter to continue or Ctrl + C to exit.\n", randomizer_seed, SEED_FILENAME);
    if (!headless){
        getchar();
    }

    // Warn that the patch will be overwritten and open for writing
    printf("The randomizer patch will be written to %s.\nWarning: If this file already exists, it will be overwritten. Press enter to continue or Ctrl + C to exit.\n", IPS_RANDOMIZER);
    if (!headless){
        getchar();
    }
    optionsfile = fopen(OPTIONS_FILENAME, "r");
    if (optionsfile == NULL){
        printf("No Options file, using default options currently; opening the menu if not headless");
    }
    else
    {
        fscanf(optionsfile, "ignoreCleansing %d", options.ignoreCleansing);
        fscanf(optionsfile, "applyAutoRunPatch %d", options.applyAutoRunPatch);
        fscanf(optionsfile, "applyNoDSSGlitchPatch %d", options.applyNoDSSGlitchPatch);
        fscanf(optionsfile, "applyAllowSpeedDash %d", options.applyAllowSpeedDash);
        fscanf(optionsfile, "breakIronMaidens %d", options.breakIronMaidens);
        fscanf(optionsfile, "lastKeyRequired %d", options.lastKeyRequired);
        fscanf(optionsfile, "lastKeyAvailable %d", options.lastKeyAvailable);
        fscanf(optionsfile, "applyBuffFamiliars %d", options.applyBuffFamiliars);
        fscanf(optionsfile, "applyBuffSubweapons %d", options.applyBuffSubweapons);
        fscanf(optionsfile, "applyShooterStrength %d", options.applyShooterStrength);
        fscanf(optionsfile, "doNotRandomizeItems %d", options.doNotRandomizeItems);
        fscanf(optionsfile, "RandomItemHardMode %d", options.RandomItemHardMode);
        fscanf(optionsfile, "halveDSSCards %d", options.halveDSSCards);
        fscanf(optionsfile, "countdown %d", options.countdown);
        fscanf(optionsfile, "subweaponShuffle %d", options.subweaponShuffle);
        fscanf(optionsfile, "noMPDrain %d", options.noMPDrain);
        fscanf(optionsfile, "allBossesRequired %d", options.allBossesRequired);
        fscanf(optionsfile, "dssRunSpeed %d", options.dssRunSpeed);
        fscanf(optionsfile, "skipCutscenes %d", options.skipCutscenes);
        fscanf(optionsfile, "skipMagicItemTutorials %d", options.skipMagicItemTutorials);
    }
    // Open the menu to allow selecting optional settings and patches
    if (!headless){
        optionMenu(&options);
    }
    int patch_success = randompatch(rom, randomizer_seed, &options);

    fclose(rom);

    if(patch_success == 0) {
        printf("Completed successfully.\nPress enter to close.\n");
    }
    if (!headless){
        getchar();
    }
    return patch_success;
}
