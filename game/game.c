#include "game.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "stdlib.h"

volatile int game_matrix[15][15];

volatile Player p1, p2;

volatile Wall w;

volatile int turn = 1;

volatile int move_mode = 1;

extern int timer0_count;

void init_game_matrix(void){

	int i, j;
	
	for(i=0;i<15; i++){
		for(j=0;j<15;j++){
			game_matrix[i][j] = 0;
		}
	}

}

// move mode

void find_possible_moves(volatile Player *p){
	
	int last_position_x = p->position.x;
	int last_position_y = p->position.y;
	int initial_x = convert_index(p->position.x);
	int initial_y = convert_index(p->position.y);
	
	// x
	
	if(last_position_x == 1){
		ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow);
	}
	else if(last_position_x == 13){
		ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow);
	}
	else {
		ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow);
		ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow);
	}
	
	// y
	
	if(p->id == 1){
		if(last_position_y == 13){
			ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow);
		}
		// avoid overlap the opponent
		else {
			if(game_matrix[last_position_x][last_position_y + 2] != 2){
				ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow);
			}
			if(game_matrix[last_position_x][last_position_y - 2] != 2){
				ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow);
			}
		}
	}
	
	if(p->id == 2){
		if(last_position_y == 1){
			ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow);
		}
		else {
			// avoid overlap the opponent
			if(game_matrix[last_position_x][last_position_y + 2] != 1){
				ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow);
			}
			if(game_matrix[last_position_x][last_position_y - 2] != 1){
				ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow);
			}
		}
	}

}

void reset_possible_moves(volatile Player *p){

	int last_position_x = p->position.x;
	int last_position_y = p->position.y;
	int initial_x = convert_index(last_position_x);
	int initial_y = convert_index(last_position_y);
	
	// x
	
	if(last_position_x == 1){
		ColorSquareThroughIndex(initial_x + 1, initial_y, White);
	}
	else if(last_position_x == 13){
		ColorSquareThroughIndex(initial_x - 1, initial_y, White);
	}
	else {
		ColorSquareThroughIndex(initial_x - 1, initial_y, White);
		ColorSquareThroughIndex(initial_x + 1, initial_y, White);
	}
	
	// y
	
	if(p->id == 1){
		if(last_position_y == 13){
			ColorSquareThroughIndex(initial_x, initial_y - 1, White);
		}
		else {
			// avoid overlap the opponent
			if(game_matrix[last_position_x][last_position_y + 2] != 2){
				ColorSquareThroughIndex(initial_x, initial_y + 1, White);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y + 2, White);
			}
			if(game_matrix[last_position_x][last_position_y - 2] != 2){
				ColorSquareThroughIndex(initial_x, initial_y - 1, White);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y - 2, White);
			}
		}
	}
	
	if(p->id == 2){
		if(last_position_y == 1){
			ColorSquareThroughIndex(initial_x, initial_y + 1, White);
		}
		else {
			// avoid overlap the opponent
			if(game_matrix[last_position_x][last_position_y + 2] != 1){
				ColorSquareThroughIndex(initial_x, initial_y + 1, White);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y + 2, White);
			}
			if(game_matrix[last_position_x][last_position_y - 2] != 1){
				ColorSquareThroughIndex(initial_x, initial_y - 1, White);
			}
			else {
				ColorSquareThroughIndex(initial_x, initial_y - 2, White);
			}
		}
	}

}

void init_players(void){

		// p1
		p1.id = 1;
		p1.available_walls = 8;
		p1.current_position.x = 7;
		p1.current_position.y = 13;
		p1.position.x = 7;
		p1.position.y = 13;
		// p2
		p2.id = 2;
		p2.available_walls = 8;
		p2.current_position.x = 7;
		p2.current_position.y = 1;
		p2.position.x = 7;
		p2.position.y = 1;

}

int convert_index(int i){
	
	return i - i/2 - 1;

}

int check_move_validity(int new_i, int new_j, int i, int j){

	int converted_new_i = convert_index(new_i);
	int converted_new_j = convert_index(new_j);
	int converted_i = convert_index(i);
	int converted_j = convert_index(j);
	
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
		return 0;
	}
	else if(converted_new_i == (converted_i+1) && converted_new_j == (converted_j-1)){
		return 0;
	}
	else if(converted_new_i == (converted_i-1) && converted_new_j == (converted_j+1)){
		return 0;
	}
	else if(converted_new_i == (converted_i+1) && converted_new_j == (converted_j+1)){
		return 0;
	}
	
	// check based on matrix
	if(turn == 1){
		if(game_matrix[new_i][new_j] == 2){
			return 2;
		}
	}
	else if(turn == 2){
		if(game_matrix[new_i][new_j]){
			return 2;
		}
	}
	
	// recursive function for walls
	

 return 1;
}

