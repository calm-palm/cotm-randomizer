#ifndef SPOILERTRANSLATOR_H_INCLUDED
#define SPOILERTRANSLATOR_H_INCLUDED

#include <stdio.h>

void writePedestal(FILE* spoiler_log, int choice);
void writePedestalItem(FILE* spoiler_log, int choice);
void writeDSSCard(FILE* spoiler_log, int choice);
void writeEnemy(FILE* spoiler_log, int choice);
void writeDropItem(FILE* spoiler_log, int choice);

#endif
