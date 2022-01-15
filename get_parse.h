// get_parse.h - library for parsing QBASIC, SCREEN 13, GET data

#ifndef GET_PARSE_H
#define GET_PARSE_H

#include "loadBSV.h"

#define GET_ERR_END_OF_DATA -1
#define GET_ERR_INCOMPLETE_IMAGE -2

typedef struct {
  uint16_t raw_width;
  uint16_t height;
  uint16_t width;   // derived for convenience
  unsigned int len; // derived for convenience
  uint8_t* data;
} GET_image;

// returns the number of images in the given data array, or GET_ERR_INCOMPLETE_IMAGE if an incomplete image is found.
int getNumImages(BSV *src) {
//(uint8_t* data, int len) {
  // walks the passed BSV, counting image records as it goes. Returns number of images found, or a negative number if an error encountered
  int count = 0;
  for (int i = 0; i < src->len; i++) {
    // read raw width
    if (i + 2 >= src->len) break;
    unsigned int width = (src->data[i] | src->data[i+1] << 8) >> 3;
    //DEBUG printf("data[$%x] = $%X\n", i, src->data[i]);
    //DEBUG printf("data[$%x] = $%X\n", i+1, src->data[i+1]);
    i += 2;// keep track of where we're up to.
    // read height
    if (i + 2 >= src->len) break;
    unsigned int height = (src->data[i] | src->data[i+1] << 8);
    //DEBUG printf("data[$%x] = $%X\n", i, src->data[i]);
    //DEBUG printf("data[$%x] = $%X\n", i+1, src->data[i+1]);
    i += 2;

    //printf("w:%i h:%i size:%i\n", width, height, width * height);
    // >>>>>>>>need to trace exactly what's being looked at here.<<<<<
    // skip width * height elements
    i += width * height - 1; // I'm not 100% sure why the -1 is needed. it is an off-by-one error, for sure

    // if the data runs off the end of the array, return as if that last one doesn't exist.
    //DEBUG printf("i:%i, src->len:%i\n", i, src->len);
    if (i >= src->len) break;

    count++;
    //DEBUG printf("w:$%x h:$%x i:$%x count:$%x\n", width, height, i, count);
  }
  return count;
}

// just use this one; it does everything we need for LD2
void loadImgArray(BSV* in, int len, GET_image imgs[]) {
  int pos = 0;  // start of the current record
  int imgNum = 0;
  while (imgNum < len && pos < in->len) {
    imgs[imgNum].raw_width = in->data[pos + 0] | (in->data[pos + 1] << 16);
    imgs[imgNum].width = imgs[imgNum].raw_width >> 3;
    imgs[imgNum].height = in->data[pos + 2] | (in->data[pos + 3] << 16);
    imgs[imgNum].len = imgs[imgNum].width * imgs[imgNum].height;
    // now read in the image data
    imgs[imgNum].data = (uint8_t *) calloc(imgs[imgNum].len, sizeof(uint8_t));
    for (int j = 0; j < imgs[imgNum].len; j++) {
      imgs[imgNum].data[j] = in->data[4 + j + pos];
    }
    pos += 4 + imgs[imgNum].len;
    imgNum++;
  }
}

// this produces incorrect results
GET_image* getImage(BSV *src, int imageNum) {//(uint8_t* data, int len, int imageNum) {
  // extracts an image from raw GET data, indexed by image(0-based). Returns a pointer to an allocated GET_image, or NULL if an error ocurred.
  // walk the array, up to the image number required.
  int currImg = 0;
  int pos = 0;

  unsigned int width;
  unsigned int height;
  while(currImg < imageNum) {
    // read raw width
    if (pos + 2 >= src->len) return NULL;
    
    unsigned int width = (src->data[pos] & src->data[pos+1] << 8) >> 3;
    pos += 2;// keep track of where we're up to.
    
    // read height
    if (pos + 2 >= src->len) return NULL;
    unsigned int height = (src->data[pos] & src->data[pos+1] << 8);
    pos += 2;

    // skip width * height elements
    pos += width * height;

    // if the data runs off the end of the array, return failure.
    if (pos >= src->len) return NULL;
    
    currImg++;
  }
  
  // pos now equals the start of the image we want.
  //read width
  width = (src->data[pos] & src->data[pos+1] << 8) >> 3;
  pos += 2;

  //read height
  height = (src->data[pos] & src->data[pos+1] << 8);
  pos += 2;

  unsigned int datalen = width * height;

  //range check
  if (datalen > (src->len - pos)) return NULL;

  //create GET_image
  GET_image* ret = (GET_image*) malloc(sizeof(GET_image));
  ret->data = (uint8_t*) malloc(sizeof(uint8_t) * datalen);

  //read data
  for (int i = 0; i < datalen; i++) {
    ret->data[i] = src->data[pos + i];
  }
  return ret;
}

#endif