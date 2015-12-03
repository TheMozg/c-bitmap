#include "stdio.h"
#include "bitmap.h"
#include "string.h"

int main(int argc, char const *argv[]) {
  const char* filename;
  bmp_image_t* image;
  motion_t motion;

  /*parse args*/
  if (argc != 3) {
    fputs("Too few/many args!\n", stderr);
    return -1;
  }
  if(strcmp(argv[2], "right")!=0 && strcmp(argv[2], "left")!=0) {
    fputs("Wrong args!\n", stderr);
    return -1;
  }
  filename = argv[1];
  if(strcmp(argv[2], "right")==0)
    motion = CLOCKWISE;
  else
    motion = C_CLOCKWISE;

  /*do stuff*/
  if((image = read_file(filename)) == NULL){
    fputs("Whoops. Couldn't read in that image.\n", stderr);
    return -1;
  }

  if(rotate_image(image, motion) == GENERIC_ERROR){
    fputs("Image doesn't feel like rotating today.\n", stderr);
    return -1;
  }

  if(write_file(filename, image) == GENERIC_ERROR){
    fputs("Can't write it. Blame the user.\n", stderr);
    return -1;
  }
  return 0;
}
