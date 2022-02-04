
#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

#include <joystick.h>

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

#define COLS 30
#define ROWS 27

// read a character from VRAM.
// this is tricky because we have to wait
// for VSYNC to start, then set the VRAM
// address to read, then set the VRAM address
// back to the start of the frame.
byte getchar(byte x, byte y) {
  // compute VRAM read address
  word addr = NTADR_A(x,y);
  // result goes into rd
  byte rd;
  // wait for VBLANK to start
  ppu_wait_nmi();
  // set vram address and read byte into rd
  vram_adr(addr);
  vram_read(&rd, 1);
  // scroll registers are corrupt
  // fix by setting vram address
  vram_adr(0x0);
  return rd;
}

//CPUT Char XY
void cputcxy(byte x, byte y, char ch) {
  vrambuf_put(NTADR_A(x,y), &ch, 1);
}

//CPUT String XY
void cputsxy(byte x, byte y, const char* str) {
  vrambuf_put(NTADR_A(x,y), str, strlen(str));
}

//clrscr
//---------------------------------------------
//clear screen to refresh VRAM data
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
} Player;

Player snake;

typedef struct {
  byte fx;
  byte fy;  
} Food;

byte gameover;
byte frames_per_move;

#define START_SPEED 12
#define MAX_SPEED 5

const char BOX_CHARS[3] = { '+','-','!' };

void draw_box(byte x, byte y, byte x2, byte y2, const char* chars) {
  byte x1 = x; //copy x into x1

  //put '+' onto the corners of the screen
  cputcxy(x, y, chars[0]);//	Top Left
  cputcxy(x2, y, chars[0]);//	Top Right
  cputcxy(x, y2, chars[0]);//	Bottom Left
  cputcxy(x2, y2, chars[0]);//	Bottom Right

  while (++x < x2) {
    cputcxy(x, y, chars[1]);//	Top side
    cputcxy(x, y2, chars[1]);// Bottom side
  }
  while (++y < y2) {
    cputcxy(x1, y, chars[2]);//	Left side
    cputcxy(x2, y, chars[2]);// Right side
  }
}

void draw_playfield() {
  draw_box(1,2,COLS,ROWS-1,BOX_CHARS); //Draw border
  cputsxy(2,1,"SCORE: ");	//Display Score
}

typedef enum { D_RIGHT, D_DOWN, D_LEFT, D_UP } dir_t;
const char DIR_X[4] = { 1, 0, -1, 0 };
const char DIR_Y[4] = { 0, 1, 0, -1 };

void init_game() {
  snake.head_attr = '0';
//  snake.tail_attr = 0x06;
  frames_per_move = START_SPEED;
}

void reset_players() {
  snake.x = snake.y = 5;
  snake.dir = D_RIGHT;
  snake.collided = 0;
}

void draw_player(Player* p) {
  cputcxy(p->x, p->y, p->head_attr);
}

void move_player(Player* p) {
  cputcxy(p->x, p->y, p->tail_attr);
  p->x += DIR_X[p->dir];
  p->y += DIR_Y[p->dir];
  if (getchar(p->x, p->y) != 0)
    p->collided = 1;
  draw_player(p);
}

void human_control(Player* p) {
  byte dir = 0xff;
  byte joy;
  joy = joy_read (JOY_1);

  if (joy & JOY_LEFT_MASK) dir = D_LEFT;
  if (joy & JOY_RIGHT_MASK) dir = D_RIGHT;
  if (joy & JOY_UP_MASK) dir = D_UP;
  if (joy & JOY_DOWN_MASK) dir = D_DOWN;
  // don't let the player reverse
  if (dir < 0x80 && dir != (p->dir ^ 2)) {
    p->dir = dir;
  }
}

void flash_colliders() {
  byte i;
  // flash players that collided
  for (i=0; i<56; i++) {
    //cv_set_frequency(CV_SOUNDCHANNEL_0, 1000+i*8);
    //cv_set_attenuation(CV_SOUNDCHANNEL_0, i/2);
    if (snake.collided) snake.head_attr ^= 0x80;
    vrambuf_flush();
    draw_player(&snake);
  }
  //cv_set_attenuation(CV_SOUNDCHANNEL_0, 28);
}

void make_move() {
  byte i;
  for (i=0; i<frames_per_move; i++) {
    human_control(&snake);
    vrambuf_flush();
  }
  // if players collide, 2nd player gets the point
  move_player(&snake);
}

void play_round() {
  ppu_off();
  clrscr();
  draw_playfield();
  reset_players();
  while (1) {
    make_move();
    if (gameover) return; // attract mode -> start
    if (snake.collided) break;
  }
  flash_colliders();
  // add scores to players that didn't collide
  if (snake.collided) snake.score++;
  // increase speed
  if (frames_per_move > MAX_SPEED) frames_per_move--;
}


void play_game() {
  gameover = 0;
  init_game();
  while (!gameover) {
    play_round();
  }
}

void main(void)
{
  // set palette colors
  pal_col(0,0x02);	// set screen to dark blue
  pal_col(1,0x14);	// fuchsia
  pal_col(2,0x20);	// grey
  pal_col(3,0x30);	// white
  
  joy_install(joy_static_stddrv);
  vrambuf_clear();
  set_vram_update(updbuf);  
  
  vrambuf_flush();
  vrambuf_flush();
//infinite loop
  while(1) {
    play_game();
  }
}