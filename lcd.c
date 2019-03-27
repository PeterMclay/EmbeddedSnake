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
    int x, y, length, dir_x, dir_y;
    volatile int delay_count; // volatile so C compiler doesn't remove the loop
    
    char text_top_lcd[17] = "Snake \0";

	//Initiliaze LCD
    init_spim0();
    init_lcd();
    clear_screen();

    // Output Progress on Page 0
    LCD_text(text_top_lcd, 0);

    /* initialize first position of box */
    x      = 0;
    y      = 16;
    length = 8;
    dir_x  = 1;
    dir_y  = 1;
    LCD_rect(x, y, length, length, 1, 1);
    refresh_buffer();

    while (1) {
        /* erase box */
        LCD_rect(x, y, length, length, 0, 1);

        // Update Snake Direction
		if(ReadButtons()){
			if (crntBTN != prevBTN) {
				if (crntBTN == 1) {
					dir_x = 1;
					dir_y = 0;
				}
				else if (crntBTN == 2) {
					dir_x = -1;
					dir_y = 0;
				}
				else if (crntBTN == 4) {
					dir_y = 1;
					dir_x = 0;
				}
				else {
					dir_y = -1;
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

int ReadButtons(void){
	if ((((*BTN)&0x01) == 1)){
		while((((*BTN)&0x01) == 1));
		printf("right");
		crntBTN = 1;
		return 1;
	}
	if ((((*BTN) & 0x02) == 2)) {
		while ((((*BTN) & 0x02) == 2));
		printf("left");
		crntBTN = 2;
		return 1;
	}
	if ((((*BTN) & 0x04) == 4)) {
		while ((((*BTN) & 0x04) == 4));
		printf("up");
		crntBTN = 4;
		return 1;
	}
	if ((((*BTN) & 0x08) == 8)) {
		while ((((*BTN) & 0x08) == 8));
		printf("down");
		crntBTN = 8;
		return 1;
	}
	return 0;
}

