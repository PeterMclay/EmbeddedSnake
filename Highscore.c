#include "address_map_arm.h"
#include "GSInterface.h"

#include "lcd_driver.h"
#include "lcd_graphic.h"

#include <stdio.h>
#include <string.h>


char type_text[5] = {};
int type_text_length = 5;

int highscore_list[5] = {50,44,33,2,0};
int highscore_list_size = 5;

char player_name1[7] = "Piet\0";
char player_name2[7] = "Tron\0";
char player_name3[7] = "Sean\0";
char player_name4[7] = "Arel\0";
char player_name5[7] = "TA\0";
char *player_name[5] = {player_name1, player_name2, player_name3, player_name4, player_name5};
char endstring[2] = "\0";
int player_name_length = 4;

// Score Display
int Read_Buttons(void){
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


int Read_Switches(int switch_num)
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



int check_highscore(int score){
	if(score >= highscore_list[highscore_list_size-1]){
		return 1;
	}
	return 0;
}

void update_highscore(int score){

	int update = 0;
	int temp_score = 0;
	char temp_name[7];
	int i;
	for(i=0; i<highscore_list_size; i++){

		if(update == 1){
			int temp = highscore_list[i];
			highscore_list[i] = temp_score;
			temp_score = temp;
			
			char temp_char[7];
			strcpy(temp_char, player_name[i]);
			strcpy(player_name[i], temp_name);
			strcpy(temp_name, type_text);
		}

		else if(score > highscore_list[i] && update == 0){
			temp_score = highscore_list[i];
			highscore_list[i] = score;
			update = 1;
			
			strcpy(temp_name, player_name[i]);
			strcpy(player_name[i], type_text);

		}
	}
}

void print_highscore(){

	
	char highscore_text[10] = "Highscore\0";

	clear_screen();

	LCD_text(highscore_text, 1);
	refresh_buffer();

	int i;
	char highscore_line[10] = "";
	for(i=0; i<5; i++){
		
		char char_lookup[10] = {'0', '1', '2', '3','4', '5', '6', '7', '8', '9'};
		int number_ten = highscore_list[i]/10;
		int number_one = highscore_list[i]%10;
		
		printf("Ten: %c \n", char_lookup[number_ten]);
		printf("One: %c \n", char_lookup[number_one]);
		
		char char_score[3];
		//{char_lookup[number_ten], char_lookup[number_one], "\0"}
		
		sprintf(char_score, "%d", highscore_list[i] );
		printf("Num: %s \n", char_score);
		printf("Name: %s \n", player_name[i]);

		char highscore_line[15] = "";
		
		
		strcat(highscore_line, player_name[i]);
		printf("TEST \n");
		strcat(highscore_line, " ");
		printf("TEST \n");
		strcat(highscore_line, char_score);

		printf("Test %s \n", highscore_line);
		LCD_text(highscore_line, i+3);
		refresh_buffer();
		
	}
}

void type(){

	volatile int count = 0;
	volatile int type_flag = 0; 

	char new_highscore_text[15] = "New Highscore!\0";
	char enter_player_name_text[20] = "Enter username:\0";
	
	clear_screen();
	LCD_text(new_highscore_text, 0);
	LCD_text(enter_player_name_text, 1);
	
	
	//Type	
	while(1){
		if (Read_Buttons() == 0 && count < player_name_length){
			int switch_value = Read_Switches(0);
			volatile char alphabet_table[31] = {'a', 'b', 'c', 'd' , 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', ' ', ' ', ' ', ' ' };
			type_text[count] = alphabet_table[switch_value];
			type_text[count+1] = '\0';
			//char player_name[5] = {alphabet_table[switch_value], '\0'};
			printf("Count: %d \n", count);
			LCD_text(type_text, 3);
			refresh_buffer();
			
			type_flag = 1;
		}

		//Increment count if letter is typed
		else if(type_flag){
			count += 1;
			type_flag = 0;
		}

		//Submit button
		else if(Read_Buttons() == 2){
			count = player_name_length;
		}
		else if(count >= player_name_length){
			break;
		}
	}
}

void game_over_text(int score){
	if(check_highscore(score)){
		type();
		update_highscore(score);

	}
	print_highscore();
}
	
	

