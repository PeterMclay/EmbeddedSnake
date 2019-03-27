#include "lcd_driver.h"
#include "lcd_graphic.h"
#include "address_map_arm.h"
/*******************************************************************************
							        SNAKE
 ******************************************************************************/
 volatile int* BTN = (int*)KEY_BASE;
 int crntBTN;
 int prevBTN = 0;

int main(void) {
	while(1){
		//int x, y, 
		int pos_x_current[3] = { 64, 56, 48 };
		int pos_y_current[3] = { 24, 24, 24 };
		int pos_x_previous[3] = { 64, 56, 48 };
		int pos_y_previous[3] = { 24, 24, 24 };
		int length, dir_x, dir_y;
		int size = 3;
		int i;
		volatile int delay_count; // volatile so C compiler doesn't remove the loop

		//Initialize LCD
		init_spim0();
		init_lcd();
		clear_screen();
		
		//Press Button to generate head
		char snke[10] = "SNAKE\0";
		LCD_text(snke, 0);
		refresh_buffer();
		while(ReadButtons() != 1);
		clear_screen();
		refresh_buffer();
		crntBTN = 0;

		//Initialize Snake
		length = 8;
		dir_x  = 0;
		dir_y  = 0;
		for (i = 0; i < size; i++) {
			LCD_rect(pos_x_current[i], pos_y_current[i], length, length, 1, 1);
		}
		refresh_buffer();

		/***********	GAME ON **********/
		while (1) {
			/* erase box */
			for (i = 0; i < size; i++) {
				LCD_rect(pos_x_current[i], pos_y_current[i], length, length, 0, 1);
			}

			//Check if Snake is out of Bounds in x and y
			if ((pos_x_current[0] + length >= SCREEN_WIDTH - 1 && dir_x == 1) || (pos_x_current[0] <= 0 && dir_x == -1)) {
				dir_x = 0;
				dir_y = 0;
				char text_gameover_lcd[17] = "Game over :(\0";
				LCD_text(text_gameover_lcd, 0);
				char text_top_lcd[35] = "Press Any ButtonTo Play Again\0";
				LCD_text(text_top_lcd, 1);
				refresh_buffer();
				while(ReadButtons() == 0);
				crntBTN = 0;
				break;
			}
			if ((pos_y_current[0] + length >= SCREEN_HEIGHT - 1 && dir_y == 1) ||(pos_y_current[0] <= 0 && dir_y == -1)){
				dir_x = 0;
				dir_y = 0;
				char text_gameover_lcd[17] = "Game over :( \0";
				LCD_text(text_gameover_lcd, 0);
				refresh_buffer();
				char text_top_lcd[35] = "Press Any ButtonTo Play Again\0";
				LCD_text(text_top_lcd, 1);
				refresh_buffer();
				while(ReadButtons() == 0);
				crntBTN = 0;
				break;
			}
			
			// Update Snake Direction
			if(ReadButtons()){
				if (crntBTN != prevBTN) {
					if (crntBTN == 1 && dir_x != -1) {
						dir_x = 8;
						dir_y = 0;
					}
					if (crntBTN == 2 && dir_x != 1) {
						dir_x = -8;
						dir_y = 0;
					}
					if (crntBTN == 4 && dir_y != 1) {
						dir_y = -8;
						dir_x = 0;
					}
					if (crntBTN == 8 && dir_y != -1) {
						dir_y = 8;
						dir_x = 0;
					}
					prevBTN = crntBTN;
				}
				else
					prevBTN = crntBTN;
			}

			//Move Sanke
			pos_x_current[0] += dir_x;
			pos_y_current[0] += dir_y;
			for (i = 1; i <= size; i++) {
				pos_x_current[i] = pos_x_previous[i - 1];
				pos_y_current[i] = pos_y_previous[i - 1];
			}

			// Update Position
			for (i = 0; i < size; i++) {
				LCD_rect(pos_x_current[i], pos_y_current[i], length, length, 1, 1);
				pos_x_previous[i] = pos_x_current[i];
				pos_y_previous[i] = pos_y_current[i];
			}
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

