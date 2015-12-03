#include "bitmap.h"
#include "malloc.h"
#include "string.h"

bmp_image_t* read_file(const char *filename){
  FILE* fp;
  bmp_image_t* image;
  uint32_t i;
  uint8_t padding;
  uint32_t width;
  uint32_t height;

  image = malloc(sizeof(bmp_image_t));

  if ((fp = fopen(filename, "rb")) == NULL)
    return NULL;

  if (fread(&(image->header), sizeof(bmp_header_t), 1, fp) < 1) {
    fclose(fp);
    return NULL;
  }

  if ((char)image->header.bfType != 'B' || image->header.bfType >> 8 != 'M')
  {
    fclose(fp);
    return NULL;
  }

  if (image->header.biBitCount != BIT_DEPTH)
  {
    fclose(fp);
    return NULL;
  }

  if(fseek(fp, image->header.bOffBits, SEEK_SET) != 0){
    fclose(fp);
    return NULL;
  }

  width = image->header.biWidth;
  height = image->header.biHeight;
  padding = padding_size(width);

  if ((image->bitmap = create_bitmap(height, width)) == NULL) {
    fclose(fp);
    return NULL;
  }
  for (i = 0; i < height; i++) {
    if (fread(image->bitmap[i], sizeof(bmp_pixel_t), width, fp) < width)
    {
      free_bitmap(image->bitmap, height);
      fclose(fp);
      return NULL;
    }
    if(fseek(fp, padding, SEEK_CUR) != 0){
      free_bitmap(image->bitmap, height);
      fclose(fp);
      return NULL;
    }
  }

  fclose(fp);
  return image;
}

int write_file(const char* filename, const bmp_image_t* image){
  FILE* fp;
  uint32_t i;
  uint8_t pbuf[4] = {0};
  uint8_t padding;
  uint32_t width;
  uint32_t height;

  if(image == NULL)
    return GENERIC_ERROR;

  if ((fp = fopen(filename, "wb")) == NULL)
    return GENERIC_ERROR;

  if (fwrite(&(image->header), sizeof(bmp_header_t), 1, fp) < 1)
  {
    fclose(fp);
    return GENERIC_ERROR;
  }

  if(fseek(fp, image->header.bOffBits, SEEK_SET) != 0){
    fclose(fp);
    return GENERIC_ERROR;
  }

  width = image->header.biWidth;
  height = image->header.biHeight;
  padding = padding_size(width);

  for (i = 0; i < height; i++) {
    if (fwrite(image->bitmap[i], sizeof(bmp_pixel_t), width, fp) < width)
    {
      fclose(fp);
      return GENERIC_ERROR;
    }
    if (fwrite(&pbuf, sizeof(uint8_t), padding, fp) < padding)
    {
      fclose(fp);
      return GENERIC_ERROR;
    }
  }

  fclose(fp);
  return SUCCESS;
}

uint8_t padding_size(uint32_t width){
  unsigned long twidth = width*sizeof(bmp_pixel_t);
  if (twidth%PADDING_SIZE == 0)
    return 0;
  return PADDING_SIZE-(twidth%PADDING_SIZE);
}

void free_bitmap(bmp_pixel_t** bitmap, uint32_t height){
  uint32_t i;
  for (i = 0; i < height; i++) {
    free(bitmap[i]);
  }
  free(bitmap);
}

int rotate_image(bmp_image_t* image, motion_t motion){
  uint32_t newwidth;
  uint32_t newheight;
  bmp_pixel_t** newbitmap;

  if(image == NULL)
    return GENERIC_ERROR;

  newwidth = image->header.biHeight;
  newheight = image->header.biWidth;

  newbitmap = rotate_bitmap((const bmp_pixel_t**)image->bitmap,
              image->header.biHeight, image->header.biWidth, motion);
  if (newbitmap == NULL)
    return GENERIC_ERROR;

  free_bitmap(image->bitmap, image->header.biHeight);
  image->header.biWidth = newwidth;
  image->header.biHeight = newheight;
  image->bitmap = newbitmap;
  return SUCCESS;
}

bmp_pixel_t** rotate_bitmap(const bmp_pixel_t** bitmap,
              uint32_t height, uint32_t width,motion_t motion){
  uint32_t i;
  uint32_t j;
  uint32_t newwidth = height;
  uint32_t newheight = width;
  bmp_pixel_t** newbitmap;

  if ((newbitmap = create_bitmap(newheight, newwidth)) == NULL) {
    return NULL;
  }

  for (i = 0; i < newheight; i++) {
    for (j = 0; j < newwidth; j++) {
        if(motion == CLOCKWISE)
          newbitmap[i][j] = bitmap[j][width-1-i];
        else
          newbitmap[i][j] = bitmap[height-1-j][i];
    }
  }
  return newbitmap;
}


bmp_pixel_t** create_bitmap(uint32_t height, uint32_t width){
  uint32_t i;
  bmp_pixel_t** bitmap;
  if ((bitmap = malloc(height * sizeof(bmp_pixel_t*))) == NULL) {
    return NULL;
  }
  for (i = 0; i < height; i++) {
    if ((bitmap[i] = malloc(width * sizeof(bmp_pixel_t))) == NULL){
      free_bitmap(bitmap, height);
      return NULL;
    }
  }
  return bitmap;
}
