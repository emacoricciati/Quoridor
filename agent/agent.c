
#include "../game/game.h"
#include "agent.h"
#include "evaluation.h"
#include "../GLCD/GLCD.h"
#include <stdio.h>
#include <stdlib.h>

extern Player p1, p2;
extern int minimizing, maximizing;
extern int game_matrix[15][15];

ActionPair actionPair;

size_t prunes = 0;

int get_legal_moves(volatile Player *p, Action actions[MAXMOVES]){
	
	int last_position_x = p->position.x;
	int last_position_y = p->position.y;
	Action a;
	
	int index=0;
	
	a.action = MOVE;
	a.agent = p->id;
	a.info.move.old_x = p->position.x;
	a.info.move.old_y = p->position.y;
	
	
		// x
		if(last_position_x == 1 && game_matrix[last_position_y][last_position_x + 1] != 3){
			// skip the opposing player
			if((p->id == 1 && game_matrix[last_position_y][last_position_x + 2] == 2) || (p->id == 2 && game_matrix[last_position_y][last_position_x + 2] == 1)){
				a.info.move.new_x = p->position.x - 4;
				a.info.move.new_y = p->position.y;
				actions[index++] = a;
			}
			else {
				a.info.move.new_x = p->position.x + 2;
				actions[index++] = a;
			}
		}
		else if(last_position_x == 13 && game_matrix[last_position_y][last_position_x - 1] != 3){
			// skip the opposing player
			if((p->id == 1 && game_matrix[last_position_y][last_position_x - 2] == 2) || (p->id == 2 && game_matrix[last_position_y][last_position_x - 2] == 1)){
				a.info.move.new_x = p->position.x - 4;
				a.info.move.new_y = p->position.y;
				actions[index++] = a;
			}
			else {
				a.info.move.new_x = p->position.x - 2;
				a.info.move.new_y = p->position.y;
				actions[index++] = a;
			}
		}
		else {
				//no wall/player on the left
			if((last_position_x >= 2 && game_matrix[last_position_y][last_position_x - 1] != 3 && game_matrix[last_position_y][last_position_x - 2] != (p->id == 1 ? 2 : 1)) || (last_position_x == 1 && game_matrix[last_position_y][last_position_x - 1] != 3)){
				
				a.info.move.new_x = p->position.x - 2;
				a.info.move.new_y = p->position.y;
				actions[index++] = a;
				}
				//no wall/player on the right
				if((last_position_x <= 12 && game_matrix[last_position_y][last_position_x + 1] != 3 && game_matrix[last_position_y][last_position_x + 2] != (p->id == 1 ? 2 : 1)) || (last_position_x == 13 && game_matrix[last_position_y][last_position_x + 1] != 3)){
					a.info.move.new_x = p->position.x + 2;
					a.info.move.new_y = p->position.y;
					actions[index++] = a;
				}
				// player on the left
				if(last_position_x >= 2 && game_matrix[last_position_y][last_position_x - 2] == (p->id == 1 ? 2 : 1)){
					// pawn at the left border
					if(last_position_x -3 == 0){
						if(game_matrix[last_position_y-1][last_position_x-2] != 3){
							a.info.move.new_x = p->position.x - 2;
							a.info.move.new_y = p->position.y - 2;
							actions[index++] = a;
						}
						if(game_matrix[last_position_y+1][last_position_x-2] != 3){
							a.info.move.new_x = p->position.x - 2;
							a.info.move.new_y = p->position.y + 2;
							actions[index++] = a;
						}
					}
					// no wall behind the pawn
					if(last_position_x >= 3 && game_matrix[last_position_y][last_position_x-3] != 3){
						a.info.move.new_x = p->position.x - 4;
						a.info.move.new_y = p->position.y;
						actions[index++] = a;
					}
					// wall behind the pawn
					else{
						if(game_matrix[last_position_y-1][last_position_x-2] != 3){
							a.info.move.new_x = p->position.x - 2;
							a.info.move.new_y = p->position.y-2;
							actions[index++] = a;
						}
						if(game_matrix[last_position_y+1][last_position_x-2] != 3){
							a.info.move.new_x = p->position.x - 2;
							a.info.move.new_y = p->position.y + 2;
							actions[index++] = a;
						}
					}
				}
				// player on the right
				if(last_position_x >= 2 && game_matrix[last_position_y][last_position_x + 2] == (p->id == 1 ? 2 : 1)){
					// pawn at the right border
					if(last_position_x + 3 == 14){
						if(last_position_x <= 12 && game_matrix[last_position_y-1][last_position_x+2] != 3){
							a.info.move.new_x = p->position.x + 2;
							a.info.move.new_y = p->position.y-2;
							actions[index++] = a;
						}
						if(last_position_x <= 12 && game_matrix[last_position_y+1][last_position_x+2] != 3){
							a.info.move.new_x = p->position.x + 2;
							a.info.move.new_y = p->position.y+2;
							actions[index++] = a;
						}
					}
					// no wall behind the pawn
					if(game_matrix[last_position_y][last_position_x+3] != 3){
						a.info.move.new_x = p->position.x + 4;
						a.info.move.new_y = p->position.y;
						actions[index++] = a;
					}
					// wall behind the pawn
					else{
						if(last_position_x <= 12 && game_matrix[last_position_y-1][last_position_x+2] != 3){
							a.info.move.new_x = p->position.x + 2;
							a.info.move.new_y = p->position.y-2;
							actions[index++] = a;
						}
						if(last_position_x <= 12 && game_matrix[last_position_y+1][last_position_x+2] != 3){
							a.info.move.new_x = p->position.x + 2;
							a.info.move.new_y = p->position.y+2;
							actions[index++] = a;
						}
					}
				}
	}
		
		// y
		
			if(last_position_y == 13){
				if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] != (p->id == 1 ? 2 : 1)){
					a.info.move.new_x = p->position.x;
					a.info.move.new_y = p->position.y-2;
					actions[index++] = a;
				}
				else if(game_matrix[last_position_y - 1][last_position_x] != 3 && game_matrix[last_position_y - 2][last_position_x] == (p->id == 1 ? 2 : 1) && game_matrix[last_position_y - 3][last_position_x] != 3 ){
					a.info.move.new_x = p->position.x;
					a.info.move.new_y = p->position.y-4;
					actions[index++] = a;
				}
			}
			// skip the opposing player
			else {
				if(last_position_y <= 12 && game_matrix[last_position_y + 2][last_position_x] != (p->id == 1 ? 2 : 1)){
					if(game_matrix[last_position_y + 1][last_position_x] != 3){
						a.info.move.new_x = p->position.x;
						a.info.move.new_y = p->position.y+2;
						actions[index++] = a;
					}
				}
				else {
					// pawn below
					if(last_position_y <= 11 && game_matrix[last_position_y + 3][last_position_x] != 3 && game_matrix[last_position_y + 1][last_position_x] != 3){
						a.info.move.new_x = p->position.x;
						a.info.move.new_y = p->position.y+4;
						actions[index++] = a;
					}
					// pawn at the bottom border
					if(last_position_y + 3 == 14){
						if(last_position_y <= 12 && game_matrix[last_position_y+2][last_position_x-1] != 3){
							a.info.move.new_x = p->position.x - 2;
							a.info.move.new_y = p->position.y + 2;
							actions[index++] = a;
						}
						if(last_position_y <= 12 && game_matrix[last_position_y+2][last_position_x+1] != 3){
							a.info.move.new_x = p->position.x + 2;
							a.info.move.new_y = p->position.y+2;
							actions[index++] = a;
						}
					}
					// wall behind the player
					if(last_position_y <= 11 && game_matrix[last_position_y + 3][last_position_x] == 3){
						if(last_position_y <= 12 && game_matrix[last_position_y+2][last_position_x-1] != 3){
							a.info.move.new_x = p->position.x - 2;
							a.info.move.new_y = p->position.y + 2;
							actions[index++] = a;
						}
						if(last_position_y <= 12 && game_matrix[last_position_y+2][last_position_x+1] != 3){
							a.info.move.new_x = p->position.x + 2;
							a.info.move.new_y = p->position.y+2;
							actions[index++] = a;
						}
					}
				}
				if(last_position_y >= 2){
					if(game_matrix[last_position_y - 2][last_position_x] != (p->id == 1 ? 2 : 1)){
						if( game_matrix[last_position_y - 1][last_position_x] != 3){
							a.info.move.new_x = p->position.x;
							a.info.move.new_y = p->position.y-2;
							actions[index++] = a;
						}
					}
					else {
						// pawn above
						if(last_position_y >= 3 && game_matrix[last_position_y - 3][last_position_x] != 3 && game_matrix[last_position_y - 1][last_position_x] != 3){
							a.info.move.new_x = p->position.x;
							a.info.move.new_y = p->position.y-4;
							actions[index++] = a;
						}
						// pawn at the left border
						if(last_position_y -3 == 0){
							if(game_matrix[last_position_y-2][last_position_x-1] != 3){
								a.info.move.new_x = p->position.x - 2;
								a.info.move.new_y = p->position.y-2;
								actions[index++] = a;
							}
							if(game_matrix[last_position_y-2][last_position_x+1] != 3){
								a.info.move.new_x = p->position.x + 2;
								a.info.move.new_y = p->position.y-2;
								actions[index++] = a;
							}
						}
						// wall behind the player
						if(last_position_y >= 3 && game_matrix[last_position_y - 3][last_position_x] == 3){
							if(game_matrix[last_position_y-2][last_position_x-1] != 3){
								a.info.move.new_x = p->position.x - 2;
								a.info.move.new_y = p->position.y-2;
								actions[index++] = a;
							}
							if(game_matrix[last_position_y-2][last_position_x+1] != 3){
								a.info.move.new_x = p->position.x + 2;
								a.info.move.new_y = p->position.y-2;
								actions[index++] = a;
							}
						}
					}
				}

			}
			p->count_pmoves = index;
			return index;
			
}

