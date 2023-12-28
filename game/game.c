#include "game.h"
#include "../GLCD/GLCD.h"

volatile int game_matrix[13][13];

volatile Player p1, p2;

volatile int turn = 1;

void init_game_matrix(void){

	int i, j;
	
	for(i=0;i<13; i++){
		for(j=0;j<13;j++){
			game_matrix[i][j] = 0;
		}
	}

}

void find_possible_moves(Player *p){
	
	int current_x = p->current_position.x;
	int current_y = p->current_position.y;
	int converted_x = convert_index(current_x);
	int converted_y = convert_index(current_y);
	
	// x
	
	if(current_x == 0){
		ColorSquareThroughIndex(converted_x + 1, converted_y, Yellow);
	}
	else if(current_x == 12){
		ColorSquareThroughIndex(converted_x - 1, converted_y, Yellow);
	}
	else {
		ColorSquareThroughIndex(converted_x - 1, converted_y, Yellow);
		ColorSquareThroughIndex(converted_x + 1, converted_y, Yellow);
	}
	
	// y
	
	if(p->id == 1){
		if(current_y == 12){
			ColorSquareThroughIndex(converted_x, converted_y - 1, Yellow);
		}
		else {
			ColorSquareThroughIndex(converted_x, converted_y - 1, Yellow);
			ColorSquareThroughIndex(converted_x, converted_y + 1, Yellow);
		}
	}
	
	if(p->id == 2){
		if(current_y == 0){
			ColorSquareThroughIndex(converted_x, converted_y + 1, Yellow);
		}
		else {
			ColorSquareThroughIndex(converted_x, converted_y - 1, Yellow);
			ColorSquareThroughIndex(converted_x, converted_y + 1, Yellow);
		}
	}


}

void init_players(void){

		// p1
		p1.id = 1;
		p1.available_walls = 8;
		p1.current_position.x = 6;
		p1.current_position.y = 12;
		// p2
		p2.id = 2;
		p2.available_walls = 8;
		p2.current_position.x = 6;
		p2.current_position.y = 0;

}

int convert_index(int i){
	
	return i - i/2;

}

int check_move_validity(int i, int j){

	if(i < 0 || j < 0 || i > 6 || j > 6){
		return 0;
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
	
	switch(move){
		case 'u':
			if(check_move_validity(converted_x, converted_y - 1)){
				p->current_position.y = current_y - 2;
				moveTo(converted_x, converted_y, converted_x, converted_y - 1, id);
				game_matrix[current_x][current_y - 2] = id;
			}
			break;
		case 'd':
			if(check_move_validity(converted_x, converted_y + 1)){
				p->current_position.y = current_y + 2;
				moveTo(converted_x, converted_y, converted_x, converted_y + 1, id);
				game_matrix[current_x][current_y + 2] = id;
			}
			break;
		case 'l':
			if(check_move_validity(converted_x - 1, converted_y )){
				p->current_position.x = current_x - 2;
				moveTo(converted_x, converted_y, converted_x - 1, converted_y, id);
				game_matrix[current_x - 2][current_y] = id;
			}
			break;
		case 'r':
			if(check_move_validity(converted_x + 1, converted_y )){
				p->current_position.x = current_x + 2;
				moveTo(converted_x, converted_y, converted_x + 1, converted_y, id);
				game_matrix[current_x + 2][current_y] = id;
			}
			break;
		default:
			break;
	}

}


