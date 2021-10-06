#include <stdio.h>
#include "menu.h"

static void displayMenu(struct seed_options *options);
static void balanceOptionMenu(struct seed_options *options);
static void displayBalanceChangesMenu(struct seed_options *options);
static int setMenuNumber(void);

// This is a naive implementation that asssumes the user, approaching the menu with goodwill, does not want to crash the program
void optionMenu(struct seed_options *options)
{
    int selection = MENU_NONE;
    int key_number;

    while (selection != MENU_COMPLETED)
    {
        displayMenu(options);
        scanf("%d", &selection);

        if (selection == MENU_CLEANSING)
        {
            options->ignoreCleansing = !options->ignoreCleansing;
        }
        else if (selection == MENU_AUTORUN)
        {
            options->applyAutoRunPatch = !options->applyAutoRunPatch;
        }
        else if (selection == MENU_NODSSGLITCH)
        {
            options->applyNoDSSGlitchPatch = !options->applyNoDSSGlitchPatch;
        }
        else if (selection == MENU_BREAKIRONMAIDENS)
        {
            options->breakIronMaidens = !options->breakIronMaidens;
        }
        else if (selection == MENU_SETKEYSREQUIRED)
        {
            key_number = setMenuNumber();

            if (key_number > options->lastKeyAvailable)
            {
                printf("\nWarning: The number of keys required to open the door may not be greater than the number placed.\nThe number required has not been changed.\n\n");
            }
            else
            {
                options->lastKeyRequired = key_number;
            }
        }
        else if (selection == MENU_SETKEYSAVAILABLE)
        {
            key_number = setMenuNumber();
            options->lastKeyAvailable = key_number;
        }
        else if (selection == MENU_DISABLEITEMRANDO)
        {
            options->doNotRandomizeItems = !options->doNotRandomizeItems;
        }
        else if (selection == MENU_ITEMRANDOHARDMODE)
        {
            options->RandomItemHardMode = !options->RandomItemHardMode;
        }
        else if (selection == MENU_BALANCECHANGES)
        {
            balanceOptionMenu(options);
        }
        else if (selection == MENU_SPEEDDASH)
        {
            options->applyAllowSpeedDash = !options->applyAllowSpeedDash;
        }
        else if (selection == MENU_HALVEDSSCARDS)
        {
            options->halveDSSCards = !options->halveDSSCards;
        }
    }

    // Flush the input buffer
    while ((getchar()) != '\n');
}

static void balanceOptionMenu(struct seed_options *options)
{
    int selection = MENU_NONE;

    while (selection != MENU_COMPLETED)
    {
        displayBalanceChangesMenu(options);
        scanf("%d", &selection);

        if (selection == BALMENU_BUFFFAMILIARS)
        {
            options->applyBuffFamiliars = !options->applyBuffFamiliars;
        }
        else if (selection == BALMENU_BUFFSUBWEAPONS)
        {
            options->applyBuffSubweapons = !options->applyBuffSubweapons;
        }
        else if (selection == BALMENU_BUFFSHOOTER)
        {
            options->applyShooterStrength = !options->applyShooterStrength;
        }
    }

    // Don't flush the inner input buffer
    // while ((getchar()) != '\n');
}

static void displayMenu(struct seed_options *options)
{
    char checked[] = "*";
    char unchecked[] = " ";

    printf("Enter the number associated with the optional setting below and then press enter to toggle its activation.\n");
    printf("When you wish to finalize your settings, type \"0\" and then press enter to proceed.\n\n");

    printf("[%s] 1. Ignore Cleansing when placing magic items. You may be required to traverse Underground Waterway without Cleansing.\n", options->ignoreCleansing ? checked : unchecked);
    printf("[%s] 2. Enable permanent dash. You will always have the effect of Dash Boots applied without needing to double tap direction inputs.\n", options->applyAutoRunPatch ? checked : unchecked);
    printf("[%s] 3. Disable switching to DSS effects from unobtained cards. You will not be able to use the DSS glitch.\n", options->applyNoDSSGlitchPatch ? checked : unchecked);
    printf("[%s] 4. Break Iron Maidens from the beginning of the game. You will not be required to press the button.\n", options->breakIronMaidens ? checked : unchecked);
    printf("[%d] 5. Set number of Last Keys required to open the door to the Ceremonial Room.\n", options->lastKeyRequired);
    printf("[%d] 6. Set number of Last Keys to be placed on pedestals.\n", options->lastKeyAvailable);
    printf("[%s] 7. Disable item randomization. Enemies will drop their default items.\n", options->doNotRandomizeItems ? checked : unchecked);
    printf("[%s] 8. Random item hard mode. Enemies below 150 HP will drop poor items.\n", options->RandomItemHardMode ? checked : unchecked);
    printf("[%s] 9. Select optional balance changes. Starred when any optional balance change is enabled.\n", (options->applyBuffFamiliars || options->applyBuffSubweapons || options->applyShooterStrength) ? checked : unchecked);
    printf("[%s] 10. Allow activating Pluto + Griffin (increased speed) even when the cards are not obtained.\n", options->applyAllowSpeedDash ? checked : unchecked);
    printf("[%s] 11. Halve the number of placed DSS cards. You will not be able to obtain all DSS cards. Cards could randomly be skewed to action or attribute.\n", options->halveDSSCards ? checked : unchecked);

    printf("\n>");
}

static void displayBalanceChangesMenu(struct seed_options *options)
{
    char checked[] = "*";
    char unchecked[] = " ";

    printf("Enter the number associated with the optional balance change below and then press enter to toggle its activation.\n");
    printf("When you wish you return to the previous menu, type \"0\" and then press enter.\n\n");

    printf("[%s] 1. Doubles the damage dealt by projectiles fired by familiars.\n", options->applyBuffFamiliars ? checked : unchecked);
    printf("[%s] 2. Increase regular and Shooter mode damage for some subweapons and corresponding item crush attacks.\n", options->applyBuffSubweapons ? checked : unchecked);
    printf("[%s] 3. Increase the Shooter mode base strength and strength per level to match Vampirekiller.\n", options->applyShooterStrength ? checked : unchecked);
    
    printf("\n>");
}

static int setMenuNumber(void)
{
    int selection = MENU_NONE;

    while (selection > MENU_KEY_MAXIMUM || selection < MENU_KEY_MINIMUM)
    {
        printf("\nEnter a selection between %i and %i:\n\n>", MENU_KEY_MINIMUM, MENU_KEY_MAXIMUM);
        scanf("%d", &selection);
    }

    return selection;
}