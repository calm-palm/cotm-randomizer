#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>

struct seed_options {
    bool ignoreCleansing;
    bool applyAutoRunPatch;
    bool applyNoDSSGlitchPatch;
    bool applyAllowSpeedDash;
    bool breakIronMaidens;
    int lastKeyRequired;
    int lastKeyAvailable;
    bool applyBuffFamiliars;
    bool applyBuffSubweapons;
    bool applyShooterStrength;
    bool doNotRandomizeItems;
    bool RandomItemHardMode;
    bool halveDSSCards;
    bool countdown;
    bool subweaponShuffle;
};

#endif
