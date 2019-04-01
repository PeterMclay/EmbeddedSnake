#include "lcd_driver.h"
#include "lcd_graphic.h"
#include "address_map_arm.h"
#include <stdlib.h>
/*******************************************************************************
SNAKE
******************************************************************************/

//Global Variables
volatile int* BTN = (int*)KEY_BASE;
int crntBTN;
int prevBTN = 0;
volatile int *HEX3_HEX0_BASE_ptr = (int*)HEX3_HEX0_BASE;
unsigned char lookUpTable[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67 };

//Functions
void display_score(int sze) {
	*(HEX3_HEX0_BASE_ptr) = (256 * lookUpTable[(sze - 1) / 10] + lookUpTable[(sze - 1) % 10]);
}
int ReadButtons(void) {
	if ((((*BTN) & 0x01) == 1)) {
		// while((((*BTN)&0x01) == 1));
		crntBTN = 1;
		return 1;
	}
	if ((((*BTN) & 0x02) == 2)) {
		// while ((((*BTN) & 0x02) == 2));
		crntBTN = 2;
		return 1;
	}
	if ((((*BTN) & 0x04) == 4)) {
		// while ((((*BTN) & 0x04) == 4));
		crntBTN = 4;
		return 1;
	}
	if ((((*BTN) & 0x08) == 8)) {
		// while ((((*BTN) & 0x08) == 8));
		crntBTN = 8;
		return 1;
	}
	return 0;
}
int game_over(int *posx, int *posy, int dir_x, int dir_y, int length, int size) {
	if ((posx[0] + length >= SCREEN_WIDTH + -4 && dir_x == 4) || (posx[0] <= 0 && dir_x == -4)) {
		return 1;
	}
	if ((posy[0] + length >= SCREEN_HEIGHT - 4 && dir_y == 4) || (posy[0] <= 0 && dir_y == -4)) {
		return 1;
	}
	for (int i = 1; i < size; i++) {
		if (posx[0] == posx[i] && posy[0] == posy[i]) {
			return 1;
		}
	}
	return 0;
}

