#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bmp_handler.h"

#define REQUIRED_NUMBER_OF_PARAMETERS 3

#define error(...) (fprintf(stderr, __VA_ARGS__))

int compare_bmp(BMP_Header *first_image, BMP_Header *second_image, FILE *first_file, FILE *second_file) {

    if (first_image->bits_per_pixel != second_image->bits_per_pixel) {
        error("First image BPP = %hdu, second image BPP = %hdu, cannot compare images with different BPP.\n",
              first_image->bits_per_pixel, second_image->bits_per_pixel);
        return -1;
    }
    if (first_image->width != second_image->width) {
        error("First image width = %lu, second image width = %lu, cannot compare images with different width.\n",
              first_image->width, second_image->width);
        return -1;
    }
    if (abs(first_image->height) != abs(second_image->height)) {
        error("The absolute height of the first image = %ld, it is not equal to the absolute height of the second image = %ld, they cannot be compared.\n",
              first_image->height, second_image->height);
        return -1;
    }

    unsigned char *first_image_data = (unsigned char *) malloc(first_image->data_uncompressed_size);
    unsigned char *second_image_data = (unsigned char *) malloc(second_image->data_uncompressed_size);

    if (fread(first_image_data, first_image->data_uncompressed_size, 1, first_file) != 1) {
        error("Cannot read data from the first BMP file, it may be corrupted.");
        return -1;
    }
    if (fread(second_image_data, second_image->data_uncompressed_size, 1, second_file) != 1) {
        error("Cannot read data from the second BMP file, it may be corrupted.");
        return -1;
    }

    unsigned int count_mismatched_pix = 0;

    long int height = abs(first_image->height);
    long int width = abs(first_image->width);

    if (first_image->height > 0 && second_image->height > 0) {

        for (long int y = 0; y < height; ++y)

            for (long int x = 0; x < width; ++x)

                if (first_image_data[y * x + x] != second_image_data[y * x + x] && count_mismatched_pix < 100) {
                    fprintf(stderr, "%ld %ld\n", x, y);
                    count_mismatched_pix++;
                }

    } else if (first_image->height > 0 && second_image->height < 0) {

        for (long int y = 0; y < height; ++y)

            for (long int x = 0; x < width; ++x)

                if (first_image_data[y * x + x] != second_image_data[(abs(second_image->height) - y - 1) * x + x] &&
                    count_mismatched_pix < 100) {
                    fprintf(stderr, "%ld %ld\n", x, y);
                    count_mismatched_pix++;
                }

    } else if (first_image->height < 0 && second_image->height > 0) {

        for (long int y = 0; y < height; ++y)

            for (long int x = 0; x < width; ++x)

                if (first_image_data[(abs(first_image->height) - y - 1) * x + x] != second_image_data[y * x + x] &&
                    count_mismatched_pix < 100) {
                    fprintf(stderr, "%ld %ld\n", x, y);
                    count_mismatched_pix++;
                }

    } else if (first_image->height < 0 && second_image->height < 0) {

        for (long int y = 0; y < height; ++y)

            for (long int x = 0; x < width; ++x)

                if (first_image_data[(abs(first_image->height) - y - 1) * x + x] !=
                    second_image_data[(abs(second_image->height) - y - 1) * x + x] &&
                    count_mismatched_pix < 100) {
                    fprintf(stderr, "%ld %ld\n", x, y);
                    count_mismatched_pix++;
                }

    }

    free(first_image_data);
    free(second_image_data);

    return (count_mismatched_pix ? -1 : 0);
}

int main(int argc, char **argv) {

    if (argc != REQUIRED_NUMBER_OF_PARAMETERS) {
        error("You should enter exactly 2 names of the files you want to compare in the following form:\n <file1_name>.bmp <file2_name>.bmp");
        return -1;
    }

    char *first_file_name = argv[1];
    char *second_file_name = argv[2];
    FILE *first_file, *second_file;

    if ((first_file = fopen(first_file_name, "rb")) == NULL) {
        error("Unable to open first file, check the filename.\n");
        return -1;
    }

    if ((second_file = fopen(second_file_name, "rb")) == NULL) {
        error("Unable to open second file, check the filename.\n");
        return -1;
    }

    BMP_Header first_image, second_image;

    int return_value = check_error(read_header(&first_image, first_file));
    if (return_value)
        return return_value;

    return_value = check_error(read_header(&second_image, second_file));
    if (return_value)
        return return_value;

    return_value = check_error(check_header(&first_image));
    if (return_value)
        return return_value;

    return_value = check_error(check_header(&second_image));
    if (return_value)
        return return_value;

    return_value = compare_bmp(&first_image, &second_image, first_file, second_file);
    if(return_value)
        return return_value;

    return 0;
}
