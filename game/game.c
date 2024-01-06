#include "game.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"
#include "stdlib.h"

volatile int game_matrix[15][15];

int marked[7][7];

uint32_t moves[100];
int ind = 0;

volatile Player p1, p2;

volatile Wall w;

volatile int turn = 1;

volatile int move_mode = -1;

extern int timer0_count;



int convert_index_bts(int i){
	
	if(i%2 == 0){
		return i/2;
	}
	return i - i/2 - 1;

}

int convert_index_stb(int i){
	
	return 2*i;
}

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

void game_setup(void){

	LCD_Clear(White);
	init_game_matrix();
	init_players();
	init_wall();
	enable_timer(0);
	enable_timer(1);
	move_mode = 1;
	turn = 1;
	display_grid();
	turn == 1 ? find_possible_moves(&p1) : find_possible_moves(&p2);

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
	

	if(current_position_x == last_position_x && current_position_y == last_position_y){
			// x
		if(last_position_x == 1 && game_matrix[last_position_y][last_position_x + 1] != 3){
			ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow2);
		}
		if(last_position_x == 13 && game_matrix[last_position_y][last_position_x - 1] != 3){
			ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow2);
		}
		if(turn == 1){
			if(game_matrix[last_position_y][last_position_x - 1] != 3 && game_matrix[last_position_y][last_position_x - 2] != 2){
				ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow2);
			}
			if(game_matrix[last_position_y][last_position_x + 1] != 3 && game_matrix[last_position_y][last_position_x + 2] != 2){
				ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow2);
			}
		}
		else {
			if(game_matrix[last_position_y][last_position_x - 1] != 3 && game_matrix[last_position_y][last_position_x - 2] != 1){
				ColorSquareThroughIndex(initial_x - 1, initial_y, Yellow2);
			}
			if(game_matrix[last_position_y][last_position_x + 1] != 3 && game_matrix[last_position_y][last_position_x + 2] != 1){
				ColorSquareThroughIndex(initial_x + 1, initial_y, Yellow2);
			}
		}
		
		// y
		
		if(p->id == 1){
			if(last_position_y == 13){
				if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] != 2){
					ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow2);
				}
				else if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] == 2 && game_matrix[last_position_y - 3][last_position_x] != 3 ){
					ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow2);
				}
			}
			// avoid overlap the opponent
			else {
				if(game_matrix[last_position_y + 2][last_position_x] != 2){
					if(game_matrix[last_position_y + 1][last_position_x] != 3){
						ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow2);
					}
				}
				else {
					if(game_matrix[last_position_y + 3][last_position_x] != 3 && game_matrix[last_position_y + 1][last_position_x] != 3 && initial_y < 5){
						ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow2);
					}
				}
				if(last_position_y >= 2){
					if(game_matrix[last_position_y - 2][last_position_x] != 2){
						if( game_matrix[last_position_y - 1][last_position_x] != 3){
							ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow2);
						}
					}
					else {
						if(game_matrix[last_position_y - 3][last_position_x] != 3 && game_matrix[last_position_y - 1][last_position_x] != 3 && initial_y >= 2){
							ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow2);
						}
					}
				}

			}
		}
		
		if(p->id == 2){
			if(last_position_y == 1){
				if(game_matrix[last_position_y + 1][last_position_x] != 3 && game_matrix[last_position_y + 2][last_position_x] != 1){
					ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow2);
				}
				else if(game_matrix[last_position_y + 1][last_position_x] != 3 && game_matrix[last_position_y + 2][last_position_x] == 1 && game_matrix[last_position_y + 3][last_position_x] != 3){
					ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow2);
				}
			}
			else {
				// avoid overlap the opponent
				if(game_matrix[last_position_y + 2][last_position_x] != 1){
					if(game_matrix[last_position_y + 1][last_position_x] != 3){
						ColorSquareThroughIndex(initial_x, initial_y + 1, Yellow2);
					}
				}
				else {
					if(game_matrix[last_position_y + 3][last_position_x] != 3 && game_matrix[last_position_y + 1][last_position_x] != 3 && initial_y < 5){
						ColorSquareThroughIndex(initial_x, initial_y + 2, Yellow2);
					}
				}
				if(last_position_y >= 2){
					if(game_matrix[last_position_y - 2][last_position_x] != 1){
						if(game_matrix[last_position_y - 1][last_position_x] != 3){
							ColorSquareThroughIndex(initial_x, initial_y - 1, Yellow2);
						}
					}
					else {
						if(game_matrix[last_position_y - 3][last_position_x] != 3 && game_matrix[last_position_y - 1][last_position_x] != 3 && initial_y >= 2){
							ColorSquareThroughIndex(initial_x, initial_y - 2, Yellow2);
						}
					}
				}
			}
		}
		
	}
	else {
		if((current_position_x == last_position_x - 2) || (current_position_x == last_position_x + 2) || (current_position_y == last_position_y - 2) || (current_position_y == last_position_y + 2) || (current_position_y == last_position_y - 4) || (current_position_y == last_position_y + 4)){
			ColorSquareThroughIndex(initial_x,initial_y,Yellow2);
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
			ColorSquareThroughIndex(initial_x,initial_y,White);
		}
	}

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
	if(converted_i < 0 || converted_i > 6 || converted_j < 0 || converted_j > 13){
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

	uint32_t move;
	int id = p->id;
	game_matrix[p->position.y][p->position.x] = 0;
	p->position.x = p->current_position.x;
	p->position.y = p->current_position.y;
	game_matrix[p->position.y][p->position.x] = id;
	
	move = p->position.x;
	move |= p->position.y << 8;
	if(id == 2){
		move |= 1 << 24;
	}
	moves[ind++] = move;
	reset_possible_moves(p);
	// check if the player wins
	if((turn == 1 && p->position.y == 1) || (turn == 2 && p->position.y == 13)){
		win_screen();
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

int check_wall_validity(int new_i, int new_j){
	
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
			if(check_wall_validity(converted_x, converted_y - 1)){
				w.position.y = current_y - 2;
				moveWall(converted_x, converted_y, converted_x, converted_y - 1);
			}
			break;
		case 'd':
			if(check_wall_validity(converted_x, converted_y + 1)){
				w.position.y = current_y + 2;
				moveWall(converted_x, converted_y, converted_x, converted_y + 1);
			}
			break;
		case 'l':
			if(check_wall_validity(converted_x - 1, converted_y)){
				w.position.x = current_x - 2;
				moveWall(converted_x, converted_y, converted_x - 1, converted_y);
			}
			break;
		case 'r':
			if(check_wall_validity(converted_x + 1, converted_y)){
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
		DrawWallHorizontalThroughIndex(3,3, Brown2);
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
	if(w.horizontal == 1 && x != 0){
		delete_wall(converted_x,converted_y);
		w.horizontal = 0;
		DrawWallVerticalThroughIndex(converted_x,converted_y,Brown2);
	}
	else if(w.horizontal == 0 && y != 0 ) {
		delete_wall(converted_x,converted_y);
		w.horizontal = 1;
		DrawWallHorizontalThroughIndex(converted_x,converted_y,Brown2);
	}
}

void moveWall(int x, int y, int new_x, int new_y){
	
	if(w.horizontal == 1){
		delete_wall(x,y);
		DrawWallHorizontalThroughIndex(new_x,new_y, Brown2);
	}
	else {
		delete_wall(x,y);
		DrawWallVerticalThroughIndex(new_x,new_y, Brown2);
	}

}

void confirm_wall(void){

	int converted_x = convert_index_bts(w.position.x);
	int converted_y = convert_index_bts(w.position.y);
	int i,j, found1 = 0, found2 = 0;
	char str[20];
	uint32_t move;
	
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
		GUI_Text(10, 285, (unsigned char*) "Do not trap players with", White, Red);
		GUI_Text(10, 300, (unsigned char*) "walls!", White, Red);
		enable_timer(1);
		//delete wall
		if(w.horizontal == 1){
			for(i = w.position.x; i <= w.position.x + 4; i++){
				game_matrix[w.position.y][i] = 0;
			}
		}
		// TODO out of matrix -> but it was horizontal
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
		DrawWallHorizontalThroughIndex(converted_x,converted_y, Brown);
		for(i = w.position.x; i <= w.position.x + 4; i++){
			game_matrix[w.position.y][i] = 3;
		}
	}
	else {
		DrawWallVerticalThroughIndex(converted_x,converted_y, Brown);
		for(i = w.position.y; i <= w.position.y + 4; i++){
			game_matrix[i][w.position.x] = 3;
		}
	}
	move = w.position.x;
	move |= w.position.y << 8;
	move |= w.horizontal << 16;
	move |= 1 << 20;
	if(turn == 2){
		move |= 1 << 24;
	}
	moves[ind++] = move;
	switch_turn();
	
}

void delete_wall(int x, int y){

	int x1 = convert_index_stb(x);
	int y1 = convert_index_stb(y);
	
	//Check if there are other walls
	if(w.horizontal == 1){
		DrawWallHorizontalThroughIndex(x,y,White);
		// horizontal
		if(game_matrix[y1][x1-1] == 0 && game_matrix[y1][x1+1] == 3){
			DrawWallHorizontalThroughIndex(x,y,Brown);
		}
		if(game_matrix[y1][x1+1] == 0 && game_matrix[y1][x1+3] == 3){
			DrawWallHorizontalThroughIndex(x+1,y,Brown);
		}
		if(game_matrix[y1][x1-3] == 0 && game_matrix[y1][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-1,y,Brown);
		}
		if(game_matrix[y1][x1+3] == 0 && game_matrix[y1][x1+5] == 3){
			DrawWallHorizontalThroughIndex(x+2,y,Brown);
		}
		if(game_matrix[y1][x1+1] == 0 && game_matrix[y1][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-2,y,Brown);
		}
		//vertical
		if(game_matrix[y1 - 1][x1] == 0 && game_matrix[y1+1][x1] == 3){
			DrawWallVerticalThroughIndex(x,y,Brown);
		}
		if(game_matrix[y1-1][x1+4] == 0 && game_matrix[y1+1][x1+4] == 3){
			DrawWallVerticalThroughIndex(x+2,y,Brown);
		}
		if(game_matrix[y1+1][x1] == 0 && game_matrix[y1-1][x1] == 3){
			DrawWallVerticalThroughIndex(x,y-2,Brown);
		}
		if(game_matrix[y1+1][x1+4] == 0 && game_matrix[y1-1][x1+4] == 3){
			DrawWallVerticalThroughIndex(x+2,y-2,Brown);
		}
		if(game_matrix[y1-3][x1] == 0 && game_matrix[y1-1][x1] == 3){
			DrawWallVerticalThroughIndex(x,y-1,Brown);
		}
		if(game_matrix[y1-3][x1+4] == 0 && game_matrix[y1-1][x1+4] == 3){
			DrawWallVerticalThroughIndex(x+2,y-1,Brown);
		}
		if(game_matrix[y1-3][x1+2] == 0 && game_matrix[y1-1][x1+2] == 3){
			DrawWallVerticalThroughIndex(x+1,y-1,Brown);
		}
		if(game_matrix[y1+1][x1+2] == 0 && game_matrix[y1-1][x1+2] == 3){
			DrawWallVerticalThroughIndex(x+1,y-2,Brown);
		}
		if(game_matrix[y1-1][x1+2] == 0 && game_matrix[y1+1][x1+2] == 3){
			DrawWallVerticalThroughIndex(x+1,y,Brown);
		}
	}
	else if(w.horizontal == 0){
		DrawWallVerticalThroughIndex(x,y,White);
		// vertical
		if(game_matrix[y1-1][x1] == 0 && game_matrix[y1+1][x1] == 3){
			DrawWallVerticalThroughIndex(x,y,Brown);
		}
		if(game_matrix[y1+1][x1] == 0 && game_matrix[y1+3][x1] == 3){
			DrawWallVerticalThroughIndex(x,y+1,Brown);
		}
		if(game_matrix[y1-3][x1] == 0 && game_matrix[y1-1][x1] == 3){
			DrawWallVerticalThroughIndex(x,y-1,Brown);
		}
		if(game_matrix[y1+3][x1] == 0 && game_matrix[y1+5][x1] == 3){
			DrawWallVerticalThroughIndex(x,y+2,Brown);
		}
		if(game_matrix[y1+1][x1] == 0 && game_matrix[y1-1][x1] == 3){
			DrawWallVerticalThroughIndex(x,y-2,Brown);
		}
		//horizontal
		if(game_matrix[y1][x1-1] == 0 && game_matrix[y1][x1+1] == 3){
			DrawWallHorizontalThroughIndex(x,y,Brown);
		}
		if(game_matrix[y1+4][x1-1] == 0 && game_matrix[y1+4][x1+1] == 3){
			DrawWallHorizontalThroughIndex(x,y+2,Brown);
		}
		if(game_matrix[y1][x1+1] == 0 && game_matrix[y1][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-2,y,Brown);
		}
		if(game_matrix[y1+4][x1+1] == 0 && game_matrix[y1+4][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-2,y+2,Brown);
		}
		if(game_matrix[y1][x1-3] == 0 && game_matrix[y1][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-1,y,Brown);
		}
		if(game_matrix[y1+4][x1-3] == 0 && game_matrix[y1+4][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-1,y+2,Brown);
		}
		if(game_matrix[y1+2][x1-3] == 0 && game_matrix[y1+2][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-1,y+1,Brown);
		}
		if(game_matrix[y1+2][x1+1] == 0 && game_matrix[y1+2][x1-1] == 3){
			DrawWallHorizontalThroughIndex(x-2,y+1,Brown);
		}
		if(game_matrix[y1+2][x1-1] == 0 && game_matrix[y1+2][x1+1] == 3){
			DrawWallHorizontalThroughIndex(x,y+1,Brown);
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
		move = p1.position.x;
		move |= p1.position.y << 8;
		move |= 1 << 16;
	}
	else {
		move = p2.position.x;
		move |= p2.position.y << 8;
		move |= 1 << 16;
		move |= 1 << 24;
	}
	moves[ind++] = move;
	switch_turn();

}

// win screen

void win_screen(void){

	disable_timer(0);
	reset_timer(0);
	disable_timer(1);
	reset_timer(1);
	move_mode = -1;
	ColorRectangle(55,87,96,128,0x7c37);
	if(turn == 1){
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

	GUI_Text(65, 100, (uint8_t *) "QUORIDOR GAME", White, Black);
	GUI_Text(35, 130, (uint8_t *) "Press INT0 to start a", Black, White);
	GUI_Text(90, 150, (uint8_t *) "match", Black, White);

}


