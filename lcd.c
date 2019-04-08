#include "lcd_driver.h"
#include "lcd_graphic.h"
#include "address_map_arm.h"
#include "GSInterface.h"
#include "Highscore.h"
/*******************************************************************************
ECE 3375 Group Projecct
SNAKE
Pieter van Gaalen, Peter McLay, Sean Troy, Ariel Tebeka
******************************************************************************/

//Global Variables
volatile int* BTN = (int*)KEY_BASE;
int crntBTN;
int prevBTN = 0;
volatile int dir_x, dir_y = 0;
unsigned char decodeTable[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
volatile long * DISPLAY = HEX3_HEX0_BASE;

//Functions
//Display Score
void displayScore(int length) {
	int startingLength = 1;
	length -= startingLength;
	//LSR and MSR are 8 bit registers
	unsigned char bit0 = length % 10; //Load 1st digit (first 4 bits)
	unsigned char bit1 = length / 10; // Load 2nd digit (last 4 bits) (shift 4 bits)
	*DISPLAY = decodeTable[bit1] * 0x100 + decodeTable[bit0];
	return;
}
//Read Button Press to Start
int ReadButtonsStart(void) {
	if ((*BTN) == 0x01) {
		return 1;
	}
	else if ((*BTN) == 0x02) {
		return 1;
	}
	else if ((*BTN) == 0x04) {
		return 1;
	}
	else if ((*BTN) == 0x08) {
		return 1;
	}
	return 0;
}
//Read Buttons
void ReadButtons(void) {
	if ((*BTN) == 0x01) {
		if (dir_x == -1) return;
		dir_x = 1;
		dir_y = 0;
		return;
	}
	else if ((*BTN) == 0x02) {
		if (dir_y == 1) return;
		dir_y = -1;
		dir_x = 0;
		return;
	}
	else if ((*BTN) == 0x04) {
		if (dir_y == -1) return;
		dir_y = 1;
		dir_x = 0;
		return;
	}
	else if ((*BTN) == 0x08) {
		if (dir_x == 1) return;
		dir_x = -1;
		dir_y = 0;
		return;
	}
	return;
}
//Read Switches
int readSwitch() {
	volatile int * switchPointer = SW_BASE;
	return (*switchPointer) & 0x01;
}
//Gyroscope Initialize
void GSInit(void) {
	//Full Resolution
	WriteGSRegister(GS_DATA_FORMAT, 0x08);
	//Sampling rate of 10 == 100Hz
	WriteGSRegister(GS_BW_RATE, 0x0B);
	// Configure to begin measurement
	WriteGSRegister(GS_POWER_CTL, 0x08);
}
//Read Accelerometer
void accelerometer_controls() {
	unsigned char position[6] = { 0,0,0,0,0,0 };
	if (ReadGSRegister(GS_INT_SOURCE) & 0x80) {
		MultiReadGS(GS_DATAX0, position, 6);
		//Sampling
		MultiReadGS(GS_DATAX0, position, 6);
		//Delay
		int d = 0;
		for (d = 0; d < 700000; d++) {}
	}
	int i;
	for (i = 0; i<3; i++) {
		//Check direction
		if (position[2 * i + 1] == 255) {
			position[2 * i] = 255 - position[2 * i];
		}
	}
	int direction = 0;
	int threshold = 30;
	//Direction Logic  
	if (position[0] <= threshold && position[2] <= threshold) {
		return;
	}
	//X axis
	if (position[0] > position[2]) {
		if (position[1] == 0) {
			if (dir_x == -1) return;
			dir_x = 1;
			dir_y = 0;
		}

		else {
			if (dir_x == 1) return;
			dir_x = -1;
			dir_y = 0;
		}
	}
	//Y axis
	else {
		if (position[3] == 0) {
			if (dir_y == 1) return;
			dir_y = -1;
			dir_x = 0;
		}
		else {
			if (dir_y == -1) return;
			dir_y = 1;
			dir_x = 0;
		}
	}
	return;
}
//Collision Detection
int game_over(int *posx, int *posy, int xRightBound, int xLeftBound, int yBotBound, int yTopBound, int length) {
	if ((posx[0] > xRightBound) || (posx[0] < xLeftBound)) {
		return 1;
	}
	if ((posy[0] > yBotBound) || (posy[0] < yTopBound)) {
		return 1;
	}
	int j;
	for (j = 1; j < length; j++) {
		if (posx[0] == posx[j] && posy[0] == posy[j]) {
			return 1;
		}
	}
	return 0;
}


/******************************************************************** MAIN ************************************************************************************************/
int main(void) {

	//Gyroscope Set Up
	I2C0Init();
	GSInit();

	char on[25] = "Press Button to Start \0";
	char snke[10] = "    SNAKE\0";
	char start[66] = "Flick Switch forMotion Control, Press Button forButton Control \0";
	char text_gameover_lcd[17] = "Game over :(\0";
	char text_top_lcd[35] = "Press Any ButtonTo Play Again\0";

	volatile int * ledPointer = (int *)LED_BASE;
	int prev_switch = readSwitch();
	int gyro_button = -1;
	int pos_x_current[50];
	int pos_y_current[50];
	int pos_x_previous[50];
	int pos_y_previous[50];
	volatile int length;
	int i;
	int collision;
	int turn;
	volatile int delay_count; // volatile so C compiler doesn't remove the loop
	int xWidth;
	int yWidth;
	int xLeftOutline;
	int xRightOutline;
	int yTopOutline;
	int yBotOutline;
	int xLeftBound;
	int xRightBound;
	int yTopBound;
	int yBotBound;
	volatile double doomValue;
	volatile int rx;
	volatile int ry;

	/******************************************************************** START SCREEN ************************************************************************************************/
	while (1) {
		pos_x_current[0] = 8;
		pos_y_current[0] = 5;
		pos_x_previous[0] = 8;
		pos_y_previous[0] = 5;
		length = 1;
		collision = 0;
		turn = 1;
		xWidth = 7;
		yWidth = 5;
		xLeftOutline = 1;
		xRightOutline = 1;
		yTopOutline = 2;
		yBotOutline = 2;
		xLeftBound = 0;
		xRightBound = (SCREEN_WIDTH - xLeftOutline - xRightOutline) / xWidth - 1;
		yTopBound = 0;
		yBotBound = (SCREEN_HEIGHT - yTopOutline - yBotOutline) / yWidth - 1;
		doomValue = 1.0;

		//Initialize LCD21
		init_spim0();
		init_lcd();
		clear_screen();

		//Selecting Style of Play
		LCD_text(snke, 0);
		LCD_text(start, 1);
		refresh_buffer();
		while (gyro_button == -1) {
			if (readSwitch() != prev_switch) {
				gyro_button = 1;
				prev_switch = readSwitch();
			}
			if (ReadButtonsStart()) {
				gyro_button = 0;
			}
		}
		clear_screen();
		refresh_buffer();
		for (delay_count = 1000000; delay_count != 0; --delay_count);

		//Button Press to Start Game
		LCD_text(on, 0);
		refresh_buffer();
		while (ReadButtonsStart() != 1);
		clear_screen();
		refresh_buffer();
		crntBTN = 0;

		//Draw bounds
		LCD_rect(0, 0, xLeftOutline, 64, 1, 1); //left
		LCD_rect(0, 0, SCREEN_WIDTH, yTopOutline, 1, 1); //Top
		LCD_rect(SCREEN_WIDTH - xRightOutline, 0, xRightOutline, 64, 1, 1); //right
		LCD_rect(0, SCREEN_HEIGHT - yBotOutline, 128, yBotOutline, 1, 1); //Bot

																		  //Initialize Initial Berry Position
		rx = rand() % ((SCREEN_WIDTH - xLeftOutline - xRightOutline) / xWidth);
		ry = rand() % ((SCREEN_HEIGHT - yTopOutline - yBotOutline) / yWidth);

		//Ensure Berry is not on Snake
		for (i = 0; i<length; i++) {
			if (pos_x_current[i] == rx && pos_y_current[i] == ry) {
				rx = rand() % (SCREEN_WIDTH - xLeftOutline - xRightOutline) / xWidth;
				ry = rand() % (SCREEN_HEIGHT - yTopOutline - yBotOutline) / yWidth;
				i = 0;
				continue;
			}
		}

		//Draw Starting Position of Snake
		for (i = 0; i < length; i++) {
			LCD_rect(pos_x_current[i] * xWidth + xLeftOutline, pos_y_current[i] * yWidth + yTopOutline, xWidth, yWidth, 1, 1);
		}
		refresh_buffer();

		/********************************************************************	GAME ON ************************************************************************************************/
		while (1) {

			//Erase Snake
			for (i = 0; i < length; i++) {
				LCD_rect(pos_x_current[i] * xWidth + xLeftOutline, pos_y_current[i] * yWidth + yTopOutline, xWidth, yWidth, 0, 1);
			}

			//Update Direction of Snake
			pos_x_current[0] += dir_x;
			pos_y_current[0] += dir_y;
			if (gyro_button == 1) {
				accelerometer_controls();
			}

			//Check Game Over
			if (game_over(pos_x_current, pos_y_current, xRightBound, xLeftBound, yBotBound, yTopBound, length)) {
				clear_screen();
				dir_x = 0;
				dir_y = 0;
				LCD_text(text_gameover_lcd, 0);
				LCD_text(text_top_lcd, 1);
				refresh_buffer();
				
				game_over_text(length-1);
				
				while (ReadButtonsStart() == 0);
				crntBTN = 0;
				prev_switch = readSwitch();
				for (delay_count = 950000; delay_count != 0; --delay_count);
				gyro_button = -1;
				displayScore(1);
				break;
			}

			//Check if berry acquired
			if ((pos_x_current[0] == rx) && (pos_y_current[0] == ry)) {
				rx = rand() % (SCREEN_WIDTH - xLeftOutline - xRightOutline) / xWidth;
				ry = rand() % (SCREEN_HEIGHT - yTopOutline - yBotOutline) / yWidth;
				for (i = 0; i<length; i++) {
					if (pos_x_current[i] == rx && pos_y_current[i] == ry) {
						rx = rand() % (SCREEN_WIDTH - xLeftOutline - xRightOutline) / xWidth;
						ry = rand() % (SCREEN_HEIGHT - yTopOutline - yBotOutline) / yWidth;
						i = 0;
						continue;
					}
				}
				length += 1;
				doomValue = 1.0 / ((1.0-(1/18.0)) + (length / 18.0));
			}

			//Display the score
			displayScore(length);

			//Update Position
			for (i = 1; i < length; i++) {
				pos_x_current[i] = pos_x_previous[i - 1];
				pos_y_current[i] = pos_y_previous[i - 1];
			}

			//Draw Snake
			for (i = 0; i < length; i++) {
				LCD_rect(pos_x_current[i] * xWidth + xLeftOutline, pos_y_current[i] * yWidth + yTopOutline, xWidth, yWidth, 1, 1);
				pos_x_previous[i] = pos_x_current[i];
				pos_y_previous[i] = pos_y_current[i];
			}

			//Draw Food
			LCD_rect(rx*xWidth + xLeftOutline, ry*yWidth + yBotOutline, xWidth, yWidth, 1, 1);
			LCD_line(rx*xWidth + xLeftOutline ,ry*yWidth + yBotOutline, 1, 0, 0);
			LCD_line(rx*xWidth + xLeftOutline + xWidth - 1 , ry*yWidth + yBotOutline, 1, 0, 0);
			LCD_line(rx*xWidth + xLeftOutline, ry*yWidth + yBotOutline+yWidth - 1, 1, 0, 0);
			LCD_line(rx*xWidth + xLeftOutline+xWidth - 1,ry*yWidth + yBotOutline+yWidth - 1, 1, 0, 0);
			LCD_line(rx*xWidth + xLeftOutline+3,ry*yWidth + yBotOutline+2, 1, 0, 0);
			
			//Draw snake head 
			LCD_line(pos_x_current[0]*xWidth + xLeftOutline+((xWidth-1)*(dir_x+dir_y>0)), pos_y_current[0]*yWidth + yBotOutline+((yWidth-1)*((dir_x+dir_y>0))), 1, 0, 0);
			LCD_line(pos_x_current[0]*xWidth + xLeftOutline+((xWidth-1)*(!(dir_y-dir_x>0))), pos_y_current[0]*yWidth + yBotOutline+((yWidth-1)*((dir_y-dir_x>0))), 1, 0, 0);
			
			refresh_buffer();

			// Delay Loop
			if (gyro_button == 1) {
				for (delay_count = 1800000 * doomValue; delay_count != 0; --delay_count);
			}
			else {
				for (delay_count = 110000 * doomValue; delay_count != 0; --delay_count) {
					for (i = 0; i < 1000; i++);
					ReadButtons();
				}
			}
		}
	}
}
