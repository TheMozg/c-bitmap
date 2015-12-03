#ifndef BITMAP_H
#define BITMAP_H

#include "stdint.h"

#define PADDING_SIZE 4
#define BIT_DEPTH 24
#define GENERIC_ERROR -1
#define SUCCESS 0

typedef enum {
  CLOCKWISE,
  C_CLOCKWISE
} motion_t;

#pragma pack(push, 2)
typedef struct {
  uint16_t bfType;
  uint32_t bfileSize;
  uint32_t bfReserved;
  uint32_t bOffBits;
  uint32_t biSize;
  uint32_t biWidth;
  uint32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  uint32_t biXPelsPerMeter;
  uint32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
} bmp_header_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
  uint8_t R;
  uint8_t G;
  uint8_t B;
} bmp_pixel_t;
#pragma pack(pop)

typedef struct {
  bmp_header_t header;
  bmp_pixel_t** bitmap;
} bmp_image_t;

bmp_image_t* read_file(const char*);
int write_file(const char*, const bmp_image_t*);
uint8_t padding_size(uint32_t width);
void free_bitmap(bmp_pixel_t**, uint32_t);
int rotate_image(bmp_image_t*, motion_t);
bmp_pixel_t** rotate_bitmap(const bmp_pixel_t**,uint32_t, uint32_t,motion_t);
bmp_pixel_t** create_bitmap(uint32_t, uint32_t);

#endif