void move_player(Player *p, char move){

	int id = p->id;
	int current_x = p->current_position.x;
	int current_y = p->current_position.y;
	int converted_x = convert_index(current_x);
	int converted_y = convert_index(current_y);
	
	int last_position_x = p->position.x;
	int last_position_y = p->position.y;
	
	
	switch(move){
		case 'u':
			if(check_move_validity(current_x, current_y - 2, last_position_x, last_position_y) == 1){
				reset_possible_moves(p);
				p->current_position.y = current_y - 2;
				moveTo(converted_x, converted_y, converted_x, converted_y - 1, id);
			}
			else if(check_move_validity(current_x, current_y - 2, last_position_x, last_position_y) == 2 ){
				reset_possible_moves(p);
				p->current_position.y = current_y - 4;
				moveTo(converted_x, converted_y, converted_x, converted_y - 2, id);
			}
			break;
		case 'd':
			if(check_move_validity(current_x, current_y + 2, last_position_x, last_position_y) == 1){
				reset_possible_moves(p);
				p->current_position.y = current_y + 2;
				moveTo(converted_x, converted_y, converted_x, converted_y + 1, id);
			}
			else if(check_move_validity(current_x, current_y + 2, last_position_x, last_position_y) == 2){
				reset_possible_moves(p);
				p->current_position.y = current_y + 4;
				moveTo(converted_x, converted_y, converted_x, converted_y + 2, id);
			}
			break;
		case 'l':
			if(check_move_validity(current_x - 2, current_y, last_position_x, last_position_y ) == 1){
				reset_possible_moves(p);
				p->current_position.x = current_x - 2;
				moveTo(converted_x, converted_y, converted_x - 1, converted_y, id);
			}
			else if(check_move_validity(current_x - 2, current_y, last_position_x, last_position_y ) == 2){
				reset_possible_moves(p);
				p->current_position.x = current_x - 4;
				moveTo(converted_x, converted_y, converted_x - 2, converted_y, id);
			}
			break;
		case 'r':
			if(check_move_validity(current_x + 2, current_y, last_position_x, last_position_y ) == 1){
				reset_possible_moves(p);
				p->current_position.x = current_x + 2;
				moveTo(converted_x, converted_y, converted_x + 1, converted_y, id);
			}
			else if(check_move_validity(current_x + 2, current_y, last_position_x, last_position_y ) == 2){
				reset_possible_moves(p);
				p->current_position.x = current_x + 4;
				moveTo(converted_x, converted_y, converted_x + 2, converted_y, id);
			}
			break;
		default:
			break;
	}

}

void confirm_move(Player *p){

	int id = p->id;
	game_matrix[p->position.x][p->position.y] = 0;
	p->position.x = p->current_position.x;
	p->position.y = p->current_position.y;
	game_matrix[p->position.x][p->position.y] = id;
	switch_turn();

}

void switch_turn(void){
	GUI_Text(105,243,(uint8_t *) "20 s", Black, White);
	timer0_count = 0;
	if(turn == 1){
		turn = 2;
		find_possible_moves(&p2);
	}
	else if(turn == 2){
		turn = 1;
		find_possible_moves(&p1);
	}
}

void switch_mode(void){
	if(move_mode){
		move_mode = 0;
		enable_wall_mode();
	}
	else {
		move_mode = 1;
		enable_move_mode();
	}

} 

 // wall mode

int check_wall_validity(int new_i, int new_j, int i, int j){


	if(w.horizontal){
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
	int converted_x = convert_index(w.position.x) + 1;
	int converted_y = convert_index(w.position.y) + 1;
	
	switch(move){
		case 'u':
			if(check_wall_validity(converted_x, converted_y - 1, converted_x, converted_y)){
				w.position.y = current_y - 2;
				moveWall(converted_x, converted_y, converted_x, converted_y - 1);
			}
			break;
		case 'd':
			if(check_wall_validity(converted_x, converted_y + 1, converted_x, converted_y)){
				w.position.y = current_y + 2;
				moveWall(converted_x, converted_y, converted_x, converted_y + 1);
			}
			break;
		case 'l':
			if(check_wall_validity(converted_x - 1, converted_y, converted_x, converted_y)){
				w.position.x = current_x - 2;
				moveWall(converted_x, converted_y, converted_x - 1, converted_y);
			}
			break;
		case 'r':
			if(check_wall_validity(converted_x + 1, converted_y, converted_x, converted_y)){
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
		DrawWallHorizontalThroughIndex(3,3, Red);
}

void enable_move_mode(void){

	// delete rectangle
	turn == 1 ? find_possible_moves(&p1) : find_possible_moves(&p2);
}

void init_wall(void){

	w.position.x = 6;
	w.position.y = 6;
	w.horizontal = 1;
	
}

void rotate_wall(void){
	
	int converted_x = convert_index(w.position.x) + 1;
	int converted_y = convert_index(w.position.y) + 1;
	if(w.horizontal){
		w.horizontal = 0;
		DrawWallHorizontalThroughIndex(converted_x,converted_y,White);
		DrawWallVerticalThroughIndex(converted_x,converted_y,Red);
	}
	else {
		w.horizontal = 1;
		DrawWallVerticalThroughIndex(converted_x,converted_y,White);
		DrawWallHorizontalThroughIndex(converted_x,converted_y,Red);
	}
}

void moveWall(int x, int y, int new_x, int new_y){
	
	if(w.horizontal){
		DrawWallHorizontalThroughIndex(x,y,White);
		DrawWallHorizontalThroughIndex(new_x,new_y, Red);
	}
	else {
		DrawWallVerticalThroughIndex(x,y,White);
		DrawWallVerticalThroughIndex(new_x,new_y, Red);
	}


}


