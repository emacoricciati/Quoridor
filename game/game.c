#include "game.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "../CAN/CAN.h"
#include "stdlib.h"
#include <stdio.h>
#include "string.h"
#include "../agent/agent.h"

volatile int game_matrix[15][15];

uint32_t moves[100];
int ind = 0;
static int game_started = 1;

volatile Player p1, p2;
volatile Wall w;

volatile int turn = 1;
volatile int move_mode = -1;

volatile Game_mode game_mode;
extern int timer0_count;
extern int current_player_id;

int minimizing=0;
int maximizing=0;

// utils

int convert_index_bts(int i){
	
	if(i%2 == 0){
		return i/2;
	}
	return i - i/2 - 1;

}

int convert_index_stb(int i){
	
	return 2*i + 1;
}

int convert_index_stb_wall(int i){
	return 2*i;
}
// init functions

void init_game_matrix(void){

	int i, j;
	
	for(i=0;i<15; i++){
		for(j=0;j<15;j++){
			game_matrix[i][j] = 0;
		}
	}

}

void init_players(void){

	// p1
	p1.id = 1;
	p1.available_walls = INITIAL_WALLS;
	p1.current_position.x = 7;
	p1.current_position.y = 13;
	p1.position.x = 7;
	p1.position.y = 13;
	// p2
	p2.id = 2;
	p2.available_walls = INITIAL_WALLS;
	p2.current_position.x = 7;
	p2.current_position.y = 1;
	p2.position.x = 7;
	p2.position.y = 1;

}

void init_game(void){

	disable_timer(0);
	disable_timer(1);
	reset_timer(0);
	reset_timer(1);
	move_mode = -1;
	game_started = 1;
	current_player_id = -1;
	game_mode.menu1 = 0;
	game_mode.menu2 = 0;

}

void menu1_screen(void){

	LCD_Clear(White);
	GUI_Text(80, 30, (unsigned char*) "Select the", Black, White);
	GUI_Text(85, 60, (unsigned char*) "GAME MODE", White, Black);
	// default case
	DrawRectangle(60,110,40,120);
	ColorRectangle(60,110,40,120,Yellow2);
	GUI_Text(75, 120, (unsigned char*) "SINGLE BOARD", Black, Yellow2);
	DrawRectangle(60,180,40,120);
	GUI_Text(80, 190, (unsigned char*) "TWO BOARDS", Black, White);
	game_mode.current_selection = 1;

}

void menu2_screen(void){

	LCD_Clear(White);
	GUI_Text(45, 30, (unsigned char*) "Single board: select", Black, White);
	GUI_Text(45, 60, (unsigned char*) "the opposite player", Black, White);
	// default case
	DrawRectangle(60,110,40,120);
	ColorRectangle(60,110,40,120,Yellow2);
	GUI_Text(100, 120, (unsigned char*) "HUMAN", Black, Yellow2);
	DrawRectangle(60,180,40,120);
	GUI_Text(110, 190, (unsigned char*) "NPC", Black, White);
	// init game mode
	game_mode.current_selection = 1;

}

void menu3_screen(void){

	LCD_Clear(White);
	GUI_Text(45, 30, (unsigned char*) "Two boards: select", Black, White);
	GUI_Text(45, 60, (unsigned char*) "your player", Black, White);
	// default case
	DrawRectangle(60,110,40,120);
	ColorRectangle(60,110,40,120,Yellow2);
	GUI_Text(100, 120, (unsigned char*) "HUMAN", Black, Yellow2);
	DrawRectangle(60,180,40,120);
	GUI_Text(110, 190, (unsigned char*) "NPC", Black, White);
	// init game mode
	game_mode.current_selection = 1;

}

void waiting_to_play(void){

	LCD_Clear(White);
	GUI_Text(55,20,(uint8_t *) "Press INT0 to be",Black,White);
	GUI_Text(85,40,(uint8_t *) "PLAYER1",White,Black);
	DrawPawn(2,2,1);
	GUI_Text(60,100,(uint8_t *) "PLAYER1",Black,White);
	DrawPawn(4,2,2);
	GUI_Text(125,100,(uint8_t *) "PLAYER2",Black,White);

}

void color_mode(int choice){

	switch(choice){
		case 1: 	
			ColorRectangle(60,180,40,120,White);
			game_mode.menu1 == 0 ? GUI_Text(80, 190, (unsigned char*) "TWO BOARDS", Black, White) : GUI_Text(110, 190, (unsigned char*) "NPC", Black, White);
			ColorRectangle(60,110,40,120,Yellow2);
			game_mode.menu1 == 0 ? GUI_Text(75, 120, (unsigned char*) "SINGLE BOARD", Black, Yellow2) : GUI_Text(100, 120, (unsigned char*) "HUMAN", Black, Yellow2);
			break;
		case 2:
			ColorRectangle(60,110,40,120,White);
			game_mode.menu1 == 0 ? GUI_Text(75, 120, (unsigned char*) "SINGLE BOARD", Black, White) : GUI_Text(100, 120, (unsigned char*) "HUMAN", Black, White);
			ColorRectangle(60,180,40,120,Yellow2);
			game_mode.menu1 == 0 ? GUI_Text(80, 190, (unsigned char*) "TWO BOARDS", Black, Yellow2) : GUI_Text(110, 190, (unsigned char*) "NPC", Black, Yellow2);
			break;
		default:
			break;
	}

}

void confirm_selection(void){
	
	if(game_mode.menu1 == 0){
		game_mode.menu1 = game_mode.current_selection;
		game_mode.current_selection = 1;
		game_mode.menu1 == 1 ? menu2_screen() : menu3_screen();
	}
	else {
		game_mode.menu2 = game_mode.current_selection;
		// single board -> human
		if(game_mode.menu1 == 1 && game_mode.menu2 == 1){
			game_setup();
		}
		// single_board -> NPC
		if(game_mode.menu1 == 1 && game_mode.menu2 == 2){
			maximizing = 2;
			minimizing = 1;
			game_setup();
		}
		// two boards -> human
		if(game_mode.menu1 == 2 && game_mode.menu2 == 1){
			// initialization
			CAN_Init();
			waiting_to_play();
		}
		// two boards -> NPC
		if(game_mode.menu1 == 2 && game_mode.menu2 == 2){
			// initialization
			CAN_Init();
			waiting_to_play();
		}
	}

}
void game_setup(void){

	LCD_Clear(White);
	init_game_matrix();
	init_players();
	init_wall();
	enable_timer(0);
	enable_timer(1);
	move_mode = 1;
	turn = 1;
	timer0_count = 0;
	display_grid();
	
	// single board
	if((game_mode.menu1 == 1 && game_mode.menu2 == 1) || (game_mode.menu1 == 1 && game_mode.menu2 == 2) ){
		if(turn == 1){
			get_possible_moves(&p1,(int (*)[15])game_matrix);
			find_possible_moves(&p1);
		}
		else {
			get_possible_moves(&p2,(int (*)[15])game_matrix);
			find_possible_moves(&p2);
		}
	}
	// two boards
	else if(game_mode.menu1 == 2 && game_mode.menu2 == 1){
		if(current_player_id == turn){
			get_possible_moves(&p1,(int (*)[15])game_matrix);
			find_possible_moves(&p1);
		}
	}
	else if(game_mode.menu1 == 2 && game_mode.menu2 == 2 && current_player_id != -1){
		maximizing = current_player_id;
		minimizing = current_player_id == 1 ? 2 : 1;
	}

}

