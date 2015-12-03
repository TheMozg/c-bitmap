#include "bitmap.h"
#include "malloc.h"
#include "string.h"

bmp_image_t* read_file(const char *filename){
  FILE* fp;
  bmp_image_t* image;
  uint8_t *data = NULL;

  image = malloc(sizeof(bmp_image_t));

  if ((fp = fopen(filename, "rb")) == NULL)
    return NULL;

  if (fread(&(image->header), sizeof(bmp_header_t), 1, fp) < 1) {
    fclose(fp);
    return NULL;
  }

  if(validate_header(image->header) != SUCCESS)
  {
    fclose(fp);
    return NULL;
  }

  if(fseek(fp, image->header.bOffBits, SEEK_SET) != 0){
    fclose(fp);
    return NULL;
  }

  data = malloc(image->header.biSizeImage*sizeof(bmp_pixel_t));

  if (fread(data, sizeof(uint8_t), image->header.biSizeImage, fp) < image->header.biSizeImage)
  {
    free(data);
    fclose(fp);
    return NULL;
  }

  image->bitmap = deserialize_bitmap(data, image->header.biHeight, image->header.biWidth);
  free(data);

  if (image->bitmap == NULL) {
    fclose(fp);
    return NULL;
  }
  fclose(fp);
  return image;
}

int write_file(const char* filename, const bmp_image_t* image){
  FILE* fp;
  uint8_t *data;

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

  data = serialize_bitmap((const bmp_pixel_t**)image->bitmap, image->header.biHeight, image->header.biWidth);

  if (fwrite(data, sizeof(uint8_t), image->header.biSizeImage, fp) < image->header.biSizeImage)
  {
    fclose(fp);
    return GENERIC_ERROR;
  }
  free(data);
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

uint8_t* serialize_bitmap(const bmp_pixel_t** bitmap, uint32_t height, uint32_t width){
  uint32_t i;
  uint32_t padding = padding_size(width);
  uint32_t twidth = width + padding;
  uint8_t pbuf[4] = {0};
  uint8_t* data = malloc(height*width*sizeof(bmp_pixel_t)+height*padding);
  for (i = 0; i < height; i++) {
    unsigned long offset = i*twidth*sizeof(bmp_pixel_t);
    memcpy(data+offset, bitmap[i], width*sizeof(bmp_pixel_t));
    memcpy(data+offset+width, pbuf, padding);
  }
  return data;
}
bmp_pixel_t** deserialize_bitmap(const uint8_t* data, uint32_t height, uint32_t width){
  uint32_t i;
  uint32_t padding = padding_size(width);
  uint32_t twidth = width + padding;
  bmp_pixel_t** bitmap;
  if ((bitmap = create_bitmap(height, width)) == NULL)
    return NULL;
  for (i = 0; i < height; i++) {
    unsigned long offset = i*twidth*sizeof(bmp_pixel_t);
    memcpy(bitmap[i], data+offset, width*sizeof(bmp_pixel_t));
  }
  return bitmap;
}

int validate_header(bmp_header_t header){
  if (header.bfType != 0x4D42)
  {
    return GENERIC_ERROR;
  }
  if (header.biBitCount != BIT_DEPTH)
  {
    return GENERIC_ERROR;
  }
  return SUCCESS;
}