//Main
int main(void) {

	//Initialize LCD
	init_spim0();
	init_lcd();
	clear_screen();

	//Variable Declaration
	char snke[10] = "SNAKE\0";
	char text_gameover_upper[17] = "Game over\0";
	char text_gameover_lower[35] = "Press a Button to Play Again\0";
	int pos_x_current[25];
	int pos_y_current[25];
	int pos_x_previous[25];
	int pos_y_previous[25];
	int dir_x = 0;
	int dir_y = 0;
	int size;
	int length = 4;
	int i;
	volatile int delay_count; // volatile so C compiler doesn't remove the loop


	while (1) {
		//Starting Position of the Snake
		pos_x_current[0] = 64;
		pos_y_current[0] = 24;
		pos_x_previous[0] = 64;
		pos_y_previous[0] = 24;
		dir_x = 0;
		dir_y = 0;
		size = 1;

		//Display Snake at Top of Screen
		LCD_text(snke, 0);
		display_score(size);
		refresh_buffer();
		while (ReadButtons() == 0);
		clear_screen();
		refresh_buffer();
		crntBTN = 0;

		//Initialize Snake Food
		int rx = rand() % 124 + 1;
		int ry = rand() % 63 + 1;
		rx = rx - rx % 4;
		ry = ry - ry % 4;

		//Ensure Food is not on Snake
		for (i = 0; i < size; i++) {
			if ((rx == pos_x_current[i] && ry == pos_y_current[i]) && (size == 1)) {
				rx = rand() % 124 + 1;
				ry = rand() % 63 + 1;
				rx = rx - rx % 4;
				ry = ry - ry % 4;
				i--;
			}
		}

		//Draw Snake, Food, and Arena
		LCD_rect(pos_x_current[i], pos_y_current[i], length, length, 1, 1);
		LCD_rect(rx, ry, length, length, 1, 1);
		LCD_line(0, 0, 61, 1, 1);
		LCD_line(127, 0, 61, 1, 1);
		LCD_line(0, 61, 127, 1, 0);
		LCD_line(5, 0, 60, 1, 0);
		refresh_buffer();

		/***********	GAME ON **********/
		while (1) {

			//Draw Border
			LCD_line(0, 0, 61, 1, 1);
			LCD_line(127, 0, 61, 1, 1);
			LCD_line(0, 61, 127, 1, 0);
			LCD_line(5, 0, 60, 1, 0);
			// Draw Food Location
			LCD_rect(rx, ry, length, length, 1, 1);

			//Erase Snake
			for (i = 0; i < size; i++) {
				LCD_rect(pos_x_current[i], pos_y_current[i], length, length, 0, 1);
			}

			//Snake Direction
			if (ReadButtons()) {
				if (crntBTN != prevBTN) {
					if (crntBTN == 1 && dir_x != -4) {
						dir_x = 4;
						dir_y = 0;
					}
					if (crntBTN == 2 && dir_x != 4) {
						dir_x = -4;
						dir_y = 0;
					}
					if (crntBTN == 4 && dir_y != 4) {
						dir_y = -4;
						dir_x = 0;
					}
					if (crntBTN == 8 && dir_y != -4) {
						dir_y = 4;
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
			for (i = 1; i < size; i++) {
				pos_x_current[i] = pos_x_previous[i - 1];
				pos_y_current[i] = pos_y_previous[i - 1];
			}

			//Eats Food
			if ((pos_x_current[0] == rx) && (pos_y_current[0] == ry)) {
				rx = rand() % 124 + 1;
				ry = rand() % 64 + 1;
				rx = rx - rx % 4;
				ry = ry - ry % 4;
				for (i = 0; i<size; i++) {
					if ((rx == pos_x_current[i] && ry == pos_y_current[i]) && (size == 1)) {
						rx = rand() % 124 + 1;
						ry = rand() % 63 + 1;
						rx = rx - rx % 4;
						ry = ry - ry % 4;
						i--;
					}
					if ((rx == pos_x_current[i] && ry == pos_y_current[i]) && (size != 1)) {
						rx = rand() % 124 + 1;
						ry = rand() % 63 + 1;
						rx = rx - rx % 4;
						ry = ry - ry % 4;
						i = 0;
					}
				}
				if (dir_x == 4) {
					pos_x_current[size] = pos_x_current[size - 1] - 4;
					pos_y_current[size] = pos_y_current[size - 1];
				}
				else if (dir_x == -4) {
					pos_x_current[size] = pos_x_current[size - 1] + 4;
					pos_y_current[size] = pos_y_current[size - 1];
				}
				else if (dir_y == 4) {
					pos_y_current[size] = pos_y_current[size - 1] + 4;
					pos_x_current[size] = pos_x_current[size - 1];
				}
				else {
					pos_y_current[size] = pos_y_current[size - 1] - 4;
					pos_x_current[size] = pos_x_current[size - 1];
				}
				++size;
				display_score(size);
			}

			// Update Position
			for (i = 0; i < size; i++) {
				LCD_rect(pos_x_current[i], pos_y_current[i], length, length, 1, 1);
				pos_x_previous[i] = pos_x_current[i];
				pos_y_previous[i] = pos_y_current[i];
			}

			//Check Collision and Gameover
			if (game_over(pos_x_current, pos_y_current, dir_x, dir_y, length, size) == 1) {
				clear_screen();
				refresh_buffer();
				LCD_text(text_gameover_upper, 0);
				LCD_text(text_gameover_lower, 1);
				refresh_buffer();
				while (ReadButtons() == 0);
				clear_screen();
				refresh_buffer();
				prevBTN = -1;
				for (delay_count = 1000000; delay_count != 0; --delay_count);
				break;
			}

			// Update Screen & Delay Loop
			refresh_buffer();
			for (delay_count = 950000; delay_count != 0; --delay_count);
		}
	}
}