// move mode
void color_moves(volatile Player *p, uint16_t color){
	
	int i;
	int x = convert_index_bts(p->position.x);
	int y = convert_index_bts(p->position.y);
	int current_x = convert_index_bts(p->current_position.x);
	int current_y = convert_index_bts(p->current_position.y);
	
	if(x != current_x || y != current_y){
		ColorSquareThroughPosition(x,y,color);
		
	}
	for(i=0; i<p->count_pmoves; i++){
		if(strcmp((char *)p->possible_moves[i],"uu")==0){
			if(current_y != y - 2){
				ColorSquareThroughPosition(x,y-2,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"u")==0){
			if(current_y != y - 1){
				ColorSquareThroughPosition(x,y-1,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"ul")==0){
			if(current_x != x - 1 || current_y != y - 1){
				ColorSquareThroughPosition(x-1,y-1,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"ur")==0){
			if(current_x != x + 1 || current_y != y -1 ){
				ColorSquareThroughPosition(x+1,y-1,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"ll")==0){
			if(current_x != x - 2){
				ColorSquareThroughPosition(x-2,y,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"l")==0){
			if(current_x != x -1 ){
				ColorSquareThroughPosition(x-1,y,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"r")==0){
			if(current_x != x + 1){
				ColorSquareThroughPosition(x+1,y,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"rr")==0){
			if(current_x != x + 2){
				ColorSquareThroughPosition(x+2,y,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"dl")==0){
			if(current_x != x - 1 || current_y != y +1){
				ColorSquareThroughPosition(x-1,y+1,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"d")==0){
			if(current_y != y +1){
				ColorSquareThroughPosition(x,y+1,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"dr")==0){
			if(current_x != x + 1 || current_y != y + 1){
				ColorSquareThroughPosition(x+1,y+1,color);
			}
		}
		if(strcmp((char *)p->possible_moves[i],"dd")==0){
			if(current_y != y + 2){
				ColorSquareThroughPosition(x,y+2,color);
			}
		}
	}



}
void get_possible_moves(volatile Player *p, int matrix[15][15]){
	
	int last_position_x = p->position.x;
	int last_position_y = p->position.y;
	
	int index=0;
	
		// x
		if(last_position_x == 1 && matrix[last_position_y][last_position_x + 1] < 3){
			// skip the opposing player
			if((p->id == 1 && matrix[last_position_y][last_position_x + 2] == 2) || (p->id == 2 && matrix[last_position_y][last_position_x + 2] == 1)){
				strcpy((char *)p->possible_moves[index++],"rr");
			}
			else {
				strcpy((char *)p->possible_moves[index++],"r");
			}
		}
		else if(last_position_x == 13 && matrix[last_position_y][last_position_x - 1] < 3){
			// skip the opposing player
			if((p->id == 1 && matrix[last_position_y][last_position_x - 2] == 2) || (p->id == 2 && matrix[last_position_y][last_position_x - 2] == 1)){
				strcpy((char *)p->possible_moves[index++],"ll");
			}
			else {
				strcpy((char *)p->possible_moves[index++],"l");
			}
		}
		else {
				//no wall/player on the left
			if((last_position_x >= 3 && matrix[last_position_y][last_position_x - 1] < 3 && matrix[last_position_y][last_position_x - 2] != (p->id == 1 ? 2 : 1))){
					strcpy((char *)p->possible_moves[index++],"l");
				}
				//no wall/player on the right
				if((last_position_x <= 11 && matrix[last_position_y][last_position_x + 1] < 3 && matrix[last_position_y][last_position_x + 2] != (p->id == 1 ? 2 : 1))){
					strcpy((char *)p->possible_moves[index++],"r");
				}
				// player on the left
				if(last_position_x >= 3 && matrix[last_position_y][last_position_x - 2] == (p->id == 1 ? 2 : 1) && matrix[last_position_y][last_position_x-1]<3){
					// pawn at the left border
					if(last_position_x -3 == 0){
						if(matrix[last_position_y-1][last_position_x-2] < 3){
							strcpy((char *)p->possible_moves[index++],"ul");
						}
						if(matrix[last_position_y+1][last_position_x-2] < 3 ){
							strcpy((char *)p->possible_moves[index++],"dl");
						}
					}
					// no wall behind the pawn
					if(last_position_x >= 3 && matrix[last_position_y][last_position_x-3] < 3){
						strcpy((char *)p->possible_moves[index++],"ll");
					}
					// wall behind the pawn
					else{
						if(matrix[last_position_y-1][last_position_x-2] < 3){
							strcpy((char *)p->possible_moves[index++],"ul");
						}
						if(matrix[last_position_y+1][last_position_x-2] < 3){
							strcpy((char *)p->possible_moves[index++],"dl");
						}
					}
				}
				// player on the right
				if(last_position_x >= 2 && matrix[last_position_y][last_position_x + 2] == (p->id == 1 ? 2 : 1) && last_position_x <= 11){
					// pawn at the right border
					if(last_position_x + 3 == 14){
						if(matrix[last_position_y-1][last_position_x+2] < 3){
							strcpy((char *)p->possible_moves[index++],"ur");
						}
						if(matrix[last_position_y+1][last_position_x+2] < 3){
							strcpy((char *)p->possible_moves[index++],"dr");
						}
					}
					// no wall behind the pawn
					if(matrix[last_position_y][last_position_x+3] < 3){
						strcpy((char *)p->possible_moves[index++],"rr");
					}
					// wall behind the pawn
					else{
						if(matrix[last_position_y-1][last_position_x+2] < 3){
							strcpy((char *)p->possible_moves[index++],"ur");
						}
						if(matrix[last_position_y+1][last_position_x+2] < 3){
							strcpy((char *)p->possible_moves[index++],"dr");
						}
					}
				}
	}
		
		// y
		
			if(last_position_y == 13){
				if(matrix[last_position_y - 1][last_position_x] < 3 && matrix[last_position_y - 2][last_position_x] != (p->id == 1 ? 2 : 1)){
					strcpy((char *)p->possible_moves[index++],"u");
				}
				else if(matrix[last_position_y - 1][last_position_x] < 3 && matrix[last_position_y - 2][last_position_x] == (p->id == 1 ? 2 : 1) && matrix[last_position_y - 3][last_position_x] < 3 ){
					strcpy((char *)p->possible_moves[index++],"uu");
				}
			}
			// skip the opposing player
			else {
				if(last_position_y <= 12 && matrix[last_position_y + 2][last_position_x] != (p->id == 1 ? 2 : 1) && matrix[last_position_y+1][last_position_x] < 3){
					if(matrix[last_position_y + 1][last_position_x] < 3){
						strcpy((char *)p->possible_moves[index++],"d");
					}
				}
				else {
					// pawn below
					if(last_position_y <= 11 && matrix[last_position_y + 3][last_position_x] < 3 && matrix[last_position_y + 1][last_position_x] < 3){
						strcpy((char *)p->possible_moves[index++],"dd");
					}
					// pawn at the bottom border
					if(last_position_y + 3 == 14){
						if(last_position_y <= 12 && matrix[last_position_y+1][last_position_x-1] < 3){
							strcpy((char *)p->possible_moves[index++],"dl");
						}
						if(last_position_y <= 12 && matrix[last_position_y+1][last_position_x+1] < 3){
							strcpy((char *)p->possible_moves[index++],"dr");
						}
					}
					// wall behind the player
					if(last_position_y <= 11 && matrix[last_position_y + 3][last_position_x] >= 3){
						if(last_position_y <= 12 && matrix[last_position_y+1][last_position_x-1] < 3 && matrix[last_position_y + 1][last_position_x]<3){
							strcpy((char *)p->possible_moves[index++],"dl");
						}
						if(last_position_y <= 12 && matrix[last_position_y+1][last_position_x+1]< 3 && matrix[last_position_y + 1][last_position_x]<3){
							strcpy((char *)p->possible_moves[index++],"dr");
						}
					}
				}
				if(last_position_y >= 2){
					if(matrix[last_position_y - 2][last_position_x] != (p->id == 1 ? 2 : 1)){
						if( matrix[last_position_y - 1][last_position_x] < 3){
							strcpy((char *)p->possible_moves[index++],"u");
						}
					}
					else {
						// pawn above
						if(last_position_y >= 3 && matrix[last_position_y - 3][last_position_x] < 3 && matrix[last_position_y - 1][last_position_x] < 3){
							strcpy((char *)p->possible_moves[index++],"uu");
						}
						// pawn at the left border
						if(last_position_y -3 == 0){
							if(matrix[last_position_y-1][last_position_x-1] < 3){
								strcpy((char *)p->possible_moves[index++],"ul");
							}
							if(matrix[last_position_y-1][last_position_x+1] < 3){
								strcpy((char *)p->possible_moves[index++],"ur");
							}
						}
						// wall behind the player
						if(last_position_y >= 3 && matrix[last_position_y - 3][last_position_x] >= 3){
							if(matrix[last_position_y-1][last_position_x-1] < 3){
								strcpy((char *)p->possible_moves[index++],"ul");
							}
							if(matrix[last_position_y-1][last_position_x+1] < 3){
								strcpy((char *)p->possible_moves[index++],"ur");
							}
						}
					}
				}

			}
			p->count_pmoves = index;
			
}

void find_possible_moves(volatile Player *p){
	
	color_moves(p,Yellow2);
}

void reset_possible_moves(volatile Player *p){

	color_moves(p,White);
}

int check_move_validity(int new_i, int new_j, int i, int j){

	int converted_new_i = convert_index_bts(new_i);
	int converted_new_j = convert_index_bts(new_j);
	int converted_i = convert_index_bts(i);
	int converted_j = convert_index_bts(j);
	
	int current_x = turn == 1 ? p1.position.x : p2.position.x;
	int current_y = turn == 1 ? p1.position.y : p2.position.y;
	
	//basic checks for moves
	
	if(converted_new_i < 0 || converted_new_j < 0 || converted_new_i > 6 || converted_new_j > 6){
		return 0;
	}
	
	if(abs(converted_new_i - converted_i) > 1){
		return 0;
	}
	else if(abs(converted_new_j - converted_j) > 1){
		return 0;
	}
	else if(converted_new_i == (converted_i-1) && converted_new_j == (converted_j-1)){
		// player on the left
		if(current_x >= 2 && game_matrix[current_y][current_x - 2] == (turn == 1 ? 2 : 1)){
			// pawn at the left border
			if(current_x -3 == 0){
				if(game_matrix[current_y-1][current_x-2] < 3){
					return 1;
				}
				if(game_matrix[current_y+1][current_x-2] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(i >= 3 && game_matrix[current_y][current_x-3] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(game_matrix[current_y-1][current_x-2] < 3){
					return 1;
				}
				if(game_matrix[current_y+1][current_x-2] < 3){
					return 1;
				}
			}
		}
		// player upon
		if(current_y >= 2 && game_matrix[current_y - 2][current_x] == (turn == 1 ? 2 : 1)){
			// pawn at the top border
			if(current_y -3 == 0){
				if(game_matrix[current_y-2][current_x-1] < 3){
					return 1;
				}
				if(game_matrix[current_y-2][current_x+1] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(j >= 3 && game_matrix[current_y-3][current_x] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(game_matrix[current_y-2][current_x-1] < 3){
					return 1;
				}
				if(game_matrix[current_y-2][current_x+1] < 3){
					return 1;
				}
			}
		}
		
		return 0;
	}
	else if(converted_new_i == (converted_i+1) && converted_new_j == (converted_j-1)){
		// player on the right
		if(current_x >= 2 && game_matrix[current_y][current_x + 2] == (turn == 1 ? 2 : 1)){
			// pawn at the right border
			if(current_x + 3 == 14){
				if(current_x <= 12 && game_matrix[current_y-1][current_x+2] < 3){
					return 1;
				}
				if(current_x <= 12 && game_matrix[current_y+1][current_x+2] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(game_matrix[current_y][current_x+3] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(i <= 12 && game_matrix[current_y-1][current_x+2] < 3){
					return 1;
				}
				if(i <= 12 && game_matrix[current_y+1][current_x+2] < 3){
					return 1;
				}
			}
		}
		// player upon
		if(current_y >= 2 && game_matrix[current_y - 2][current_x] == (turn == 1 ? 2 : 1)){
			// pawn at the top border
			if(current_y -3 == 0){
				if(game_matrix[current_y-2][current_x-1] < 3){
					return 1;
				}
				if(game_matrix[current_y-2][current_x+1] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(j >= 3 && game_matrix[current_y-3][current_x] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(game_matrix[current_y-2][current_x-1] < 3){
					return 1;
				}
				if(game_matrix[current_y-2][current_x+1] < 3){
					return 1;
				}
			}
		}
		return 0;
	}
	else if(converted_new_i == (converted_i-1) && converted_new_j == (converted_j+1)){
		// player on the left
		if(current_x >= 2 && game_matrix[current_y][current_x - 2] == (turn == 1 ? 2 : 1)){
			// pawn at the left border
			if(current_x -3 == 0){
				if(game_matrix[current_y-1][current_x-2] < 3){
					return 1;
				}
				if(game_matrix[current_y+1][current_x-2] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(i >= 3 && game_matrix[current_y][current_x-3] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(game_matrix[current_y-1][current_x-2] < 3){
					return 1;
				}
				if(game_matrix[current_y+1][current_x-2] < 3){
					return 1;
				}
			}
		}
		// player above
		if(current_y >= 2 && game_matrix[current_y+2][current_x] == (turn == 1 ? 2 : 1)){
			// pawn at the right border
			if(current_y + 3 == 14){
				if(current_y <= 12 && game_matrix[current_y+2][current_x-1] < 3){
					return 1;
				}
				if(current_y <= 12 && game_matrix[current_y+2][current_x+1] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(game_matrix[current_y+3][current_x] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(j <= 12 && game_matrix[current_y+2][current_x-1] < 3){
					return 1;
				}
				if(j <= 12 && game_matrix[current_y+2][current_x+1] < 3){
					return 1;
				}
			}
		}
		return 0;
	}
	else if(converted_new_i == (converted_i+1) && converted_new_j == (converted_j+1)){
		// player on the right
		if(current_x >= 2 && game_matrix[current_y][current_x + 2] == (turn == 1 ? 2 : 1)){
			// pawn at the right border
			if(current_x + 3 == 14){
				if(current_x <= 12 && game_matrix[current_y-1][current_x+2] < 3){
					return 1;
				}
				if(current_x <= 12 && game_matrix[current_y+1][current_x+2] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(game_matrix[current_y][current_x+3] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(i <= 12 && game_matrix[current_y-1][current_x+2] < 3){
					return 1;
				}
				if(i <= 12 && game_matrix[current_y+1][current_x+2] < 3){
					return 1;
				}
			}
		}
		// player above
		if(current_y >= 2 && game_matrix[current_y+2][current_x] == (turn == 1 ? 2 : 1)){
			// pawn at the right border
			if(current_y + 3 == 14){
				if(current_y <= 12 && game_matrix[current_y+2][current_x-1] < 3){
					return 1;
				}
				if(current_y <= 12 && game_matrix[current_y+2][current_x+1] < 3){
					return 1;
				}
			}
			// no wall behind the pawn
			if(game_matrix[current_y+3][current_x] < 3){
				return 0;
			}
			// wall behind the pawn
			else{
				if(j <= 12 && game_matrix[current_y+2][current_x-1] < 3){
					return 1;
				}
				if(j <= 12 && game_matrix[current_y+2][current_x+1] < 3){
					return 1;
				}
			}
		}
		return 0;
	}
	
		
	// check based on matrix
	if(turn == 1){
		if(game_matrix[new_j][new_i] == 2){
			return 2;
		}
	}
	else if(turn == 2){
		if(game_matrix[new_j][new_i] == 1){
			return 2;
		}
	}
	
	
 return 1;
}

void check_available_path(int i, int j, int* found, int id, int marked[7][7]){

	int converted_i = convert_index_bts(i);
	int converted_j = convert_index_bts(j);
	if(*found) return;
	if(i < 1 || i > 13 || j < 1 || j > 13){
		return;
	}
	if(converted_i < 0 || converted_i > 6 || converted_j < 0 || converted_j > 13){
		return;
	}
	if(marked[converted_i][converted_j] == 1) return;
	marked[converted_i][converted_j] = 1;
	
	// check if there is the opposing player
	if(id == 1){
		if(game_matrix[i][j] == 2){
			return;
		}
	}
	else if(id == 2){
		if(game_matrix[i][j] == 1){
			return;
		}
	}
	if(id == 1 && i == 1){
		*found = 1;
		return;
	}
	if(id == 2 && i == 13){
		*found = 1;
		return;
	}
	if(id == 2 && game_matrix[i+1][j] < 3){
		check_available_path(i+2,j,found,id,marked);
	}
	if(id == 1 && game_matrix[i-1][j] < 3){
		check_available_path(i-2,j,found,id,marked);
	}
	if(game_matrix[i][j-1] < 3) 	check_available_path(i,j-2,found,id,marked);
	if(game_matrix[i][j+1] < 3) check_available_path(i,j+2,found,id,marked);
	if(id == 2 && game_matrix[i-1][j] < 3) check_available_path(i-2,j,found,id,marked);
	if(id == 1 && game_matrix[i+1][j] < 3) 	check_available_path(i+2,j,found,id,marked);
	marked[converted_i][converted_j] = 0;
	return;

}

void move_player(Player *p, char move){

	int id = p->id;
	int current_x = p->current_position.x;
	int current_y = p->current_position.y;
	int converted_x = convert_index_bts(current_x);
	int converted_y = convert_index_bts(current_y);
	
	int last_position_x = p->position.x;
	int last_position_y = p->position.y;
	
	// delete warning 
	
	if(p1.available_walls == 0 || p2.available_walls == 0){
		GUI_Text(10, 285, (unsigned char*) "No walls available, move the", White, White);
		GUI_Text(10, 300, (unsigned char*) "token", White, White);
	}
	
	switch(move){
		case 'u':
			if(check_move_validity(current_x, current_y - 2, last_position_x, last_position_y) == 1){
				if(game_matrix[current_y - 1][current_x] >= 3) break;
				p->current_position.y = current_y - 2;
				moveTo(converted_x, converted_y, converted_x, converted_y - 1, id);
			}
			else if(check_move_validity(current_x, current_y - 2, last_position_x, last_position_y) == 2 ){
				if(game_matrix[current_y - 1][current_x] >= 3 || game_matrix[current_y - 3][current_x] >= 3 || current_y < 5) break;
				p->current_position.y = current_y - 4;
				moveTo(converted_x, converted_y, converted_x, converted_y - 2, id);
			}
			break;
		case 'd':
			if(check_move_validity(current_x, current_y + 2, last_position_x, last_position_y) == 1){
				if(game_matrix[current_y + 1][current_x] >= 3) break;
				p->current_position.y = current_y + 2;
				moveTo(converted_x, converted_y, converted_x, converted_y + 1, id);
			}
			else if(check_move_validity(current_x, current_y + 2, last_position_x, last_position_y) == 2){
				if(game_matrix[current_y + 1][current_x] >= 3 || game_matrix[current_y + 3][current_x] >= 3 || current_y > 9) break;
				p->current_position.y = current_y + 4;
				moveTo(converted_x, converted_y, converted_x, converted_y + 2, id);
			}
			break;
		case 'l':
			if(check_move_validity(current_x - 2, current_y, last_position_x, last_position_y ) == 1){
				if(game_matrix[current_y][current_x - 1] >= 3) break;
				p->current_position.x = current_x - 2;
				moveTo(converted_x, converted_y, converted_x - 1, converted_y, id);
			}
			else if(check_move_validity(current_x - 2, current_y, last_position_x, last_position_y ) == 2){
				if(game_matrix[current_y][current_x-1] >= 3 || game_matrix[current_y][current_x - 3] >= 3 || current_x < 5) break;
				p->current_position.x = current_x - 4;
				moveTo(converted_x, converted_y, converted_x - 2, converted_y, id);
			}
			break;
		case 'r':
			if(check_move_validity(current_x + 2, current_y, last_position_x, last_position_y ) == 1){
				if(game_matrix[current_y][current_x + 1] >= 3) break;
				p->current_position.x = current_x + 2;
				moveTo(converted_x, converted_y, converted_x + 1, converted_y, id);
			}
			else if(check_move_validity(current_x + 2, current_y, last_position_x, last_position_y ) == 2){
				if(game_matrix[current_y][current_x+1] >= 3 || game_matrix[current_y][current_x+3] >= 3 || current_x > 9) break;
				p->current_position.x = current_x + 4;
				moveTo(converted_x, converted_y, converted_x + 2, converted_y, id);
			}
			break;
		default:
			break;
	}
	

}

void confirm_move(volatile Player *p){

	uint32_t move;
	int id = p->id;
	
	if(game_started){
		game_started = 0;
		return;
	}
	
	reset_possible_moves(p);
	game_matrix[p->position.y][p->position.x] = 0;
	p->position.x = p->current_position.x;
	p->position.y = p->current_position.y;
	game_matrix[p->position.y][p->position.x] = id;
	
	move = convert_index_bts(p->position.x);
	move |= convert_index_bts(p->position.y) << 8;
	if(id == 2){
		move |= 1 << 24;
	}
	moves[ind++] = move;
	
	// two boards / human
	if(game_mode.menu1 == 2 && game_mode.menu2 == 1){
		// send message
		memcpy(&CAN_TxMsg.data,&move,4);
		CAN_TxMsg.id = (current_player_id == 1 ? 2 : 1);
		CAN_TxMsg.len = 4;
		CAN_TxMsg.format = STANDARD_FORMAT;
		CAN_TxMsg.type = DATA_FRAME;
		CAN_wrMsg(1,&CAN_TxMsg); // transmit message
	}
	// check if the player wins
	if((turn == 1 && p->position.y == 1) || (turn == 2 && p->position.y == 13)){
		win_screen(turn);
		return;
	}
	switch_turn();


}

void switch_turn(void){
	GUI_Text(105,243,(uint8_t *) "20 s", Black, White);
	timer0_count = 0;
	move_mode = 1;
	if(turn == 1){
		turn = 2;	
		//NPC single board
		if(game_mode.menu1 == 1 && game_mode.menu2 == 2){
			// perform action
			PlayNPCAction();
			if(p2.position.y == 13){
				win_screen(turn);
				return;
			}
			turn = 1;
			get_possible_moves(&p1,(int (*)[15])game_matrix);
			find_possible_moves(&p1);
		}
		// human single board
		else if(game_mode.menu1 == 1 && game_mode.menu2 == 1){
			get_possible_moves(&p2,(int (*)[15])game_matrix);
			find_possible_moves(&p2);
		}
		// human two boards
		else if(game_mode.menu1 == 2 && game_mode.menu2 == 1){
			if(current_player_id == turn){
				get_possible_moves(&p2,(int (*)[15])game_matrix);
				find_possible_moves(&p2);
			}
		}
	}
	else if(turn == 2){
		turn = 1;
		// human single board
		if(game_mode.menu1 == 1 && game_mode.menu2 == 1){
			get_possible_moves(&p1,(int (*)[15])game_matrix);
			find_possible_moves(&p1);
		}
		// human two boards
		else if(game_mode.menu1 == 2 && game_mode.menu2 == 1){
			if(current_player_id == turn){
				get_possible_moves(&p1,(int (*)[15])game_matrix);
				find_possible_moves(&p1);
			}
		}
	}
}

void switch_mode(void){
	if(move_mode == 1){
		if(turn == 1 && p1.available_walls == 0){
			GUI_Text(10, 285, (unsigned char*) "No walls available, move the", White, Red);
			GUI_Text(10, 300, (unsigned char*) "token", White, Red);
			enable_timer(1);
			return;
		}
		else if(turn == 2 && p2.available_walls == 0){
			GUI_Text(10, 285, (unsigned char*) "No walls available, move the", White, Red);
			GUI_Text(10, 300, (unsigned char*) "token", White, Red);
			enable_timer(1);
			return;
		}
		if(turn == 1 && equal_position(p1.current_position,p1.position)){
			move_mode = 0;
			enable_wall_mode();
		}
		else if(turn == 2 && equal_position(p2.current_position,p2.position)){
			move_mode = 0;
			enable_wall_mode();
		}
	}
	else {
		move_mode = 1;
		enable_move_mode();
	}

} 

 // wall mode

void init_wall(void){

	w.position.x = 6;
	w.position.y = 6;
	w.horizontal = 1;
	
}

int check_wall_validity(int new_i, int new_j, volatile Wall *w){
	
	// out of border
	if(w->horizontal == 1){
		if(new_i < 0 || new_i > 5){
			return 0;
		}
		else if(new_j < 1 || new_j > 6){
			return 0;
		}
	}
	else {
		if(new_i < 1 || new_i > 6){
			return 0;
		}
		else if(new_j < 0 || new_j > 5){
			return 0;
		}
	}
	
	
	return 1;

}

void move_wall(char move){

	int current_x = w.position.x;
	int current_y = w.position.y;
	int converted_x = convert_index_bts(w.position.x);
	int converted_y = convert_index_bts(w.position.y);
	
	switch(move){
		case 'u':
			if(check_wall_validity(converted_x, converted_y - 1,&w)){
				w.position.y = current_y - 2;
				moveWall(converted_x, converted_y, converted_x, converted_y - 1);
			}
			break;
		case 'd':
			if(check_wall_validity(converted_x, converted_y + 1,&w)){
				w.position.y = current_y + 2;
				moveWall(converted_x, converted_y, converted_x, converted_y + 1);
			}
			break;
		case 'l':
			if(check_wall_validity(converted_x - 1, converted_y,&w)){
				w.position.x = current_x - 2;
				moveWall(converted_x, converted_y, converted_x - 1, converted_y);
			}
			break;
		case 'r':
			if(check_wall_validity(converted_x + 1, converted_y,&w)){
				w.position.x = current_x + 2;
				moveWall(converted_x, converted_y, converted_x + 1, converted_y);
			}
			break;
		default:
			break;
	}
}

void enable_wall_mode(void){

		turn == 1 ? reset_possible_moves(&p1) : reset_possible_moves(&p2);
		init_wall();
		DrawWallHorizontalThroughPosition(3,3,Brown2);
}

void enable_move_mode(void){

	int converted_x = convert_index_bts(w.position.x);
	int converted_y = convert_index_bts(w.position.y);
	// delete wall and check possible moves if it the wall is not placed
	delete_wall(converted_x, converted_y);
	// only if the position of the player is not changed
	if(turn == 1 && equal_position(p1.current_position, p1.position)){
		find_possible_moves(&p1);
	}
	else if(turn == 2 && equal_position(p2.current_position, p2.position)){
		find_possible_moves(&p2);
	}
}

int equal_position(Position p1, Position p2){

	return p1.x == p2.x && p1.y == p2.y;
}

void rotate_wall(void){
	
	int x = w.position.x;
	int y = w.position.y;
	int converted_x = convert_index_bts(x);
	int converted_y = convert_index_bts(y);
	if(w.horizontal == 1){
		delete_wall(converted_x,converted_y);
		w.horizontal = 0;
		w.position.x += 2;
		w.position.y -= 2;
		DrawWallVerticalThroughPosition(converted_x+1,converted_y-1,Brown2);
	}
	else if(w.horizontal == 0) {
		delete_wall(converted_x,converted_y);
		w.horizontal = 1;
		w.position.x -= 2;
		w.position.y += 2;
		DrawWallHorizontalThroughPosition(converted_x-1,converted_y+1,Brown2);
	}
}

void moveWall(int x, int y, int new_x, int new_y){
	
	if(w.horizontal == 1){
		delete_wall(x,y);
		DrawWallHorizontalThroughPosition(new_x,new_y, Brown2);
	}
	else {
		delete_wall(x,y);
		DrawWallVerticalThroughPosition(new_x,new_y, Brown2);
	}

}

void confirm_wall(void){

	int converted_x = convert_index_bts(w.position.x);
	int converted_y = convert_index_bts(w.position.y);
	int i, d1 = 0, d2 = 0;
	char str[20];
	uint32_t move;
	
	// if position of the wall is valid
	
	if(w.horizontal == 1){
		// avoid overlap
		if((w.position.x <= 13 && game_matrix[w.position.y][w.position.x] >= 3 && game_matrix[w.position.y][w.position.x + 1] >= 3) || (w.position.x <=11 && game_matrix[w.position.y][w.position.x + 3] >= 3)){
			GUI_Text(10, 285, (unsigned char*) "Invalid wall position!", White, Red);
			enable_timer(1);
			return;
		}
		// avoid cross
		if(w.position.x <= 12 && w.position.y >= 1 && w.position.y <= 13 && game_matrix[w.position.y][w.position.x + 2] >= 3 && game_matrix[w.position.y + 1][w.position.x + 2] >= 3 && game_matrix[w.position.y - 1][w.position.x + 2] >= 3 ){
			GUI_Text(10, 285, (unsigned char*) "Invalid wall position!", White, Red);
			enable_timer(1);
			return;
		}
	}
	else {
		// avoid overlap
		if((w.position.y <= 13 && game_matrix[w.position.y][w.position.x] >= 3 && game_matrix[w.position.y + 1][w.position.x] >= 3) || (w.position.y <= 11 && game_matrix[w.position.y + 3][w.position.x] >= 3)){
			GUI_Text(10, 285, (unsigned char*) "Invalid wall position!", White, Red);
			enable_timer(1);
			return;
		}
		// avoid cross
		if(w.position.y <= 12 && w.position.x >= 1 && w.position.x <= 13 && game_matrix[w.position.y + 2][w.position.x] >= 3 && game_matrix[w.position.y + 2][w.position.x + 1] >= 3 && game_matrix[w.position.y + 2][w.position.x - 1] >= 3 ){
			GUI_Text(10, 285, (unsigned char*) "Invalid wall position!", White, Red);
			enable_timer(1);
			return;
		}
	}
	
	//place wall
	if(w.horizontal == 1){
		for(i = w.position.x; i <= w.position.x + 4; i++){
			game_matrix[w.position.y][i] = 3;
		}
	}
	else {
		for(i = w.position.y; i <= w.position.y + 4; i++){
			game_matrix[i][w.position.x] = 3;
		}
	}
	// check
	d1 = distance_to_goal(&p1,(int (*)[15])game_matrix);
	d2 = distance_to_goal(&p2,(int (*)[15])game_matrix);
	if(d1 == INFINITY) {
		GUI_Text(10, 285, (unsigned char*) "Do not trap players with", White, Red);
		GUI_Text(10, 300, (unsigned char*) "walls!", White, Red);
		enable_timer(1);
		//delete wall
		if(w.horizontal == 1){
			for(i = w.position.x; i <= w.position.x + 4; i++){
				game_matrix[w.position.y][i] = 0;
			}
		}
		else {
			for(i = w.position.y; i <= w.position.y + 4; i++){
				game_matrix[i][w.position.x] = 0;
			}
		}
		return;
	};
	
	if(d2 == INFINITY){
		GUI_Text(10, 285, (unsigned char*) "Do not trap players with", White, Red);
		GUI_Text(10, 300, (unsigned char*) "walls!", White, Red);
		enable_timer(1);
		//delete wall
		if(w.horizontal == 1){
			for(i = w.position.x; i <= w.position.x + 4; i++){
				game_matrix[w.position.y][i] = 0;
			}
		}
		else {
			for(i = w.position.y; i <= w.position.y + 4; i++){
				game_matrix[i][w.position.x] = 0;
			}
		}
		return;
	}

	
	if(turn == 1){
		p1.available_walls--;
		sprintf(str, "%d", p1.available_walls);
		GUI_Text(40, 260, (unsigned char*)str, Black, White);
	}
	else{
		p2.available_walls--;
		sprintf(str, "%d", p2.available_walls);
		GUI_Text(190, 260, (unsigned char*)str, Black, White);
	}
	if(w.horizontal == 1){
		DrawWallHorizontalThroughPosition(converted_x,converted_y, Brown);
		for(i = w.position.x; i <= w.position.x + 4; i++){
			game_matrix[w.position.y][i] = 3;
		}
	}
	else {
		DrawWallVerticalThroughPosition(converted_x,converted_y, Brown);
		for(i = w.position.y; i <= w.position.y + 4; i++){
			game_matrix[i][w.position.x] = 3;
		}
	}
	move = convert_index_bts(w.position.x);
	move |= convert_index_bts(w.position.y) << 8;
	move |= w.horizontal << 16;
	move |= 1 << 20;
	if(turn == 2){
		move |= 1 << 24;
	}
	moves[ind++] = move;
	switch_turn();
	if(game_mode.menu1 == 2 && game_mode.menu2 == 1){
		// send message
		memcpy(&CAN_TxMsg.data,&move,4);
		CAN_TxMsg.id = (current_player_id == 1 ? 2 : 1);
		CAN_TxMsg.len = 4;
		CAN_TxMsg.format = STANDARD_FORMAT;
		CAN_TxMsg.type = DATA_FRAME;
		CAN_wrMsg(1,&CAN_TxMsg); // transmit message
	}
	
}

void delete_wall(int x, int y){

	int x1 = convert_index_stb_wall(x);
	int y1 = convert_index_stb_wall(y);
	
	//Check if there are other walls
	if(w.horizontal == 1){
		DrawWallHorizontalThroughPosition(x,y,White);
		// horizontal
		if(game_matrix[y1][x1+1] == 3 && game_matrix[y1][x1+3] == 3){
			DrawWallHorizontalThroughPosition(x,y,Brown);
		}
		if(x1 <= 9 && game_matrix[y1][x1+3] == 3 && game_matrix[y1][x1+5] == 3){
			DrawWallHorizontalThroughPosition(x+1,y,Brown);
		}
		if(x1 >= 1 && game_matrix[y1][x1-1] == 3 && game_matrix[y1][x1+1] == 3){
			DrawWallHorizontalThroughPosition(x-1,y,Brown);
		}
		if(x1 <= 7 && game_matrix[y1][x1+5] == 3 && game_matrix[y1][x1+7] == 3){
			DrawWallHorizontalThroughPosition(x+2,y,Brown);
		}
		if(x1 >= 3 && game_matrix[y1][x1-3] == 3 && game_matrix[y1][x1-1] == 3){
			DrawWallHorizontalThroughPosition(x-2,y,Brown);
		}
		//vertical
		if(y1 <= 11 && game_matrix[y1 + 1][x1] == 3 && game_matrix[y1+3][x1] == 3){
			DrawWallVerticalThroughPosition(x,y,Brown);
		}
		if(y1 <= 11 && game_matrix[y1+1][x1+4] == 3 && game_matrix[y1+3][x1+4] == 3){
			DrawWallVerticalThroughPosition(x+2,y,Brown);
		}
		if(y1 >= 3 && game_matrix[y1-1][x1] == 3 && game_matrix[y1-3][x1] == 3){
			DrawWallVerticalThroughPosition(x,y-2,Brown);
		}
		if(y1 >= 3 && game_matrix[y1-1][x1+4] == 3 && game_matrix[y1-3][x1+4] == 3){
			DrawWallVerticalThroughPosition(x+2,y-2,Brown);
		}
		if(y1 >= 1 && game_matrix[y1-1][x1] == 3 && game_matrix[y1+1][x1] == 3){
			DrawWallVerticalThroughPosition(x,y-1,Brown);
		}
		if(y1 >= 1 && game_matrix[y1-1][x1+4] == 3 && game_matrix[y1+1][x1+4] == 3){
			DrawWallVerticalThroughPosition(x+2,y-1,Brown);
		}
		if(y1 >= 1 && game_matrix[y1-1][x1+2] == 3 && game_matrix[y1+1][x1+2] == 3){
			DrawWallVerticalThroughPosition(x+1,y-1,Brown);
		}
		if(y1 >= 3 && game_matrix[y1-1][x1+2] == 3 && game_matrix[y1-3][x1+2] == 3){
			DrawWallVerticalThroughPosition(x+1,y-2,Brown);
		}
		if(y1 <= 11 && game_matrix[y1+1][x1+2] == 3 && game_matrix[y1+3][x1+2] == 3){
			DrawWallVerticalThroughPosition(x+1,y,Brown);
		}
	}
	else if(w.horizontal == 0){
		DrawWallVerticalThroughPosition(x,y,White);
		// vertical
		if(game_matrix[y1+1][x1] == 3 && game_matrix[y1+3][x1] == 3){
			DrawWallVerticalThroughPosition(x,y,Brown);
		}
		if(y1 <= 9 && game_matrix[y1+3][x1] == 3 && game_matrix[y1+5][x1] == 3){
			DrawWallVerticalThroughPosition(x,y+1,Brown);
		}
		if(y1 >= 1 && game_matrix[y1-1][x1] == 3 && game_matrix[y1+1][x1] == 3){
			DrawWallVerticalThroughPosition(x,y-1,Brown);
		}
		if(y1 <= 7 && game_matrix[y1+5][x1] == 3 && game_matrix[y1+7][x1] == 3){
			DrawWallVerticalThroughPosition(x,y+2,Brown);
		}
		if(y1 >= 3 && game_matrix[y1-3][x1] == 3 && game_matrix[y1-1][x1] == 3){
			DrawWallVerticalThroughPosition(x,y-2,Brown);
		}
		//horizontal
		if(x1 <= 11 && game_matrix[y1][x1+1] == 3 && game_matrix[y1][x1+3] == 3){
			DrawWallHorizontalThroughPosition(x,y,Brown);
		}
		if(x1 <= 11 && game_matrix[y1+4][x1+1] == 3 && game_matrix[y1+4][x1+3] == 3){
			DrawWallHorizontalThroughPosition(x,y+2,Brown);
		}
		if(x1 >= 3 && game_matrix[y1][x1-1] == 3 && game_matrix[y1][x1-3] == 3){
			DrawWallHorizontalThroughPosition(x-2,y,Brown);
		}
		if(x1 >= 3 && game_matrix[y1+4][x1-1] == 3 && game_matrix[y1+4][x1-3] == 3){
			DrawWallHorizontalThroughPosition(x-2,y+2,Brown);
		}
		if(x1 >= 1 && game_matrix[y1][x1-1] == 3 && game_matrix[y1][x1+1] == 3){
			DrawWallHorizontalThroughPosition(x-1,y,Brown);
		}
		if(x1 >= 1 && game_matrix[y1+4][x1-1] == 3 && game_matrix[y1+4][x1+1] == 3){
			DrawWallHorizontalThroughPosition(x-1,y+2,Brown);
		}
		if(x1 >= 1 && game_matrix[y1+2][x1-1] == 3 && game_matrix[y1+2][x1+1] == 3){
			DrawWallHorizontalThroughPosition(x-1,y+1,Brown);
		}
		if(x1 >= 3 && game_matrix[y1+2][x1-1] == 3 && game_matrix[y1+2][x1-3] == 3){
			DrawWallHorizontalThroughPosition(x-2,y+1,Brown);
		}
		if(x1 <= 11 && game_matrix[y1+2][x1+1] == 3 && game_matrix[y1+2][x1+3] == 3){
			DrawWallHorizontalThroughPosition(x,y+1,Brown);
		}
	}
}

void reset_choice(void){
	
	int converted_x_wall = convert_index_bts(w.position.x);
	int converted_y_wall = convert_index_bts(w.position.y);
	int converted_initial_x_p;
	int converted_initial_y_p;
	int converted_current_x_p;
	int converted_current_y_p;
	uint32_t move;
	
	if(move_mode == 1){
		if(turn == 1){
			converted_initial_x_p = convert_index_bts(p1.position.x);
			converted_initial_y_p = convert_index_bts(p1.position.y);
			converted_current_x_p = convert_index_bts(p1.current_position.x);
			converted_current_y_p = convert_index_bts(p1.current_position.y);
			reset_possible_moves(&p1);
			if(!equal_position(p1.current_position,p1.position)){
				moveTo(converted_current_x_p,converted_current_y_p,converted_initial_x_p,converted_initial_y_p,p1.id);
			}
		}
		else {
			converted_initial_x_p = convert_index_bts(p2.position.x);
			converted_initial_y_p = convert_index_bts(p2.position.y);
			converted_current_x_p = convert_index_bts(p2.current_position.x);
			converted_current_y_p = convert_index_bts(p2.current_position.y);
			reset_possible_moves(&p2);
			if(!equal_position(p2.current_position,p2.position)){
				moveTo(converted_current_x_p,converted_current_y_p,converted_initial_x_p,converted_initial_y_p,p2.id);
			}
		}
		p1.current_position.x = p1.position.x;
		p1.current_position.y = p1.position.y;
		p2.current_position.x = p2.position.x;
		p2.current_position.y = p2.position.y;
	}
	else {
		delete_wall(converted_x_wall,converted_y_wall);
	}
	if(turn == 1){
		move = convert_index_bts(p1.position.x);
		move |= convert_index_bts(p1.position.y) << 8;
		move |= 1 << 16;
	}
	else {
		move = convert_index_bts(p2.position.x);
		move |= convert_index_bts(p2.position.y) << 8;
		move |= 1 << 16;
		move |= 1 << 24;
	}
	moves[ind++] = move;
	// two boards / human
	if(game_mode.menu1 == 2 && game_mode.menu2 == 1){
		// send message
		memcpy(&CAN_TxMsg.data,&move,4);
		CAN_TxMsg.id = (current_player_id == 1 ? 2 : 1);
		CAN_TxMsg.len = 4;
		CAN_TxMsg.format = STANDARD_FORMAT;
		CAN_TxMsg.type = DATA_FRAME;
		CAN_wrMsg(1,&CAN_TxMsg); // transmit message
	}
	switch_turn();

}

// win screen

void win_screen(int id){

	disable_timer(0);
	reset_timer(0);
	disable_timer(1);
	reset_timer(1);
	move_mode = -1;
	ColorRectangle(55,87,96,128,0x7c37);
	if(id == 1){
		GUI_Text(70, 110, (unsigned char*) "PLAYER1", White, 0x7c37);
		GUI_Text(135, 110, (unsigned char*) "WINS!", Black, 0x7c37);
	}
	else {
		GUI_Text(70, 110, (unsigned char*) "PLAYER2", Red, 0x7c37);
		GUI_Text(135, 110, (unsigned char*) "WINS!", Black, 0x7c37);
	}
	GUI_Text(65, 140, (unsigned char*) "Press INT0 for", Black, 0x7c37);
	GUI_Text(90, 155, (unsigned char*) "rematch.", Black, 0x7c37);
}

// initial screen

void initial_screen(void){

	LCD_Clear(White);
	GUI_Text(65, 100, (uint8_t *) "QUORIDOR GAME", White, Black);
	GUI_Text(35, 130, (uint8_t *) "Press INT0 to start a", Black, White);
	GUI_Text(90, 150, (uint8_t *) "match", Black, White);

}

// 

void DrawPlayerMove (uint32_t move){
	char str[20];
	int i;
	int x = move & 0xFF;
	int y = (move >> 8) & 0xFF;
	int horizontal = (move >> 16) & 0x0F;
	int wall_mode = (move >> 20) & 0x0F;
	int player_id = (move >> 24) & 0xFF;
	
	int converted_x = convert_index_stb(x);
	int converted_y = convert_index_stb(y);
	
	int p1_x_converted = convert_index_bts(p1.position.x);
	int p1_y_converted = convert_index_bts(p1.position.y);
	
	int p2_x_converted = convert_index_bts(p2.position.x);
	int p2_y_converted = convert_index_bts(p2.position.y);
	
	int not_valid = wall_mode == 0 && horizontal == 1;
	//wall mode
	if(wall_mode == 1){
		if(horizontal == 1){
			DrawWallHorizontalThroughPosition(x,y,Brown);
			for(i = converted_x - 1; i < converted_x + 4; i++){
				game_matrix[converted_y-1][i] = 3;
			}
		}
		else {
			DrawWallVerticalThroughPosition(x,y,Brown);
			for(i = converted_y-1; i < converted_y + 4; i++){
				game_matrix[i][converted_x-1] = 3;
			}
		}
		if(player_id == 0){
			p1.available_walls--;
			sprintf(str, "%d", p1.available_walls);
			GUI_Text(40, 260, (unsigned char*)str, Black, White);
		}
		else {
			p2.available_walls--;
			sprintf(str, "%d", p2.available_walls);
		  GUI_Text(190, 260, (unsigned char*)str, Black, White);
		}
	}
	// move mode
	else {
		// p2 -> receives p1
		if(player_id == 0){
			ColorSquareThroughPosition(p1_x_converted, p1_y_converted, White);
			game_matrix[p1.position.y][p1.position.x] = 0;
			game_matrix[converted_y][converted_x] = 1;
			p1.position.x = converted_x;
			p1.position.y = converted_y;
		}
		// p1 -> receives p2
		else {
			ColorSquareThroughPosition(p2_x_converted, p2_y_converted, White);
			game_matrix[p2.position.y][p2.position.x] = 0;
			game_matrix[converted_y][converted_x] = 2;
			p2.position.x = converted_x;
			p2.position.y = converted_y;
		}
		DrawPawn(x,y,player_id == 0 ? 1 : 2);
	}
	if(game_mode.menu1 == 2 && game_mode.menu2 == 2){
		if((converted_y == 1 || converted_y == 13) && wall_mode == 0 && !not_valid){
			reset_possible_moves(player_id == 0 ? &p2 : &p1);
			win_screen(player_id == 0 ? 1 : 2);
		}
		else {
			PlayNPCAction();
		}
		
	}
	else{
		if((converted_y == 1 || converted_y == 13) && wall_mode == 0 && !not_valid){
			reset_possible_moves(player_id == 0 ? &p2 : &p1);
			win_screen(player_id == 0 ? 1 : 2);
		}
		else {
			switch_turn();
		}
	}
	
}


void PlayNPCAction(){

	ActionPair value;
	uint32_t move;
	int new_x,new_y, old_x, old_y;
	int id;
	char str[5];
	value = perform_action(DEPTH);
	play_action(value.action);
	id = value.action.agent;
	if(value.action.action == MOVE){
		old_x = convert_index_bts(value.action.info.move.old_x);
		old_y = convert_index_bts(value.action.info.move.old_y);
		new_x = convert_index_bts(value.action.info.move.new_x);
		new_y = convert_index_bts(value.action.info.move.new_y);
		moveTo(old_x,old_y,new_x,new_y,value.action.agent);
		move = convert_index_bts(value.action.info.move.new_x);
		move |= convert_index_bts(value.action.info.move.new_y) << 8;
	}
	else if(value.action.action == PLACE) {
		if (value.action.info.wall.horizontal){
		new_x = convert_index_bts(value.action.info.wall.x);
		new_y = convert_index_bts(value.action.info.wall.y);
		DrawWallHorizontalThroughPosition(new_x,new_y, Brown);
		}
		else {
			new_x = convert_index_bts(value.action.info.wall.x);
			new_y = convert_index_bts(value.action.info.wall.y);
			DrawWallVerticalThroughPosition(new_x,new_y, Brown);
		}
		if(id == 1){
			sprintf(str, "%d", p1.available_walls);
			GUI_Text(40, 260, (unsigned char*)str, Black, White);
		}
		else if(id == 2){
			sprintf(str, "%d", p2.available_walls);	
			GUI_Text(190, 260, (unsigned char*)str, Black, White);
		}
		move = convert_index_bts(value.action.info.wall.x);
		move |= convert_index_bts(value.action.info.wall.y) << 8;
		move |= value.action.info.wall.horizontal << 16;
		move |= 1 << 20;
	}
	if(id == 2){
		move |= 1 << 24;
	}
	// two boards / NPC
	if(game_mode.menu1 == 2 && game_mode.menu2 == 2){
		// send message
		memcpy(&CAN_TxMsg.data,&move,4);
		CAN_TxMsg.id = (current_player_id == 1 ? 2 : 1);
		CAN_TxMsg.len = 4;
		CAN_TxMsg.format = STANDARD_FORMAT;
		CAN_TxMsg.type = DATA_FRAME;
		CAN_wrMsg(1,&CAN_TxMsg); // transmit message
		if(p1.position.y == 1 || p2.position.y == 13){
			win_screen(id);
		}
		//Switch turn
		turn = (turn == 1) ? 2 : 1;
	}

}
