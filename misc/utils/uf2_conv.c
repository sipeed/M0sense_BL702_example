#include <stdio.h>
#include <string.h>

#include "../../libs/uf2_format/uf2_format.h"

typedef struct uf2_block UF2_Block;

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "USAGE: %s file.bin [file.uf2]\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        fprintf(stderr, "No such file: %s\n", argv[1]);
        return 1;
    }

    fseek(f, 0L, SEEK_END);
    uint32_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    const char *outname = argc > 2 ? argv[2] : "flash.uf2";

    FILE *fout = fopen(outname, "wb");

    UF2_Block bl;
    memset(&bl, 0, sizeof(bl));

    bl.magic_start0 = UF2_MAGIC_START0;
    bl.magic_start1 = UF2_MAGIC_START1;
    bl.magic_end = UF2_MAGIC_END;
    bl.flags = UF2_FLAG_NOT_MAIN_FLASH | UF2_FLAG_FAMILY_ID_PRESENT;
    bl.family_id = M0SENSE_FAMILY_ID;
    bl.target_addr = 0;
    bl.num_blocks = (sz + 255) / 256;
    bl.payload_size = 256;
    int numbl = 0;
    while (fread(bl.data, 1, bl.payload_size, f)) {
        bl.block_no = numbl++;
        fwrite(&bl, 1, sizeof(bl), fout);
        bl.target_addr += bl.payload_size;
        // clear for next iteration, in case we get a short read
        memset(bl.data, 0, sizeof(bl.data));
    }
    fclose(fout);
    fclose(f);
    printf("Wrote %d blocks to %s\n", numbl, outname);
    return 0;
}