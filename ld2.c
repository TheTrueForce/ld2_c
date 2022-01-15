// LD2.c - Larry the Dinosaur 2, C rewrite.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "rcgl/rcgl.h"
#include "ld2.h"

void imgPut(uint8_t* buffer, int x, int y, GET_image image, int flip) {
  // remember the old buffer so we can switch back.
  uint8_t *oldBuf = rcgl_getbuf();
  rcgl_setbuf(buffer);

  // actually draw the sprite
  for (int spriteY = 0; spriteY < image.height; spriteY++) {
    for (int spriteX = 0; spriteX < image.width; spriteX++) {
      if (image.data[spriteY * image.width + spriteX] != 0) {
        if (spriteX + x >= 0 && spriteX + x < 320) {
          if (spriteY + y >= 0 && spriteY + y < 200) {
            if (flip == FLIP_YES) {
              rcgl_plot(spriteX + x, spriteY + y, image.data[spriteY * image.width + (image.width - spriteX)]);
            } else {
              rcgl_plot(spriteX + x, spriteY + y, image.data[spriteY * image.width + spriteX]);
            }
          }
        }
      }
    }
  }
  // return to the buffer we were using before.
  rcgl_setbuf(oldBuf);
}

void loadBMP(char *filename, uint8_t *buffer) {
  uint8_t byte;
  uint8_t byteR;
  uint8_t byteG;
  uint8_t byteB;
  int bmWidth;
  int bmHeight;

  FILE *inFile = fopen(filename, "r");

  // LD2E.BAS, line 610
  fseek(inFile, 23L - 1, SEEK_SET);
  uint16_t dummy;
  BSV_getShort(&dummy, inFile);
  bmHeight = dummy;

  // LD2E, line 612
  fseek(inFile, 0L, SEEK_END);
  long fileLen = ftell(inFile) + 1;
  bmWidth = (fileLen - 1079) / bmHeight;
  bmHeight--;
  int c = 55 - 1;
  fseek(inFile, c, SEEK_SET);

  uint32_t bmpPal[256];
  // extract the palette as-is
  for (int n = 0; n < 256; n++) {
    fread(&byteB, sizeof(byteB), 1, inFile);
    c++;
    fread(&byteG, sizeof(byteG), 1, inFile);
    c++;
    fread(&byteR, sizeof(byteR), 1, inFile);
    c += 2;
    fread(&byte, sizeof(byte), 1, inFile);

    // the original divides these by 4 to get them into the range of the VGA
    // hardware. RCGL has no such limitation.
    bmpPal[n] = 0x00000000 | (byteR << 16) | (byteG << 8) | byteB;
  }
  rcgl_setpalette(bmpPal);
  // put up the image
  c = fileLen - bmWidth - 1;
  // DEBUG printf("%ix%i = %i\n", bmWidth, bmHeight, bmWidth*bmHeight);
  // DEBUG printf("%i/%i\n", c, fileLen);
  fseek(inFile, c, SEEK_SET);

  for (int y = 0; y <= bmHeight + 1; y++) {
    for (int x = 0; x <= bmWidth + 1; x++) {
      fread(&byte, sizeof(byte), 1, inFile);
      c++;
      buffer[x + (y * 320)] = byte;
    }
    c -= ((bmWidth + 1) * 2);
    fseek(inFile, c, SEEK_SET);
  }
  fclose(inFile);
}

void showBMP(char *filename) {
  LD2_LoadBitmap(filename, rcgl_getbuf());
  rcgl_update();
}

// waits for a keypress, and returns what it was
SDL_Keycode waitKeyPress() {
  SDL_Event event;
  while (1) {
    SDL_WaitEvent(&event);
    if (event.type == SDL_KEYDOWN) {
      break;
    }
  }
  return event.key.keysym.sym;
}

// waits for a key to be released, and returns what it was
SDL_Keycode waitAnyKeyRelease() {
  SDL_Event event;
  while (1) {
    SDL_WaitEvent(&event);
    if (event.type == SDL_KEYUP) {
      break;
    }
  }
  return event.key.keysym.sym;
}

// waits for a specific key to be released
void waitKeyRelease(SDL_Keycode key) {
  while (1) {
    SDL_Keycode released = waitAnyKeyRelease();
    if (released == key) {
      break;
    }
  }
}

int mainMenu() {
  // display background image
  showBMP("gfx/title.bmp");

  printf("DEBUG: image displayed\n");
  while (1) {
    // wait for key input
    SDL_Keycode key = waitAnyKeyRelease();

    // return MAIN_START_GAME, MAIN_SHOW_CREDITS, or MAIN_QUIT, depending on keyboard input. There is no "none" output.
    if (key == SDLK_1) {
      return MAIN_START_GAME;
    } else if (key == SDLK_2) {
      return MAIN_SHOW_CREDITS;
    } else if (key == SDLK_3) {
      return MAIN_QUIT;
    }
  }
}

void delayInterruptible(int msDelay) {
  // get current time, preferably in milliseconds
  // add msDelay milliseconds
  // loop until future time is reached or passed
  //   if keypress received, exit loop early.
}

int main(int argc, char* argv[]) {
  // seed RNG
  srand(time(NULL));

  // initialise the engine. Mostly graphics, as there is no sound in this implementation
  // initialise RCGL and surrounding machinery
  if (rcgl_init(320, 200, 640, 400, "Larry the Dinosaur 2", RCGL_INTSCALE)) {
    printf("RCGL Init failed\n");
    rcgl_quit();
    exit(1);
  }

  // load sprites
  // load palette

  // pre-roll screens (content warning, delta code screen)
  showBMP("warning.bmp");
  delayInterruptible(1000);
  showBMP("logo.bmp");
  delayInterruptible(1000);
  // main menu
  /*
  int mainMenuChoice = 0;
  do {
    printf("DEBUG: Entering Main Menu\n");
    mainMenuChoice = mainMenu();
    if (mainMenuChoice == MAIN_START_GAME) {
      // start game
      printf("DEBUG: Start Game option selected\n");
      //doGame();
    } else if (mainMenuChoice == MAIN_SHOW_CREDITS) {
      // show credits screen
      printf("DEBUG: Show Credits option selected\n");
      //showCredits();
    } // otherwise do nothing
  } while (mainMenuChoice != MAIN_QUIT);// this ensures that after the game or credits is completed, you are dumped back at the menu.
*/
  printf("DEBUG: Quitting\n");

  rcgl_quit();
  return 0;
}