#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include <stdlib.h>

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
#define FLOOR_Y 192

#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster

unsigned char i, j;

unsigned char pad1, p1_facing, p1grounded, p1score, p1changed, p1_x, p1_y, b1_x, b1_y, b1_multi, b1;
signed char p1_dy, p1_dx, b1_vx;

unsigned char pad2, p2_facing, p2grounded, p2score, p2changed, p2_x, p2_y, b2_x, b2_y, b2_multi, b2;
signed char p2_dy, p2_dx, b2_vx;

unsigned char map_id;
unsigned char game_state;
unsigned char powerup_active;
unsigned char random;
unsigned char new_random;

struct Platform {
	unsigned char x;
	unsigned char y;
	unsigned char w;
};

const struct Platform maps[3][3] = {
    // MAP 1
    { 
        {40, 180, 48}, {140, 180, 48}, {100, 155, 32} 
    },
    // MAP 2
    { 
        {10, 150, 64}, {180, 150, 64}, {100, 180, 48} 
    },
    // MAP 3
    { 
        {40, 180, 48}, {100, 150, 48}, {160, 120, 48} 
    }
};

const unsigned char palette[]={
BLACK, 0x16, 0x28, WHITE,
0,0,0,0,
0,0,0,0,
0,0,0,0
}; 

// Draws the title screen

