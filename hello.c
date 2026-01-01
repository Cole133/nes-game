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

unsigned char i, j;

unsigned char pad1, p1_facing, p1_x, p1_y, b1_x, b1_y, b1;
signed char p1_dy, p1_dx, b1_vx;

unsigned char pad2, p2_facing, p2_x, p2_y, b2_x, b2_y, b2;
signed char p2_dy, p2_dx, b2_vx;

struct Platform {
	unsigned char x;
	unsigned char y;
	unsigned char w;
};

const struct Platform platforms[] = {
	{40,  180, 48}, 
    {140, 180, 48}, 
    {100, 155, 32}  
};

const unsigned char palette[]={
BLACK, 0x16, 0x28, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 


void main (void) {
	// Setup Player Variables
	p1_x = 40;
	p2_x = 200;

	p1_y = 200;
	p2_y = 200;

	p1_dy = 0;
	p1_dx = 0;
	p2_dy = 0;
	p2_dx = 0;
	
	ppu_off(); // screen off

	pal_bg(palette); //	load the BG palette
	pal_spr(palette); //	load the sprite palette
		
	vram_adr(NTADR_A(0,26)); // screen is 32 x 30 tiles

	for (i=0; i<32; ++i){
		vram_put(0x01);
	}

	for (i = 0; i < 3; i++){
		
		vram_adr(NTADR_A((platforms[i].x + j) / 8 , platforms[i].y / 8));

		for(j = 0; j < (platforms[i].w / 8); j++){
			vram_put(0x01);
		}
	}
	
	ppu_on_all(); //	turn on screen
	
	
	while (1){
		ppu_wait_nmi();
		oam_clear(); 

		pad1 = pad_poll(0);
		pad2 = pad_poll(1);

		// -------------------------
        // PLAYER 1 LOGIC
        // -------------------------

		// Horizontal Movment
		if(pad1 & PAD_LEFT){
			p1_facing = 0;
			
			if(p1_dx > -3) --p1_dx;
		}
		else if(pad1 & PAD_RIGHT){
			p1_facing = 1;
			
			if(p1_dx < 3) ++p1_dx;
		}
		else{
			if(p1_dx > 0) --p1_dx;
			if(p1_dx < 0) ++p1_dx;
		}

		if(p1_dx > 0 && p1_x < 255) p1_x += p1_dx;
		if(p1_dx < 0 && p1_x > 0) p1_x += p1_dx;

		// Vertical Movment

		if((pad1 & PAD_A) && (p1_y == FLOOR_Y)){
			p1_dy = -8;
		}

		if (p1_dy < 4) p1_dy += 1;

		p1_y += p1_dy;
		

		// Collision
		if(p1_dy >= 0){
			if(p1_y >= FLOOR_Y){
				p1_y = FLOOR_Y;
				p1_dy = 0;
			}

			for(i = 0; i < 3; i++){

				if (p1_y >= platforms[i].y && p1_y < platforms[i].y + 8){

					if (p1_x + 8 > platforms[i].x && p1_x < platforms[i].x + platforms[i].w){
						p1_y = platforms[i].y + 16;
						p1_dy = 0;
					}
				}
			}
		}

		// Shooting
		if( (pad1 & PAD_B) && (b1 == 0)){
			b1_y = p1_y + 2;
			b1 = 1;


			if(p1_facing == 0){
				b1_x = p1_x - 4;
				b1_vx = -4;
				p1_dx = 8;
			} else {
				b1_x = p1_x + 4;
				b1_vx = 4;
				p1_dx = -8;
			}
		}

		if(b1 == 1){
			oam_spr(b1_x, b1_y, 0x02, 0);

			b1_x += b1_vx;
			
			if(b1_x > 250) b1 = 0;
		}

		// -------------------------
        // PLAYER 2 LOGIC
        // -------------------------

		// Horizontal Movment
		if(pad2 & PAD_LEFT){
			p2_facing = 0;
			
			if(p2_dx > -3) --p2_dx;
		}
		else if(pad2 & PAD_RIGHT){
			p2_facing = 1;
			
			if(p2_dx < 3) ++p2_dx;
		}
		else{
			if(p2_dx > 0) --p2_dx;
			if(p2_dx < 0) ++p2_dx;
		}

		if(p2_dx > 0 && p2_x < 255) p2_x += p2_dx;
		if(p2_dx < 0 && p2_x > 0) p2_x += p2_dx;

		// Vertical Movment

		if((pad2 & PAD_A) && (p2_y == FLOOR_Y)){
			p2_dy = -8;
		}

		if (p2_y < FLOOR_Y) p2_dy += 1;
		if (p2_dy > 4) p2_dy = 4; 

		p2_y += p2_dy;
		

		// Collision with floor
		if(p2_y >= FLOOR_Y){
			p2_y = FLOOR_Y;
			p2_dy = 0;
		}

		// Shooting
		if( (pad2 & PAD_B) && (b2 == 0)){
			b2_y = p2_y + 2;
			b2 = 1;


			if(p2_facing == 0){
				b2_x = p2_x - 4;
				b2_vx = -4;
				p2_dx = 8;
			} else {
				b2_x = p2_x + 4;
				b2_vx = 4;
				p2_dx = -8;
			}
		}

		if(b2 == 1){
			oam_spr(b2_x, b2_y, 0x02, 0);

			b2_x += b2_vx;

			if(b2_x > 250) b2 = 0;
		}
		
		// Hit Detection

		if(b1 == 1){
			if( (b1_x >= p2_x) && (b1_x <= p2_x + 15) && (b1_y >= p2_y) && (b1_y <= p2_y + 15) ){
				p1_x = 40;
				p2_x = 200;

				p1_y = 200;
				p2_y = 200;

				b1 = 0;
				b2 = 0;
			}
		}

		if(b2 == 1){
			if( (b2_x >= p1_x) && (b2_x <= p1_x + 15) && (b2_y >= p1_y) && (b2_y <= p1_y + 15) ){
				p1_x = 40;
				p2_x = 200;

				p1_y = 200;
				p2_y = 200;

				b1 = 0;
				b2 = 0;
			}
		}

		// Sprite Drawing
		
		oam_spr(p1_x, p1_y, 0x41, 0);
		oam_spr(p1_x + 8, p1_y, 0x42, 0);
		oam_spr(p1_x, p1_y + 8, 0x43, 0);
		oam_spr(p1_x + 8, p1_y + 8, 0x44, 0);

		oam_spr(p2_x, p2_y, 0x41, 0);
		oam_spr(p2_x + 8, p2_y, 0x42, 0);
		oam_spr(p2_x, p2_y + 8, 0x43, 0);
		oam_spr(p2_x + 8, p2_y + 8, 0x44, 0);

	}
}
	
	