int check_wall_position(volatile Wall *w){

	//int marked[7][7];
	int converted_x = convert_index_bts(w->position.x);
	int converted_y = convert_index_bts(w->position.y);
	//int i,j; 
	//int found1 = 0, found2 = 0;
	
	// if position of the wall is valid
	
	if(w->horizontal == 1){
		// avoid overlap
		if((w->position.x <= 13 && (game_matrix[w->position.y][w->position.x] == 3 || game_matrix[w->position.y][w->position.x] == 4) && (game_matrix[w->position.y][w->position.x + 1] == 3 || game_matrix[w->position.y][w->position.x + 1] == 4)) || (w->position.x <=11 && (game_matrix[w->position.y][w->position.x + 3] == 3 || game_matrix[w->position.y][w->position.x + 3] == 4))){
			return 0;
		}
		// avoid cross
		if(w->position.x <= 12 && w->position.y >= 1 && w->position.y <= 13 && (game_matrix[w->position.y][w->position.x + 2] == 3 || game_matrix[w->position.y][w->position.x + 2] == 4) && (game_matrix[w->position.y + 1][w->position.x + 2] == 3 || game_matrix[w->position.y + 1][w->position.x + 2] == 4) && (game_matrix[w->position.y - 1][w->position.x + 2] == 3 || game_matrix[w->position.y - 1][w->position.x + 2] == 4) ){
			return 0;
		}
	}
	else {
		// avoid overlap
		if((w->position.y <= 13 && (game_matrix[w->position.y][w->position.x] == 3 || game_matrix[w->position.y][w->position.x] == 4) && (game_matrix[w->position.y + 1][w->position.x] == 3 || game_matrix[w->position.y + 1][w->position.x] == 4)) || (w->position.y <= 11 && (game_matrix[w->position.y + 3][w->position.x] == 3 || game_matrix[w->position.y + 3][w->position.x] == 4))){
			return 0;
		}
		// avoid cross
		if(w->position.y <= 12 && w->position.x >= 1 && w->position.x <= 13 && game_matrix[w->position.y + 2][w->position.x] == 3 && game_matrix[w->position.y + 2][w->position.x + 1] == 3 && game_matrix[w->position.y + 2][w->position.x - 1] == 3 ){
			return 0;
		}
	}
	
	
	
	return 1;
	
}

