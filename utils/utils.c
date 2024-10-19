#include "utils.h"

static Position_e q[maxN];
static int N, head, tail;
static int marked[15][15];
static Player player;

int QUEUEempty() {
	return head%N == tail;
}

void QUEUEput(Position_e val) {
	q[tail++] = val;
	tail = tail%N;
}
Position_e QUEUEget() {
	head = head%N;
	return q[head++];
}

void QUEUEinit() {

	N = maxN+1;
	head = N;
	tail = 0;
}

void init_player_bfs(volatile Player *p, Position_e pos, int id){

	p->id = id;
	p->count_pmoves = 0;
	p->position.x = pos.x;
	p->position.y = pos.y;


}

void QUEUE_add_moves(volatile Player *p, int distance){

	int i, j;
	int x = p->position.x;
	int y = p->position.y;
	Position_e pos;
	for(i=0; i<p->count_pmoves; i++){
		x = p->position.x;
		y = p->position.y;
		for(j=0; j<strlen((char *)p->possible_moves[i]); j++) {
			if(p->possible_moves[i][j] == 'u') y-=2;
			else if(p->possible_moves[i][j] == 'd') y+=2;
			else if(p->possible_moves[i][j]== 'l') x-=2;
			else if(p->possible_moves[i][j]== 'r') x+=2;
		}
		if(marked[y][x] < 0) {
			pos.x = x;
			pos.y = y;
			pos.dist = distance;
			QUEUEput(pos);
			marked[y][x] = 0;
		}
	}

}

void bfs(Position_e e, int id, int game_matrix[15][15]) {
	int distance;
	init_player_bfs(&player,e,id);
	QUEUEinit();
	marked[e.y][e.x] = 1;
	get_possible_moves(&player,game_matrix);
	QUEUE_add_moves(&player,1);

	while (!QUEUEempty()){
		e = QUEUEget();
		distance = (e.dist)+1;
		if(marked[e.y][e.x] <= 0){
			marked[e.y][e.x] = e.dist;
			init_player_bfs(&player, e,id);
			get_possible_moves(&player,game_matrix);
			QUEUE_add_moves(&player,distance);
	}
	}
}

int distance_to_goal(volatile Player *p, int game_matrix[15][15]){

	int i,j,min=INFINITY;
	int id = p->id;
	int x = p->position.x;
	int y = p->position.y;
	Position_e pos;
	pos.x = x;
	pos.y = y;
	for(i=0; i<15;i++){
		for(j=0; j<15;j++){
			marked[i][j] = -1;
		}
	}
	bfs(pos,id,game_matrix);
	if(id == 1){
		for(i=0; i<15;i++){
			if(marked[1][i] < min && marked[1][i] > 0){
				min = marked[1][i];
			}
		}
	}
	else {
		for(i=0; i<15;i++){
			if(marked[13][i] < min && marked[13][i] > 0){
				min = marked[13][i];
			}
		}
	}
	
	return min;

}
