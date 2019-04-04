#ifndef _INC_HIGHSCORE_H_
#define _INC_HIGHSCORE_H_

int Read_Buttons(void);
int Read_Switches(int switch_num);
int check_highscore(int score);
void update_highscore(int score);
void print_highscore();
void type();
void game_over_text(int score);

#endif
