#ifndef GAME_H__
#define GAME_H__
#include "stdint.h"

#define INITIAL_WALLS 8
#define DEPTH 2

typedef struct position {
	int x;
	int y;
} Position;

typedef struct player {

	int id;
	int available_walls;
	Position current_position;
	Position position;
	char possible_moves[12][3];
	int count_pmoves;
	
} Player;

typedef struct wall {

	Position position;
	int horizontal;
	
} Wall;

typedef struct game_mode {

	int menu1;
	int menu2;
	int current_selection;

} Game_mode;

void init_game_matrix(void);
void init_players(void);
int convert_index_bts(int i);
int convert_index_stb(int i);
void find_possible_moves(volatile Player *p);
int check_move_validity(int new_i, int new_j, int i, int j);
void move_player(Player *p, char move);
void reset_possible_moves(volatile Player *p);
void switch_turn(void);
void confirm_move(volatile Player *p);
void enable_wall_mode(void);
void switch_mode(void);
void enable_move_mode(void);
void init_wall(void);
void move_wall(char move);
int check_wall_validity(int new_i, int new_j, volatile Wall *w);
void moveWall(int x, int y, int new_x, int new_y);
void rotate_wall(void);
void confirm_wall(void);
void delete_wall(int x, int y);
void reset_choice(void);
int equal_position(Position p1, Position p2);
void check_available_path(int i, int j, int *found, int id, int marked[7][7]);
void win_screen(int id);
void initial_screen(void);
void game_setup(void);
void menu1_screen(void);
void menu2_screen(void);
void menu3_screen(void);
void color_mode(int choice);
void init_game(void);
void confirm_selection(void);
void DrawPlayerMove (uint32_t move);
void get_possible_moves(volatile Player *p, int matrix[15][15]);
void PlayNPCAction(void);
void waiting_to_play(void);
#endif // GAME_H__
