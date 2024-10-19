#include <stdlib.h>
#include "evaluation.h"
#include "../game/game.h"
#include "../utils/utils.h"

extern Player p1,p2;
extern int maximizing, minimizing;

int evaluate(int game_matrix[15][15])
{
	
	  int sum = 0;
    int distance_p1, distance_p2;

	
    // The better the state for the maximizing player the larger the evaluation
    // Evaluation must be in [-INFINITY, INFINITY]

    if (p1.position.y == 1)
        return (maximizing == 1) ? INFINITY : -INFINITY;
    else if (p2.position.y == 13)
        return (maximizing == 1) ? -INFINITY : INFINITY;

		distance_p1 = distance_to_goal(&p1,game_matrix);
		distance_p2 = distance_to_goal(&p2,game_matrix);
		
    if (maximizing == 1) // w
    {
        // Take into account enemy distance to goal
        sum -= distance_p1;
        sum += distance_p2;
        // If enemy very close to goal give a little penalty
        if (distance_p2 <= 3)
            sum -= (4 - distance_p2);

        // Having much less walls than the enemy is a very disadvantageous position
        if (p2.available_walls > p1.available_walls)
            sum -= (p2.available_walls - p1.available_walls)*2;

    }
    else //b
    {
        sum -= distance_p2;
        sum += distance_p1;
        if (distance_p1 <= 3)
            sum -= (4 - distance_p1);
            
        if (p1.available_walls > p2.available_walls)
            sum -= (p1.available_walls - p2.available_walls)*2;
    }


    return sum;
}

// Trivial evaluation function
/*
int tevaluate(Board board)
{
    evaluations++;

    if (board->player.i == 0)
        return (maximizing == 'W') ? INFINITY : -INFINITY;
    else if (board->enemy.i == board->dimension - 1)
        return (maximizing == 'W') ? -INFINITY : INFINITY;
    
    return 1;
}
*/
