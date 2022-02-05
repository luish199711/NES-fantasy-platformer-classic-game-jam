
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

#include <joystick.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

// function to write a string into the name table
//   adr = start address in name table
//   str = pointer to string
void put_str(unsigned int adr, const char *str) {
  vram_adr(adr);        // set PPU read/write address
  vram_write(str, strlen(str)); // write bytes to PPU
}

void clrscr() {
  vrambuf_clear();
  ppu_off();
  vram_adr(0x2000);
  vram_fill(0, 32*28);
  vram_adr(0x0);
//  ppu_on_all();
}

byte pad;
byte iy;

void story_win()
{
  put_str(NTADR_A(2,3), "You exited from your house,");
  put_str(NTADR_A(2,4), "and tried to manage and wade");
  put_str(NTADR_A(2,5), "through a series of obstacl-");
  put_str(NTADR_A(2,6), "es that took too much time");
  put_str(NTADR_A(2,7), "to get through. However, you");
  put_str(NTADR_A(2,8), "barely managed to get to the");
  put_str(NTADR_A(2,9), "bus station and get into your");
  put_str(NTADR_A(2,10), "bus. After some time has gone");
  put_str(NTADR_A(2,11), "by traveling the long winding");
  put_str(NTADR_A(2,12), "road, you managed to get to");
  put_str(NTADR_A(2,13), "your class on time!");
  put_str(NTADR_A(12,20), "YOU WIN!");
  
  ppu_on_all();
  
  while(1){}
  
}

void story_over()
{
  put_str(NTADR_A(2,3), "Tired, you close your eyes,");
  put_str(NTADR_A(2,4), "and go back to sleep. You");
  put_str(NTADR_A(2,5), "sleep for what felt like mi-");
  put_str(NTADR_A(2,6), "nutes, but once you wake up,");
  put_str(NTADR_A(2,7), "you slept for two hours! YOU");
  put_str(NTADR_A(2,8), "MISSED YOUR CLASS!!!");
  put_str(NTADR_A(11,20), "GAME OVER!");
  
  ppu_on_all();
  
  while(1){}
  
}

void play_storyB()
{
  put_str(NTADR_A(2,3), "You get up from your bed and");
  put_str(NTADR_A(2,4), "quickly get dressed with the");
  put_str(NTADR_A(2,5), "clothes you organized from");
  put_str(NTADR_A(2,6), "yesterday. After you put");
  put_str(NTADR_A(2,7), "them on as fast as you can,");
  put_str(NTADR_A(2,8), "you checked the clock and 5");
  put_str(NTADR_A(2,9), "minutes have passed away.");
  put_str(NTADR_A(2,18), "What do you do?");
  put_str(NTADR_A(2,21), "A.) Rush to the bus station");
  put_str(NTADR_A(2,22), "B.) Go back to sleep");
  
  ppu_on_all();
  
  while(1){
    pad = pad_trigger(0);
    if (pad & PAD_A){
      clrscr();
      story_win();
    }
    else if(pad & PAD_B){
      clrscr();
      story_over();
    }
  }  
  
}

void play_storyA()
{
  put_str(NTADR_A(2,3), "Today is Monday");
  put_str(NTADR_A(2,5), "You open your eyes and look-"); 
  put_str(NTADR_A(2,6), "ed at the clock on your left.");
  put_str(NTADR_A(2,7), "It's 11:00 AM, you have class.");
  put_str(NTADR_A(2,8), "at 12:00 PM, and it takes at");
  put_str(NTADR_A(2,9), "least 25 minutes to get to y-");
  put_str(NTADR_A(2,10), "our campus from where you li-");
  put_str(NTADR_A(2,11), "ve");
  put_str(NTADR_A(2,18), "What do you do?");
  put_str(NTADR_A(2,21), "A.) Get dressed");
  put_str(NTADR_A(2,22), "B.) Go back to sleep");
  
  ppu_on_all();
  
  while(1){
    pad = pad_trigger(0);
    if (pad & PAD_A){
      clrscr();
      play_storyB();
    }
    else if(pad & PAD_B){
      clrscr();
      story_over();
    }
  }  
}


void game_title()
{
  // write text to name table
  put_str(NTADR_A(11,12), "Quest Game!");
  put_str(NTADR_A(6,14), "Press enter to start...");
  // enable PPU rendering (turn on screen)
  ppu_on_all();
  
  // infinite loop
  while (1) {
    scroll(-8,iy>>4);
    if(pad_trigger(0)&PAD_START) break;
  }

  scroll(-8,0);//if start is pressed, show the title at whole
  clrscr();
  
  play_storyA();
}


/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,   // background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

void main(void)
{
  joy_install (joy_static_stddrv);
    
  setup_graphics();
  // enable PPU rendering (turn on screen)
  
  //play_storyB();
  game_title();
  
//  memset(str,0,sizeof(str));
  
  // draw message  
//  vram_adr(NTADR_A(1,3));
//  vram_write("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123",30);
  // enable rendering
//  ppu_on_all();
  // infinite loop

}
