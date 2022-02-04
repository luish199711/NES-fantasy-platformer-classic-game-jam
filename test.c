
#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
//#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

#define COLS 32
#define ROWS 27

byte attract;

//CPUT Ch XY

void cputcxy(byte x, byte y, char ch) {
  vrambuf_put(NTADR_A(x,y), &ch, 1);
}

void cputsxy(byte x, byte y, const char* str) {
  vrambuf_put(NTADR_A(x,y), str, strlen(str));
}

void clrscr() {
  vrambuf_clear();
  ppu_off();
  vram_adr(0x2000);
  vram_fill(0, 32*28);
  vram_adr(0x0);
  ppu_on_bg();
}

////////// GAME DATA

typedef struct {
  byte x;
  byte y;
  byte dir;
  word score;
  char head_attr;
  char tail_attr;
  int collided:1;
  int human:1;
} Player;

Player players[2];


const char BOX_CHARS[8] = { '+','+','+','+','-','-','!','!' };

void draw_box(byte x, byte y, byte x2, byte y2, const char* chars) {
  byte x1 = x; //copy x into x1

  //put '+' onto the corners of the screen
  cputcxy(x, y, chars[2]);//	Top Left
  cputcxy(x2, y, chars[3]);//	Top Right
  cputcxy(x, y2, chars[0]);//	Bottom Left
  cputcxy(x2, y2, chars[1]);//	Bottom Right

  while (++x < x2) {
    cputcxy(x, y, chars[5]);//	Top side
    cputcxy(x, y2, chars[4]);// Bottom side
  }
  while (++y < y2) {
    cputcxy(x1, y, chars[6]);//	Left side
    cputcxy(x2, y, chars[7]);// Right side
  }
}

void draw_playfield() {
  draw_box(1,2,COLS-2,ROWS-1,BOX_CHARS);
//  cputcxy(28,1,'0');	//Player 2 Score
//  if (attract) {
//    cputsxy(3,ROWS-1,"ATTRACT MODE - PRESS ENTER");
//  } else {
//    cputsxy(1,1,"PLYR1:");
//    cputsxy(20,1,"PLYR2:");
//  }
}

void main(void)
{
//  setup_graphics();
  
  // set palette colors
  pal_col(0,0x02);	// set screen to dark blue
  pal_col(1,0x14);	// fuchsia
  pal_col(2,0x20);	// grey
  pal_col(3,0x30);	// white
  
  vrambuf_clear();
  
  set_vram_update(updbuf);
  memset(players,0,sizeof(players));
  
//draw message  
//  vram_adr(NTADR_A(2,2));
//  vram_write("HELLO, WORLD!", 12);
//enable rendering
//  ppu_off();
//  setup_attrib_table();
//  setup_palette();
  clrscr();
  draw_playfield();
  vrambuf_flush();
  vrambuf_flush();
//  ppu_on_all();
//infinite loop
  while(1) {
    
  }
}
