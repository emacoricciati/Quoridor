
typedef struct position {
	int x;
	int y;
} Position;

typedef struct player {

	int id;
	int available_walls;
	Position current_position;
	Position position;
	
} Player;

typedef struct wall {

	Position position;
	
} Wall;

void init_game_matrix(void);
void init_players(void);
int convert_index(int i);
void find_possible_moves(volatile Player *p);
int check_move_validity(int new_i, int new_j, int i, int j);
void move_player(Player *p, char move);
void reset_possible_moves(volatile Player *p);
void switch_turn(void);
void confirm_move(Player *p);
void enable_wall_mode(void);
void switch_mode(void);
void enable_move_mode(void);
void init_wall(void);
void move_wall(char move);
int check_wall_validity(int new_i, int new_j, int i, int j);
