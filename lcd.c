#include "lcd_driver.h"
#include "lcd_graphic.h"
#include "address_map_arm.h"
/*******************************************************************************
 * The program performs the following:
 * 1. Writes INTEL FPGA COMPUTER SYSTEMS to the top of the LCD.
 * 2. Bounces a filled in rectangle around the display and off the displayed
 * text.
 ******************************************************************************/
 volatile int* BTN = (int*)KEY_BASE;
 int crntBTN;
 int prevBTN = 0;

int main(void) {
	while(1){
		int x, y, length, dir_x, dir_y;
		volatile int delay_count; // volatile so C compiler doesn't remove the loop

		//Initialize LCD
		init_spim0();
		init_lcd();
		clear_screen();
	
		char snke[10] = "SNAKE\0";
		LCD_text(snke, 0);
		refresh_buffer();
		while(ReadButtons() != 1);
		clear_screen();
		refresh_buffer();
		/* initialize first position of box */
		x      = 64;
		y      = 24;
		length = 8;
		dir_x  = 0;
		dir_y  = 0;
		LCD_rect(x, y, length, length, 1, 1);
		crntBTN = 0;
		refresh_buffer();

		while (1) {
			/* erase box */
			LCD_rect(x, y, length, length, 0, 1);

			if ((x + length >= SCREEN_WIDTH - 1 && dir_x == 1) || (x <= 0 && dir_x == -1)) {
				dir_x = 0;
				dir_y = 0;
				char text_gameover_lcd[17] = "Game over :(\0";
				LCD_text(text_gameover_lcd, 0);
				char text_top_lcd[35] = "Press Any ButtonTo Play Again\0";
				LCD_text(text_top_lcd, 1);
				refresh_buffer();
				while(ReadButtons() == 0);
				break;
			}
			
			if ((y + length >= SCREEN_HEIGHT - 1 && dir_y == 1) ||(y <= 0 && dir_y == -1)){
				dir_x = 0;
				dir_y = 0;
				char text_gameover_lcd[17] = "Game over :( \0";
				LCD_text(text_gameover_lcd, 0);
				refresh_buffer();
				char text_top_lcd[35] = "Press Any ButtonTo Play Again\0";
				LCD_text(text_top_lcd, 1);
				refresh_buffer();
				while(ReadButtons() == 0);
				break;
			}
			
			// Update Snake Direction
			if(ReadButtons()){
				if (crntBTN != prevBTN) {
					if (crntBTN == 1) {
						dir_x = 1;
						dir_y = 0;
					}
					if (crntBTN == 2) {
						dir_x = -1;
						dir_y = 0;
					}
					if (crntBTN == 4) {
						dir_y = -1;
						dir_x = 0;
					}
					if (crntBTN == 8) {
						dir_y = 1;
						dir_x = 0;
					}
					prevBTN = crntBTN;
				}
				else
					prevBTN = crntBTN;
			}

			//Move Sanke
			x += dir_x;
			y += dir_y;

			// Update Position
			LCD_rect(x, y, length, length, 1, 1);
			refresh_buffer();

			// Delay Loop
			for (delay_count = 100000; delay_count != 0; --delay_count); 
		}
	}
}

int ReadButtons(void){
	if ((((*BTN)&0x01) == 1)){
		// while((((*BTN)&0x01) == 1));
		printf("right");
		crntBTN = 1;
		return 1;
	}
	if ((((*BTN) & 0x02) == 2)) {
		// while ((((*BTN) & 0x02) == 2));
		printf("left");
		crntBTN = 2;
		return 1;
	}
	if ((((*BTN) & 0x04) == 4)) {
		// while ((((*BTN) & 0x04) == 4));
		printf("up");
		crntBTN = 4;
		return 1;
	}
	if ((((*BTN) & 0x08) == 8)) {
		// while ((((*BTN) & 0x08) == 8));
		printf("down");
		crntBTN = 8;
		return 1;
	}
	return 0;
}

