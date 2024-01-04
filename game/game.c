#include "game.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "stdlib.h"

volatile int game_matrix[15][15];
int marked[7][7];

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
	int initial_x = convert_index_bts(last_position_x);
	int initial_y = convert_index_bts(last_position_y);
	int current_position_x = p->current_position.x;
	int current_position_y = p->current_position.y;
	int current_x = convert_index_bts(current_position_x);
	int current_y = convert_index_bts(current_position_y);
	

	// 0xff91 
	if(current_position_x == last_position_x && current_position_y == last_position_y){
			// x
		if(last_position_x == 1 && game_matrix[last_position_y][last_position_x + 1] != 3){
			ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow);
		}
		if(last_position_x == 13 && game_matrix[last_position_y][last_position_x - 1] != 3){
			ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow);
		}
		if(turn == 1){
			if(game_matrix[last_position_y][last_position_x - 1] != 3 && game_matrix[last_position_y][last_position_x - 2] != 2){
				ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow);
			}
			if(game_matrix[last_position_y][last_position_x + 1] != 3 && game_matrix[last_position_y][last_position_x + 2] != 2){
				ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow);
			}
		}
		else {
			if(game_matrix[last_position_y][last_position_x - 1] != 3 && game_matrix[last_position_y][last_position_x - 2] != 1){
				ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow);
			}
			if(game_matrix[last_position_y][last_position_x + 1] != 3 && game_matrix[last_position_y][last_position_x + 2] != 1){
				ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow);
			}
		}


		
		
		// y
		
		if(p->id == 1){
			if(last_position_y == 13){
				if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] != 2){
					ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow);
				}
				else if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] == 2 && game_matrix[last_position_y - 3][last_position_x] != 3 ){
					ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow);
				}
			}
			// avoid overlap the opponent
			else {
				if(game_matrix[last_position_y + 2][last_position_x] != 2){
					if(game_matrix[last_position_y + 1][last_position_x] != 3){
						ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow);
					}
				}
				else {
					if(game_matrix[last_position_y + 3][last_position_x] != 3 && game_matrix[last_position_y + 1][last_position_x] != 3 && initial_y < 5){
						ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow);
					}
				}
				if(last_position_y >= 2){
					if(game_matrix[last_position_y - 2][last_position_x] != 2){
						if( game_matrix[last_position_y - 1][last_position_x] != 3){
							ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow);
						}
					}
					else {
						if(game_matrix[last_position_y - 3][last_position_x] != 3 && game_matrix[last_position_y - 1][last_position_x] != 3 && initial_y >= 2){
							ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow);
						}
					}
				}

			}
		}
		
		if(p->id == 2){
			if(last_position_y == 1){
				if(game_matrix[last_position_y + 1][last_position_x] != 3 && game_matrix[last_position_y + 2][last_position_x] != 1){
					ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow);
				}
				else if(game_matrix[last_position_y + 1][last_position_x] != 3 && game_matrix[last_position_y + 2][last_position_x] == 1 && game_matrix[last_position_y + 3][last_position_x] != 3){
					ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow);
				}
			}
			else {
				// avoid overlap the opponent
				if(game_matrix[last_position_y + 2][last_position_x] != 1){
					if(game_matrix[last_position_y + 1][last_position_x] != 3){
						ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow);
					}
				}
				else {
					if(game_matrix[last_position_y + 3][last_position_x] != 3 && game_matrix[last_position_y + 1][last_position_x] != 3 && initial_y < 5){
						ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow);
					}
				}
				if(last_position_y >= 2){
					if(game_matrix[last_position_y - 2][last_position_x] != 1){
						if(game_matrix[last_position_y - 1][last_position_x] != 3){
							ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow);
						}
					}
					else {
						if(game_matrix[last_position_y - 3][last_position_x] != 3 && game_matrix[last_position_y - 1][last_position_x] != 3 && initial_y >= 2){
							ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow);
						}
					}
				}
			}
		}
		
	}
	else {
		if((current_position_x == last_position_x - 2) || (current_position_x == last_position_x + 2) || (current_position_y == last_position_y - 2) || (current_position_y == last_position_y + 2) || (current_position_y == last_position_y - 4) || (current_position_y == last_position_y + 4)){
			ColorSquareThroughIndex(initial_x,initial_y,Yellow);
		}
	
	}
	


}

