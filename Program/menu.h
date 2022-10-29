#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#define MENU_NONE -1
#define MENU_COMPLETED 0
#define MENU_CLEANSING 1
#define MENU_AUTORUN 2
#define MENU_NODSSGLITCH 3
#define MENU_BREAKIRONMAIDENS 4
#define MENU_SETKEYSREQUIRED 5
#define MENU_SETKEYSAVAILABLE 6
#define MENU_DISABLEITEMRANDO 7
#define MENU_ITEMRANDOHARDMODE 8
#define MENU_BALANCECHANGES 9
#define MENU_SPEEDDASH 10
#define MENU_HALVEDSSCARDS 11
#define MENU_COUNTDOWN 12
#define MENU_SUBWEAPONSHUFFLE 13

#define BALMENU_BUFFFAMILIARS 1
#define BALMENU_BUFFSUBWEAPONS 2
#define BALMENU_BUFFSHOOTER 3

#define MENU_KEY_MINIMUM 0
#define MENU_KEY_MAXIMUM 9

#include <stdbool.h>
#include "options.h"

//bool* ignoreCleansing, bool* applyAutoRunPatch, bool* applyNoDSSGlitchPatch, bool* breakIronMaidens, 
void optionMenu(struct seed_options *options);

#endif
