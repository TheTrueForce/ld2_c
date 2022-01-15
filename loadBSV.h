// loadBSV.h - BSV loader library for C
// This library does not attempt to respect the VARSEG or VARPTR fields, as these are largely irrelevant to C.
//  It does load them, however.

#ifndef LOADBSV_H
#define LOADBSV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BSV_MAGIC 0xFD
#define BSV_ERR_MAGIC -3
#define BSV_ERR_EOF   -1
#define BSV_ERR_FILE  -2

// header structure. This is not preallocated by this library.
typedef struct {
  // should always be 0xFD; here for completeness
  uint8_t magic;
  // The x86 data segment the data originated from.
  uint16_t VARSEG;
  // The offset from VARSEG that the data originated from. Neither of these are used by this library.
  uint16_t VARPTR;
  // The length of the file's payload (the data array).
  uint16_t len;
  // A pointer to the actual data. This array is allocated by loadBSV(), and needs to be either freed manually or via freeBSV().
  uint8_t* data;
} BSV;

// grabs a uint16_t from stream, in little-endian format. This is not specific to the BSV library, but is named the way it is to
//  avoid name collisions.
// returns
// 0           on success
// BSV_ERR_EOF on unexpected EOF.
int BSV_getShort(uint16_t* dest, FILE* stream) {
  int acquiredVal = fgetc(stream);
  if (acquiredVal == EOF) {
    fclose(stream);
    return BSV_ERR_EOF;
  }
  uint16_t output = acquiredVal;

  acquiredVal = fgetc(stream);
  if (acquiredVal == EOF) {
    fclose(stream);
    return BSV_ERR_EOF;
  }
  output |= acquiredVal << 8;

  *dest = output;

  return 0;
}

// loads a BSV file using the data structure above. Allocates the data array.
// Returns:
// 0             on success
// BSV_ERR_EOF   on unexpected EOF
// BSV_ERR_FILE  on failed to open file
// BSV_ERR_MAGIC on wrong magic
int loadBSV(const char* filename, BSV* dest) {
  // open the file
  FILE* inFile = fopen(filename, "r");
  if (inFile == NULL) {
    return BSV_ERR_FILE;
  }
  
  // read and validate the magic
  int acquiredVal = fgetc(inFile);
  if (acquiredVal == EOF) {
    fclose(inFile);
    return BSV_ERR_EOF;
  }
  if ((uint8_t)acquiredVal != BSV_MAGIC) {
    fclose(inFile);
    return BSV_ERR_MAGIC;
  }

  dest->magic = (uint8_t) acquiredVal;

  // load VARSEG and VARPTR. Just store them.
  // this code is repetitive. I don't think I can make it any less so, unfortunately
  // VARSEG
  uint16_t temp;
  acquiredVal = BSV_getShort(&temp, inFile);
  if (acquiredVal) {
    fclose(inFile);
    return acquiredVal;
  }
  dest->VARSEG = temp;

  // VARPTR
  acquiredVal = BSV_getShort(&temp, inFile);
  if (acquiredVal) {
    fclose(inFile);
    return acquiredVal;
  }
  dest->VARPTR = temp;

  // load len, and malloc the array
  acquiredVal = BSV_getShort(&temp, inFile);
  if (acquiredVal) {
    fclose(inFile);
    return acquiredVal;
  }
  dest->len = temp;

  dest->data = (uint8_t *)malloc(dest->len);

  // read in the data
  for (int i = 0; i <= dest->len; i++){
    acquiredVal = fgetc(inFile);
    if (acquiredVal == EOF) {
      fclose(inFile);
      return BSV_ERR_EOF;
    }
    dest->data[i] = (uint8_t) acquiredVal;
  }

  // if we get here, we have succeeded.
  return 0;
}

// frees the data array in a BSV structure.
void freeBSV(BSV *bsv) {
  free(bsv->data);
}
#endif
