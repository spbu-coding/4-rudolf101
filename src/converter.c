#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_handler.h"

#define REQUIRED_NUMBER_OF_PARAMETERS 4

#define error(...) (fprintf(stderr, __VA_ARGS__))

extern int convert_to_negative_using_external_lib(char **);

int convert_bpp8_to_negative(FILE *input, FILE *output, BMP_Header *bmp) {

    unsigned char palette[BMP_PALETTE_SIZE_8bpp];

    if (fread(palette, BMP_PALETTE_SIZE_8bpp, 1, input) != 1)
        return BMP_INPUT_ERROR_CODE;

    for (int i = 0; i < BMP_PALETTE_SIZE_8bpp; ++i)
        // We shouldn't touch every fourth byte, as the format requires
        if ((i + 1) % 4 != 0)
            palette[i] = ~palette[i];

    if (fwrite(palette, BMP_PALETTE_SIZE_8bpp, 1, output) != 1)
        return BMP_OUTPUT_ERROR_CODE;

    unsigned char *pixel_data = (unsigned char *) malloc(sizeof(unsigned char) * bmp->data_uncompressed_size);

    if (fread(pixel_data, bmp->data_uncompressed_size, 1, input) != 1)
        return BMP_INPUT_ERROR_CODE;

    if (fwrite(pixel_data, bmp->data_uncompressed_size, 1, output) != 1)
        return BMP_OUTPUT_ERROR_CODE;

    free(pixel_data);
    return 0;
}

int convert_bpp24_to_negative(FILE *input, FILE *output, BMP_Header *bmp) {

    unsigned char *pixel_data = (unsigned char *) malloc(sizeof(unsigned char) * bmp->data_uncompressed_size);

    if (fread(pixel_data, bmp->data_uncompressed_size, 1, input) != 1)
        return BMP_INPUT_ERROR_CODE;

    for (int i = 0; i < bmp->data_uncompressed_size; ++i)
        pixel_data[i] = ~pixel_data[i];

    if (fwrite(pixel_data, bmp->data_uncompressed_size, 1, output) != 1)
        return BMP_OUTPUT_ERROR_CODE;

    free(pixel_data);
    return 0;
}

int convert_to_negative_using_my_implementation(char **argv) {

    char *input_file_name = argv[2];
    char *output_file_name = argv[3];
    FILE *input, *output;

    if ((input = fopen(input_file_name, "rb")) == NULL) {
        error("%s", ERROR_STRING[OPEN_FILE_ERROR_CODE]);
        return -1;
    }

    BMP_Header bmp;

    int return_value = check_error(read_header(&bmp, input));
    if (return_value)
        return return_value;

    return_value = check_error(check_header(&bmp));
    if (return_value)
        return return_value;

    if ((output = fopen(output_file_name, "wb")) == NULL) {
        error("%s", ERROR_STRING[OPEN_FILE_ERROR_CODE]);
        return -1;
    }

    return_value = check_error(write_header(&bmp, output));
    if (return_value)
        return return_value;

    if (bmp.bits_per_pixel == 8) {

        return_value = check_error(convert_bpp8_to_negative(input, output, &bmp));
        if (return_value)
            return return_value;

    } else if (bmp.bits_per_pixel == 24) {

        return_value = check_error(convert_bpp24_to_negative(input, output, &bmp));
        if (return_value)
            return return_value;

    }

    fclose(input);
    fclose(output);
    return 0;
}

int main(int argc, char **argv) {

    if (argc != REQUIRED_NUMBER_OF_PARAMETERS) {
        error("%s", ERROR_STRING[INVALID_PARAMETERS_ERROR_CODE]);
        return -1;
    }

    int return_value;

    if (strcmp(argv[1], "--mine") == 0)
        return_value = convert_to_negative_using_my_implementation(argv);

    else if (strcmp(argv[1], "--theirs") == 0)
        return_value = convert_to_negative_using_external_lib(argv);

    else {
        error("%s", ERROR_STRING[INVALID_PARAMETERS_ERROR_CODE]);
        return -1;
    }

    return (return_value ? return_value : 0);
}
