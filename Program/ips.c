#include <string.h>
#include "ips.h"

// Internal function for patching.
static void patchOffset(FILE* rom, unsigned int offset, char data[], size_t bytes);

int applyIPS(FILE* rom, char* ips_patch_filename)
{
    char buff[IPS_BUFFER_SIZE] = { '\0' };
    FILE* ips_patch;
    unsigned int target_offset;
    size_t bytes;
    
    ips_patch = fopen(ips_patch_filename, "rb");
    if (!ips_patch)
    {
        printf("File %s appears to not exist.\n", ips_patch_filename);
        return 1;
    }
        
    // Verify file begins with "PATCH"
    fgets(buff, 6, ips_patch);
    if (strncmp(buff, "PATCH", 5) != 0)
    {
        printf("File %s appears to not be an IPS patch.\n", ips_patch_filename);
        return 1;
    }

    // Read three bytes for address to insert payload, but stop if those three bytes are "EOF"
    while(fread(buff, 1, 3, ips_patch) == 3)
    {
        if (strncmp(buff, "EOF", 3) == 0)
        {
            printf("Reached EOF in patch %s\n", ips_patch_filename);
            return 0;
        }

        // If not EOF, this is the offset to write the data to
        target_offset = (((unsigned int)buff[0] << 16) & 0x00FF0000) | 
                        (((unsigned int)buff[1] << 8 ) & 0x0000FF00) | 
                        ((unsigned int)buff[2] & 0x000000FF);

        // Next read should be the two bytes giving size of data
        if(fread(buff, 1, 2, ips_patch) != 2)
        {
            printf("Malformed IPS patch.\n");
            return 1;
        }

        bytes =         (((unsigned int)buff[0] << 8 ) & 0x0000FF00) | 
                        ((unsigned int)buff[1] & 0x000000FF);

        // Normal write
        if (bytes != 0)
        {
            // Read the block of size bytes to write to offset
            if(fread(buff, 1, bytes, ips_patch) != bytes)
            {
                printf("Malformed IPS patch.\n");
                return 1;
            }
            
            // Patch the ROM
            patchOffset(rom, target_offset, buff, bytes);
        }
        // RLE Encoding: Write one byte value x times
        else
        {
            // Read the two byte RLE size to determine how many times to write
            if(fread(buff, 1, 2, ips_patch) != 2)
            {
                printf("Malformed IPS patch.\n");
                return 1;
            }
            bytes =     (((unsigned int)buff[0] << 8 ) & 0x0000FF00) | 
                        ((unsigned int)buff[1] & 0x000000FF);
            
            // Read the one byte size value to repeatedly write
            if(fread(buff, 1, 1, ips_patch) != 1)
            {
                printf("Malformed IPS patch.\n");
                return 1;
            }

            while(bytes > 0)
            {
                patchOffset(rom, target_offset, buff, 1);
                target_offset++;
                bytes--;
            }
        }   

        printf("Wrote to offset: %#010x\n", target_offset);
    }

    printf("Patch %s does not end with EOF.\n", ips_patch_filename);
    return 1;
}

static void patchOffset(FILE* rom, unsigned int offset, char data[], size_t bytes)
{
    fseek(rom, offset, SEEK_SET);
    fwrite(data, sizeof(unsigned char), bytes, rom);

    return;
}
