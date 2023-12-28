
typedef struct position {
	int x;
	int y;
} Position;

typedef struct player {

	int id;
	int available_walls;
	Position current_position;
	
} Player;

void init_game_matrix(void);
void init_players(void);
int convert_index(int i);
void find_possible_moves(Player *p);
int check_move_validity(int x, int y);
void move_player(Player *p, char move);