int compare_walls(const void *a, const void *b){
		Action a1 = *((Action *) a);
		Action b1 = *((Action *) b);
		return (a1.info.wall.dist_from_enemy - b1.info.wall.dist_from_enemy);
}
int compare_positions1(const void *a, const void *b){
		Action a1 = *((Action *) a);
		Action b1 = *((Action *) b);
		return (a1.info.move.new_y - a1.info.move.old_y)-(b1.info.move.new_y - b1.info.move.old_y);
}

int compare_positions2(const void *a, const void *b){
		Action a1 = *((Action *) a);
		Action b1 = *((Action *) b);
		return -((a1.info.move.new_y - a1.info.move.old_y)-(b1.info.move.new_y - b1.info.move.old_y));
}

int get_legal_actions(volatile Player *p, Action actions[MAXMOVES]){

	int i,j,length,ox,oy,x1,y1,initial_length;
	Wall w;
	Action a, initial_actions[77];
	int index = 0;
	
	
	// get moves
	length = get_legal_moves(p,actions);
	initial_length = length;
	// get wall placements
	for(i=0;i<15;i=i+2){
		for(j=0; j<15; j=j+2){
			w.position.x = j;
			w.position.y = i;
			x1 = convert_index_bts(j);
			y1 = convert_index_bts(i);
			//place horizontal
			w.horizontal = 1;
			if(check_wall_validity(x1,y1,&w)== 1 && check_wall_position(&w)== 1){
				a.action = PLACE;
				a.agent = p->id;
				a.info.wall.horizontal = w.horizontal;
				a.info.wall.x = w.position.x;
				a.info.wall.y = w.position.y;
				// manhattan distance to opposite player
				ox = (p->id == 1 ? p2.position.x : p1.position.x);
				oy = (p->id == 1 ? p2.position.y : p1.position.y);
				a.info.wall.dist_from_enemy = abs(ox-j)+abs(oy-i);
				initial_actions[index++] = a;
			}
			//place vertical
			w.horizontal = 0;
			if(check_wall_validity(x1,y1,&w)== 1 && check_wall_position(&w)== 1){
				a.action = PLACE;
				a.agent = p->id;
				a.info.wall.horizontal = w.horizontal;
				a.info.wall.x = w.position.x;
				a.info.wall.y = w.position.y;
				// manhattan distance to opposite player
				ox = (p->id == 1 ? p2.position.x : p1.position.x);
				oy = (p->id == 1 ? p2.position.y : p1.position.y);
				a.info.wall.dist_from_enemy = abs(ox-j)+abs(oy-i);
				initial_actions[index++] = a;
			}
			
		}
	
	}
	if(p->id == 1){
		qsort(actions,initial_length, sizeof(Action),compare_positions1);
	}
	else {
		qsort(actions,initial_length, sizeof(Action),compare_positions2);
	}
	qsort(initial_actions,index, sizeof(Action),compare_walls);
	for(i=0;i<MAXMOVES-length;i++){
		actions[length++] = initial_actions[i];
	}
	return index;
}

