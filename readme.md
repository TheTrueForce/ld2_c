# What is this?

This is a rewrite of Larry the Dinosaur 2 in C, using SDL2 and RCGL.
The original was written in QuickBASIC and x86 ASM. While it is playable under DOSBox, it's quite slow, even with the simulation speed cranked up.

I'm not including the sound functionality in this, _for now_. It may come later, and if it does, it'll be a command-line switch at first.  
I'm also implementing what may well be a number of anti-patterns at this stage, as I am loosely following the structure of the original. I plan to restructure this in future, and then upgrade it with things like a save function, and re-bindable controls.

The assets of the original have not been included in this repo, as the license stated in LD2's original readme is "Anticopyright". I don't know precisely what this means, so I'm including only my own work in this repo to be on the safer side.
Some of this code is more or less a line-for-line reimplementation in C. Some of this is my own work. However, I do refer to the original source quite a lot.

The assets and the original source code are available with the original game at [Phat Code](http://www.phatcode.net/downloads.php?id=114). I believe there are other places that host it, but this is the one I use.

The original folder structure will be used.

## Build prerequisites

- Linux-based OS
  - I have no clue if this will compile under Mac OS
  - No consideration has been given to Windows here. An unknown amount of work may be needed.
- SDL2
- make (I'm using GNU Make, though I'm not using anything fancy in my makefile)

## Building

**Note that at present, this does not build successfully.**
This project is currently incomplete.

Run: `make setupDevEnv`  
Run: `make`
