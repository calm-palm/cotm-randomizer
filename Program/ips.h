#ifndef IPS_H_INCLUDED
#define IPS_H_INCLUDED

#include <stdio.h>

#define IPS_BUFFER_SIZE 8192

int applyIPS(FILE* rom, char* ips_patch_filename);

#endif