void reset_possible_moves(volatile Player *p){

	int last_position_x = p->position.x;
	int last_position_y = p->position.y;
	int initial_x = convert_index_bts(last_position_x);
	int initial_y = convert_index_bts(last_position_y);
	int current_position_x = p->current_position.x;
	int current_position_y = p->current_position.y;
	int current_x = convert_index_bts(current_position_x);
	int current_y = convert_index_bts(current_position_y);
	
	if(current_position_x == last_position_x && current_position_y == last_position_y){
	// x
		if(last_position_x == 1 && game_matrix[last_position_y][last_position_x + 1] != 3){
			ColorSquareThroughIndex(initial_x + 1, initial_y, White);
		}
		if(last_position_x == 13 && game_matrix[last_position_y][last_position_x - 1] != 3){
			ColorSquareThroughIndex(initial_x - 1, initial_y, White);
		}
		if(turn == 1){
			if(game_matrix[last_position_y][last_position_x - 1] != 3 && game_matrix[last_position_y][last_position_x - 2] != 2){
				ColorSquareThroughIndex(initial_x - 1, initial_y, White);
			}
			if(game_matrix[last_position_y][last_position_x + 1] != 3 && game_matrix[last_position_y][last_position_x + 2] != 2){
				ColorSquareThroughIndex(initial_x + 1, initial_y, White);
			}
		}
		else {
			if(game_matrix[last_position_y][last_position_x - 1] != 3 && game_matrix[last_position_y][last_position_x - 2] != 1){
				ColorSquareThroughIndex(initial_x - 1, initial_y, White);
			}
			if(game_matrix[last_position_y][last_position_x + 1] != 3 && game_matrix[last_position_y][last_position_x + 2] != 1){
				ColorSquareThroughIndex(initial_x + 1, initial_y, White);
			}
		}


		
		
		// y
		
		if(p->id == 1){
			if(last_position_y == 13){
				if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] != 2){
					ColorSquareThroughIndex(initial_x, initial_y - 1, White);
				}
				else if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] == 2 && game_matrix[last_position_y - 3][last_position_x] != 3 ){
					ColorSquareThroughIndex(initial_x, initial_y - 2, White);
				}
			}
			// avoid overlap the opponent
			else {
				// TODO no reset if win
				if(game_matrix[last_position_y + 2][last_position_x] != 2){
					if(game_matrix[last_position_y + 1][last_position_x] != 3){
						ColorSquareThroughIndex(initial_x, initial_y + 1, White);
					}
				}
				else {
					if(game_matrix[last_position_y + 3][last_position_x] != 3 && game_matrix[last_position_y + 1][last_position_x] != 3 && initial_y < 5){
						ColorSquareThroughIndex(initial_x, initial_y + 2, White);
					}
				}
				if(last_position_y >= 2){
					if(game_matrix[last_position_y - 2][last_position_x] != 2){
						if( game_matrix[last_position_y - 1][last_position_x] != 3){
							ColorSquareThroughIndex(initial_x, initial_y - 1, White);
						}
					}
					else {
						if(game_matrix[last_position_y - 3][last_position_x] != 3 && game_matrix[last_position_y - 1][last_position_x] != 3 && initial_y >= 2){
							ColorSquareThroughIndex(initial_x, initial_y - 2, White);
						}
					}
				}

			}
		}
		
		if(p->id == 2){
			if(last_position_y == 1){
				if(game_matrix[last_position_y + 1][last_position_x] != 3 && game_matrix[last_position_y + 2][last_position_x] != 1){
					ColorSquareThroughIndex(initial_x, initial_y + 1, White);
				}
				else if(game_matrix[last_position_y + 1][last_position_x] != 3 && game_matrix[last_position_y + 2][last_position_x] == 1 && game_matrix[last_position_y + 3][last_position_x] != 3){
					ColorSquareThroughIndex(initial_x, initial_y + 2, White);
				}
			}
			else {
				// avoid overlap the opponent
				if(game_matrix[last_position_y + 2][last_position_x] != 1){
					if(game_matrix[last_position_y + 1][last_position_x] != 3){
						ColorSquareThroughIndex(initial_x, initial_y + 1, White);
					}
				}
				else {
					if(game_matrix[last_position_y + 3][last_position_x] != 3 && game_matrix[last_position_y + 1][last_position_x] != 3 && initial_y < 5){
						ColorSquareThroughIndex(initial_x, initial_y + 2, White);
					}
				}
				if(last_position_y >= 2){
					if(game_matrix[last_position_y - 2][last_position_x] != 1){
						if(game_matrix[last_position_y - 1][last_position_x] != 3){
							ColorSquareThroughIndex(initial_x, initial_y - 1, White);
						}
					}
					else {
						if(game_matrix[last_position_y - 3][last_position_x] != 3 && game_matrix[last_position_y - 1][last_position_x] != 3 && initial_y >= 2){
							ColorSquareThroughIndex(initial_x, initial_y - 2, White);
						}
					}
				}
			}
		}
	}
	else {
		if((current_position_x == last_position_x - 2) || (current_position_x == last_position_x + 2) || (current_position_y == last_position_y - 2) || (current_position_y == last_position_y + 2) || (current_position_y == last_position_y - 4) || (current_position_y == last_position_y + 4)){
			ColorSquareThroughIndex(initial_x,initial_y,Yellow);
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

int convert_index_bts(int i){
	
	if(i%2 == 0){
		return i/2;
	}
	return i - i/2 - 1;

}

int convert_index_stb(int i){
	
	return 2*i;
}

int check_move_validity(int new_i, int new_j, int i, int j){

	int converted_new_i = convert_index_bts(new_i);
	int converted_new_j = convert_index_bts(new_j);
	int converted_i = convert_index_bts(i);
	int converted_j = convert_index_bts(j);
	
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
	if(marked[converted_i][converted_j] == 1) return;
	marked[converted_i][converted_j] = 1;
	// check if there is the other player
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
	if(id == 2 && game_matrix[i+1][j] != 3){
		check_available_path(i+2,j,found,id,marked);
	}
	if(id == 1 && game_matrix[i-1][j] != 3){
		check_available_path(i-2,j,found,id,marked);
	}
	if(game_matrix[i][j-1] != 3) 	check_available_path(i,j-2,found,id,marked);
	if(game_matrix[i][j+1] != 3) check_available_path(i,j+2,found,id,marked);
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
		// ColorRectangle(0,284,36,240,White);
		GUI_Text(10, 285, (unsigned char*) "No walls available, move the", White, White);
		GUI_Text(10, 300, (unsigned char*) "token", White, White);
	}
	
	switch(move){
		case 'u':
			if(check_move_validity(current_x, current_y - 2, last_position_x, last_position_y) == 1){
				if(game_matrix[current_y - 1][current_x] == 3) break;
				reset_possible_moves(p);
				p->current_position.y = current_y - 2;
				moveTo(converted_x, converted_y, converted_x, converted_y - 1, id);
			}
			else if(check_move_validity(current_x, current_y - 2, last_position_x, last_position_y) == 2 ){
				if(game_matrix[current_y - 1][current_x] == 3 || game_matrix[current_y - 3][current_x] == 3 || current_y < 5) break;
				reset_possible_moves(p);
				p->current_position.y = current_y - 4;
				moveTo(converted_x, converted_y, converted_x, converted_y - 2, id);
			}
			break;
		case 'd':
			if(check_move_validity(current_x, current_y + 2, last_position_x, last_position_y) == 1){
				if(game_matrix[current_y + 1][current_x] == 3) break;
				reset_possible_moves(p);
				p->current_position.y = current_y + 2;
				moveTo(converted_x, converted_y, converted_x, converted_y + 1, id);
			}
			else if(check_move_validity(current_x, current_y + 2, last_position_x, last_position_y) == 2){
				if(game_matrix[current_y + 1][current_x] == 3 || game_matrix[current_y + 3][current_x] == 3 || current_y > 9) break;
				reset_possible_moves(p);
				p->current_position.y = current_y + 4;
				moveTo(converted_x, converted_y, converted_x, converted_y + 2, id);
			}
			break;
		case 'l':
			if(check_move_validity(current_x - 2, current_y, last_position_x, last_position_y ) == 1){
				if(game_matrix[current_y][current_x - 1] == 3) break;
				reset_possible_moves(p);
				p->current_position.x = current_x - 2;
				moveTo(converted_x, converted_y, converted_x - 1, converted_y, id);
			}
			break;
		case 'r':
			if(check_move_validity(current_x + 2, current_y, last_position_x, last_position_y ) == 1){
				if(game_matrix[current_y][current_x + 1] == 3) break;
				reset_possible_moves(p);
				p->current_position.x = current_x + 2;
				moveTo(converted_x, converted_y, converted_x + 1, converted_y, id);
			}
			break;
		default:
			break;
	}
	
	find_possible_moves(p);

}

void confirm_move(volatile Player *p){

	int id = p->id;
	game_matrix[p->position.y][p->position.x] = 0;
	p->position.x = p->current_position.x;
	p->position.y = p->current_position.y;
	game_matrix[p->position.y][p->position.x] = id;
	// check if the player wins
	if((turn == 1 && p->position.y == 1) || (turn == 2 && p->position.y == 13)){
		win_screen();
		return;
	}
	reset_possible_moves(p);
	switch_turn();

}

void switch_turn(void){
	GUI_Text(105,243,(uint8_t *) "20 s", Black, White);
	timer0_count = 0;
	move_mode = 1;
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
	if(move_mode == 1){
		if(turn == 1 && p1.available_walls == 0){
			GUI_Text(10, 285, (unsigned char*) "No walls available, move the", White, Red);
			GUI_Text(10, 300, (unsigned char*) "token", White, Red);
			return;
		}
		else if(turn == 2 && p2.available_walls == 0){
			GUI_Text(10, 285, (unsigned char*) "No walls available, move the", White, Red);
			GUI_Text(10, 300, (unsigned char*) "token", White, Red);
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

int check_wall_validity(int new_i, int new_j, int i, int j){
	
	// TODO remove i and j from parameters
	// out of border
	if(w.horizontal == 1){
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

	int converted_x = convert_index_bts(w.position.x);
	int converted_y = convert_index_bts(w.position.y);
	// delete rectangle and check possible moves if it the wall is not placed
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
	if(w.horizontal == 1 && x != 0){
		delete_wall(converted_x,converted_y);
		w.horizontal = 0;
		DrawWallVerticalThroughIndex(converted_x,converted_y,Red);
	}
	else if(w.horizontal == 0 && y != 0 ) {
		delete_wall(converted_x,converted_y);
		w.horizontal = 1;
		DrawWallHorizontalThroughIndex(converted_x,converted_y,Red);
	}
}

void moveWall(int x, int y, int new_x, int new_y){
	
	if(w.horizontal == 1){
		delete_wall(x,y);
		DrawWallHorizontalThroughIndex(new_x,new_y, Red);
	}
	else {
		delete_wall(x,y);
		DrawWallVerticalThroughIndex(new_x,new_y, Red);
	}

}

void confirm_wall(void){

	int converted_x = convert_index_bts(w.position.x);
	int converted_y = convert_index_bts(w.position.y);
	int i,j, found1 = 0, found2 = 0;
	char str[20];
	
	// if position of the wall is valid
	
	if(w.horizontal == 1){
		// avoid overlap
		if((game_matrix[w.position.y][w.position.x] == 3 && game_matrix[w.position.y][w.position.x + 1] == 3) || game_matrix[w.position.y][w.position.x + 3] == 3){
			return;
		}
		// avoid cross
		if(game_matrix[w.position.y][w.position.x + 2] == 3 && game_matrix[w.position.y + 1][w.position.x + 2] == 3 && game_matrix[w.position.y - 1][w.position.x + 2] == 3 ){
			return;
		}
	}
	else {
		// avoid overlap
		if((game_matrix[w.position.y][w.position.x] == 3 && game_matrix[w.position.y + 1][w.position.x] == 3) || game_matrix[w.position.y + 3][w.position.x] == 3){
			return;
		}
		// avoid cross
		if(game_matrix[w.position.y + 2][w.position.x] == 3 && game_matrix[w.position.y + 2][w.position.x + 1] == 3 && game_matrix[w.position.y + 2][w.position.x - 1] == 3 ){
			return;
		}
	}
	
	// avoid trap players
	
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			marked[i][j] = 0;
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
	check_available_path(p2.position.y, p2.position.x, &found1,p2.id,marked);
	
	if(found1 == 0) {
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
	
	for(i=0;i<7;i++){
		for(j=0;j<7;j++){
			marked[i][j] = 0;
		}
	}
	check_available_path(p1.position.y, p1.position.x, &found2,p1.id,marked);
	
	if(found2 == 0){
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
		DrawWallHorizontalThroughIndex(converted_x,converted_y, Blue2);
		for(i = w.position.x; i <= w.position.x + 4; i++){
			game_matrix[w.position.y][i] = 3;
		}
	}
	else {
		DrawWallVerticalThroughIndex(converted_x,converted_y, Blue2);
		for(i = w.position.y; i <= w.position.y + 4; i++){
			game_matrix[i][w.position.x] = 3;
		}
	}
	switch_turn();
	
}

void delete_wall(int x, int y){

	int converted_x = convert_index_stb(x);
	int converted_y = convert_index_stb(y);

	if(w.horizontal == 1){
		DrawWallHorizontalThroughIndex(x,y,White);
		//Check if there are other walls
		if(game_matrix[converted_y][converted_x] == 3 && game_matrix[converted_y][converted_x + 3] == 3){
			DrawWallHorizontalThroughIndex(x,y,Blue2);
		}
		else if(game_matrix[converted_y - 2][converted_x] == 3 && game_matrix[converted_y + 1][converted_x] == 3){
			DrawWallVerticalThroughIndex(x,y - 1,Blue2);
		}
		if(game_matrix[converted_y - 2][converted_x + 2] == 3 && game_matrix[converted_y + 1][converted_x + 2] == 3){
			DrawWallVerticalThroughIndex(x + 1,y - 1,Blue2);
		}
		if(game_matrix[converted_y - 2][converted_x + 4] == 3 && game_matrix[converted_y + 1][converted_x + 4] == 3){
			DrawWallVerticalThroughIndex(x + 2,y - 1,Blue2);
		}
		if(game_matrix[converted_y][converted_x + 2] == 3 && game_matrix[converted_y][converted_x + 5] == 3){
			DrawWallHorizontalThroughIndex(x + 1,y,Blue2);
		}
		if(game_matrix[converted_y][converted_x + 4] == 3 && game_matrix[converted_y][converted_x + 7] == 3){
			DrawWallHorizontalThroughIndex(x + 2,y,Blue2);
		}
		if(game_matrix[converted_y][converted_x - 2] == 3 && game_matrix[converted_y][converted_x + 2] == 3){
			DrawWallHorizontalThroughIndex(x - 1,y,Blue2);
		}
		
	}
	else if(w.horizontal == 0){
		DrawWallVerticalThroughIndex(x,y,White);
		if(game_matrix[converted_y][converted_x] == 3 && game_matrix[converted_y + 3][converted_x] == 3){
			DrawWallVerticalThroughIndex(x,y,Blue2);
		}
		else if(game_matrix[converted_y][converted_x - 2] == 3 && game_matrix[converted_y][converted_x + 1] == 3){
			DrawWallHorizontalThroughIndex(x - 1,y,Blue2);
		}
		if(game_matrix[converted_y + 2][converted_x - 2] == 3 && game_matrix[converted_y + 2][converted_x + 1] == 3){
			DrawWallHorizontalThroughIndex(x - 1,y + 1,Blue2);
		}
		if(game_matrix[converted_y + 4][converted_x - 2] == 3 && game_matrix[converted_y + 4][converted_x + 1] == 3){
			DrawWallHorizontalThroughIndex(x - 1,y + 2,Blue2);
		}
		if(game_matrix[converted_y + 2][converted_x] == 3 && game_matrix[converted_y + 5][converted_x] == 3){
			DrawWallVerticalThroughIndex(x,y + 1,Blue2);
		}
		if(game_matrix[converted_y + 4][converted_x] == 3 && game_matrix[converted_y + 7][converted_x] == 3){
			DrawWallVerticalThroughIndex(x,y + 2,Blue2);
		}
		if(converted_y >= 2 && converted_y <= 12 && game_matrix[converted_y - 2][converted_x] == 3 && game_matrix[converted_y + 2][converted_x] == 3){
			DrawWallVerticalThroughIndex(x,y - 1,Blue2);
		}

	}
}

void confirm_choice(void){
	
	int converted_x = convert_index_bts(w.position.x);
	int converted_y = convert_index_bts(w.position.y);
	
	if(move_mode == 1){
		turn == 1 ? confirm_move(&p1) : confirm_move(&p2);
	}
	else {
		delete_wall(converted_x,converted_y);
		switch_turn();
	}

}

// win screen

void win_screen(void){

	disable_timer(0);
	move_mode = -1;
	ColorRectangle(55,87,96,128,0x4d7f);
	if(turn == 1){
		GUI_Text(70, 110, (unsigned char*) "PLAYER1", White, 0x4d7f);
		GUI_Text(135, 110, (unsigned char*) "WINS!", Black, 0x4d7f);
	}
	else {
		GUI_Text(70, 110, (unsigned char*) "PLAYER2", Red, 0x4d7f);
		GUI_Text(135, 110, (unsigned char*) "WINS!", Black, 0x4d7f);
	}
	GUI_Text(65, 140, (unsigned char*) "Press INT0 for", Black, 0x4d7f);
	GUI_Text(90, 155, (unsigned char*) "rematch.", Black, 0x4d7f);
}

