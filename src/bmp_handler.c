#include <stdio.h>
#include <stdlib.h>
#include "bmp_handler.h"

void read_INT(long int *value, const unsigned char *buff, unsigned int *offset) {
    *value = (buff[*offset + 3] << 24 | buff[*offset + 2] << 16 | buff[*offset + 1] << 8 | buff[*offset]);
    *offset += INT_SIZE;
}

void read_USHORT(unsigned short *value, const unsigned char *buff, unsigned int *offset) {
    *value = (buff[*offset + 1] << 8u | buff[*offset]);
    *offset += SHORT_SIZE;
}

void read_UINT(unsigned long int *value, const unsigned char *buff, unsigned int *offset) {
    *value = (buff[*offset + 3] << 24u | buff[*offset + 2] << 16u | buff[*offset + 1] << 8u | buff[*offset]);
    *offset += INT_SIZE;
}

void write_INT(long int value, unsigned char *buff, unsigned int *offset) {
    buff[*offset + 3] = (unsigned char) ((value & 0xff000000) >> 24);
    buff[*offset + 2] = (unsigned char) ((value & 0x00ff0000) >> 16);
    buff[*offset + 1] = (unsigned char) ((value & 0x0000ff00) >> 8);
    buff[*offset] = (unsigned char) (value & 0x000000ff);
    *offset += INT_SIZE;
}

void write_UINT(unsigned long int value, unsigned char *buff, unsigned int *offset) {
    buff[*offset + 3] = (unsigned char) ((value & 0xff000000u) >> 24u);
    buff[*offset + 2] = (unsigned char) ((value & 0x00ff0000u) >> 16u);
    buff[*offset + 1] = (unsigned char) ((value & 0x0000ff00u) >> 8u);
    buff[*offset] = (unsigned char) (value & 0x000000ffu);
    *offset += INT_SIZE;
}

void write_USHORT(unsigned short value, unsigned char *buff, unsigned int *offset) {
    buff[*offset + 1] = (unsigned char) ((value & 0xff00u) >> 8u);
    buff[*offset] = (unsigned char) (value & 0x00ffu);
    *offset += SHORT_SIZE;
}

int check_error(int return_value){
    if(!return_value) return 0;

    fprintf(stderr, "%s", ERROR_STRING[return_value]);

    //Error codes are ordered, all codes up to BMP_IDENTIFICATION_ERROR_CODE are responsible for errors not related to the violation of the BMP file structure.
    if(return_value <= BMP_IDENTIFICATION_ERROR_CODE)
        return -1;
    else
        return -2;
}

int read_header(BMP_Header *bmp, FILE *file) {

    unsigned char buff[BMP_OFFSET_OF_BITMAP];

    if (fread(buff, BMP_OFFSET_OF_BITMAP, 1, file) != 1)
        return BMP_INPUT_ERROR_CODE;

    unsigned int offset = 0;

    read_USHORT(&(bmp->bmp_id_symbols), buff, &offset);
    read_UINT(&(bmp->file_size), buff, &offset);
    read_USHORT(&(bmp->reserved1), buff, &offset);
    read_USHORT(&(bmp->reserved2), buff, &offset);
    read_UINT(&(bmp->data_offset), buff, &offset);
    read_UINT(&(bmp->header_size), buff, &offset);
    read_INT(&(bmp->width), buff, &offset);
    read_INT(&(bmp->height), buff, &offset);
    read_USHORT(&(bmp->num_of_color_planes), buff, &offset);
    read_USHORT(&(bmp->bits_per_pixel), buff, &offset);
    read_UINT(&(bmp->type_of_compression), buff, &offset);
    read_UINT(&(bmp->data_uncompressed_size), buff, &offset);
    read_UINT(&(bmp->horizontal_resolution), buff, &offset);
    read_UINT(&(bmp->vertical_resolution), buff, &offset);
    read_UINT(&(bmp->colors_used), buff, &offset);
    read_UINT(&(bmp->colors_required), buff, &offset);

    return 0;
}

int write_header(BMP_Header *bmp, FILE *file) {

    unsigned char buff[BMP_OFFSET_OF_BITMAP];
    unsigned int offset = 0;

    write_USHORT(bmp->bmp_id_symbols, buff, &offset);
    write_UINT(bmp->file_size, buff, &offset);
    write_USHORT(bmp->reserved1, buff, &offset);
    write_USHORT(bmp->reserved2, buff, &offset);
    write_UINT(bmp->data_offset, buff, &offset);
    write_UINT(bmp->header_size, buff, &offset);
    write_INT(bmp->width, buff, &offset);
    write_INT(bmp->height, buff, &offset);
    write_USHORT(bmp->num_of_color_planes, buff, &offset);
    write_USHORT(bmp->bits_per_pixel, buff, &offset);
    write_UINT(bmp->type_of_compression, buff, &offset);
    write_UINT(bmp->data_uncompressed_size, buff, &offset);
    write_UINT(bmp->horizontal_resolution, buff, &offset);
    write_UINT(bmp->vertical_resolution, buff, &offset);
    write_UINT(bmp->colors_used, buff, &offset);
    write_UINT(bmp->colors_required, buff, &offset);

    if (fwrite(buff, BMP_OFFSET_OF_BITMAP, 1, file) != 1)
        return BMP_OUTPUT_ERROR_CODE;

    return 0;
}

int check_header(BMP_Header *bmp) {

    if (bmp->bmp_id_symbols != 0x4D42)
        return BMP_IDENTIFICATION_ERROR_CODE;

    if (bmp->reserved1 != 0 || bmp->reserved2 != 0)
        return BMP_RESERVED_BYTES_ERROR_CODE;

    // Data offset must be less then size of the BMPFILEHEADER + size of the BMPINFO
    if (bmp->data_offset < BMP_OFFSET_OF_BITMAP)
        return BMP_OFFSET_OF_BITMAP_ERROR_CODE;

    if (bmp->header_size != BMP_HEADER_SIZE)
        return BMP_HEADER_SIZE_ERROR_CODE;

    if (bmp->width < 0)
        return BMP_WITH_NEGATIVE_ERROR_CODE;

    if (bmp->num_of_color_planes != 1)
        return BMP_COLOR_PLANES_ERROR_CODE;

    if (bmp->bits_per_pixel != 8 && bmp->bits_per_pixel != 24)
        return BMP_BPP_ERROR_CODE;

    if (bmp->type_of_compression)
        return BMP_COMPRESSION_ERROR_CODE;

    // If the size of pixel storage is zero -> calculate the total number of bytes necessary to store pixels
    if (bmp->data_uncompressed_size == 0)
        bmp->data_uncompressed_size = (bmp->bits_per_pixel * bmp->width + 31) / 32 * 4 * abs(bmp->height);

    // If the size of pixel storage more than the size of the file without offset data or size of pixel storage still zero -> incorrect size
    if (bmp->data_uncompressed_size + bmp->data_offset > bmp->file_size || bmp->data_uncompressed_size == 0)
        return BMP_IMAGE_SIZE_ERROR_CODE;

    return 0;
}