int play_action(Action a){
	
	int i,result1,result2;
	int id = a.agent;
	int n_walls = (id == 1) ? p1.available_walls : p2.available_walls;

	if(a.action == MOVE){
		if(id == 1){
			game_matrix[p1.position.y][p1.position.x] = 0;
			p1.position.x = a.info.move.new_x;
			p1.position.y = a.info.move.new_y;
			game_matrix[p1.position.y][p1.position.x] = id;

		}
		else {
			game_matrix[p2.position.y][p2.position.x] = 0;
			p2.position.x = a.info.move.new_x;
			p2.position.y = a.info.move.new_y;
			game_matrix[p2.position.y][p2.position.x] = id;
		}	
	}
	else {
		// place wall
		if(n_walls>0){
			if(a.info.wall.horizontal == 1){
				for(i = a.info.wall.x; i <= a.info.wall.x + 4; i++){
					game_matrix[a.info.wall.y][i] += 3;
				}
			}
			else {
				for(i = a.info.wall.y; i <= a.info.wall.y + 4; i++){
					game_matrix[i][a.info.wall.x] += 3;
				}
			}
			result1 = distance_to_goal(&p1);
			result2 = distance_to_goal(&p2);
			if(result1 == INFINITY || result2 == INFINITY){
				return 0;
			}
			id == 1 ? p1.available_walls-- : p2.available_walls--;
		}
		else {
			return 0;
		}
	
	}
	return 1;

}

