#include "address_map_arm.h"
#include "GSInterface.h"

#include "lcd_driver.h"
#include "lcd_graphic.h"

#include <stdio.h>
#include <string.h>

volatile int count = 0;
volatile int type_flag = 1; 
char lcd_letter[5] = {};

// Score Display
void DisplayHex_NUM(char MSR, char LSR, int d)
{
	volatile long *DISPLAY0 = HEX3_HEX0_BASE;
	volatile long *DISPLAY1 = HEX5_HEX4_BASE;
	volatile long display[] = {*DISPLAY0, *DISPLAY1};

	unsigned char bit0 = LSR&0xF;
	unsigned char bit1 = LSR>>4;
	unsigned char bit2 = MSR&0xF;
	unsigned char bit3 = MSR>>4;

	volatile int lookUpTable[17] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
	display[d] = lookUpTable[bit0] + (lookUpTable[bit1] * 256) + (lookUpTable[bit2] * 65536) + (lookUpTable[bit3] * 16777216);
}

int ReadButtons(void){
	volatile int* BTN = (int*)KEY_BASE;
	if ((((*BTN)&0x01) == 1)){
		// while((((*BTN)&0x01) == 1));
		printf("right");
		return 1;
	}
	if ((((*BTN) & 0x02) == 2)) {
		// while ((((*BTN) & 0x02) == 2));
		printf("left");
		return 1;
	}
	if ((((*BTN) & 0x04) == 4)) {
		// while ((((*BTN) & 0x04) == 4));
		printf("up");
		return 1;
	}
	if ((((*BTN) & 0x08) == 8)) {
		// while ((((*BTN) & 0x08) == 8));
		printf("down");
		return 1;
	}
	return 0;
}


int ReadSwitches(int switch_num)
{
	volatile int * SW_ptr = (int *)SW_BASE;
	
	switch(switch_num){
	  case 1:
		return (*SW_ptr)&0x01;
		break;
	
		 case 2:
			return (*SW_ptr)&0x01;
			break;
		
		 case 3:
			return (*SW_ptr)&0x01;
			break;
			
		 case 4:
			return (*SW_ptr)&0x01;
			break;
			
		 case 5:
			return (*SW_ptr)&0x01;
			break;
			
		case 6:
			return (*SW_ptr)&0x01;
			break;

	  default:
		return (*SW_ptr)&0x01F;
    }
}

void type(){
	
	// if (HiSc == 1)
		//{
			//LCD_text(HiSc, 3);
			
		//}
	

	if (ReadButtons() == 0){
		int switch_value = ReadSwitches(0);
		volatile char alphabet_table[26] = {'a', 'b', 'c', 'd' , 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
		char letter = alphabet_table[26];
		lcd_letter[count] = alphabet_table[switch_value];
		lcd_letter[count+1] = alphabet_table['\0'];
		//char lcd_letter[5] = {alphabet_table[switch_value], '\0'};
		printf("Count: %d \n", count);
		
		LCD_text(lcd_letter, 3);
		refresh_buffer();
		type_flag = 1;
	}
	
	else if(type_flag){
		count += 1;
		type_flag = 0;
	}

}


void DisplayStatus(int status, int display_num){
	volatile long * DISPLAY = HEX3_HEX0_BASE; //DISPLAY0 points to the three rightmost 7-segment displays
	volatile int lookUpTable[16] ={ 0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71,};
	
	if(display_num == 1){
		*DISPLAY = lookUpTable[status];
	}
	
	if(display_num == 2){
		*DISPLAY = lookUpTable[status] * 0x100;
	}
	
	if(display_num == 3){
		*DISPLAY = lookUpTable[status] * 0x10000;
	}
	
	if(display_num == 4){
		*DISPLAY = lookUpTable[status] * 0x1000000;
	}	
}



void GSInit(void){

	//Full Resolution
	WriteGSRegister(GS_DATA_FORMAT, 0x08);
	//Sampling rate of 10 == 100Hz
	WriteGSRegister(GS_BW_RATE, 0x0B);
	// Configure to begin measurement
	WriteGSRegister(GS_POWER_CTL, 0x08);
}
int accelerometer_controls(){
		
	unsigned char position[6] = {0,0,0,0,0,0};
	if (ReadGSRegister(GS_INT_SOURCE)&0x80){
		MultiReadGS(GS_DATAX0, position, 6);

		//Sampling
		MultiReadGS(GS_DATAX0, position, 6);
		
	
		//Delay
		int d = 0;
		for (d = 0; d < 700000; d++){}
	}
	
	
	
	//Reorder
	// 0 <=====> 255
	int i;
	for(i = 0; i<3; i++){
      //Check direction
      if(position[2*i+1] == 255){
        position[2*i] = 255 - position[2*i];
      }
    }
	printf("Array: 5: %d 4: %d 3: %d 2: %d 1: %d 0: %d \n", position[5], position[4], position[3], position[2], position[1], position[0]);

  
  	int direction = 0;
	int threshold = 30;
	
	
	//Direction Logic  
	if(position[0] <= threshold && position[2] <= threshold){
		return 0;
	}	

	//X axis
	if(position[0] > position[2]){
		if(position[1] == 0){
		  direction = 2;
		}

		else{
		  direction = 4;
		}
	}

	//Y axis
	else{
		if(position[3] == 0){
		  direction = 1;
		}
		else{
		  direction = 3;
		}
	}

	
	return direction;
}


int main(void){

	I2C0Init();
	
	//Initialize LCD
	I2C0Init();
	init_spim0();
	init_lcd();
	clear_screen();
	
	char mode_control_text[15] = "Control Mode:\0";
	char accelerometer_text[15] = "Accelerometer\0";
	char button_text[10] = "Button\0";
	 
	LCD_text(mode_control_text, 0);
	refresh_buffer();
	
	if (ReadSwitches(1) == 0){
		LCD_text(accelerometer_text, 1);
		refresh_buffer();
	}
	
	else{
		LCD_text(button_text, 1);
		refresh_buffer();
	}
	// Start program once communication is established
	while(1){
		
		// DEVID register holds fixed device ID of 0xE5
		if(ReadGSRegister(GS_DEVID) == 0xE5){
			DisplayStatus(1,0);
			break;
		}

		else{
			DisplayStatus(3,0);
		}
	}

	// Initialize the GS unit for sampling
	GSInit();

	//Sampling
	while (1)
	{
		type();
		int move = accelerometer_controls();
		DisplayStatus(move,2);
	}
}
