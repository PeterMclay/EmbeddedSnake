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
int main(void) {
    int          x, y, length, dir_x, dir_y;
    volatile int delay_count; // volatile so C compiler doesn't remove the loop
    /* create a message to be displayed on the VGA display */
    char text_top_lcd[5]    = "Snake\0";
    //char text_bottom_lcd[17] = "By pete_sauce \0";

    init_spim0();
    init_lcd();

    clear_screen();

    /* output the text message on the LCD display */
    LCD_text(text_top_lcd, 0);
    //LCD_text(text_bottom_lcd, 1);

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

        /* update direction */
        /*if ((x + length >= SCREEN_WIDTH - 1 && dir_x == 1) ||
            (x <= 0 && dir_x == -1))
            dir_x = -dir_x;

        if ((y + length >= SCREEN_HEIGHT - 1 && dir_y == 1) ||
            (y <= 16 && dir_y == -1))
            dir_y = -dir_y;
		*/
        /* update coordinates */
		if(ReadButtons()){
			x += 8;
		}

        //x += dir_x;
        // y += dir_y;

        /* draw box */
        LCD_rect(x, 16, length, length, 1, 1);
        refresh_buffer();

        for (delay_count = 100000; delay_count != 0; --delay_count)
            ; // delay loop
    }
}

int ReadButtons(void){
	printf("HI \n");
	if ((((*BTN)&0x01) == 1)){
		while((((*BTN)&0x01) == 1));
		printf("hello");
		return 1;
	}
	else return 0;
}