void drawl_title_screen(){
	vram_adr(NTADR_A(10, 14)); 
    vram_put(0x31); 

    vram_adr(NTADR_A(16, 14)); 
    vram_put(0x32); 

    vram_adr(NTADR_A(22, 14));
    vram_put(0x33);
}

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

	p1score = 0;
	p2score = 0;

	p1changed = 1;
	p2changed = 1;

	b1_multi = 1;
	b2_multi = 1;
	
	game_state = 0;
	map_id = 0;
	powerup_active = 0;

	new_random = 0;
	random = (rand() % (150 - 50 + 1)) + 50;

	ppu_off(); 

	// Load palettes
	pal_bg(palette); 
	pal_spr(palette); 
		
	vram_adr(NTADR_A(0,26)); 

	drawl_title_screen();
	
	ppu_on_all(); 

	while (1){

		// -------------------------
        // TITLE SCREEN SELECT
        // -------------------------

        if (game_state == 0) {
            ppu_wait_nmi();
            
            pad1 = pad_poll(0);
            
            
            if (pad1 & PAD_LEFT)  map_id = 0;
            if (pad1 & PAD_UP)    map_id = 1; 
            if (pad1 & PAD_RIGHT) map_id = 2;
            
            
            oam_clear();
            oam_spr(80 + (map_id * 48), 100, 0x41, 0); 
            
            // SELECT MAP
            if (pad1 & PAD_START || pad1 & PAD_A) {
                
                ppu_off();

				// Clear Screen
				vram_adr(NTADR_A(0,0)); 
                for(i = 0; i < 254; i++) {
                     vram_put(0x00); 
                }
				for(i = 0; i < 240; i++) {
                     vram_put(0x00); 
                }
                
                // Draw the Floor
                vram_adr(NTADR_A(0,26));
                for (i=0; i<32; ++i) vram_put(0x01);

                // 3. Draw the SELECTED Map
                for (i = 0; i < 3; i++){
                    vram_adr(NTADR_A((maps[map_id][i].x) / 8 , maps[map_id][i].y / 8));
                    for(j = 0; j < (maps[map_id][i].w / 8); j++){
                        vram_put(0x01);
                    }
                }
                
                // Reset Positions
                p1_x = 40; p1_y = 180;
                p2_x = 200; p2_y = 180;
                
                // Switch State
                game_state = 1;
                ppu_on_all();
            }
        }

		// -------------------------
		// STATE 1: MAIN GAMEPLAY
		// -------------------------

		else if (game_state == 1) {
			const struct Platform* platforms = maps[map_id];
			unsigned char p1_tens = p1score / 10;
			unsigned char p1_ones = p1score % 10;
			
			unsigned char p2_tens = p2score / 10;
			unsigned char p2_ones = p2score % 10;

			ppu_wait_nmi(); 
			oam_clear(); 

			if (p1changed) {
				vram_adr(NTADR_A(2, 1));      
				vram_put(0x30 + p1_tens);     
				vram_put(0x30 + p1_ones);     
				p1changed = 0;
			}

			if (p2changed) {
				vram_adr(NTADR_A(26, 1));
				vram_put(0x30 + p2_tens);
				vram_put(0x30 + p2_ones);
				p2changed = 0; 
			}

			scroll(0, 0);
			
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

			if(p1_dx > 0 && p1_x < 245) p1_x += p1_dx;
			if(p1_dx < 0 && p1_x > 10) p1_x += p1_dx;

			// Vertical Movment

			if((pad1 & PAD_A) && p1grounded){
				p1_dy = -8;
				p1grounded = 0;
			}

			if (p1_dy < 4) p1_dy += 1;

			p1_y += p1_dy;
			p1grounded = 0;

			// Collision
			if(p1_dy >= 0){
				if(p1_y >= FLOOR_Y){
					p1_y = FLOOR_Y;
					p1_dy = 0;
					p1grounded = 1;
				}

				for(i = 0; i < 3; i++){

					if (p1_y >= (platforms[i].y-22) && p1_y < (platforms[i].y-22) + 8){

						if (p1_x + 8 > platforms[i].x && p1_x < platforms[i].x + platforms[i].w){
							p1_y = (platforms[i].y-22);
							p1_dy = 0;
							p1grounded = 1;
						}
					}
				}
			}

			// Shooting
			if((pad1 & PAD_B) && (b1 == 0)){
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
				if(p1_facing == 0)
					oam_spr(b1_x, b1_y, 0x02, 0);
				else
					oam_spr(b1_x, b1_y, 0x03, 0);

				b1_x += b1_vx * b1_multi;
				
				if(b1_x > 245) b1 = 0;
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

			if(p2_dx > 0 && p2_x < 245) p2_x += p2_dx;
			if(p2_dx < 0 && p2_x > 10) p2_x += p2_dx;

			// Vertical Movment

			if((pad2 & PAD_A) && p2grounded){
				p2_dy = -8;
				p2grounded = 0;
			}

			if (p2_dy < 4) p2_dy += 1;

			p2_y += p2_dy;
			p2grounded = 0;
			
			// Collision

			if(p2_dy >= 0){
				if(p2_y >= FLOOR_Y){
					p2_y = FLOOR_Y;
					p2_dy = 0;
					p2grounded = 1;
				}

				for(i = 0; i < 3; i++){

					if (p2_y >= (platforms[i].y-22) && p2_y < (platforms[i].y-22) + 8){

						if (p2_x + 8 > platforms[i].x && p2_x < platforms[i].x + platforms[i].w){
							p2_y = (platforms[i].y-22);
							p2_dy = 0;
							p2grounded = 1;
						}
					}
				}
			}

			// Shooting

			if((pad2 & PAD_B) && (b2 == 0)){
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
				if(p2_facing == 0)
					oam_spr(b2_x, b2_y, 0x02, 0);
				else
					oam_spr(b2_x, b2_y, 0x03, 0);

				b2_x += b2_vx * b2_multi;

				if(b2_x > 245) b2 = 0;
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

					p1score++;
					p1changed = 1;

					b1_multi = 1;
					b2_multi = 1;
					powerup_active = 0;

					random = (rand() % (150 - 50 + 1)) + 50;
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

					p2score++;
					p2changed = 1;

					b1_multi = 1;
					b2_multi = 1;
					powerup_active = 0;

					random = (rand() % (150 - 50 + 1)) + 50;
				}
			}

			// Power Up

			if(p1_x >= random - 10 && p1_x <= random + 10 && p1_y >= 118 && p1_y <= 130){
				b1_multi = 2;
				powerup_active = 1;
			}

			if(p2_x >= random - 10 && p2_x <= random + 10 && p2_y >= 118 && p2_y <= 130){
				b2_multi = 2;
				powerup_active = 1;
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

			if(powerup_active == 0){
				oam_spr(random, 120, 0x04, 0);
			}

			if(pad1 & PAD_SELECT){
				
				ppu_off();

				// Clear Screen
				vram_adr(NTADR_A(0,0)); 
				for(i = 0; i < 254; i++) {
					 vram_put(0x00); 
				}
				for(i = 0; i < 250; i++) {
					 vram_put(0x00); 
				}
				for(i = 0; i < 250; i++) {
					 vram_put(0x00); 
				}
				for(i = 0; i < 130; i++) {
					 vram_put(0x00); 
				}

				drawl_title_screen();
				
				ppu_on_all();

				game_state = 0;
			}
		}
	}
}