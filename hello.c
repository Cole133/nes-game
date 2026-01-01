#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
#define FLOOR_Y 192
// there's some oddities in the palette code, black must be 0x0f, white must be 0x30

#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster

unsigned char i;
unsigned char pad1;
unsigned char x, y;

signed char jump, dy;

const unsigned char text[]="six seven"; // zero terminated c string

const unsigned char palette[]={
BLACK, 0x16, 0x28, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 


void main (void) {
	x = 128;
	y = 128;
	
	ppu_off(); // screen off

	pal_bg(palette); //	load the BG palette
	pal_spr(palette); //	load the sprite palette
		
	// set a starting point on the screen
	// vram_adr(NTADR_A(x,y));
	vram_adr(NTADR_A(2,26)); // screen is 32 x 30 tiles

	for (i=0; i<28; ++i){
		vram_put(0x01);
	}
	
	ppu_on_all(); //	turn on screen
	
	
	while (1){
		ppu_wait_nmi();
		pad1 = pad_poll(0);

		// Horizontal Movment
		if(pad1 & PAD_LEFT){
			if(x>0) --x;
		}
		if(pad1 & PAD_RIGHT){
			if(x<255) ++x;
		}

		// Vertical Movment

		if((pad1 & PAD_A) && (y == FLOOR_Y)){
			dy = -8;
		}
		
		if (y < FLOOR_Y) { dy += 1; }
		if (dy > 4) { dy = 4; }

		y += dy;
		

		// Collision with floor
		if(y >= FLOOR_Y){
			y = FLOOR_Y;
			dy = 0;
		}
		

		// Sprite Drawling
		oam_clear(); 

		oam_spr(x, y, 0x41, 0);
		oam_spr(x + 8, y, 0x42, 0);
		oam_spr(x, y + 8, 0x43, 0);
		oam_spr(x + 8, y + 8, 0x44, 0);
		
	}
}
	
	