#ifndef T4_CONVERT_BMP_HANDLER_H
#define T4_CONVERT_BMP_HANDLER_H

#define BMP_PALETTE_SIZE_8bpp (256 * 4)
#define BMP_OFFSET_OF_BITMAP 54
#define BMP_HEADER_SIZE 40
#define INT_SIZE 4
#define SHORT_SIZE 2

#define INVALID_PARAMETERS_ERROR_CODE 1
#define OPEN_FILE_ERROR_CODE 2
#define BMP_INPUT_ERROR_CODE 3
#define BMP_OUTPUT_ERROR_CODE 4
#define BMP_IDENTIFICATION_ERROR_CODE 5
#define BMP_RESERVED_BYTES_ERROR_CODE 6
#define BMP_OFFSET_OF_BITMAP_ERROR_CODE 7
#define BMP_HEADER_SIZE_ERROR_CODE 8
#define BMP_COLOR_PLANES_ERROR_CODE 9
#define BMP_BPP_ERROR_CODE 10
#define BMP_COMPRESSION_ERROR_CODE 11
#define BMP_IMAGE_SIZE_ERROR_CODE 12
#define BMP_WITH_NEGATIVE_ERROR_CODE 13

static const char *ERROR_STRING[] = {
        "",
        "You should enter exactly 3 parameters in the format:\n--mine(--theirs) <input_name>.bmp <output_name>.bmp\n",
        "Unable to open the input file or create the output file.\nCheck the filenames and the permission to create files in the folder.\n",
        "Cannot read data from the BMP file, it may be corrupted.\n",
        "Data cannot be written to a new file, check the permission to write to the file.\n",
        "The file is corrupted or a non-BMP file is being used.\n",
        "Expected to see '00' in reserved bytes, found a different value.\n",
        "An incorrect offset of bitmap image data.\nCheck the version of your BMP, you can use only 3rd version.\n",
        "The size of the header was expected to be 40, found a different value.\nVersion of your BMP file must be 3rd.\n",
        "The number of color planes was expected to be 1, found a different value.\n",
        "An incorrect bits per pixel count, you can use only 8bpp and 24 bpp.\n",
        "Expected to see '00' in the type of compression filed, found a different value.\n",
        "An incorrect size of the raw bitmap data.\n",
        "The image width must be a non-negative value.\n"
};

typedef struct BMP_Header {
    unsigned short bmp_id_symbols;
    unsigned long int file_size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned long int data_offset;
    unsigned long int header_size;
    signed long int width;
    signed long int height;
    unsigned short num_of_color_planes;
    unsigned short bits_per_pixel;
    unsigned long int type_of_compression;
    unsigned long int data_uncompressed_size;
    unsigned long int horizontal_resolution;
    unsigned long int vertical_resolution;
    unsigned long int colors_used;
    unsigned long int colors_required;
} BMP_Header;

int read_header(BMP_Header *bmp, FILE *file);

int write_header(BMP_Header *bmp, FILE *file);

int check_header(BMP_Header *bmp);

int check_error(int return_value);

#endif //T4_CONVERT_BMP_HANDLER_H
