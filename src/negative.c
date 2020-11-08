#include "qdbmp.h"
#include <stdio.h>

int convert_to_negative_using_external_lib(char **argv) {

    UCHAR r, g, b;
    UINT width, height;
    UINT x, y;
    BMP *bmp;

    bmp = BMP_ReadFile(argv[2]);
    BMP_CHECK_ERROR(stdout, -3);

    width = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    if (bmp->Header.BitsPerPixel == 24) {
        for (x = 0; x < width; ++x) {
            for (y = 0; y < height; ++y) {
                BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);
                BMP_SetPixelRGB(bmp, x, y, ~r, ~g, ~b);
            }
        }
    } else if (bmp->Header.BitsPerPixel == 8) {
        for (UINT i = 0; i < BMP_PALETTE_SIZE_8bpp; ++i)
            // We shouldn't touch every fourth byte, as the format requires
            if ((i + 1) % 4 != 0)
                bmp->Palette[i] = ~bmp->Palette[i];
    } else {
        fprintf(stderr, "An incorrect bits per pixel count! You can use only 8bpp and 24 bpp!\n");
        return -3;
    }

    BMP_WriteFile(bmp, argv[3]);
    BMP_CHECK_ERROR(stdout, -3);
    BMP_Free(bmp);

    return 0;
}