void undo_action(Action a){

	int i;
	int id = a.agent;
	int n_walls = (id == 1) ? p1.available_walls : p2.available_walls;
	
	if(a.action == MOVE){
		if(id == 1){
			game_matrix[p1.position.y][p1.position.x] = 0;
			p1.position.x = a.info.move.old_x;
			p1.position.y = a.info.move.old_y;
			game_matrix[p1.position.y][p1.position.x] = id;

		}
		else {
			game_matrix[p2.position.y][p2.position.x] = 0;
			p2.position.x = a.info.move.old_x;
			p2.position.y = a.info.move.old_y;
			game_matrix[p2.position.y][p2.position.x] = id;
		}	
	}
	else {
		// delete wall
		if(n_walls>0){
			if(a.info.wall.horizontal == 1){
				for(i = a.info.wall.x; i <= a.info.wall.x + 4; i++){
					game_matrix[a.info.wall.y][i] -= 3;
				}
			}
			else {
				for(i = a.info.wall.y; i <= a.info.wall.y + 4; i++){
					game_matrix[i][a.info.wall.x] -= 3;

				}
			}
			id == 1 ? p1.available_walls++ : p2.available_walls++;
	}
	}

}


int is_terminal_state(void){
    return (p1.position.y == 1) || (p2.position.y == 13);
}

ActionPair perform_action(int depth){

	int i,j;
	ActionPair value = alpha_beta(game_matrix,depth,1,-INFINITY,INFINITY,evaluate);
	// restore matrix
	for(i=0; i<15;i++){
		for(j=0; j<15;j++){
			if(game_matrix[i][j] > 3){
				game_matrix[i][j] = 3;
			}
		}
	}
	return value;

}
ActionPair alpha_beta(int game_matrix[15][15], uint32_t depth, uint8_t agent, int alpha, int beta, EvaluationFunction eval)
{
	
		int i;
		uint8_t next_agent;
		Action action, actions[MAXMOVES];
		ActionPair pair, result, to_return;
	
		 // Reached depth limit
    if (depth == 0 || is_terminal_state()){
				result.evaluation = eval((game_matrix) + depth);
        return result;
		}
		
		for(i=0;i<MAXMOVES;i++){
			action.agent = -1;
			action.info.move.new_x = 0;
			action.info.move.new_y = 0;
			action.info.move.old_x = 0;
			action.info.move.old_y = 0;
			action.info.wall.dist_from_enemy = 0;
			action.info.wall.horizontal = 0;
			action.info.wall.x = 0;
			action.info.wall.y = 0;
			actions[i] = action;
		}
		
    // agent -> 1 maximizing , 0 minmizing

		if(agent == 1){
			if(maximizing == 1){
				get_legal_actions(&p1,actions);
			}
			else if(maximizing == 2){
				 get_legal_actions(&p2,actions);
			}
		}
		if(agent == 0){
			if(minimizing == 1){
				get_legal_actions(&p1,actions);
			}
			else if(minimizing == 2){
				get_legal_actions(&p2,actions);
			}
		}

    next_agent = !agent;

		pair.evaluation = agent ? (- INFINITY - 1) : (INFINITY + 1);
    
    for ( i = 0; i < MAXMOVES; i++){
        if ((agent == 1 && pair.evaluation >= beta) || (agent == 0 && pair.evaluation <= alpha)){
           prunes++;
           break;
        }

				action = actions[i];
				
				if(play_action(action)){
				

        to_return = alpha_beta(game_matrix, depth - 1, next_agent, alpha, beta, eval);

				undo_action(action);
				
				to_return.action = action;
				
        if (agent == 1 && pair.evaluation < to_return.evaluation){
					pair = to_return;
					if(alpha < pair.evaluation){
						alpha = pair.evaluation;
					}
        }
        else if (agent == 0 && pair.evaluation > to_return.evaluation){
					pair = to_return;
					if(beta > pair.evaluation){
						beta = pair.evaluation;
					}
					
        }
			}	
    }

    return pair;

}

